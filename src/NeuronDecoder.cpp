//file: NeuronDecoder.cpp
//Main code of the arbiter module
#include "NeuronDecoder.h"

//reads dest_neur_ID for multiplexor and calculates synapseID for CAM
void NeuronDecoder::unpacker() {
	if (rx.read()) {
		power.Decoding();
		NoximPacket pack = packetIn.read();
		//if (pack.dst_neur_id <= MAX_NEURON_IN_PE) {
			//cout<<sc_time_stamp()<<" Decoder CAM input "<<sndCAMstruct(pack.src_id, pack.src_neur_id, pack.dst_neur_id)<<endl;
			CAMin.write(sndCAMstruct(pack.src_id, pack.src_neur_id, pack.dst_neur_id));
			destIDint.write(pack.dst_neur_id);
		//} else {
		//	cout << "Received a packet with dst_neur_id >= MAX_NEURON_IN_PE in "
		//			<< pack.dst_id;
		//	assert(0);
		//}
	}
}

NeuronDecoder::NeuronDecoder(sc_module_name name_)
	: sc_module(name_), destShiftReg("shiftReg"), cam("CAM"), mem("Mem"), txShiftReg("decoderOutputs")
  {

	destShiftReg.input(destIDint);
	destShiftReg.clock(clock);
	destShiftReg.output(srDestOut);

	txShiftReg.input(rx);
	txShiftReg.clock(clock);
	txShiftReg.output(srTxOut);

	cam.clock(clock);
	cam.data(CAMin);
	cam.hit(weightAddr);

	mem.clock(clock);
	mem.address(weightAddr);
	mem.data(memWeightOut);
	mem.write(writeMem);

	writeMem.write(false); //make memory read only for now

    SC_METHOD(unpacker);
    sensitive<<rx<<packetIn;

    SC_METHOD(writeOut);
    sensitive<<srTxOut<<srDestOut<<memWeightOut;
  }

void NeuronDecoder::initComponents(map<sndCAMstruct, int > cam_, vector<float> mem_){
	cam.init(cam_);
	mem.init(mem_);
}

void NeuronDecoder::writeOut(){
	if (srTxOut.read()){
		destIDout.write(srDestOut.read());
		weightOut.write(memWeightOut.read());
	}
	else{
		weightOut.write(0);
	}
}
