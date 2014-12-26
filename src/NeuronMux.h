/*
 * NeuronMux.h
 *
 *  Created on: Dec 22, 2014
 *      Author: ser
 */

#ifndef SRC_NEURONMUX_H_
#define SRC_NEURONMUX_H_

#include "systemc.h"

template <class T>
SC_MODULE(NeuronMux){
	sc_in<T> dataIn;
	sc_in<int> control;
	sc_vector<sc_out<T> > dataOut;

	SC_HAS_PROCESS(NeuronMux);
	NeuronMux(sc_module_name name_, int size_ = 4)
		: sc_module(name_), dataOut("muxDataOut")
	{
		dataOut.init(size_);
		size = size_;

		SC_METHOD(proc);
		sensitive<<dataIn<<control;
	}

private:
	int size;

	void proc(){
		for_each(dataOut.begin(), dataOut.end(), [](sc_out<T>& a ) {a.write(0);});
		if (control.read() < size){
			//cout<<sc_time_stamp()<<" mux recieved "<<dataIn.read()<<" for "<<control.read()<<endl;
			dataOut[control.read()]->write(dataIn.read());
		}
		else{
			cout<< "ERROR: MUX control signal ("<< control.read() <<") larger than its size (" << size << ")" <<endl;
		}
	}
};

#endif /* SRC_NEURONMUX_H_ */
