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

from icecube import icetray,tableio
from icecube.hdfwriter import I3HDFTableService
from icecube.tableio import I3TableTranscriber

inservice = I3HDFTableService('foo.hd5',1,'r')
outservice = I3HDFTableService('foo_transcribed.hd5',1,'w')

scribe = I3TableTranscriber(inservice,outservice)

scribe.Execute()

# inservice.Finish()
# outservice.Finish()
