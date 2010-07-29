/**
 * copyright  (C) 2010
 * The Icecube Collaboration
 *
 * $Id$
 *
 * @version $Revision$
 * @date $LastChangedDate$
 * @author Eike Middell <eike.middell@desy.de> $LastChangedBy$
 */

#include "tableio/converter/I3WaveformSeriesMapConverter.h"

#include <dataclasses/I3DOMFunctions.h>
#include <dataclasses/physics/I3Waveform.h>
#include <dataclasses/calibration/I3Calibration.h>
#include <dataclasses/status/I3DetectorStatus.h>
#include <icetray/I3Units.h>

#include <boost/foreach.hpp>


namespace {

  size_t getWaveformLength(const I3WaveformSeriesMap &waveforms)
  {
    for (I3WaveformSeriesMap::const_iterator iter = waveforms.begin();
	 iter != waveforms.end(); ++iter)
      {
	BOOST_FOREACH(const I3Waveform &wf, iter->second) {
	  if (wf.GetWaveform().size()) return wf.GetWaveform().size();
	}
      }

    return 0;
  }

}

/******************************************************************************/

I3WaveformSeriesMapConverter::I3WaveformSeriesMapConverter(bool calibrate)
  : I3ConverterImplementation<I3WaveformSeriesMap>(),
    calibrate_(calibrate) 
{}

/******************************************************************************/

I3WaveformSeriesMapConverter::I3WaveformSeriesMapConverter()
  : I3ConverterImplementation<I3WaveformSeriesMap>(),
    calibrate_(false)
{}

/******************************************************************************/

I3TableRowDescriptionPtr I3WaveformSeriesMapConverter::CreateDescription(const I3WaveformSeriesMap& waveforms)
{
  I3TableRowDescriptionPtr desc = I3TableRowDescriptionPtr(new I3TableRowDescription() );
  std::string unit;
  std::string doc;
  if (calibrate_) {
    unit = "pe";
    doc = "calibrated waveform in units of photoelectrons";
  } else {
    unit = "mV";
    doc = "calibrated waveform in units of mV";
  }

  size_t wfSize(getWaveformLength(waveforms));
  if (wfSize == 0)
    log_error("Got some zero length waveforms.");
 
  desc->isMultiRow_ = true;
  desc->AddField<int8_t>("string", "", "String number");
  desc->AddField<uint8_t>("om", "", "OM number");
    
  desc->AddField<bool>("ok", "bool", "status flag that waveform has been convertered");
  desc->AddField<double>("t0", "ns", "start time of waveform");
  desc->AddField<double>("dt", "ns", "width of waveform bins");
  desc->AddField<double>("wf", unit, doc, wfSize);
    
  return desc;
}

size_t I3WaveformSeriesMapConverter::FillRows(const I3WaveformSeriesMap& waveforms, 
					      I3TableRowPtr rows)
{
     
  I3DetectorStatusConstPtr detectorstatus;
  I3CalibrationConstPtr calibration;
  if (calibrate_) {
    // retrieve calibration and detector status information
    // bail-out if not available
    detectorstatus = currentFrame_->Get<I3DetectorStatusConstPtr>();
    calibration = currentFrame_->Get<I3CalibrationConstPtr>();
    if (!detectorstatus) {
      log_fatal("%s: couldn't find detector status information in current frame!",
		__PRETTY_FUNCTION__);
    }
    if (!calibration) {
      log_fatal("%s: couldn't find calibration information in current frame!",
		__PRETTY_FUNCTION__);
    }
  }

  
  I3Map<OMKey, std::vector<I3Waveform> >::const_iterator iter;
    
  const size_t startRow = rows->GetCurrentRow();

  size_t currentRow;
  for (iter = waveforms.begin(), currentRow = rows->GetCurrentRow(); 
       iter != waveforms.end(); ++iter, ++currentRow)
    {
      rows->SetCurrentRow(currentRow);

      OMKey key = iter->first;

      double GI = NAN;
      bool ok = true;
      
      if (calibrate_) {
	const I3DOMCalibration &domcal = calibration->domCal.find(key)->second;
	const I3DOMStatus &domstatus   = detectorstatus->domStatus.find(key)->second;
	GI = SPEMean(domstatus,domcal)*domcal.GetFrontEndImpedance();
        
	if ( isnan(GI) ) {
	  log_info("OM (%d,%d) has an invalid gain. Skipping the OM.", 
		   key.GetString(), key.GetOM());
	  ok = false;
	}
      }
        
      // /!\ only the first atwd waveform is extracted
      const I3Waveform& wf = iter->second.front();
      const std::vector<double>& readout = wf.GetWaveform();
      rows->Set<int8_t>("string", key.GetString());
      rows->Set<uint8_t>("om", key.GetOM());
      rows->Set<bool>("ok", ok);
      rows->Set<double>("t0", wf.GetStartTime()/I3Units::ns);
      rows->Set<double>("dt", wf.GetBinWidth()/I3Units::ns);
      double VoltToNPE = wf.GetBinWidth()/GI;

      std::vector<double>::const_iterator wfiter;
      double* buffer = rows->GetPointer<double>("wf");
      int i = 0;

      if (calibrate_) {
	for (wfiter = readout.begin(); 
	     i < 128 && wfiter != readout.end(); 
	     ++i, ++wfiter++)
	  {
	    buffer[i] = (*wfiter)*VoltToNPE;
	  }
      } else {
	for (wfiter = readout.begin(); 
	     i < 128 && wfiter != readout.end(); 
	     ++i, ++wfiter++)
	  {
	    buffer[i] = *wfiter/I3Units::mV;
	  }
      }
    }
  return currentRow - startRow;

}

/******************************************************************************/

size_t I3WaveformSeriesMapConverter::GetNumberOfRows(const I3WaveformSeriesMap& waveforms) {
  return waveforms.size();
}

/******************************************************************************/

I3_CONVERTER(I3WaveformSeriesMapConverter, I3WaveformSeriesMap);
