# 
# copyright  (C) 2010
# The Icecube Collaboration
# 
# $Id: I3Converter.h 61500 2010-02-07 03:52:36Z jvansanten $
# 
# @version $Revision$
# @date $LastChangedDate$
# @author Jakob van Santen <vansanten@wisc.edu> Last changed by: $LastChangedBy: jvansanten $
# 

from icecube import icetray
from icecube.hdf_writer import I3HDFTableService,I3TableTranscriber

inservice = I3HDFTableService('foo.hd5',1,'r')
outservice = I3HDFTableService('foo_transcribed.hd5',1,'w')

scribe = I3TableTranscriber(inservice,outservice)

scribe.Execute()

# inservice.Finish()
# outservice.Finish()
