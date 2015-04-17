/*
 * NeuronPEmain.h
 *
 *  Created on: Dec 22, 2014
 *      Author: ser
 */

#ifndef SRC_NEURONPE_H_
#define SRC_NEURONPE_H_

#include "NoximMain.h"
#include "NeuronDecoder.h"
#include "NeuronArbiter.h"
#include "Neuron.h"
#include "NeuronMux.h"
#include "NeuronEncoder.h"

SC_MODULE(NeuronPE){
	  sc_in_clk clock;
	  sc_in<bool>  reset;
	  sc_in<NoximPacket> packetIn;
	  sc_in<bool> rx;

	  sc_out<NoximPacket> packetOut;
	  sc_out<bool> tx;

	  NeuronDecoder decoder;
	  NeuronMux<float> mux;
	  sc_vector<Neuron> neurons;
	  NeuronArbiter arbiter;
	  NeuronEncoder encoder;

	  SC_HAS_PROCESS(NeuronPE);

	  NeuronPE(sc_module_name name_, int clusterID);

	  double getPower();

private:
	sc_signal<int> decoderDestIDout;
	sc_signal<float> decoderWeightOut;

	sc_vector<sc_signal<float> > muxOut;
	sc_vector<sc_signal<bool> > fire_flags;
	sc_vector<sc_signal<float> > V_reset;
	sc_vector<sc_signal<float> > V_th;
	sc_vector<sc_signal<float> > E;
	sc_vector<sc_signal<float> > tau;
	sc_vector<sc_signal<float> > Iex;
	sc_signal<float> step;

	sc_signal<bool> encoderAck;
	sc_signal<int> arbiterSernderID;
	sc_signal<bool> arbiterTx;

	sc_signal<int> localID;
};

#endif /* SRC_NEURONPE_H_ */
