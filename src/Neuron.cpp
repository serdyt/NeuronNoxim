//file: Neuron.cpp
//Main code of one neuron
#include "Neuron.h"
using namespace std;

Neuron::~Neuron(){
	//logFile.close();
}

float Neuron::getVolt(){
	return V_mem_tmp;
}

Neuron::Neuron(sc_module_name name_) :
	sc_module(name_)
		{
	V_reset_tmp = DEFAULT_RESET_VOLTAGE;
	V_th_tmp = DEFAULT_THRESHOLD_VOLTAGE;
	E_tmp = DEFAULT_E;
	tau_tmp = DEFAULT_TAU;
	Iex_tmp = DEFAULT_IEX;
	weight_tmp = DEFAULT_WEIGHT;
	V_mem_tmp = DEFAULT_RESET_VOLTAGE;
	reset_mem = true;

	string tileName = this->name();
	string neuronName = this->name();
	toSpikeLog += tileName.substr(tileName.find('['), tileName.find_last_of(']') - tileName.find('[') + 1);
	toSpikeLog += neuronName.substr(neuronName.find_last_of('_') + 1, neuronName.npos);

	int tileX = atoi(tileName.substr(tileName.find('[')+1, tileName.find(']') - tileName.find('[') ).c_str());
	int tileY = atoi(tileName.substr(tileName.find_last_of('[')+1, tileName.find_last_of(']') - tileName.find_last_of('[') ).c_str());
	int neuronID = atoi(neuronName.substr(neuronName.find('_') + 1, neuronName.npos).c_str());
	logNumber = (tileX + tileY * NoximGlobalParams::mesh_dim_x) * NoximGlobalParams::nConfig->maxNeurons + neuronID;

	SC_METHOD(leaky_integrator); //leaky integration
	sensitive << reset;
	sensitive << clock.pos();

	SC_METHOD(comparator); //compare and fire
	sensitive << reset;
	sensitive << clock.pos();
}

//If neuron receives reset signal from comparator, membrane potential is reset to V_reset, otherwise, keep integrating (and leaking)
void Neuron::leaky_integrator() {
	//while (true) {
	//	wait();
		if (reset.read()) {
			V_reset_tmp = DEFAULT_RESET_VOLTAGE;
			V_th_tmp = DEFAULT_THRESHOLD_VOLTAGE;
			E_tmp = DEFAULT_E;
			tau_tmp = DEFAULT_TAU;
			Iex_tmp = DEFAULT_IEX;
			weight_tmp = DEFAULT_WEIGHT;
			V_mem_tmp = V_reset;
		} else {
			if (reset_mem == true) {
				V_mem_tmp = V_reset;
				reset_mem = false;
			} else {
				V_reset_tmp = V_reset;
				V_th_tmp = V_th;
				E_tmp = E;
				tau_tmp = tau;
				Iex_tmp = Iex;
				weight_tmp = weight;
				if (weight.read() != 0) {
					power.Integration();
					if (NoximGlobalParams::consoleLogPolicy > 0){
						cout << sc_time_stamp() << " " << this->name()
							<< " got input " << weight_tmp <<"; Curr V = "<<V_mem_tmp<<" V_th = "<<V_th_tmp<< endl;
					}
				}
				//V_mem_tmp   = weight_tmp + Iex_tmp + (V_mem_tmp-E)/tau; //I+Iex+(V-E)/tau
				V_mem_tmp = step.read()*(-(V_mem_tmp - E_tmp) / tau_tmp + Iex_tmp) + V_mem_tmp + weight_tmp; //time-step*(V-E)/tau+V+I+Iex
			}
		}
	//}
}

//If V_mem>=V_th, comparator fires a spike and issues a reset signal to the leaky integrator 
void Neuron::comparator() {
	if (reset.read()) {
		fire_flag = false;
		reset_mem = false;
	} else {
		if (V_mem_tmp >= V_th) {
			fire_flag = true;
			reset_mem = true;
			//save to spike log
			fprintf(NoximGlobalParams::spikeLogFile, "%s\n", toSpikeLog.c_str());
			if (NoximGlobalParams::consoleLogPolicy > 1) {
				cout << sc_time_stamp() << " " << this->name() << " fires"
						<< endl;
			}
		} else {
			fire_flag = false;
			reset_mem = false;
		}
		power.Comparison();
	}
}
