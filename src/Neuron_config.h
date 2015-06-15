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

using namespace std;
struct triplet{
	int clusterID;
	int neuronID;
	float weight;

	triplet(): clusterID(0), neuronID(0), weight(0){
	}

	triplet(int _clusterID, int _neuronID, float _weight):
		clusterID(_clusterID), neuronID(_neuronID), weight(_weight){
	}

	bool operator < (triplet t){
		if (this->clusterID < t.clusterID) {
			return true;
		}
		else if (this->clusterID > t.clusterID) {
			return false;
		}
		else {
			if (this->neuronID < t.neuronID){
				return true;
			}
			else {
				return false;
			}
		}
	}

	inline triplet& operator= (triplet t){
		this->clusterID = t.clusterID;
		this->neuronID = t.neuronID;
		this->weight = t.weight;
		return *this;
	}
};

struct srcAxon{
	int axonID;
	int blockSize;
	//pair<dstNeuron, weight>
	vector <pair<int, float> > data;
};

struct dstAxon{
	int blockSize;
	//map<int, map<int, pair<int,float> > > weights;
	vector<pair<int,float> > data;
};

struct NeuronConfig{
/*
 *	Destinations is a global memory for decoder.
 *	It stores for each [clusterID][NeuronID] a vector of [dstCluster, dstNeuron, weight]
 *	vector<vector<> > is the [clusterID][NeuronID]
 *	triplet<int dstClust, int dstNeur, int weight> is the destination cluster, neuron and weight
 */

/* all this thing should be removed. It is too heavy
 *
	vector<vector<vector<triplet > > > destinations;

	//sources is opposite to destinations; it is actually global memory space in encoder
	//[dstID][dstNeuronID]
	//triplet<int srcClust, int srcNeuron, int srcWeight>
	vector<vector<vector<triplet > > > sources;



	//this structure stores offsets for input axons (for decoder)
	//[dstCluster][dstNeuron][AxonID] -> offset
	vector<vector<vector<int> > > offsets;

	//transfers destination neuron to AxonID (for encoder)
	//[dstCluster][dstNeuron][srcCluster][srcNeuron]->axonID
	vector<vector<vector<vector<int> > > > axons;

*/

/*
 * New version
 */
	//[srcCluster][srcNeuron][dstCluster] -> axon, blockSize, vector<dstNeuron, weight>
	//[srcCluster].map<srcNeuron, map<dstCluster, dstAxon{blockSize, [srcCluster].pair<dstNeuron, weight>}> >
	vector<map<int, map<int, srcAxon> > >  srcToDst;

	//[dstCluster][axon] -> blockSize, [srcCluster][srcNeuron]weight
	vector<map< int, dstAxon > > dstToSrc;

	//map<string,float> is the map of parameters, like pair("V",20)
	vector<map< int, map<string,float> > > parameters;

	float step;

	//the following parameters are just for statistics
	int maxNeurons;
	int maxDestPerNeuron;
	int maxSourcesPerNeuron;
	int maxDstAxons;
	int maxEncoderOffsetRecords;
	int maxDecoderOffsetRecords;
};
