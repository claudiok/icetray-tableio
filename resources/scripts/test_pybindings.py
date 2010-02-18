from icecube import icetray,dataclasses,tableio,dataio
import array
import unittest
import exceptions

try:
	import numpy
	have_numpy = True
except ImportError:
	print "Numpy isn't installed, skipping numpy-specific tests"
	have_numpy = False
	
class I3PythonConverterTest(unittest.TestCase):
	def setUp(self):
		desc = tableio.I3TableRowDescription()
		
		self.types = {
		              'char':          'c', 
		              'signed_char':   'b',
		              'unsigned_char': 'B',
		              'signed_short':  'h',
		              'unsigned_short':'H',
		              'signed_int':    'i',
		              'unsigned_int':  'I',
		              'signed_long':   'l',
		              'unsigned_long': 'L',
		              'float':         'f',
		              'double':        'd',
		              'bool':          'o'}
		self.conversions = {int:   ['c','b','B','h','H','i'],
		                    long:  ['I','h','H'],
		                    float: ['f','d'],
		                    bool:  ['o']}
		
		
		for t,code in self.types.iteritems():
			desc.add_field(t,code,'','')
			desc.add_field('%s_vec'%t,code,'','',128)
		
		desc.add_field('double_py',     float,'','')
		desc.add_field('signed_int_py',        int,'','')
		desc.add_field('bool_py',       bool,'','')
		
		desc.add_field('trigger_type',dataclasses.I3DOMLaunch.TriggerType,'','')
		
		self.desc = desc
		self.rows = tableio.I3TableRow(desc,1)
		
			
	def testTypeCodes(self):
		fields = list(self.desc.field_names)
		typecodes = list(self.desc.field_type_codes)
		hdftypes = list(self.desc.field_hdf_types)
		for i,field in enumerate(fields):
			base = field
			if field.endswith('_vec'):
				base = field[:-4]
			elif field.endswith('_py'):
				base = field[:-3]
			if base in self.types:
				tc_set = self.types[base]
			elif field == 'trigger_type':
				tc_set = 'i'
			tc_got = typecodes[i]
			self.assertEquals( tc_set, tc_got, "Field '%s' set up as type '%s', came back as '%s'"%(field,tc_set,tc_got) )
	def testKeys(self):
		fields = list(self.desc.field_names);
		self.assertEquals( fields, self.rows.keys() )
	def testEnum(self):
		field = 'trigger_type'
		val = dataclasses.I3DOMLaunch.TriggerType.SPE_DISCRIMINATOR_TRIGGER
		self.rows[field] = val
		got = self.rows[field]
		self.assertEquals( int(val), got )
	def testIntegerScalars(self):
		types = self.conversions[int]
		reverse_types = dict([(b,a) for a,b in self.types.iteritems()])
		for i,t in enumerate(types):
			if t == 'c': continue # no from_python converter for char...ah well
			val = i+7
			field = reverse_types[t]
			self.rows[field] = val
			got_val = self.rows[field]
			self.assertEquals( val, got_val, "Set field '%s' to %d, got %d back."%(field,val,got_val))
	def testLongScalars(self):
		import sys
		field,val = 'signed_long',sys.maxint
		self.rows[field] = val;
		got_val = self.rows[field]
		self.assertEquals( val, got_val, "Set field '%s' to %d, got %d back."%(field,val,got_val))
		bad_news = lambda: self.rows.set(field,sys.maxint+1)
		self.assertRaises(exceptions.OverflowError, bad_news)
	def testArray(self):
		field = 'signed_int_vec'
		arr = array.array(self.types[field[:-4]],range(128))
		self.rows[field] = arr
		got = self.rows[field]
		self.assertEquals( list(arr), got )
	if have_numpy:
		def testNumpy(self):
			field = 'signed_long_vec'
			arr = numpy.array(range(128),self.types[field[:-4]])
			self.rows[field] = arr
			got = self.rows[field]
			self.assertEquals( list(arr), got )
			import sys
			# try passing an array in byte-swapped order
			swapped = '>' if (sys.byteorder == 'little') else '<'
			arr = numpy.array(range(128), (swapped + self.types[field[:-4]]) )
			bad_news = lambda: self.rows.set(field,arr)
			self.assertRaises(exceptions.RuntimeError,bad_news)
	def testVectorInt(self):
		vec = dataclasses.I3VectorInt()
		for r in xrange(128): vec.append(r)
		field = 'signed_int_vec'
		self.rows[field] = vec
		got = self.rows[field]
		self.assertEquals( range(128), got )
	def testVectorDouble(self):
		vec = dataclasses.I3VectorDouble()
		for r in xrange(128): vec.append(r+3)
		field = 'double_vec'
		self.rows[field] = vec
		got = self.rows[field]
		for a,b in zip(vec,got):
			self.assertAlmostEqual(a,b)
		

