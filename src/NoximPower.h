/*
 * Noxim - the NoC Simulator
 *
 * (C) 2005-2010 by the University of Catania
 * For the complete list of authors refer to file ../doc/AUTHORS.txt
 * For the license applied to these sources refer to file ../doc/LICENSE.txt
 *
 * This file contains the declaration of the power model
 */

#ifndef __NOXIMPOWER_H__
#define __NOXIMPOWER_H__

#include <cassert>
#include "NoximMain.h"
using namespace std;

/*

The average energy dissipated by a flit for a hop switch was estimated
as being 0.151nJ, 0.178nJ, 0.182nJ and 0.189nJ for XY, Odd-Even, DyAD,
and NoP-OE respectively

We assumed the tile size to be 2mm x 2mm and that the tiles were
arranged in a regular fashion on the floorplan. The load wire
capacitance was set to 0.50fF per micron, so considering an average of
25% switching activity the amount of energy consumed by a flit for a
hop interconnect is 0.384nJ.



#define PWR_ROUTING_XY             0.151e-9
#define PWR_ROUTING_WEST_FIRST     0.155e-9
#define PWR_ROUTING_NORTH_LAST     0.155e-9
#define PWR_ROUTING_NEGATIVE_FIRST 0.155e-9
#define PWR_ROUTING_ODD_EVEN       0.178e-9
#define PWR_ROUTING_DYAD           0.182e-9
#define PWR_ROUTING_FULLY_ADAPTIVE 0.0
#define PWR_ROUTING_TABLE_BASED    0.185e-9

#define PWR_SEL_RANDOM             0.002e-9
#define PWR_SEL_BUFFER_LEVEL       0.006e-9
#define PWR_SEL_NOP                0.012e-9

#define PWR_FORWARD_FLIT           0.384e-9
#define PWR_INCOMING               0.002e-9
#define PWR_STANDBY                0.0001e-9/2.0
*/

class NoximPower {

  public:

    NoximPower();

    void Buffering();
    void Routing();
    void Selection();
    void Arbitration();
    void Crossbar();
    void Link(bool low_voltage);
    void EndToEnd();
    void Leakage();

    void Decoding();
    void Integration();
    void Comparison();
    void NArbitration();
    void Encoding();

    bool LoadPowerData(const char *fname);

    double getPower() {
	return pwr;
    } 

    double getPwrRouting() {
	return pwr_routing;
    }
    
    double getPwrSelection() {
	return pwr_selection;
    }
    
    double getPwrBuffering() {
	return pwr_buffering;
    }
    
    double getArbitration() {
	return pwr_arbitration;
    }

    double getCrossbar() {
	return pwr_crossbar;
    }

    double getLeakage() {
	return pwr_leakage;
    }

    double getPwrLink(bool low_voltage) {
      if (low_voltage)
	return pwr_link_lv;
      else
	return pwr_link;
    }

    double getPwrEndToEnd() {
      return pwr_end2end;
    }

  private:
    
    static double pwr_buffering;
    static double pwr_routing;
    static double pwr_selection;
    static double pwr_arbitration;
    static double pwr_crossbar;
    static double pwr_link;
    static double pwr_link_lv;
    static double pwr_leakage;
    static double pwr_end2end;
    //neurons
    static double pwr_decoding; //memory + mux + sr
    static double pwr_integration;
    static double pwr_comparison;
    static double pwr_encoding; //memory + sr + FSM
    static double pwr_nArbitration; // ???

    static bool   power_data_loaded;

    double pwr;
};

#endif
