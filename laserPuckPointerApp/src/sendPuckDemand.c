#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <dbEvent.h>
#include <dbDefs.h>
#include <dbCommon.h>
#include <aSubRecord.h>
#include <registryFunction.h>
#include <epicsExport.h>


/*
	Takes a decimal demand in "demand" in the range 0-64. This will represent the puck number.
	This number is then split into 4 so it can be used with 4xMBBI records, ie, pucks 1-16 are
	in bank01, pucks 17-32 are in bank02 etc. Where the input "demand" represents the puck number
	the value in bankXX must be a BCD representation of that puck with resepect to its bank (1-16),
	eg, demand = 3, bank01 = 4, demand = 17, bank02 = 1.  
*/

static long sendPuckDemand( aSubRecord *prec) {

	unsigned short demand = *(unsigned short *)prec->a;
	unsigned short bank01,bank02,bank03,bank04;
	bank01 = 0;
	bank02 = 0;
	bank03 = 0;
	bank04 = 0;
	//Write the result to VALA	
	

	if(demand >= 1 && demand <= 16){
		bank01 = pow(2,demand-1);
	}
	if(demand >= 17 && demand <= 32){
		bank02 = pow(2,demand-17);
	}
	if(demand >= 33 && demand <= 48){
		bank03 = pow(2,demand-33);
	}
	if(demand >= 49 && demand <= 64){
		bank04 = pow(2,demand-49);
	}

	*(unsigned short*)prec->vala = bank01;
	*(unsigned short*)prec->valb = bank02;
	*(unsigned short*)prec->valc = bank03;
	*(unsigned short*)prec->vald = bank04;
       return(0); /* process output links */
}

epicsRegisterFunction(sendPuckDemand);
