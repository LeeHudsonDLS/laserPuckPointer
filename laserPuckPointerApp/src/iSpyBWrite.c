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

    For this to work the following support modules need to be at least
    the following versions:

    mitsubishiRobot     2-6
    FQCRBarcodeCamera   1-2
    
*/
static long isbWrite( aSubRecord *prec) {
    char dom[255];
    char scriptCommand[255];
    char barcode[255];
    unsigned short position;
    char scriptPath[] = "/dls_sw/apps/ispyb-api/ispyb/assignPucks.py";
    char pythonPath[] = "/dls_sw/apps/python/anaconda/1.7.0/64/bin/python";

    unsigned short initialised = *(unsigned short *)prec->c;
    unsigned short writeEnable = *(unsigned short *)prec->f;
    unsigned short debugEnable = *(unsigned short *)prec->g;
    if(initialised > 0){
        strcpy(dom,prec->a);
        position = *(unsigned short*)prec->b;
        strcpy(barcode,prec->d);

        sprintf(scriptCommand,"%s %s %s %s %d",pythonPath,scriptPath,dom,barcode,position);
        if(debugEnable == 1){
            if(writeEnable == 1){
                printf("Ran: %s\n",scriptCommand);
            }else{
                printf("Would have ran (write currently disabled): %s\n",scriptCommand);
            }           
            printf("Write enabled : %d\n",writeEnable);
        }
        if(writeEnable == 1){
            system(scriptCommand);
        }
    }
    *(unsigned short*)prec->vala = 1;
    return(0);
}



epicsRegisterFunction(isbWrite);
