/*
 * NeuronPE.cpp
 *
 *  Created on: Dec 22, 2014
 *      Author: ser
 */

#include "NeuronPE.h"

double NeuronPE::getPower()
{
	double pwr = 0;
	pwr += decoder.power.getPower();
	for (auto it = neurons.begin(); it != neurons.end(); it++){
		pwr += it->power.getPower();
	}
	pwr += arbiter.power.getPower();
	pwr += encoder.power.getPower();
	return pwr;
}

void NeuronPE::FIFOctrl(){
	//TODO : FIFO size is not limited yet!!!
	if (rx.read()){
		FIFO.push(packetIn.read().axon_id);
		localAxonAck.write(false);
	}
	else if(localAxonRx.read()){
		FIFO.push(localAxon);
		localAxonAck.write(true);
	}
	else{
		localAxonAck.write(false);
	}

	if (decoderAck.read()){
		if (FIFO.size() != 0){
			FIFO.pop();
		}
	}

	if (FIFO.size() != 0){
		FIFOctrlReady.write(true);
		FIFOaxonOut.write(FIFO.front());
	}
	else{
		FIFOctrlReady.write(false);
		FIFOaxonOut.write(0);
	}

	FIFOmax = (FIFOmax < FIFO.size()) ? FIFO.size() : FIFOmax;
}


