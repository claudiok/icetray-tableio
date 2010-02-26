from icecube import icetray,dataclasses,dataio,tableio,hdfwriter,textwriter
from I3Tray import I3Tray
from test_pybindings import DOMLaunchBookie

class pywriter(tableio.I3TableService):
    def __init__(self):
        print "pywriter"

tray = I3Tray()

tray.AddModule('I3Reader','reader',filename='/opt/kjmeagher/analysis/ic40_grb_analysis//GRB080408_00000001.level2a.i3')

tray.AddModule(tableio.I3TableWriterModule,'writer',
               tableservice = pywriter() ,
               types = {dataclasses.I3DOMLaunchSeriesMap: DOMLaunchBookie()}
               )

tray.AddModule('TrashCan','yeswecan')

tray.Execute(20)
tray.Finish()
