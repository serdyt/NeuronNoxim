//file: NeuronArbiter.h
//Header of the arbiter module
//Input: multiple fire flags from neurons
//Output: one senderID at a time

// TODO: to rewrite this block
#include "systemc.h"

using namespace std;

// free when lock == -1
// this is sucks, but I am too lazy to modify it to normal sc_mutex
struct mutex{
	int lock;

	mutex(){
		lock = -1;
	}

	int trylock(int proc){
		if (lock == -1){
			lock = proc;
			return proc;
		}
		return -1;
	}

	void unlock(){
		lock = -1;
	}
};

SC_MODULE(NeuronArbiter)
{
  //I/O ports
  sc_in_clk   clock;
  sc_vector<sc_in<bool> > fire_flag;
  sc_out<int> senderID;
  sc_in<bool> ack;
  sc_out<bool> tx;

  SC_HAS_PROCESS(NeuronArbiter);

  NeuronArbiter(sc_module_name name_, int size_);

private:
	mutex sendMutex; //mutual exclusion lock
	int size;
	vector<int> counters;
	int currNeuron;
	bool currTx;

	//inventories: receive and store spikes from neurons
	void inv();
	void roundRobin();
};
