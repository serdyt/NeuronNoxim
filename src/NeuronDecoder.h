/*
 * First table memOffset stores the amount of connected neurons and the offset to the weight table
 * Second table stores weight and corresponding destination
 *
 * Theoretically memories could be combined in one
 */

#include "NoximMain.h"
#include <string.h>
#include "NeuronMemory.h"
#include "NeuronShiftRegister.h"
#include "NoximStats.h"

using namespace std;

SC_MODULE(NeuronDecoder)
{
public:
  sc_in_clk clock;
  sc_in<int> axonIDin;
  sc_in<bool> ready;

  sc_out<int> destIDout;
  sc_out<float> weightOut;
  sc_out<bool> ack;

  NoximPower power;

  void initComponents(
		  vector<twoFieldMemStruct<int, int> > memOffset_,
		  vector<twoFieldMemStruct<int, float> > memWeight_);


  //constructor
  SC_HAS_PROCESS(NeuronDecoder);

  NeuronDecoder(sc_module_name name_);

private:

  sc_signal<twoFieldMemStruct<int,int> > memOffsetData;
  sc_signal<int> weightOffset;
  sc_signal<int> blockSize;
  sc_signal<bool> memOffsetWrite;

  sc_signal<twoFieldMemStruct<int,float> > memWeightData;
  sc_signal<float> weight;
  sc_signal<int> dest;
  sc_signal<bool> memWeightWrite;

  sc_signal<int> weightAddr;

  sc_signal<bool> srTxOut;

  ShiftRegister<bool, 1> txShiftReg;

  //first -> weightOffset; second -> blockSize;
  NeuronMemory<twoFieldMemStruct<int, int> > memOffset;

  //first -> destIDout; second -> weightOut;
  NeuronMemory<twoFieldMemStruct<int, float> > memWeight;

  enum {IDLE, START, INJECT, INJECTACK, STOP} FSMstate;
  int FSMblockSize;
  int FSMlocalAddr;
  int FSMoffset;
  bool FSMlocalAck;
  bool FSMlocalTx;

  sc_signal<bool> FSMtx;
  sc_signal<int> FSMaddr;

  void unpacker();
  void writeOut();
  void FSM();
  //void combineAddr();
  void GetMemOffsetOutput();
  void GetMemWeightOutput();
};
