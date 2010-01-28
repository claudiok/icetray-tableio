from icecube import icetray,dataclasses,hdf_writer,dataio
import array
import numpy

class DOMLaunchBookie(hdf_writer.I3Converter):
	booked = dataclasses.I3DOMLaunch
	def CreateDescription(self,dl):
		desc = hdf_writer.I3TableRowDescription()
		desc.add_field('start_time','f','ns','')
		desc.add_field('pedestal_sub',bool,'','Has the pedestal been subtracted?')
		desc.add_field('lc_bit',bool,'','')
		desc.add_field('trigger_type',self.booked.TriggerType,'','')
		desc.add_field('trigger_mode',self.booked.TriggerMode,'','')
		desc.add_field('raw_charge_stamp',int,'','',len(dl.GetRawChargeStamp()))
		for i in xrange(3):
			desc.add_field('raw_atwd_%d'%i,'i','counts','Uncalibrated ATWD digitizer counts',128)
		desc.add_field('raw_fadc','H','counts','Uncalibrated fADC digitizer counts',256)
		return desc
	def GetNumberOfRows(self,frameobj):
		return 1
	def Convert(self,domlaunch,rows,frame):
		rows.set('start_time',domlaunch.GetStartTime())
		rows.set('raw_charge_stamp',domlaunch.GetRawChargeStamp())
		rows.set('pedestal_sub',domlaunch.GetIsPedestalSub())
		rows.set('lc_bit',domlaunch.GetLCBit())
		rows.set('trigger_type',domlaunch.GetTriggerType())
		rows.set('trigger_mode',domlaunch.GetTriggerMode())
		# we can pass I3VectorInts
		rows.set('raw_atwd_0',domlaunch.GetRawATWD(0))
		# ooooor arrays with the proper typcode
		rows.set('raw_atwd_1',array.array('i',domlaunch.GetRawATWD(1)))
		# or even numpy ndarrays!
		rows.set('raw_atwd_2',numpy.array(domlaunch.GetRawATWD(2),'i'))
	
		rows.set('raw_fadc',array.array('H',domlaunch.GetRawFADC()))
		return 1
		
def test(fname='/Users/jakob/Documents/IceCube/nugen_nue_ic80_dc6.001568.000000.hits.001.1881140.domsim.001.2028732.i3.gz'):
	f = dataio.I3File(fname)
	fr = f.pop_physics()
	dl = fr['InIceRawData'].values()[0][0]
	booker = DOMLaunchBookie()

	test.desc = booker.CreateDescription(dl)
	# return desc
	n = booker.GetNumberOfRows(dl)
	rows = hdf_writer.I3TableRow(test.desc,n)
	n_written = booker.Convert(dl,rows,fr)
	f.close()
	return rows
	
if __name__ == "__main__":
	test()
	