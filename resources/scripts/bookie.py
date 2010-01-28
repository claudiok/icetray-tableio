from icecube import icetray,dataclasses,hdf_writer,dataio
import array
import unittest

try:
	import numpy
except ImportError:
	print "Numpy isn't installed, skipping numpy-specific tests"
	

class Bookie(hdf_writer.I3Converter):
	def CreateDescription(self,thing):
		desc = hdf_writer.I3TableRowDescription()
		
		desc.add_field('char',          'c','','')
		desc.add_field('signed_char',   'b','','')
		desc.add_field('unsigned_char', 'B','','')
		desc.add_field('signed_short',  'h','','')
		desc.add_field('unsigned_short','H','','')
		desc.add_field('signed_int',    'i','','')
		desc.add_field('unsigned_int',  'I','','')
		desc.add_field('signed_long',   'h','','')
		desc.add_field('unsigned_long', 'H','','')
		desc.add_field('float',         'f','','')
		desc.add_field('double',        'd','','')
		desc.add_field('bool',          'o','','')
		
		desc.add_field('double_py',     float,'','')
		desc.add_field('int_py',        int,'','')
		desc.add_field('bool_py',       bool,'','')
		
		desc.add_field('trigger_type',dataclasses.I3DOMLaunch.TriggerType,'','')
		return desc
		
types = {'char':          'c',
         'signed_char':   'b',
         'unsigned_char': 'B',
         'signed_short':  'h',
         'unsigned_short':'H',
         'signed_int':    'i',
         'unsigned_int':  'I',
         'signed_long':   'h',
         'unsigned_long': 'H',
         'float':         'f',
         'double':        'd',
         'bool':          'o'}

convertible = {int:   ['c','b','B','h','H','i'],
               long:  ['I','h','H'],
               float: ['f','d'],
               bool:  ['o']
}

class I3PythonConverterTest(unittest.TestCase):
	def setUp(self):
		desc = hdf_writer.I3TableRowDescription()
		
		self.types = {
		              'char':          'c', 
		              'signed_char':   'b',
		              'unsigned_char': 'B',
		              'signed_short':  'h',
		              'unsigned_short':'H',
		              'signed_int':    'i',
		              'unsigned_int':  'I',
		              'signed_long':   'h',
		              'unsigned_long': 'H',
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
		self.rows = hdf_writer.I3TableRow(desc,1)
		
			
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
				tc_set = types[base]
			elif field == 'trigger_type':
				tc_set = 'i'
			tc_got = typecodes[i]
			self.assertEquals( tc_set, tc_got, "Field '%s' set up as type '%s', came back as '%s'"%(field,tc_set,tc_got) )
	def testKeys(self):
		fields = list(self.desc.field_names);
		self.assertEquals( fields, self.rows.keys() )
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
		
		field,val = 'signed_long',32767+1
		self.rows[field] = val; # OMFG, this is SHRT_MAX+1 going into a signed long!
		got_val = self.rows[field]
		self.assertEquals( val, got_val, "Set field '%s' to %d, got %d back."%(field,val,got_val))

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
	unittest.main()
	