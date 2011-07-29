/*
 * MultiComponentByteRaw.h - Class for multi-component byte raw volume renderering data sets (.raw).
 * 
 * Author: Patrick O'Leary
 * Created: October 21, 2007
 * Copyright: 2007
 */
#ifndef MULTICOMPONENTBYTERAW_H_
#define MULTICOMPONENTBYTERAW_H_

/* Vrui includes */
#include <Geometry/ComponentArray.h>
#include <Geometry/Point.h>

// begin Forward Declarations
class Volume;
// end Forward Declarations

class MultiComponentByteRaw {
public:
	typedef float Scalar;
	typedef Geometry::ComponentArray<Scalar,3> Size;
	typedef Geometry::Point<Scalar,3> Point;
	typedef unsigned char Voxel;
	MultiComponentByteRaw();
	~MultiComponentByteRaw();
	void readMultiComponentByteRaw(const char* filename, Volume* volume, int typeOfComponents);
	void readMultiComponentByteRaw(const char* filenameA, const char* filenameB, Volume* volume, int typeOfComponents);
	void readMultiComponentByteRaw(const char* redFilename, const char* greenFilename, const char* blueFilename, Volume* volume);
};

#endif /*MULTICOMPONENTBYTERAW_H_*/
