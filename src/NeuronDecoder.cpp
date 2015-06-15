//file: NeuronDecoder.cpp
//Main code of the arbiter module
#include "NeuronDecoder.h"

//TODO : add energy model!!!

void NeuronDecoder::FSM(){

	switch (FSMstate) {
		case IDLE: {
			if (ready.read()) {
				FSMlocalTx = false;
				FSMlocalAck = false;
				//FSMlocalAddr = FSMlocalAddr;
				FSMstate = START;
			}
			else{
				FSMlocalAck = false;
				FSMlocalTx = false;
				//FSMlocalAddr = FSMlocalAddr;
				FSMstate = IDLE;
			}
			break;
		}
		case START: {
			FSMblockSize = blockSize.read();
			if (FSMblockSize == 1){
				FSMlocalTx = true;
				FSMlocalAck = true;
				FSMlocalAddr = 0;
				FSMstate = STOP;
			}
			else{
				FSMlocalTx = true;
				FSMlocalAck = false;
				FSMlocalAddr = 0;
				FSMstate = INJECT;
			}
			break;
		}
		case INJECT: {
			FSMlocalAddr++;
//			if (FSMlocalAddr == FSMblockSize-2){
//				FSMlocalTx = true;
//				FSMlocalAck = true;
//				FSMlocalAddr++;
//				FSMstate = INJECTACK;
//			}
//			else
				if (FSMlocalAddr >= FSMblockSize-1){
				FSMlocalTx = true;
				FSMlocalAck = true;
				FSMstate = STOP;
				//FSMlocalAddr = FSMlocalAddr;
			}
			else {
				FSMlocalTx = true;
				FSMlocalAck = false;
				FSMstate = INJECT;
			}
			break;
		}
//		case INJECTACK: {
//			if(ready.read() != true){
//				FSMlocalTx = false;
//				FSMlocalAck = true;
//				FSMstate = IDLE;
//				FSMlocalAddr = 0;
//			}
//			else {
//				FSMlocalTx = true;
//				FSMlocalAck = true;
//				FSMstate = START;
//				FSMlocalAddr = 0;
//			}
//			break;
//		}
		case STOP: {
			FSMlocalTx = false;
			FSMlocalAck = false;
			FSMstate = IDLE;
			//FSMlocalAddr = 0;
			break;
		}
	}
	weightAddr.write(FSMoffset + FSMlocalAddr);
	FSMtx.write(FSMlocalTx);
	ack.write(FSMlocalAck);
	FSMaddr.write(FSMlocalAddr);

//	if (this->name()[10] == '0' && this->name()[14] == '0'){
//	cout<< "***********************"<<endl;
//	cout<< sc_time_stamp() << " FSMlocalAck = " << FSMlocalAck << endl;
//	cout<< sc_time_stamp() << " FSMlocalTx = " << FSMlocalTx << endl;
//	cout<< sc_time_stamp() << " FSMstate = " << FSMstate << endl;
//	cout<< sc_time_stamp() << " FSMlocalAddr = " << FSMlocalAddr << endl;
//	cout<< sc_time_stamp() << " FSMblockSize = " << FSMblockSize << endl;
//	}
}


//void NeuronDecoder::combineAddr(){
//	weightAddr.write(weightOffset.read() + FSMaddr.read());
//}

void NeuronDecoder::GetMemOffsetOutput(){
	weightOffset.write(memOffsetData.read().content.first);
	blockSize.write(memOffsetData.read().content.second);

	//cout<< sc_time_stamp() << " weightOffset = " << weightOffset << endl;
	//cout<< sc_time_stamp() << " blockSize = " << blockSize << endl;
}

NeuronDecoder::NeuronDecoder(sc_module_name name_)
	: sc_module(name_), memWeight("mem_weight"), memOffset("mem_ofset"),
	  txShiftReg("decoderTxShift")
  {

	memOffset.clock(clock);
	memOffset.address(axonIDin);
	memOffset.data(memOffsetData);
	memOffset.write(memOffsetWrite);
	memOffsetWrite.write(false); //make memory read only for now

	memWeight.clock(clock);
	memWeight.address(weightAddr);
	memWeight.data(memWeightData);
	memWeight.write(memWeightWrite);
	memWeightWrite.write(false); //read only memory

	txShiftReg.clock(clock);
	txShiftReg.input(FSMtx);
	txShiftReg.output(srTxOut);

	FSMlocalAck = false;
	FSMblockSize = 0;
	FSMlocalAddr = 0;
	FSMlocalTx = false;
	FSMstate = IDLE;

    SC_METHOD(GetMemOffsetOutput);
    sensitive<<memOffsetData;

    //SC_METHOD(combineAddr);
    //sensitive<<weightOffset<<FSMaddr;

    SC_METHOD(FSM);
    sensitive<<clock.pos();

    SC_METHOD(writeOut);
    sensitive<<srTxOut<<memWeightData;
  }

void NeuronDecoder::initComponents(
		vector<twoFieldMemStruct<int, int> > memOffset_,
		vector<twoFieldMemStruct<int, float> > memWeight_
		)
{
	memOffset.init(memOffset_);
	memWeight.init(memWeight_);
}

void NeuronDecoder::writeOut(){
	if (srTxOut.read()){

		destIDout.write(memWeightData.read().content.first);
		weightOut.write(memWeightData.read().content.second);
	}
	else{
		weightOut.write(0);
	}
	//cout<< sc_time_stamp() << " destIDout = " << memWeightData.read().content.first << endl;
	//cout<< sc_time_stamp() << " weightOut = " << memWeightData.read().content.second << endl;
	//cout<< sc_time_stamp() << " srTxOut = " << srTxOut << endl;
}
