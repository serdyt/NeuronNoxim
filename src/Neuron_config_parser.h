/*
 * Neuron_config_parser.h
 *
 *  Created on: Nov 26, 2014
 *      Author: ser
 */

#ifndef SRC_NEURON_CONFIG_PARSER_H_
#define SRC_NEURON_CONFIG_PARSER_H_

#include "NoximMain.h"

NeuronConfig * NeuronConfigParser(const char fname[]);

struct axonTriplet{
	int srcClusterID;
	int srcNeuronID;
	int dstNeuronID;

	axonTriplet() : srcClusterID(0), srcNeuronID(0), dstNeuronID(0)
	{};

	axonTriplet(int _srcClusterID, int _srcNeuronID, int _dstNeuronID) :
		srcClusterID(_srcClusterID), srcNeuronID(_srcNeuronID), dstNeuronID(_dstNeuronID)
		{};

	bool operator < (axonTriplet t){
			if (this->srcClusterID < t.srcClusterID) {
				return true;
			}
			else if (this->srcClusterID > t.srcClusterID) {
				return false;
			}
			else {
				if (this->srcNeuronID < t.srcNeuronID){
					return true;
				}
				else {
					return false;
				}
			}
		}

		inline axonTriplet& operator= (axonTriplet t){
			this->srcClusterID = t.srcClusterID;
			this->srcNeuronID = t.srcNeuronID;
			this->dstNeuronID = t.dstNeuronID;
			return *this;
		}
};


#endif /* SRC_NEURON_CONFIG_PARSER_H_ */
