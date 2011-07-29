/*
 * FloatVolume.h - Class for float volume renderering data set (.fvol).
 * 
 * Author: Patrick O'Leary
 * Created: October 19, 2007
 * Copyright: 2007
 */
#ifndef FLOATVOLUME_H_
#define FLOATVOLUME_H_

/* Vrui includes */
#include <Geometry/ComponentArray.h>
#include <Geometry/Point.h>

// begin Forward Declarations
class Volume;
// end Forward Declarations

class FloatVolume {
public:
	typedef float Scalar;
	typedef Geometry::ComponentArray<Scalar,3> Size;
	typedef Geometry::Point<Scalar,3> Point;
	typedef unsigned char Voxel;
	typedef float FloatVoxel;
	FloatVolume();
	~FloatVolume();
	void readFloatVolume(const char* filename, Volume* volume);
};

#endif /*FLOATVOLUME_H_*/
