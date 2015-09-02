#!/usr/bin/env python

"""
Ensure that I3MCTree and I3LinearizedMCTree can both be written
"""

try:
    from icecube.hdfwriter import I3HDFWriter
except ImportError:
    import sys
    sys.exit(0)

from icecube import icetray, dataclasses, tableio, phys_services
from I3Tray import I3Tray

def make_mctree(frame, linearized=True):
    tree = dataclasses.I3MCTree()
    p1 = dataclasses.I3Particle()
    tree.insert(p1)
    p2 = dataclasses.I3Particle()
    tree.append_child(p1, p2)
    if linearized:
        tree = dataclasses.I3LinearizedMCTree(tree)
    frame['I3MCTree'] = tree

def try_to_write(linearized=False):
    tray = I3Tray()
    
    tray.Add('I3InfiniteSource')
    tray.Add('I3NullSplitter', 'nullsplit')
    tray.Add(make_mctree, linearized=linearized)
        
    tray.Add(I3HDFWriter,
        Output='foo.hdf5',
        Keys=['I3MCTree'],
        Types=[],
        SubEventStreams=["nullsplit"],
    )
    
    tray.Execute(1)
    tray.Finish()

try_to_write()
try_to_write(linearized=True)
