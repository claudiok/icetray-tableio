from icecube.load_pybindings import load_pybindings
import icecube.icetray # be nice and pull in our dependencies
import icecube.dataclasses
load_pybindings(__name__,__path__)

from I3TableWriterModule import I3TableWriterModule

import types

# clean up the local dictionary
del icecube
