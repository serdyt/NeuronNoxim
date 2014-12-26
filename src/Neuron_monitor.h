//file Neuron_monitor.h
//To monitor neuron activities
#include <systemc.h>
#include "NoximMain.h"
using namespace std;

SC_MODULE(Neuron_monitor) {
  sc_in<float> V_mem;
  sc_in<bool> fire_flag;
  sc_in<bool> fire_ack;

  void Neuron_info() {
    cout << "At " << sc_time_stamp().to_double()/1000 
	 << ", membrane potential is: " << V_mem 
	 << ", fire_flag is: " << fire_flag
	 << ", fire_ack is: " << fire_ack << endl;
  }

  SC_CTOR(Neuron_monitor) {
    SC_METHOD(Neuron_info);
    sensitive << V_mem << fire_flag << fire_ack; //whenever V_mem change or fire_flag change
  }
};
