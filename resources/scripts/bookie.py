from icecube import icetray,dataclasses,hdf_writer,dataio

class DOMLaunchBookie(hdf_writer.I3Converter):
	booked = dataclasses.I3DOMLaunch
	def CreateDescription(self,dl):
		desc = hdf_writer.I3TableRowDescription()
		desc.add_field('start_time',float,'ns','')
		desc.add_field('pedestal_sub',bool,'','Has the pedestal been subtracted?')
		desc.add_field('lc_bit',bool,'','')
		desc.add_field('trigger_type',self.booked.TriggerType,'','')
		desc.add_field('trigger_mode',self.booked.TriggerMode,'','')
		desc.add_field('raw_charge_stamp',int,'','',len(dl.GetRawChargeStamp()))
		for i in xrange(3):
			desc.add_field('raw_atwd_%d'%i,int,'counts','Uncalibrated ATWD digitizer counts',128)
		desc.add_field('raw_fadc_%d'%i,int,'counts','Uncalibrated fADC digitizer counts',256)
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
		for i in xrange(3):
			rows.set('raw_atwd_%d'%i,domLaunch.GetRawATWD(i))
		rows.set('raw_fadc',domlaunch.GetRawFADC())
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
	