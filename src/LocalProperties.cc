#include <cmath>

#include "saga/LocalProperties.h"

namespace saga{

/*********************************************************************************************************/ 
// Constructor
LocalProperties::LocalProperties(double rho, double Bx, double By, double Bz)
{
    LocalDensity = rho;
    LocalMagneticFieldX = Bx;
    LocalMagneticFieldY = By;
    LocalMagneticFieldZ = Bz;
}
LocalProperties::LocalProperties()
{
}

/*********************************************************************************************************/ 
// Destructor
LocalProperties::~LocalProperties()
{
}

/*********************************************************************************************************/ 
// Sets the density
// Input: 
//   density: density in simulation units.
// 
void LocalProperties::setDensity(double rho)
{
    LocalDensity = rho;
}

/*********************************************************************************************************/ 
// Sets the magnetic field
// Input:
//   Bx: x component of the magnetic field in simulation units
// 
void LocalProperties::setBx(double Bx)
{
    LocalMagneticFieldX = Bx;
}

/*********************************************************************************************************/
// Sets the magnetic field
// Input:
//   By: y component of the magnetic field in simulation units
//  
void LocalProperties::setBy(double By)
{
    LocalMagneticFieldY = By;
}

/*********************************************************************************************************/ 
// Sets the magnetic field
// Input:
//   Bz: z component of the magnetic field in simulation units
// 
void LocalProperties::setBz(double Bz)
{
    LocalMagneticFieldZ = Bz;
}

/*********************************************************************************************************/ 
// Returns the density
// Output:
//   rho: density in simulation units
// 
double LocalProperties::getDensity()
{
    return LocalDensity;
}

/*********************************************************************************************************/ 
// Returns the magnetic field
// Output:
//   Bx: x component of the magnetic field in simulation units
// 
double LocalProperties::getBx()
{
    return LocalMagneticFieldX;
}

/*********************************************************************************************************/
// Returns the magnetic field
// Output:
//   By: y component of the magnetic field in simulation units
//  
double LocalProperties::getBy()
{
    return LocalMagneticFieldY;
}

/*********************************************************************************************************/ 
// Returns the magnetic field
// Output:
//   Bz: z component of the magnetic field in simulation units
// 
double LocalProperties::getBz()
{
    return LocalMagneticFieldZ;
}

/*********************************************************************************************************/ 
// Returns the magnetic field
// Output:
//   Btot: strength of the magnetic field
// 
double LocalProperties::getBtot()
{
    return sqrt(pow(LocalMagneticFieldX, 2) + pow(LocalMagneticFieldY, 2) + pow(LocalMagneticFieldZ, 2));
}



/*********************************************************************************************************/ 
// Converts the local properties into physical units.
// Input:
//   convLength: conversion factor for the distances from simulation to physical units
//   convDensity: conversion factor for the density from simulation to physical units
//   convMagneticField: conversion factor for the magnetic field from simulation to physical units
// 
void LocalProperties::convertUnits(double convLength, double convDensity, double convMagneticField)
{
    LocalDensity *= convDensity;
    LocalMagneticFieldX *= convMagneticField;
    LocalMagneticFieldY *= convMagneticField;
    LocalMagneticFieldZ *= convMagneticField;

}

} // namespace