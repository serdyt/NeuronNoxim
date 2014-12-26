//file: Neuron.h
//Header of one neuron

#include "NoximMain.h"
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

  SC_CTOR(Neuron){
		V_reset_tmp = DEFAULT_RESET_VOLTAGE;
		V_th_tmp = DEFAULT_THRESHOLD_VOLTAGE;
		E_tmp = DEFAULT_E;
		tau_tmp = DEFAULT_TAU;
		Iex_tmp = DEFAULT_IEX;
		weight_tmp = DEFAULT_WEIGHT;
		V_mem_tmp = DEFAULT_RESET_VOLTAGE;
		reset_mem = true;

		char fname [128] = "./log/";
		char tileName [128];
		strncpy(tileName, &this->name()[4], 12);
		char neuronName [128];
		strcpy(neuronName, &this->name()[39]);
		strcat(fname, tileName);
		strcat(fname, neuronName);
		strcat(fname, ".log");
		logFile.open(fname, fstream::out);

		SC_THREAD(log);
		sensitive<<clock.pos();

		SC_THREAD(leaky_integrator); //leaky integration
		sensitive << clock.pos();

		SC_METHOD(comparator); //compare and fire
		sensitive << reset;
		sensitive << clock.pos();
	}
  ~Neuron();

private:
	float V_mem_tmp; //internal membrane potential of the neuron
	bool reset_mem; //reset signal from comparator to leaky integrator
	//internal value of the parameters
    float V_reset_tmp, V_th_tmp;
    float E_tmp, tau_tmp, Iex_tmp; 
    float weight_tmp;
    ofstream logFile;

    void leaky_integrator();
    void comparator();
    void log();
};
