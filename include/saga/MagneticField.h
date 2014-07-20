#pragma once

#include "saga/Referenced.h"
#include "saga/AMRgrid.h"

#include <vector>
#include <string>
#include <exception>
#include <sstream>
#include <iostream>

namespace saga {

class MagneticField: public Referenced {
private:
	ref_ptr<AMRgrid> TheGrid;
public:
	MagneticField(ref_ptr<AMRgrid> grid);
	virtual ~MagneticField() { }
    std::vector<double> getField(double x, double y, double z) const;
};


} // namespace
