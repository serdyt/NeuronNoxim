/*
 * sndCAMstruct.h
 *
 *  Created on: Dec 18, 2014
 *      Author: ser
 */

#ifndef SRC_SNDCAMSTRUCT_H_
#define SRC_SNDCAMSTRUCT_H_

#include "systemc.h"

struct sndCAMstruct {
	unsigned int srcID;
	unsigned int srcNeurID;
	unsigned int dstNeurID;

	sndCAMstruct(){};

	sndCAMstruct(int srcID_, int srcNeurID_, int dstNeurID_)
		: srcID(srcID_), srcNeurID(srcNeurID_), dstNeurID(dstNeurID_)
	{
	}

	inline bool operator ==(const sndCAMstruct & rec) const {
		return (rec.srcID == srcID && rec.srcNeurID == srcNeurID
				&& rec.dstNeurID == dstNeurID);
	}

	inline bool operator >(const sndCAMstruct & rec) const {
		if (srcID > rec.srcID) return true;
		else if (srcID < rec.srcID) return false;
		else if (srcNeurID > rec.srcNeurID) return true;
		else if (srcNeurID < rec.srcNeurID) return false;
		else if (dstNeurID > rec.dstNeurID) return true;
		else if (dstNeurID < rec.dstNeurID) return false;
		else return false;

		//unsigned long long int a = srcID<<48 + srcNeurID<<32 + dstNeurID;
		//unsigned long long int b = rec.srcID<<48 + rec.srcNeurID<<32 + rec.dstNeurID;
		//return (a>b);
	}

	inline bool operator <(const sndCAMstruct & rec) const {
		if (srcID < rec.srcID) return true;
		else if (srcID > rec.srcID) return false;
		else if (srcNeurID < rec.srcNeurID) return true;
		else if (srcNeurID > rec.srcNeurID) return false;
		else if (dstNeurID < rec.dstNeurID) return true;
		else if (dstNeurID > rec.dstNeurID) return false;
		else return false;

		//unsigned long long int a = srcID<<48 + srcNeurID<<32 + dstNeurID;
		//unsigned long long int b = rec.srcID<<48 + rec.srcNeurID<<32 + rec.dstNeurID;
		//return (a<b);
	}

	inline sndCAMstruct& operator =(const sndCAMstruct& rec) {
		srcID = rec.srcID;
		srcNeurID = rec.srcNeurID;
		dstNeurID = rec.dstNeurID;
		return *this;
	}

	inline friend void sc_trace(sc_trace_file *tf, const sndCAMstruct & rec,
			const std::string & NAME) {
		sc_trace(tf, rec.srcID, NAME + "srcID");
		sc_trace(tf, rec.srcNeurID, NAME + "srcNeurID");
		sc_trace(tf, rec.dstNeurID, NAME + "dstNeurID");
	}

	inline friend ostream& operator <<(ostream& os, sndCAMstruct const & rec) {
		os << "(" << rec.srcID << "," << rec.srcNeurID << "," << rec.dstNeurID << ")";
		return os;
	}

};


#endif /* SRC_SNDCAMSTRUCT_H_ */
