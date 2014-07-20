#include "saga/MagneticField.h"

#include <stdint.h>
#include <stdlib.h>
#include <sys/time.h>
#include <omp.h>

namespace saga {



/*********************************************************************************************************/ 
// Constructor Magnetic Field
MagneticField::MagneticField(ref_ptr<AMRgrid> grid)
{
    TheGrid = grid;
}


/*********************************************************************************************************/ 
// Get Magnetic Field
std::vector<double> MagneticField::getField(double x, double y, double z) const
{
    std::vector<double> b;
    LocalProperties lp = TheGrid->getLocalProperties(x,y,z);
    b.push_back(lp.getBx());
    b.push_back(lp.getBy());
    b.push_back(lp.getBz());
	return b;
}


} // namespace
