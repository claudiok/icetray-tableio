#!/usr/bin/env python
# 
# copyright  (C) 2010
# The Icecube Collaboration
# 
# $Id$
# 
# @version $Revision$
# @date $LastChangedDate$
# @author Jakob van Santen <vansanten@wisc.edu> Last changed by: $LastChangedBy: jvansanten $
# 

from optparse import OptionParser
import os

parser = OptionParser()
parser.add_option("-f","--format",dest="format",help="format to output",default='hdf5')
parser.add_option("-z","--compress",dest="compress",help="compression level",default=1,type=int)
parser.add_option("-n","--frames",dest="nframes",help="number of frames to process",default=None,type=int)

options,args = parser.parse_args()
if len(args) != 1:
	parser.error("You must supply an input file name")
	
infile = args[0]
iformat = 'hdf5' # only service that supports reading at this point
oformat = options.format
outfile = os.path.basename(infile) + '.' + options.format

from icecube import icetray,tableio
from icecube.tableio import I3TableTranscriber

# try to import the appropriate services
if 'hdf5' in [iformat,oformat]:
	from icecube.hdfwriter import I3HDFTableService
if 'root' in [iformat,oformat]:
	from icecube.rootwriter import I3ROOTTableService
if 'csv' in [iformat,oformat]:
	from icecube.textwriter import I3CSVTableService

if iformat == 'hdf5':
	inservice = I3HDFTableService(infile,1,'r')
elif iformat == 'root':
	inservice = I3ROOTTableService(infile,'r')
else:
	raise "Unknown input format '%s'" % iformat
	
if oformat == 'hdf5':
	outservice = I3HDFTableService(outfile,options.compress,'w')
elif oformat == 'root':
	outservice = I3ROOTTableService(outfile,options.compress)
elif oformat == 'csv':
	outservice = I3CSVTableService(outfile)
else:
	raise "Unknown out format '%s'" % oformat

scribe = I3TableTranscriber(inservice,outservice)

if options.nframes is not None:
	scribe.Execute(options.nframes)
else:
	scribe.Execute()

inservice.Finish()
outservice.Finish()
