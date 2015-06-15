/*
 * NeuronEncoder.h
 *
 *  Created on: Dec 20, 2014
 *      Author: ser
 */

#ifndef SRC_NEURONENCODER_H_
#define SRC_NEURONENCODER_H_

#include "NeuronMemory.h"
#include "NoximMain.h"
#include "NoximStats.h"
#include "NeuronShiftRegister.h"

SC_MODULE(NeuronEncoder){
	sc_in_clk clock;
	sc_in<bool> rx;
	sc_in<int> senderID;
	sc_in<int> localClusterID;
	sc_out<bool> neuronAck;
	sc_out<bool> tx;
	sc_out<NoximPacket> packetOut;

	sc_out<bool> localAxonTx;
	sc_out<int> localAxonID;
	sc_in<bool> localAxonAck;

	NoximPower power;

	SC_HAS_PROCESS(NeuronEncoder);
	NeuronEncoder(sc_module_name name_);
	void initComponents(vector<dstOffsetMemStruct> memOffset_ ,vector<dstMemStruct> memDest_);

private:
	NeuronMemory<dstMemStruct> memDest;
	NeuronMemory<dstOffsetMemStruct> memOffset;

	ShiftRegister<bool, 1> txSr;
	ShiftRegister<bool, 1> localAxonTxSr;

	sc_signal<int> localMemAddr;
	sc_signal<int> memAddr;
	sc_signal<int> dstID;
	sc_signal<int> dstAxon;
	sc_signal<bool> FSMtx;
	sc_signal<bool> FSMsrAxonTx;
	sc_signal<dstMemStruct> memDestOut;
	sc_signal<dstOffsetMemStruct> memOffsetOut;
	sc_signal<bool> memDestWrite;
	sc_signal<bool> memOffsetWrite;

	int FSMlocalN;
	int FSMlocalAddr;
	int FSMoffset;
	bool FSMlocalNeuronAck;
	bool FSMlocalTx;
	bool FSMlocalL;

	bool FSMlocalAxonTx;
	enum {IDLE, START, SEND, LOCAL, SENDACK, STOP} FSMstate;

	void FSM();
	//void memAddrCalc();
	void formPacket();
};

#endif /* SRC_NEURONENCODER_H_ */
