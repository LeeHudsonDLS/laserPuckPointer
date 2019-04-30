
import subprocess
from iocbuilder import Device, AutoSubstitution, Xml
from iocbuilder.modules.asyn import AsynIP
from iocbuilder.modules.streamDevice import AutoProtocol
from iocbuilder.arginfo import *

class laserPuckPointerLib(Device):
    LibFileList = ['laserPuckPointer']
    DbdFileList = ['laserPuckPointer']
    AutoInstantiate = True

class laserPuckPointer(AutoSubstitution, AutoProtocol ):
    ## Parse this template file for macros
    TemplateFile = 'laserPuckPointer.db'
    ProtocolFiles = ['laserPuckPointer.proto']
    Dependencies = (laserPuckPointerLib, AsynIP  )
    scanList = ['.1 second','.2 second','.5 second','1 second','2 second','5 second','10 second','I/O Intr','Event','Passive']
    versions = subprocess.check_output(['dls-list-releases.py', 'laserPuckPointer'])
    versions = versions.replace("'","")
    versions = versions.replace(" ","")
    verChoice = versions.split('[')
    verChoice = verChoice[1].split(']')[0].split(',')
    
    verChoice.insert(0,'work')
    ArgInfo = makeArgInfo(
        P = Simple("Device Prefix", str),
        PORT = Ident("Asyn Port", AsynIP),
        DEMAND_IN = Simple("Demand position from PLC", str),
        SCAN = Choice("Scan time", scanList),
        ROBOT = Simple("Robot prefix", str),
        BL = Simple("Beamline prefix to run iSpyB write script", str),
        VERS = Choice("Version of this support module used", verChoice),
    )