NeuronPE::NeuronPE(sc_module_name name_, int clusterID) :
		sc_module(name_), decoder("Decoder"), mux("Mux",NoximGlobalParams::nConfig->maxNeurons),
		arbiter("Arbiter",NoximGlobalParams::nConfig->maxNeurons), encoder("Encoder")
{

	SC_METHOD(FIFOctrl);
	sensitive<<clock.pos();

	/*
	 * Init decoder
	 */

	vector<twoFieldMemStruct<int, int> > decMemOffset;
	vector<twoFieldMemStruct<int, float> > decMemWeight;

	int memoryAddressDec = 0;
	for (auto ait : NoximGlobalParams::nConfig->dstToSrc[clusterID]){
		decMemOffset.push_back(twoFieldMemStruct<int, int>(memoryAddressDec, ait.second.blockSize));
		for (auto rit : ait.second.data){
			decMemWeight.push_back(twoFieldMemStruct<int, float>(rit));
		}
		memoryAddressDec += ait.second.blockSize;
	}

	decMemOffset.resize(NoximGlobalParams::nConfig->maxDecoderOffsetRecords,twoFieldMemStruct<int, int>(0,0));

	if (decMemWeight.size() == 0){
		decMemWeight.push_back(twoFieldMemStruct<int, float> (0,0));
	}

	decoder.initComponents(decMemOffset, decMemWeight);

	if (NoximGlobalParams::consoleLogPolicy > 0){
		cout<<"*** Processing element "<<clusterID<<" ***"<<endl;
		cout << "Decoder offsetMem:" << endl;
		for (auto a : decMemOffset) {
			cout << a <<endl;
		}

		cout << "Decoder weightMemory:" << endl;
		for (auto a : decMemWeight) {
			cout << a << endl;
		}
	}

	decMemOffset.clear();
	vector<twoFieldMemStruct<int, int> >().swap(decMemOffset);
	decMemWeight.clear();
	vector<twoFieldMemStruct<int, float> >().swap(decMemWeight);

	/*
	 * Init neurons
	 */
	//construct modules normally
	neurons.init(NoximGlobalParams::nConfig->maxNeurons);

	fire_flags.init(NoximGlobalParams::nConfig->maxNeurons);
	V_reset.init(NoximGlobalParams::nConfig->maxNeurons);
	V_th.init(NoximGlobalParams::nConfig->maxNeurons);
	E.init(NoximGlobalParams::nConfig->maxNeurons);
	tau.init(NoximGlobalParams::nConfig->maxNeurons);
	Iex.init(NoximGlobalParams::nConfig->maxNeurons);
	muxOut.init(NoximGlobalParams::nConfig->maxNeurons);
	for_each(V_reset.begin(), V_reset.end(), [](sc_signal<float>& a) {a.write(DEFAULT_RESET_VOLTAGE);});
	for_each(V_th.begin(), V_th.end(), [](sc_signal<float>& a) {a.write(DEFAULT_THRESHOLD_VOLTAGE);});
	for_each(E.begin(), E.end(), [](sc_signal<float>& a) {a.write(DEFAULT_E);});
	for_each(tau.begin(), tau.end(), [](sc_signal<float>& a) {a.write(DEFAULT_TAU);});
	for_each(Iex.begin(), Iex.end(), [](sc_signal<float>& a) {a.write(DEFAULT_IEX);});

	step.write(NoximGlobalParams::nConfig->step);

	for (int i = 0; i <NoximGlobalParams::nConfig->parameters[clusterID].size(); i++) {
		for (auto mit :NoximGlobalParams::nConfig->parameters[clusterID][i]) {
			if (mit.first == "V_reset") {
				V_reset[i].write(mit.second);
			} else if (mit.first == "V_th") {
				V_th[i].write(mit.second);
			} else if (mit.first == "E") {
				E[i].write(mit.second);
			} else if (mit.first == "tau") {
				tau[i].write(mit.second);
			} else if (mit.first == "Iex") {
				Iex[i].write(mit.second);
			} else {
				cout
						<< "It is not the place to check this, but configuration has wrong parameter name "
						<< mit.first << endl;
			}
		}
	}

	/*
	 * Init encoder
	 */
	
	vector<dstOffsetMemStruct> encMemOffset;
	vector<dstMemStruct> encMemDest;
	
	int memoryAddressEnc = 0;
	for (auto nit : NoximGlobalParams::nConfig->srcToDst[clusterID]){
		int blockSize = 0;
		bool local = false;
		int localAxonID;
		for (auto dcit : nit.second){
			blockSize++;
			if (clusterID == dcit.first){
				local = true;
				localAxonID = dcit.second.axonID;
			}
			else{
				encMemDest.push_back(dstMemStruct(dcit.first, dcit.second.axonID));
			}
		}
		if (local){
			encMemDest.push_back(dstMemStruct(clusterID, localAxonID));
		}
		encMemOffset.push_back(dstOffsetMemStruct(memoryAddressEnc, blockSize, local));
		memoryAddressEnc += blockSize;
	}

	encMemOffset.resize(NoximGlobalParams::nConfig->maxEncoderOffsetRecords, dstOffsetMemStruct(0,0,false));

	if (encMemDest.size() == 0){
		encMemDest.push_back(dstMemStruct(0,0));
	}

	encoder.initComponents(encMemOffset, encMemDest);

	if (NoximGlobalParams::consoleLogPolicy > 0){
		cout<<"Encoder offset memory:"<<endl;
		for (auto a : encMemOffset){
			cout<<a<<endl;
		}
		cout<<"Encoder destination memory:"<<endl;
		for (auto a : encMemDest){
			cout<<a<<endl;
		}
	}

	encMemDest.clear();
	vector<dstMemStruct>().swap(encMemDest);
	encMemOffset.clear();
	vector<dstOffsetMemStruct>().swap(encMemOffset);

	/*
	 * Connect modules
	 */
	decoder.clock(clock);
	decoder.axonIDin(FIFOaxonOut);
	decoder.ready(FIFOctrlReady);
	decoder.destIDout(decoderDestIDout);
	decoder.weightOut(decoderWeightOut);
	decoder.ack(decoderAck);

	mux.dataIn(decoderWeightOut);
	mux.control(decoderDestIDout);
	for (int i = 0; i < neurons.size(); i++){
		mux.dataOut[i](muxOut[i]);
	}

	for (int i = 0; i < neurons.size(); i++){
		neurons[i].clock(clock);
		neurons[i].reset(reset);
		neurons[i].step(step);
		neurons[i].weight(muxOut[i]);
		neurons[i].fire_flag(fire_flags[i]);
		neurons[i].V_reset(V_reset[i]);
		neurons[i].V_th(V_th[i]);
		neurons[i].E(E[i]);
		neurons[i].tau(tau[i]);
		neurons[i].Iex(Iex[i]);
	}

	arbiter.clock(clock);
	arbiter.ack(encoderNeuronAck);
	arbiter.tx(arbiterTx);
	arbiter.senderID(arbiterSernderID);
	for (int i = 0; i < neurons.size(); i++){
		arbiter.fire_flag[i](fire_flags[i]);
	}

	encoder.clock(clock);
	encoder.rx(arbiterTx);
	encoder.senderID(arbiterSernderID);
	encoder.localClusterID(localClusterID);
	encoder.neuronAck(encoderNeuronAck);
	encoder.tx(tx);
	encoder.packetOut(packetOut);
	encoder.localAxonID(localAxon);
	encoder.localAxonTx(localAxonRx);
	encoder.localAxonAck(localAxonAck);

	localClusterID.write(clusterID);
}
