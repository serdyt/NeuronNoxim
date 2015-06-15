/*
 * NeuronEncoder.cpp
 *
 *  Created on: Dec 20, 2014
 *      Author: ser
 */

#include "NeuronEncoder.h"

//TODO : change power model.

NeuronEncoder::NeuronEncoder(sc_module_name name_)
	: sc_module(name_), memDest("DestMemory"), memOffset("OffsetMemory")
	  , txSr("txSR"), localAxonTxSr("localAxonTxSr")
{
	FSMlocalN = 0;
	FSMlocalAddr = 0;
	FSMoffset = 0;
	FSMlocalNeuronAck = false;
	FSMlocalTx = false;
	FSMlocalAxonTx = false;
	FSMlocalL = 1;

	memOffset.clock(clock);
	memOffset.address(senderID);
	memOffset.data(memOffsetOut);
	memOffset.write(memOffsetWrite);
	memOffsetWrite.write(false);

	txSr.clock(clock);
	txSr.input(FSMtx);
	txSr.output(tx);

	localAxonTxSr.clock(clock);
	localAxonTxSr.input(FSMsrAxonTx);
	localAxonTxSr.output(localAxonTx);

	memDest.clock(clock);
	memDest.address(memAddr);
	memDest.data(memDestOut);
	memDest.write(memDestWrite);
	memDestWrite.write(false);

	FSMstate = IDLE;

	//SC_METHOD(memAddrCalc);
	//sensitive<<memOffsetOut<<localMemAddr;

	SC_METHOD(formPacket);
	sensitive<<tx<<memDestOut;

	SC_METHOD(FSM);
	sensitive<<clock.pos();
}

//void NeuronEncoder::memAddrCalc(){
//	memAddr.write(memOffsetOut.read().offset + localMemAddr.read());
//}

void NeuronEncoder::formPacket(){
	NoximPacket p(localClusterID.read(), memDestOut.read().dstCluster, memDestOut.read().dstAxon,
				  sc_time_stamp().to_double() / 1000, 1);
	packetOut.write(p);

	localAxonID.write(memDestOut.read().dstAxon);

	//cout<< sc_time_stamp() << " OutputPacket = " << memDestOut.read().dstID << "," << memDestOut.read().dstAxon << endl;
}

void NeuronEncoder::initComponents(vector<dstOffsetMemStruct> memOffset_ ,vector<dstMemStruct> memDest_){
	memOffset.init(memOffset_);
	memDest.init(memDest_);
}

