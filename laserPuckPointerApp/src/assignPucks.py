#!/dls_sw/apps/python/anaconda/1.7.0/64/bin/python

import sys,re

print "Running "+str(sys.argv)
try:
	beamline = sys.argv[1]
	barcode	= sys.argv[2]
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
config_file = "/home/ndg63276/pythonmodules/ispyb/ispyb.cfg"

with ispyb.open(config_file) as conn:
    shipping = ispyb.factory.create_data_area(ispyb.factory.DataAreaType.SHIPPING, conn)
    try:
	    shipping.update_container_assign(beamline, barcode, puckposition)
	    print "assignPucks: Puck "+barcode+" assigned."
    except ispyb.exception.ISPyBWriteFailed:
	    print "assignPucks: Puck with barcode "+barcode+" not found."


