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
    char dom[255], ver[255];
    char scriptCommand[255];
    char barcode[255];
    unsigned short position;
    char scriptPath[255];
    char pythonPath[] = "/dls_sw/apps/python/anaconda/1.7.0/64/bin/python";

    unsigned short initialised = *(unsigned short *)prec->c;
    unsigned short writeEnable = *(unsigned short *)prec->e;
    unsigned short debugEnable = *(unsigned short *)prec->f;

    strcpy(ver,prec->g);

    if(strcmp(ver,"work") == 0){
        strcpy(scriptPath,"/dls_sw/work/R3.14.12.7/support/laserPuckPointer/laserPuckPointerApp/src/");
    }else{
        strcpy(scriptPath,"/dls_sw/prod/R3.14.12.7/support/laserPuckPointer/");
        strcat(scriptPath,ver);
        strcat(scriptPath,"/laserPuckPointerApp/src/");
    }  

    if(initialised > 0){
        strcpy(dom,prec->a);
        position = *(unsigned short*)prec->b;
        strcpy(barcode,prec->d);

        sprintf(scriptCommand,"%s %sassignPucks.py %s %s %d %sispyb.cfg",pythonPath,scriptPath,dom,barcode,position,scriptPath);
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