class DOMLaunchBookie(tableio.I3Converter):
    booked = dataclasses.I3DOMLaunchSeriesMap
    dl = dataclasses.I3DOMLaunch
    def CreateDescription(self,dlsm):
        desc = tableio.I3TableRowDescription()
        # make this a "ragged" table
        desc.is_multi_row = True
        # grab an example DOMLaunch
        dl = self._get_nonempty(dlsm)
        # unrolling fields
        desc.add_field('string','h','','String number')
        desc.add_field('om','H','','OM number')
        desc.add_field('index','H','','Index within the vector')
        # DOMLaunch fields
        desc.add_field('start_time','f','ns','')
        desc.add_field('pedestal_sub',bool,'','Has the pedestal been subtracted?')
        desc.add_field('lc_bit',bool,'','')
        desc.add_field('trigger_type',self.dl.TriggerType,'','')
        desc.add_field('trigger_mode',self.dl.TriggerMode,'','')
        desc.add_field('raw_charge_stamp',int,'','',len(dl.GetRawChargeStamp()))
        for i in xrange(3):
            desc.add_field('raw_atwd_%d'%i,'i','counts','Uncalibrated ATWD digitizer counts',128)
        desc.add_field('raw_fadc','H','counts','Uncalibrated fADC digitizer counts',256)
        return desc
    def _get_nonempty(self,dlsm):
        dl = None
        for vec in dlsm.itervalues():
            if len(vec) == 0: continue
            dl = vec[0]
            if dl != None: break
        return dl
    def GetNumberOfRows(self,frameobj):
        nrows = 0
        for vec in frameobj.itervalues():
            nrows += len(vec)
        return nrows
    def Convert(self,dlsm,rows,frame):
        rowno = 0
        for omkey,dl_series in dlsm.iteritems():
            string = omkey.GetString()
            om = omkey.GetOM()
            for i,domlaunch in enumerate(dl_series):
                rows.current_row = rowno
                rowno += 1
                rows['string']           = string
                rows['om']               = om
                rows['index']            = i
                rows['pedestal_sub']     = domlaunch.GetIsPedestalSub()
         
                rows['start_time']       = domlaunch.GetStartTime()
                rows['raw_charge_stamp'] = domlaunch.GetRawChargeStamp()
                rows['pedestal_sub']     = domlaunch.GetIsPedestalSub()
                rows['lc_bit']           = domlaunch.GetLCBit()
                rows['trigger_type']     = domlaunch.GetTriggerType()
                rows['trigger_mode']     = domlaunch.GetTriggerMode()
                # we can pass I3VectorInts
                rows['raw_atwd_0']       = domlaunch.GetRawATWD(0)
                # ooooor arrays with the proper typecode
                rows['raw_atwd_1']       = array.array('i',domlaunch.GetRawATWD(1))
                # or even numpy ndarrays!
                rows['raw_atwd_2']       = numpy.array(domlaunch.GetRawATWD(2),'i')
    
                rows['raw_fadc']         = array.array('H',domlaunch.GetRawFADC())
        return rowno
        
class I3TableWriterPythonModuleTest(unittest.TestCase):
		"""Test the option-parsing magic."""
		def setUp(self):
			from icecube import icetray,tableio,dataclasses
			from module import I3TableWriterModule
			import tempfile
			from I3Tray import I3Tray, load
			load("libphys-services")
			
			tray = I3Tray()
			# tray.AddService("I3EmptyStreamsFactory","sound_and_fury_signifying_nothing",
			#         installGeometry=True,
			#         installCalibration=True,
			#         installStatus=True,
			#         eventTimeYear=2008,
			#         eventTimeNnanosec=163163250000000011,
			#         nFrames=1)
			tray.AddService("I3EmptyStreamsFactory","sound_and_fury_signifying_nothing")
			tray.AddModule("I3Muxer","muxalot")
			self.tray = tray
			self.tempfile = tempfile.NamedTemporaryFile()
			self.hdf_service = tableio.I3HDFTableService(self.tempfile.name,0)
			self.target = I3TableWriterModule
			self.bookie = DOMLaunchBookie()
			
		def tearDown(self):
			# self.hdf_service.CloseFile()
			self.tempfile.close() # implicit delete
			
		def testNoArgs(self):
			"""TableService is a required argument"""
			self.tray.AddModule(self.target,'scribe')
			self.assertRaises(TypeError,self.tray.Execute)
			# self.tray.Execute()
			self.tray.Finish()
		def testNotATable(self):
			"""Things that are not TableServices are rejected"""
			self.tray.AddModule(self.target,'scribe',
				tableservice = 'foo',
				keys = ['I3EventHeader','InIceRawData']
				)
			self.assertRaises(TypeError,self.tray.Execute)
			# self.tray.Execute()
			self.tray.Finish()
		def testKeyList(self):
			"""A simple list of keys, no error"""
			self.tray.AddModule(self.target,'scribe',
				tableservice = self.hdf_service,
				keys = ['I3EventHeader','InIceRawData']
				)
			self.tray.Execute()
			self.tray.Finish()
		def testKeyDict(self):
			"""A dict key: booker"""
			self.tray.AddModule(self.target,'scribe',
				tableservice = self.hdf_service,
				keys = {'InIceRawData': self.bookie}
				)
			self.tray.Execute()
			self.tray.Finish()
		def testKeyTuples(self):
			"""Tuples of (key, booker)"""
			self.tray.AddModule(self.target,'scribe',
				tableservice = self.hdf_service,
				keys = [('InIceRawData', self.bookie),('InIceRawData',None)] # repeat with default booker
				)
			self.tray.Execute()
			self.tray.Finish()
		
def test(fname='/Users/jakob/Documents/IceCube/nugen_nue_ic80_dc6.001568.000000.hits.001.1881140.domsim.001.2028732.i3.gz'):
	f = dataio.I3File(fname)
	fr = f.pop_physics()
	dl = fr['InIceRawData'].values()[0][0]
	booker = DOMLaunchBookie()

	test.desc = booker.CreateDescription(dl)
	# return desc
	n = booker.GetNumberOfRows(dl)
	rows = tableio.I3TableRow(test.desc,n)
	n_written = booker.Convert(dl,rows,fr)
	f.close()
	return rows
	
if __name__ == "__main__":
	unittest.main()
	