/*
 * CAM.h
 *
 *  Created on: Dec 16, 2014
 *      Author: ser
 *
 *	simplified CAM
 *	for each record, it stores an output hit value
 *
 */

#include "systemc.h"
#include <map>

#ifndef SRC_CAM_H_
#define SRC_CAM_H_

using namespace std;

template <class T>
SC_MODULE(CAM)
{
public:
	sc_in_clk clock;
	sc_in<T> data;
	sc_out<int> hit;

	SC_CTOR(CAM){
		SC_METHOD(CAMproc);
		//set_stack_size(0x5000);
		sensitive<<clock.pos();
	}

	void init(map<T, int> b);

private:
	void CAMproc();
	map<T, int> buffer;
};


#endif /* SRC_CAM_H_ */

template <class T>
void CAM<T>::CAMproc(){
	//while(true){
	//	wait();
		//typename map<T, int>::iterator it = buffer.find(data.read());
		auto it = buffer.find(data.read());
		if (it != buffer.end()){
			hit.write(it->second);
		}
		else{
			hit.write(0);
		}
	//}
}

template <class T>
void CAM<T>::init(map<T, int> b){
	buffer = b; //Copies the map
}
