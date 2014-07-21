#include "saga/MagneticField.h"



namespace saga {

/*********************************************************************************************************/ 
// Constructor Magnetic Field
BaryonDensity::BaryonDensity(ref_ptr<AMRgrid> grid)
{
    TheGrid = grid;
}

/*********************************************************************************************************/ 
// Destructor Magnetic Field
MagneticField::~MagneticField()
{
}

/*********************************************************************************************************/ 
// Get denisty
double MagneticField::getDensity(double x, double y, double z) const
{
    LocalProperties lp = TheGrid->getLocalProperties(x,y,z);
	return lp.getDensity();
}


} // namespace
