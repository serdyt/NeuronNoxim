//file: NeuronArbiter.cpp
//Main code of the arbiter module
#include "NeuronArbiter.h"

void NeuronArbiter::roundRobin() {
	if (ack.read()) {
		currTx = false;
		sendMutex.unlock();
	}
	for (int i = currNeuron + 1; i < size; i++) {
		if (counters[i] > 0) {
			if (sendMutex.trylock(i) != -1) {
				currTx = true;
				currNeuron = i;
				counters[i]--;
			}
		}
	}
	for (int i = 0; i < currNeuron + 1; i++) {
		if (counters[i] > 0) {
			if (sendMutex.trylock(i) != -1) {
				currTx = true;
				currNeuron = i;
				counters[i]--;
			}
		}
	}
	senderID.write(currNeuron);
	tx.write(currTx);
	power.NArbitration();
}

NeuronArbiter::NeuronArbiter(sc_module_name name_, int size_)
	: sc_module(name_)
{
	size = size_;
	fire_flag.init(size_);
	counters.resize(size, 0);
	currNeuron = 0;
	currTx = false;
	sendMutex.unlock();
    //inventories: react to fire flags from neurons
    SC_METHOD(inv);
    sensitive<<clock.pos();

    SC_METHOD(roundRobin);
    sensitive<<clock.pos();
  }

void NeuronArbiter::inv() {
		for (int i = 0; i < size; i++){
			if (fire_flag[i]->read()){
				counters[i]++;
			}
		}
}
