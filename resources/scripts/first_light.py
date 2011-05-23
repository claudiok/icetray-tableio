#!/usr/bin/env python
# hey look, i can a) write HDF5 files, and b) writer bookers in python!

import os
from optparse import OptionParser

parser = OptionParser()
parser.add_option("-f","--format",dest="format",help="format to output [hdf5, root, or csv]",default='hdf5')
parser.add_option("-z","--compress",dest="compression",help="compression level",default=1,type=int)
parser.add_option("-n","--frames",dest="nframes",help="number of frames to process",default=None,type=int)

options,args = parser.parse_args()

if len(args) != 1:
	parser.error("You must supply an input filename")
	
infile = args[0]
outfile = os.path.basename(args[0]) + '.' + options.format

from icecube import icetray,dataclasses,dataio,tableio
from I3Tray import I3Tray

if options.format == 'hdf5':
	try:
		from icecube import hdfwriter
	except ImportError:
		raise "Couldn't find the HDF writer service"
	tabler = hdfwriter.I3HDFTableService(outfile,options.compression)
elif options.format == 'root':
	try:
		from icecube import rootwriter
	except ImportError:
		raise "Couldn't find the ROOT writer service"
	tabler = rootwriter.I3ROOTTableService(outfile,options.compression)
elif options.format == 'csv':
	try:
		from icecube import textwriter
	except ImportError:
		raise "Couldn't find the text writer service"
	tabler = textwriter.I3CSVTableService(outfile[:-4] + '_csv')
else:
	raise ValueError, "I don't have a writer service for format '%s'"%options.format

tray = I3Tray()

tray.AddModule('I3Reader','reader',filename=infile)

count = 0
def counter(frame):
    global count
    if (count%100==0):
        print "%d frames"%count
    count +=1

tray.AddModule(tableio.I3TableWriter,'writer', tableservice = tabler, bookeverything=True)
tray.AddModule(counter,'count-count')
tray.AddModule('TrashCan','yeswecan')

if options.nframes is not None:
	tray.Execute(options.nframes)
else:
	tray.Execute()
tray.Finish()
