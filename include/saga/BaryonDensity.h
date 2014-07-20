#pragma once

#include "saga/Referenced.h"
#include "saga/AMRgrid.h"

#include <vector>
#include <string>
#include <exception>
#include <sstream>
#include <iostream>

namespace saga {

class BaryonDensity: public Referenced {
private:
	ref_ptr<AMRgrid> TheGrid;
public:
	BaryonDensity(ref_ptr<AMRgrid> grid);
	virtual ~MagneticField();
    std::vector<double> getDensity(double x, double y, double z) const;
};


} // namespace
