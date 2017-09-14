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
    Gets the last barcode from the barcode camera (INPD), the
    position of that last barcode (INPB), the beamline domain (INPA)
    and runs a python script with those arguments to write the puck
    to the ISpyB Database.  
*/
static long isbWrite( aSubRecord *prec) {
    char dom[255];
    char scriptCommand[255];
    char barcode[255];
    unsigned short position;
    char scriptPath[] = "/dls_sw/apps/ispyb-api/ispyb/assignPucks.py";
    char pythonPath[] = "/dls_sw/apps/python/anaconda/1.7.0/64/bin/python";

    unsigned short initialised = *(unsigned short *)prec->c;
    if(initialised > 0){
        strcpy(dom,prec->a);
        position = *(unsigned short*)prec->b;
        strcpy(barcode,prec->d);

        sprintf(scriptCommand,"%s %s %s %s %d",pythonPath,scriptPath,dom,barcode,position);
        system(scriptCommand);
        //printf("Will run: %s\n",scriptCommand);
    }
    *(unsigned short*)prec->vala = 1;
    return(0);
}



epicsRegisterFunction(isbWrite);
