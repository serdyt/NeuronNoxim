#include "Neuron_config_parser.h"
#include <fstream>
#include <cassert>
#include <cstdlib>
#include <iostream>
//#include <regex>

using namespace std;

//TODO : add checks for valid values

NeuronConfig * NeuronConfigParser(const char fname[]) {
	ifstream infile(fname);
	const string delimiter = ",";
	const string neuronNew = "#";
	const string neuronDest = ">";
	const string globalParam = "@";
	const string comment = "%";
	const string optParamDelimiter = "=";
	size_t pos = 0;
	string token;
	string line;
	enum recordType {
		NNEW, NDEST, GPARAM
	};
	recordType type;
	int lineNum = 0;
	int curSrcNeuron = 0;
	int curSrcCluster = 0;
	int clusterID;
	int neuronID;

	vector<int> currentAxon;

	string errLog;

	NeuronConfig * nConfig = new NeuronConfig();

	nConfig->step = DEFAULT_STEP;

	nConfig->srcToDst.resize(NoximGlobalParams::mesh_dim_x * NoximGlobalParams::mesh_dim_y);
	nConfig->dstToSrc.resize(NoximGlobalParams::mesh_dim_x * NoximGlobalParams::mesh_dim_y);
	nConfig->parameters.resize(NoximGlobalParams::mesh_dim_x * NoximGlobalParams::mesh_dim_y);

	// -1 where we have no axons, as I do currentAxon++ without checks
	currentAxon.resize(NoximGlobalParams::mesh_dim_x * NoximGlobalParams::mesh_dim_y, -1);

	if (!infile){
		cout<<"No file"<<endl;
		assert(0);
	}

	//TODO : rewrite with regexp maybe? or ... that string function dividing line into subsequences by delimiter...
	// hm, no, regexp won't work on the server O_o

	while (getline(infile, line)) {
		lineNum++;

		//erase everything behind the comment
		pos = line.find(comment);
		if (pos != string::npos) {
			line.erase(pos, line.length());
		}

		//skip empty lines
		if (line.length() == 0) {
			continue;
		}

		//check first symbol to define type of the line; stop if not neuron related
		if (!line.substr(0, 1).compare(neuronNew)) {
			type = NNEW;
		} else if (!line.substr(0, 1).compare(neuronDest)) {
			type = NDEST;
		} else if (!line.substr(0, 1).compare(globalParam)){
			type = GPARAM;
		}
		else{
			cout<<"Line " << lineNum << "is not neuron related";
			assert(0);
		}
		line.erase(0, 1);

		if (type == NNEW || type == NDEST) {
			//in any case, there is a pair of 2 integers first "clusterID,neuronID"
			pos = line.find(delimiter);
			if (pos != string::npos) {
				//read the first as current cluster integer and erase
				clusterID = atoi(line.substr(0, pos).c_str()); //does not have a check for valid integers
				line.erase(0, pos + delimiter.length());

				//check whether something left
				if (line.length() == 0) {
					cout << "Line " << lineNum << " no neuronID";
					assert(0);
				}

				pos = line.find(delimiter);
				//neuronID is either till next delimiter or end of a string
				if (pos != string::npos) {
					neuronID = atoi(line.substr(0, pos).c_str());
					line.erase(0, pos + delimiter.length());
				} else {
					neuronID = atoi(line.substr(0, line.length()).c_str());
					line.erase();
				}
			} else {
				cout << "No separators ',' in line " << lineNum;
				assert(0);
			}
		} else if (type == GPARAM){
			//divide a string into vector of substring divided by delimiter ","
			//check each substring to be one of parameters

			istringstream ss(line);
			vector<string> params;
			while (ss) {
				string s;
				if (!getline(ss, s, ','))
					break;
				params.push_back(s);
			}

			//regex floatValue("[-+]?([0-9]*\.[0-9]+)");

			for (auto param : params) {
				istringstream ss(param);
				string s;
				if (getline(ss, s, '=')) {
					if (s == "step") {
						getline(ss, s);
						nConfig->step = atof(s.c_str());
						/*
						//regexp do not work on the server =]
						smatch m;
						if (regex_search(s, m, floatValue)) {
							if (m.size() != 1){
								cout << "Hm, wrong format in " << m.str() << " taking first match " << m[0].str() << endl;
							}
							nConfig->step = atof(m[0].str().c_str());
						}
						*/
					}
				}
			}

			/*
			regex step("step=");
			regex floatValue("[-+]?([0-9]*\.[0-9]+|[0-9]+)");
			bool err = false;
			for (auto x : params){
				if (regex_search(x, m, step)){
					if (regex_search(m.suffix().str(), m, floatValue)){
						nConfig->step = atof(m.str().c_str());
					}
					else{
						cout<<"Hm, wrong format in "<<x<<endl;
					}
				}
				else {
					cout<<"Unknown parameter, or wrong format in "<<x<<endl;
				}
			}*/

		}

		//check that IDs are in the range
//		if (clusterID >= NoximGlobalParams::mesh_dim_x * NoximGlobalParams::mesh_dim_y || neuronID >= NoximGlobalParams::max_neuron_per_cluster){
//			cout<<"Cluster or Neuron ID are out of NOXIM range at line "<< lineNum<<endl;
//		}

		//add new neuron
		if (type == NNEW) {
			curSrcCluster = clusterID;
			curSrcNeuron = neuronID;
			//next we should read optional neuron parameters
			while (line.size() != 0) {
				//take one parameter "name=value"
				pos = line.find(delimiter);

				//while we are in the middle of parameters line
				if ( pos != string::npos){
					token = line.substr(0, pos);
					line.erase(0, pos + delimiter.length());
				}
				else{
					//when we reach the final parameter
					token = line;
					line.erase();
				}
				pos = token.find(optParamDelimiter);
				if (pos != string::npos){
					string pName = token.substr(0, pos);
					float pValue = atof(token.substr(pos + optParamDelimiter.length(), token.length()).c_str());
					nConfig->parameters[curSrcCluster][curSrcNeuron].insert(pair<string, float>(pName, pValue));
				}
				else{
					cout<<"Invalid parameters format, missing '=' in line "<<lineNum<<endl;
					assert(0);
				}

			}
		}
		//add a new destination point for a neuron
		else if (type == NDEST) {

			if (line.size() != 0){
				//no check for valid value =/
				float weight = atof(line.c_str());

				if (nConfig->srcToDst[curSrcCluster][curSrcNeuron].find(clusterID) != nConfig->srcToDst[curSrcCluster][curSrcNeuron].end()){
					nConfig->srcToDst[curSrcCluster][curSrcNeuron][clusterID].blockSize++;
					nConfig->srcToDst[curSrcCluster][curSrcNeuron][clusterID].data.push_back(pair<int, float>(neuronID, weight));
				}
				else{
					currentAxon[clusterID]++;
					nConfig->srcToDst[curSrcCluster][curSrcNeuron][clusterID].axonID = currentAxon[clusterID];
					nConfig->srcToDst[curSrcCluster][curSrcNeuron][clusterID].blockSize = 1;
					nConfig->srcToDst[curSrcCluster][curSrcNeuron][clusterID].data.push_back(pair<int, float>(neuronID, weight));

				}

				nConfig->dstToSrc[clusterID][currentAxon[clusterID]].blockSize++;
				nConfig->dstToSrc[clusterID][currentAxon[clusterID]].data.push_back(pair<int, float>(neuronID, weight)) ;
			}
			else{
				cout<<"Connection should have a weight. Line "<<lineNum<<endl;
				assert(0);
			}

		}
	}

	nConfig->maxDestPerNeuron = 0;
	for (auto cit : nConfig->srcToDst){
		int offsetRecords = 0;
		for (auto nit : cit){
			for (auto vit : nit.second){
				nConfig->maxDestPerNeuron = max(nConfig->maxDestPerNeuron, vit.second.blockSize);
			}
			offsetRecords += nit.second.size();
		}
		nConfig->maxEncoderOffsetRecords = max(nConfig->maxEncoderOffsetRecords, offsetRecords);
	}



	nConfig->maxSourcesPerNeuron = 0;
	for (auto cit : nConfig->dstToSrc) {
		int offsetRecords = 0;
		for (auto nit : cit) {
			nConfig->maxSourcesPerNeuron = max(nConfig->maxSourcesPerNeuron, nit.second.blockSize);
			offsetRecords += nit.second.blockSize;
		}
		nConfig->maxDecoderOffsetRecords = max(nConfig->maxDecoderOffsetRecords, offsetRecords);
	}

	nConfig->maxNeurons = 0;
	for (auto cit: nConfig->parameters){
		nConfig->maxNeurons = max(nConfig->maxNeurons, (int)cit.size());
	}

	return nConfig;
}

