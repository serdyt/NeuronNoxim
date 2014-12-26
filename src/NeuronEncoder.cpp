/*
 * NeuronEncoder.cpp
 *
 *  Created on: Dec 20, 2014
 *      Author: ser
 */

#include "NeuronEncoder.h"

NeuronEncoder::NeuronEncoder(sc_module_name name_)
	: sc_module(name_), mem("memory"), txSr("txSR"), senderIDsr("senderIDsr")
{
	blockSize = 0;
	FSMlocalAddr = 0;
	FSMlocalAck = false;
	FSMlocalTx = false;

	txSr.clock(clock);
	txSr.input(txFSMsr);
	txSr.output(txSrOut);

	senderIDsr.clock(clock);
	senderIDsr.input(senderID);
	senderIDsr.output(senderIDsrOut);

	memWrite.write(false);

	mem.clock(clock);
	mem.address(memAddr);
	mem.data(dstOut);
	mem.write(memWrite);

	FSMstate = IDLE;
	FSMlocalAck = false;

	SC_METHOD(offsetCalc);
	sensitive<<senderID;

	SC_METHOD(memAddrCalc);
	sensitive<<offset<<localMemAddr;

	SC_METHOD(formPacket);
	sensitive<<senderIDsrOut<<dstOut;

	SC_METHOD(txCalc);
	sensitive<<txSrOut<<dstOut;

	SC_THREAD(FSM);
	sensitive<<clock.pos();
}

void NeuronEncoder::txCalc(){
	tx.write(txSrOut.read() & dstOut.read().exist);
}

void NeuronEncoder::offsetCalc(){
	offset.write(senderID.read() * blockSize);
}

void NeuronEncoder::memAddrCalc(){
	//cout<< sc_time_stamp() << " Encoder offset " << offset.read() << " localMemAddr " << localMemAddr.read()<<endl;
	memAddr.write(offset.read() + localMemAddr.read());
}

void NeuronEncoder::formPacket(){
	//TODO : check that it works with size = 1
	NoximPacket p(localID.read(), senderIDsrOut.read(),
						dstOut.read().dstID, dstOut.read().dstNeurID,
						sc_time_stamp().to_double() / 1000, 2);
	packetOut.write(p);
}

void NeuronEncoder::initComponents(vector<dstMemStruct> mem_, int blockSize_){
	mem.init(mem_);
	blockSize = blockSize_;
}

void NeuronEncoder::FSM() {
	while (true) {
		wait();

		switch (FSMstate) {
		case IDLE: {
			FSMlocalAck = false;
			FSMlocalTx = false;
			if(rx.read()){
				FSMlocalAddr = 0;
				FSMstate = SEND;
				FSMlocalTx = true;
				//cout<< sc_time_stamp() << " Encoder received senderID " << senderID.read() << endl;
			}
			break;
		}
		case SEND: {
			FSMlocalAddr++;
			FSMlocalTx = true;
			FSMlocalAck = false;
			if (FSMlocalAddr >= blockSize || dstOut.read().exist == false){
				FSMlocalAck = true;
				FSMlocalTx = false;
				FSMstate = STOP;
				//FSMlocalAddr = 0;
			}
			break;
		}
		case STOP: {
			FSMlocalTx = false;
			//FSMlocalAddr = 0;
			FSMlocalAck = false;
			FSMstate = IDLE;
			break;
		}
		}
		txFSMsr.write(FSMlocalTx);
		ack.write(FSMlocalAck);
		localMemAddr.write(FSMlocalAddr);
	}
}
