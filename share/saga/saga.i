%module(directors="1") saga

%{
// workaround for SWIG < 2.0.5 with GCC >= 4.7
#include <cstddef>
using std::ptrdiff_t;
%}

%include stl.i
%include std_set.i
%include std_multiset.i
%include std_map.i
%include std_pair.i
%include std_multimap.i
%include std_vector.i
%include std_string.i
%include std_list.i
%include stdint.i
%include std_container.i
%include "exception.i"
%include "typemaps.i"

%{
#include "saga/LocalProperties.h"
#include "saga/AMRgrid.h"
//#include "saga/Referenced.h"
%}



%exception
{
	try
	{
		$action
	}
	catch (const std::exception& e) {
		SWIG_exception(SWIG_RuntimeError, e.what());
	}
	catch( Swig::DirectorException &e ) {
		PyErr_Print();
		SWIG_exception(SWIG_RuntimeError, e.getMessage());
	} catch (...) {
		SWIG_exception(SWIG_RuntimeError, "unknown exception");
	}
}

%include "saga/LocalProperties.h"
%include "saga/AMRgrid.h"
%ignore operator<<;
%ignore operator>>;
%ignore *::operator=;
%ignore *::operator!;
%ignore operator saga::**;

//#include "saga/Referenced.h"
//%define TPL_REF_PTR(name, type)
//%template(name) type;
//%implicitconv saga::ref_ptr< type >;
//%template(name ## RefPtr) saga::ref_ptr< type >;
//%enddef

//%define REF_PTR(name, type)
//%implicitconv saga::ref_ptr< type >;
//%template(name ## RefPtr) saga::ref_ptr< type >;
//%enddef



//%feature("ref") saga::Referenced "$this->addReference();"
//%feature("unref") saga::Referenced "$this->removeReference();"




