/**
 * copyright  (C) 2010
 * The Icecube Collaboration
 *
 * $Id$
 *
 * @version $Revision$
 * @date $LastChangedDate$
 * @author Jakob van Santen <vansanten@wisc.edu> $LastChangedBy$
 */

#include "tableio/internals/I3ConverterFactory.h"
#include "dataclasses/physics/I3Particle.h"

class I3ParticleConverter : public I3ConverterImplementation<I3Particle> {
private:
    I3TableRowDescriptionPtr CreateDescription(const I3Particle& particle);
    size_t FillRows(const I3Particle& particle, I3TableRowPtr rows);
};
