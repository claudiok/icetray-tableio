# create the registry first, so that compiled bookers can be added
from registry import I3ConverterRegistry

from icecube.load_pybindings import load_pybindings
import icecube.icetray # be nice and pull in our dependencies
import icecube.dataclasses
load_pybindings(__name__,__path__)

from I3TableWriterModule import I3TableWriter, default

import types

from enum import enum

# clean up the local dictionary
del icecube

