/*
 * Neuron_config.h
 *
 *  Created on: Nov 27, 2014
 *      Author: ser
 *
 *  The structure to store data from neuron config file
 */
#include <vector>
#include <map>
#include <string>

#define DEFAULT_STEP 0.1

using namespace std;
struct triplet{
	int first;
	int second;
	float third;

	triplet(): first(0), second(0), third(0){
	}

	triplet(int _first, int _second, float _third):
		first(_first), second(_second), third(_third){
	}
};

struct NeuronConfig{
/*
 *	Destinations is a global memory for decoder.
 *	It stores for each [clusterID][NeuronID] a vector of [dstCluster, dstNeuron, weight]
 *	vector<vector<> > is the [clusterID][NeuronID]
 *	triplet<int dstClust, int dstNeur, int weight> is the destination cluster, neuron and weight
 */
	vector<vector<vector<triplet > > > destinations;

	//sources is opposite to destinations; it is actually global memory space in encoder
	vector<vector<vector<triplet > > > sources;

	//map<string,float> is the map of parameters, like pair("V",20)
	vector<vector<map<string,float> > > parameters;

	int maxNeurons;
	int maxDest;
	int maxSources;
	float step;
};


//TODO : replace all that with auto
typedef vector<vector<vector<triplet > > >::iterator connectIteratorClust;
typedef vector<vector<triplet > >::iterator connectIteratorNeur;
typedef vector<triplet >::iterator connectIteratorDest;

typedef vector<vector<map<string,float> > >::iterator paramIteratorClust;
typedef vector<map<string,float> >::iterator paramIteratorNeur;
typedef map<string,float>::iterator paramIteratorParam;