void NeuronEncoder::FSM() {
	switch (FSMstate) {
	case IDLE: {
		if (rx.read()) {
			FSMlocalNeuronAck = false;
			FSMlocalAxonTx = false;
			FSMlocalTx = false;
			//FSMlocalAddr = 0;
			FSMstate = START;
		} else {
			FSMlocalNeuronAck = false;
			FSMlocalAxonTx = false;
			FSMlocalTx = false;
			//FSMlocalAddr = FSMlocalAddr;
			FSMstate = IDLE;
		}
		break;
	}
	case START: {
		FSMlocalN = memOffsetOut.read().N;
		FSMlocalL = memOffsetOut.read().local;
		FSMoffset = memOffsetOut.read().offset;
		if (FSMlocalN == 0){
			FSMlocalNeuronAck = true;
			FSMlocalAxonTx = false;
			FSMlocalTx = false;
			FSMstate = STOP;
		}
		else if (FSMlocalN == 1 && FSMlocalL){
			FSMlocalNeuronAck = false;
			FSMlocalAxonTx = true;
			FSMlocalTx = false;
			FSMstate = LOCAL;
			FSMlocalAddr = 0;
		}
		else if (FSMlocalN == 1){
			FSMlocalNeuronAck = true;
			FSMlocalAxonTx = false;
			FSMlocalTx = true;
			FSMstate = IDLE;
			FSMlocalAddr = 0;
		}
		else {
			FSMlocalNeuronAck = false;
			FSMlocalAxonTx = false;
			FSMlocalTx = true;
			FSMstate = SEND;
			FSMlocalAddr = 0;
		}
		break;
	}
	case SEND: {
		FSMlocalAddr++;
		if (FSMlocalAddr >= FSMlocalN-1 && FSMlocalL){
			FSMlocalNeuronAck = false;
			FSMlocalTx = false;
			FSMlocalAxonTx = true;
			FSMstate = LOCAL;
		}
		else if (FSMlocalAddr >= FSMlocalN-1 && FSMlocalL != true){
			FSMlocalNeuronAck = true;
			FSMlocalTx = true;
			FSMlocalAxonTx = false;
			FSMstate = STOP;
		}
//		else if (FSMlocalAddr >= FSMlocalN-2 && FSMlocalL != true){
//			FSMlocalNeuronAck = true;
//			FSMlocalTx = true;
//			FSMlocalAxonTx = false;
//			FSMstate = SENDACK;
//		}
		else {
			FSMlocalNeuronAck = false;
			FSMlocalTx = true;
			FSMlocalAxonTx = false;
			FSMstate = SEND;
		}
		break;
	}
//	case SENDACK : {
//		if (rx.read() != true){
//			FSMlocalNeuronAck = false;
//			FSMlocalTx = false;
//			FSMlocalAxonTx = false;
//			FSMlocalAddr++;
//			FSMstate = IDLE;
//		}
//		else {
//			FSMlocalNeuronAck = false;
//			FSMlocalTx = true;
//			FSMlocalAxonTx = false;
//			FSMlocalAddr++;
//			FSMstate = START;
//		}
//		break;
//	}
	case LOCAL: {
		if (localAxonAck.read()){
			FSMlocalNeuronAck = true;
			FSMlocalTx = false;
			FSMlocalAxonTx = false;
			//FSMlocalAddr = FSMlocalAddr;
			FSMstate = STOP;
			cout<< sc_time_stamp() << " " << this->name() << " Produces local spike to axon " << memDestOut.read().dstAxon << endl;
		}
		else {
			FSMlocalNeuronAck = false;
			FSMlocalTx = false;
			FSMlocalAxonTx = true;
			//FSMlocalAddr = FSMlocalAddr;
			FSMstate = LOCAL;
		}
		break;
	}
	case STOP: {
		FSMlocalNeuronAck = false;
		FSMlocalAxonTx = false;
		FSMlocalTx = false;
		//FSMlocalAddr = FSMlocalAddr;
		FSMstate = IDLE;
		break;
	}
	}

	int FSMmemAddr = FSMoffset + FSMlocalAddr;
	memAddr.write(FSMmemAddr);

	neuronAck.write(FSMlocalNeuronAck);
	FSMtx.write(FSMlocalTx);
	//tx.write(FSMlocalTx);
	FSMsrAxonTx.write(FSMlocalAxonTx);
	//localAxonTx.write(FSMlocalAxonTx);
	localMemAddr.write(FSMlocalAddr);

	/*if (this->name()[10] == '1' && this->name()[14] == '1'){

	cout<< "******************"<<endl;
	cout<< sc_time_stamp() << " FSMlocalTx = " << FSMlocalTx << endl;
	cout<< sc_time_stamp() << " FSMlocalNeuronAck = " << FSMlocalNeuronAck << endl;
	cout<< sc_time_stamp() << " FSMlocalAxonTx = " << FSMlocalAxonTx << endl;
	cout<< sc_time_stamp() << " FSMlocalN = " << FSMlocalN << endl;
	cout<< sc_time_stamp() << " FSMoffset = " << FSMoffset << endl;
	cout<< sc_time_stamp() << " FSMlocalAddr = " << FSMlocalAddr << endl;
	cout<< sc_time_stamp() << " FSMstate = " << FSMstate << endl;

	cout<< sc_time_stamp() << " rx = " << rx.read() << endl;
	cout<< sc_time_stamp() << " FSMlocalNeuronAck = " << FSMlocalNeuronAck << endl;
	}*/
}
