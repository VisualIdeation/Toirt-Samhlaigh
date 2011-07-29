/*
 * ByteVolume.h - Class for byte volume renderering data set (.vol).
 * 
 * Author: Patrick O'Leary
 * Created: October 19, 2007
 * Copyright: 2007
 */
#ifndef BYTEVOLUME_H_
#define BYTEVOLUME_H_

/* Vrui includes */
#include <Geometry/ComponentArray.h>
#include <Geometry/Point.h>

// begin Forward Declarations
class Volume;
// end Forward Declarations

class ByteVolume {
public:
	typedef float Scalar;
	typedef Geometry::ComponentArray<Scalar,3> Size;
	typedef Geometry::Point<Scalar,3> Point;
	typedef unsigned char Voxel;
	ByteVolume();
	~ByteVolume();
	void readByteVolume(const char* filename, Volume* volume);
};

#endif /*BYTEVOLUME_H_*/
