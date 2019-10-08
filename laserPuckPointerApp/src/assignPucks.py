#!/dls_sw/apps/python/anaconda/1.7.0/64/bin/python

import sys,re
import requests
from epics import caput, caget

url='https://ispyb.diamond.ac.uk/api/proposal/auto'

print "Running "+str(sys.argv)
try:
    beamline = sys.argv[1]
    barcode = sys.argv[2]
    puckposition = int(sys.argv[3])
except:
    print "assignPucks: Not enough parameters."
    sys.exit()
if beamline not in ["i03", "i04", "i04-1", "i24"]:
    print "assignPucks: Beamline not valid."
    sys.exit()
reg=re.compile('^[A-Z0-9\-]+$')
if not reg.match(barcode):
    print "assignPucks: Barcode not valid."
    sys.exit()
if puckposition < 1 or puckposition > 37:
    print "assignPucks: Position must be between 1 and 37." 
    sys.exit()
import ispyb.factory, ispyb
#config_file = "/dls_sw/dasc/mariadb/credentials/ispyb.cfg"
#config_file = "/home/ndg63276/pythonmodules/ispyb/ispyb.cfg"
config_file = sys.argv[4]

visit = caget(sys.argv[5])
#Clear auto collect flag PV
caput(sys.argv[5],"")

with ispyb.open(config_file) as conn:
    shipping = ispyb.factory.create_data_area(ispyb.factory.DataAreaType.SHIPPING, conn)
    try:
        rs = shipping.update_container_assign(beamline, barcode, puckposition)
        containerId = rs[0]['containerId']
        # queuedCount introduced as part of ticket SCI-8642
        queuedCount = rs[0]['queuedCount']
        if rs[0]['containerStatus'] == 'processing':
            load_unload = ' loaded'
            if queuedCount > 0:
                payload = {'CONTAINERID': containerId, 'bl':beamline}
                r = requests.get(url, params=payload)
                if r.status_code == 200:
                    print "Visit created: " + r.json()["VISIT"]
                    #Set auto collect flag PV
                    caput(sys.argv[5], r.json()["VISIT"])
                else:
                    print "Error creating visit: " + r.text

        else:
            load_unload = ' unloaded'
            if queuedCount > 0:
                visit_finished = True
                for i in range(1,38):
                    pv = re.sub(r'PUCK[0-9][0-9]','PUCK'+str(i).zfill(2),sys.argv[5])
                    if caget(pv) == visit:
                        visit_finished = False
                if visit_finished:
                    payload = {'CONTAINERID': containerId}
                    r = requests.delete(url, params=payload)
                    if r.status_code == 200:
                        print "Visit closed: " + r.json()["VISIT"]
                    else:
                        print "Error closing visit: " + r.text

        print "assignPucks: Puck "+barcode+load_unload+" - containerId: "+str(containerId)
    except:
        print "assignPucks: Puck with barcode "+barcode+" not found."
