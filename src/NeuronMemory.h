/*
 * Memory.h
 *
 *  Created on: Dec 16, 2014
 *      Author: ser
 *
 *  Memory size should be initialized or a complete memory block should be provided to make a copy
 *
 */

#include <systemc.h>
#include <vector>

#ifndef SRC_MEMORY_H_
#define SRC_MEMORY_H_

using namespace std;

template <class T>
SC_MODULE(NeuronMemory)
{
public:
	sc_in_clk clock;
	sc_in<int> address;
	sc_in<bool> write; //read = 0; write = 1;
	sc_out<T> data;

	void init(int size);
	void init(vector<T> buf);
	void memoryProc();

	SC_CTOR(NeuronMemory){

		SC_THREAD(memoryProc);
		sensitive << clock.pos();
	}

private:
	vector<T> buffer;
};

template <class T>
void NeuronMemory<T>::init(int size){
	buffer.resize(size);
}

template <class T>
void NeuronMemory<T>::init(vector<T> buf){
	buffer = buf;
}

template <class T>
void NeuronMemory<T>::memoryProc() {
	while (true) {
		wait();
		if (address.read() > buffer.size()){
			cout<< "Wow, wow, wrong input address " << address.read() <<" to the memory"<<endl;
			assert(0);
		}
		//cout<< sc_time_stamp() << " Memory received " << address.read() << " output " << buffer[address.read()] <<endl;
		if (write) {
			buffer[address.read()] = data;
		} else {
			data.write(buffer[address.read()]);
		}
	}
}

#endif /* SRC_MEMORY_H_ */
