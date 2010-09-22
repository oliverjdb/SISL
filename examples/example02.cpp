/* SISL - SINTEF Spline Library version 4.4.                              */
/* Definition and interrogation of NURBS curves and surface.              */
/* Copyright (C) 1978-2005, SINTEF ICT, Applied Mathematics, Norway.      */

/* This program is free software; you can redistribute it and/or          */
/* modify it under the terms of the GNU General Public License            */
/* as published by the Free Software Foundation version 2 of the License. */

/* This program is distributed in the hope that it will be useful,        */
/* but WITHOUT ANY WARRANTY; without even the implied warranty of         */
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          */
/* GNU General Public License for more details.                           */

/* You should have received a copy of the GNU General Public License      */
/* along with this program; if not, write to the Free Software            */
/* Foundation, Inc.,                                                      */
/* 59 Temple Place - Suite 330,                                           */
/* Boston, MA  02111-1307, USA.                                           */

/* Contact information: e-mail: tor.dokken@sintef.no                      */
/* SINTEF ICT, Department of Applied Mathematics,                         */
/* P.O. Box 124 Blindern,                                                 */
/* 0314 Oslo, Norway.                                                     */

/* SISL commercial licenses are available for:                            */
/* - Building commercial software.                                        */
/* - Building software whose source code you wish to keep private.        */

#include <iostream>
#include <fstream>
#include <string>
#include <stdexcept>
#include <cstdlib>

#include "sisl.h"
#include "GoReadWrite.h"

using namespace std;


namespace {
    string OUT_FILE_POINTS = "example2_points.g2";
    string OUT_FILE_CURVE  = "example2_curve.g2";

    string DESCRIPTION = 
    "This program will generate a SISL spline curve object by \n"
    "interpolating a set of given points.  The parametrization \n"
    "will be calculated automatically.  No derivative \n"
    "information is provided.  The routine used is 's1356'. The \n"
    "points will be written in Go-format to the file: \n'" 
    + OUT_FILE_POINTS + "'.  The resulting curve will be written \n"
    "in Go-format to the file: '" + OUT_FILE_CURVE + "'\n";

    const int num_points = 6;
    const double cstartpar = 0;

    int type[] = {1, 1, 1, 1, 1, 1};

    double points[] = {0, 0, 0,
		       1, 1, 0,
		       2, -1, 0,
		       3, 0, 0,
		       4, 1, 1,
		       3, 0, 4};
}; // end anonymous namespace 

//===========================================================================
int main(int avnum, char** vararg)
//===========================================================================
{
    cout << '\n' << vararg[0] << ":\n" << DESCRIPTION << endl;
    cout << "To proceed, press enter, or ^C to quit." << endl;
    getchar();


    try {

	double cendpar;
	SISLCurve* result_curve = 0;
	double* gpar = 0;
	int jnbpar;
	int jstat;

	s1356(points,        // pointer to where the point coordinates are stored
	      num_points,    // number of points to be interpolated
	      3,             // the dimension
	      type,          // what type of information is stored at a particular point
	      0,             // no additional condition at start point
	      0,             // no additional condition at end point
	      1,             // open curve
	      4,             // order of the spline curve to be produced
	      cstartpar,     // parameter value to be used at start of curve
	      &cendpar,      // parameter value at the end of the curve (to be determined)
	      &result_curve, // the resulting spline curve (to be determined)
	      &gpar,         // pointer to the parameter values of the points in the curve
	                     // (to be determined)
	      &jnbpar,       // number of unique parameter values (to be determined)
	      &jstat);       // status message

	if (jstat < 0) {
	    throw runtime_error("Error occured inside call to SISL routine.");
	} else if (jstat > 0) {
	    cerr << "WARNING: warning occured inside call to SISL routine. \n" << endl;
	}
	      
	cout << "Total parameter interval of curve: [" << cstartpar << ", " 
	     << cendpar << "]\n\n";
	cout << "Point parameter values was decided to be: \n";
	for (int i = 0; i < num_points; ++i) {
	    cout << gpar[i] << ' ';
	}
	cout << endl;

	ofstream os_points(OUT_FILE_POINTS.c_str());
	ofstream os_curve(OUT_FILE_CURVE.c_str());
	if (!os_points || !os_curve) {
	    throw runtime_error("Unable to open output file.");
	}

	// write result to file
	writeGoPoints(num_points, points, os_points);
	writeGoCurve(result_curve, os_curve);
	
	// cleaning up
	freeCurve(result_curve);
	free(gpar);
	os_points.close();
	os_curve.close();

    } catch (exception& e) {
	cerr << "Exception thrown: " << e.what() << endl;
	return 0;
    }

    return 1;
};
