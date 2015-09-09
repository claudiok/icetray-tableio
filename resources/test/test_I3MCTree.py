#!/usr/bin/env python

"""
Ensure that I3MCTree and I3LinearizedMCTree can both be written
"""

import unittest
import os
import re
import sys
from icecube import icetray, dataclasses, tableio, phys_services
from I3Tray import I3Tray

def add_eventheader(frame):
    eh = dataclasses.I3EventHeader()
    eh.run_id = 1
    eh.event_id = add_eventheader.event_id
    add_eventheader.event_id += 1
    frame['I3EventHeader'] = eh
add_eventheader.event_id = 1

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
    
    try:
        from icecube.hdfwriter import I3HDFWriter
    except ImportError:
        raise unittest.SkipTest("hdfwriter project missing")
    
    tray = I3Tray()
    
    tray.Add('I3InfiniteSource')
    tray.Add(add_eventheader, Streams=[icetray.I3Frame.DAQ])
    tray.Add("I3NullSplitter", "nullsplit")
    
    tray.Add(make_mctree, linearized=linearized)
    tray.Add(I3HDFWriter,
        Output='foo.hdf5',
        Keys=['I3MCTree'],
        Types=[],
        SubEventStreams=["nullsplit"],
    )
    
    tray.Execute(1)
    tray.Finish()

class MCTreeTest(unittest.TestCase):
    def setUp(self):
        try_to_write()
    def tearDown(self):
        os.unlink("foo.hdf5")
    def testNumberOfRows(self):
        try:
            import tables
        except ImportError:
            raise unittest.SkipTest("pytables missing")
        hdf = tables.openFile('foo.hdf5')
        self.assertIsNotNone(hdf.getNode('/I3MCTree'), "I3MCTree table exists")
        self.assertEquals(hdf.getNode('/I3MCTree').nrows, 2, "I3MCTree table has 2 rows")

class LinearizedMCTreeTest(MCTreeTest):
    def setUp(self):
        try_to_write(linearized=True)

if __name__ == "__main__":
    unittest.main()
