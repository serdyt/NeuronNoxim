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
	sc_in<int> localID;
	sc_out<bool> ack;
	sc_out<bool> tx;
	sc_out<NoximPacket> packetOut;

	NoximPower power;

	SC_HAS_PROCESS(NeuronEncoder);
	NeuronEncoder(sc_module_name name_);
	void initComponents(vector<dstMemStruct> mem_, int blockSize_);

private:
	NeuronMemory<dstMemStruct> mem;
	ShiftRegister<bool, 1> txSr;
	ShiftRegister<int, 1> senderIDsr;

	sc_signal<int> offset;
	sc_signal<int> localMemAddr;
	sc_signal<int> memAddr;
	sc_signal<int> dstID;
	sc_signal<int> dstNeurID;
	sc_signal<bool> txFSMsr;
	sc_signal<dstMemStruct> dstOut;
	sc_signal<bool> memWrite;
	sc_signal<int> senderIDsrOut;
	sc_signal<bool> txSrOut;

	int blockSize;
	int FSMlocalAddr;
	bool FSMlocalAck;
	bool FSMlocalTx;
	enum {IDLE, SEND, STOP} FSMstate;

	void FSM();
	void memAddrCalc();
	void offsetCalc();
	void formPacket();
	void txCalc();
};

#endif /* SRC_NEURONENCODER_H_ */
