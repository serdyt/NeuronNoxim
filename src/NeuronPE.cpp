/*
 * NeuronPE.cpp
 *
 *  Created on: Dec 22, 2014
 *      Author: ser
 */

#include "NeuronPE.h"

NeuronPE::NeuronPE(sc_module_name name_, NeuronConfig& nConfig, int clusterID) :
		sc_module(name_), decoder("Decoder"), arbiter("Arbiter", nConfig.maxNeurons),
		encoder("Encoder"), mux("Mux", nConfig.maxNeurons), neurons("Neurons"), muxOut("muxOut")
	{

	/*
	 * Init decoder
	 */
	map<sndCAMstruct, int> cam;
	vector<float> weightMem;

	for (auto nit = nConfig.sources[clusterID].begin(); nit != nConfig.sources[clusterID].end(); nit++){
		for (auto dit = nit->begin(); dit != nit->end(); dit++){
			static int i = 0;
			//sourceCluster, source neuron, destNeuron
			//TODO : check this, or use static int to track neuron
			cam[sndCAMstruct(dit->first, dit->second, nit - nConfig.sources[clusterID].begin())] = i;
			i++;
			weightMem.push_back(dit->third);
		}
	}

	if (cam.size() == 0){
		cam[sndCAMstruct(0,0,0)] = 0;
	}

	if (weightMem.size() == 0){
		weightMem.resize(1);
	}

	decoder.initComponents(cam, weightMem);

	cout<<"*** Processing element "<<clusterID<<" ***"<<endl;
	cout << "Decoder CAM:" << endl;
	for (auto a : cam) {
		cout << a.first.srcID<<","<<a.first.srcNeurID<<","<< a.first.dstNeurID<<"->"<<a.second<<endl;
	}

	cout << "Decoder memory:" << endl;
	for (auto a : weightMem) {
		cout << a << endl;
	}

	cam.clear();
	weightMem.clear();

	/*
	 * Init neurons
	 */
	//construct modules normally
	neurons.init(nConfig.maxNeurons);

	fire_flags.init(nConfig.maxNeurons);
	V_reset.init(nConfig.maxNeurons);
	V_th.init(nConfig.maxNeurons);
	E.init(nConfig.maxNeurons);
	tau.init(nConfig.maxNeurons);
	Iex.init(nConfig.maxNeurons);
	muxOut.init(nConfig.maxNeurons);
	for_each(V_reset.begin(), V_reset.end(), [](sc_signal<float>& a) {a.write(DEFAULT_RESET_VOLTAGE);});
	for_each(V_th.begin(), V_th.end(), [](sc_signal<float>& a) {a.write(DEFAULT_THRESHOLD_VOLTAGE);});
	for_each(E.begin(), E.end(), [](sc_signal<float>& a) {a.write(DEFAULT_E);});
	for_each(tau.begin(), tau.end(), [](sc_signal<float>& a) {a.write(DEFAULT_TAU);});
	for_each(Iex.begin(), Iex.end(), [](sc_signal<float>& a) {a.write(DEFAULT_IEX);});

	step.write(nConfig.step);

	for (int i = 0; i < nConfig.parameters[clusterID].size(); i++) {
		cout<<"Neuron "<<i<<" ";
		for (auto mit : nConfig.parameters[clusterID][i]) {
			cout<<mit.first<< "="<<mit.second<<" ";
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
		cout<<endl;
	}

	/*
	 * Init encoder
	 */
	vector<dstMemStruct> destMem;
	destMem.resize(nConfig.maxNeurons * nConfig.maxDest, dstMemStruct(0,0,false));

	for (int i = 0; i < nConfig.destinations[clusterID].size(); i++){
		for (int j = 0; j < nConfig.destinations[clusterID][i].size(); j++){
			destMem[i*nConfig.maxDest+j] = dstMemStruct(nConfig.destinations[clusterID][i][j].first, nConfig.destinations[clusterID][i][j].second, true);
		}
	}

	encoder.initComponents(destMem, nConfig.maxDest);

	cout<<"Encoder memory:"<<endl;
	for (auto a : destMem){
		cout<<a.dstID<<","<<a.dstNeurID<<","<<a.exist<<endl;
	}

	destMem.clear();

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
