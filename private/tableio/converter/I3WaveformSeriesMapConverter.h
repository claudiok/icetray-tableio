/**
 * copyright  (C) 2010
 * The Icecube Collaboration
 *
 * $Id: I3WaveformConverter.h 64296 2010-05-25 16:13:48Z jvansanten $
 *
 * @version $Revision$
 * @date $LastChangedDate$
 * @author Eike Middell <eike.middell@desy.de> $LastChangedBy$
 */

#ifndef TABLEIO_I3WAVEFORMSERIESMAPCONVERTER_H_INCLUDED
#define TABLEIO_I3WAVEFORMSERIESMAPCONVERTER_H_INCLUDED

#include "tableio/I3ConverterFactory.h"
#include "dataclasses/physics/I3Waveform.h"

class I3WaveformSeriesMapConverter : public I3ConverterImplementation< I3WaveformSeriesMap > {
public:
    I3WaveformSeriesMapConverter();
    I3WaveformSeriesMapConverter(bool calibrate);

private:
    I3TableRowDescriptionPtr CreateDescription(const I3WaveformSeriesMap& waveforms);
    size_t FillRows(const I3WaveformSeriesMap& waveforms, I3TableRowPtr rows);
    size_t GetNumberOfRows(const I3WaveformSeriesMap& waveforms);

    bool calibrate_;
};

#endif // TABLEIO_I3WAVEFORMCONVERTER_H_INCLUDED
