TOP=../..

include $(TOP)/configure/CONFIG

LIBRARY_IOC = laserPuckPointer

# laserPuckPointer.dbd will be installed into <top>/dbd
DBD += laserPuckPointer.dbd


# laserPuckPointer.dbd will be created from these files
laserPuckPointer_DBD += base.dbd
laserPuckPointer_DBD += functions.dbd

# laserPuckPointer_registerRecordDeviceDriver.cpp will be created
# laserPuckPointer.dbd
laserPuckPointer_SRCS += laserPuckPointer_registerRecordDeviceDriver.cpp

# Add locally compiled object code
laserPuckPointer_SRCS += sendPuckDemand.c
laserPuckPointer_SRCS += iSpyBWrite.c

# We need to link this IOC Application against the EPICS Base libraries
laserPuckPointer_LIBS += $(EPICS_BASE_IOC_LIBS)



include $(TOP)/configure/RULES
