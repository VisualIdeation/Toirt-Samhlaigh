/*
 * ByteRaw.h - Class for byte raw volume renderering data set (.raw).
 * 
 * Author: Patrick O'Leary
 * Created: October 21, 2007
 * Copyright: 2007. All rights reserved.
 */
#ifndef BYTERAW_H_
#define BYTERAW_H_

/* Vrui includes */
#include <Geometry/ComponentArray.h>
#include <Geometry/Point.h>

// begin Forward Declarations
class Volume;
// end Forward Declarations

class ByteRaw {
public:
	typedef float Scalar;
	typedef Geometry::ComponentArray<Scalar,3> Size;
	typedef Geometry::Point<Scalar,3> Point;
	typedef unsigned char Voxel;
	ByteRaw();
	~ByteRaw();
	void readByteRaw(const char* filename, Volume* volume);
};

#endif /*BYTERAW_H_*/
