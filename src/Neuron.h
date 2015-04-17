//file: Neuron.h
//Header of one neuron

#include "NoximMain.h"
#include "NoximStats.h"
#include <fstream>
using namespace std;

SC_MODULE(Neuron) {
  //I/O ports
  sc_in_clk    clock;   //Clock signal for the neuron
  sc_in<bool>  reset;   //reset signal for the neuron
  sc_in<float> V_reset; //reset voltage of the neuron
  sc_in<float> V_th;    //threshold voltage of the neuron
  sc_in<float> E;       
  sc_in<float> tau;     //leaking parameter
  sc_in<float> Iex;    //?
  sc_in<float> weight;  //input weight value
  sc_in<float> step;

  sc_out<bool> fire_flag; //indicate if the neuron will fire a spike or not

  NoximPower power;

  SC_HAS_PROCESS(Neuron);

  Neuron(sc_module_name _name);
  ~Neuron();

  float getVolt();

private:
	float V_mem_tmp; //internal membrane potential of the neuron
	bool reset_mem; //reset signal from comparator to leaky integrator
	//internal value of the parameters
    float V_reset_tmp, V_th_tmp;
    float E_tmp, tau_tmp, Iex_tmp; 
    float weight_tmp;

    int logNumber;

    string toSpikeLog;

    void leaky_integrator();
    void comparator();
};
