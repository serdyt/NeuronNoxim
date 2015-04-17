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

NeuronPE::NeuronPE(sc_module_name name_, int clusterID) :
		sc_module(name_), decoder("Decoder"), mux("Mux",NoximGlobalParams::nConfig->maxNeurons),
		arbiter("Arbiter",NoximGlobalParams::nConfig->maxNeurons), encoder("Encoder")
{

	/*
	 * Init decoder
	 */
	map<sndCAMstruct, int> cam;
	vector<float> weightMem;

	int memoryAddress = 0;
	int neuronNumber = 0;
	for (auto nit :NoximGlobalParams::nConfig->sources[clusterID]){
		for (auto dit : nit){
			//sourceCluster, source neuron, destNeuron
			cam[sndCAMstruct(dit.clusterID, dit.neuronID, neuronNumber)] = memoryAddress;
			memoryAddress++;
			weightMem.push_back(dit.weight);
		}
		neuronNumber++;
	}

	if (cam.size() == 0){
		cam[sndCAMstruct(0,0,0)] = 0;
	}

	if (weightMem.size() == 0){
		weightMem.resize(1);
	}

	decoder.initComponents(cam, weightMem);

	if (NoximGlobalParams::consoleLogPolicy > 0){
		cout<<"*** Processing element "<<clusterID<<" ***"<<endl;
		cout << "Decoder CAM:" << endl;
		for (auto a : cam) {
			cout << a.first.srcID<<","<<a.first.srcNeurID<<","<< a.first.dstNeurID<<"->"<<a.second<<endl;
		}

		cout << "Decoder memory:" << endl;
		for (auto a : weightMem) {
			cout << a << endl;
		}
	}

	cam.clear();
	map<sndCAMstruct, int>().swap(cam);
	weightMem.clear();
	vector<float>().swap(weightMem);

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
	vector<dstMemStruct> destMem;
	destMem.resize(NoximGlobalParams::nConfig->maxNeurons *NoximGlobalParams::nConfig->maxDest, dstMemStruct(0,0,false));

	for (int i = 0; i <NoximGlobalParams::nConfig->destinations[clusterID].size(); i++){
		for (int j = 0; j <NoximGlobalParams::nConfig->destinations[clusterID][i].size(); j++){
			destMem[i*NoximGlobalParams::nConfig->maxDest+j] = dstMemStruct(NoximGlobalParams::nConfig->destinations[clusterID][i][j].clusterID,NoximGlobalParams::nConfig->destinations[clusterID][i][j].neuronID, true);
		}
	}

	encoder.initComponents(destMem,NoximGlobalParams::nConfig->maxDest);

	if (NoximGlobalParams::consoleLogPolicy > 0){
		cout<<"Encoder memory:"<<endl;
		for (auto a : destMem){
			cout<<a.dstID<<","<<a.dstNeurID<<","<<a.exist<<endl;
		}
	}

	destMem.clear();
	vector<dstMemStruct>().swap(destMem);

	/*
	 * Connect modules
	 */
	decoder.clock(clock);
	decoder.packetIn(packetIn);
	decoder.rx(rx);
	decoder.destIDout(decoderDestIDout);
	decoder.weightOut(decoderWeightOut);

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
	arbiter.ack(encoderAck);
	arbiter.tx(arbiterTx);
	arbiter.senderID(arbiterSernderID);
	for (int i = 0; i < neurons.size(); i++){
		arbiter.fire_flag[i](fire_flags[i]);
	}

	encoder.clock(clock);
	encoder.rx(arbiterTx);
	encoder.senderID(arbiterSernderID);
	encoder.localID(localID);
	encoder.ack(encoderAck);
	encoder.tx(tx);
	encoder.packetOut(packetOut);

	localID.write(clusterID);
}
