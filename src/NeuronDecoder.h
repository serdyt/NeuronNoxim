/*
 *	When tx signal is set, decoder processes packetIn
 *	and output corresponding weight in 3 clock cycles
 *
 *
 *	unpacker forms correct CAM input and pushes destID to shiftRegister
 *
 *	CAM searches weight table address for current source
 *	weight Memory is set to read always
 */

#include "NoximMain.h"
#include "string.h"
#include "CAM.h"
#include "NeuronMemory.h"
#include "NeuronShiftRegister.h"
#include "NoximStats.h"

using namespace std;

SC_MODULE(NeuronDecoder)
{
public:
  sc_in_clk clock;
  sc_in<NoximPacket> packetIn;
  sc_in<bool> rx;

  sc_out<int> destIDout;
  sc_out<float> weightOut;

  NoximPower power;

  void unpacker();
  void initComponents(map<sndCAMstruct, int > cam_, vector<float> mem_);
  void writeOut();

  //constructor
  SC_HAS_PROCESS(NeuronDecoder);

  NeuronDecoder(sc_module_name name_);

private:
  sc_signal<sndCAMstruct> CAMin;
  sc_signal<int> weightAddr;

  sc_signal<bool> writeMem;

  sc_signal<int> srDestOut;
  sc_signal<bool> srTxOut;
  sc_signal<float> memWeightOut;

  sc_signal<int> destIDint;
  ShiftRegister<int, 2> destShiftReg;
  ShiftRegister<bool, 3> txShiftReg;

  CAM<sndCAMstruct> cam;
  NeuronMemory<float> mem;

};
