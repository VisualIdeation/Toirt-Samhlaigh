/*
 * DatVolume.h - Class for two byte data volume renderering data set (.dat).
 * 
 * Author: Patrick O'Leary
 * Created: March 4, 2008
 * Copyright: 2008. All rights reserved.
 */
#ifndef DATVOLUME_H_
#define DATVOLUME_H_

/* Vrui includes */
#include <Geometry/ComponentArray.h>
#include <Geometry/Point.h>

// begin Forward Declarations
class Volume;
// end Forward Declarations

class DatVolume {
public:
	typedef float Scalar;
	typedef Geometry::ComponentArray<Scalar,3> Size;
	typedef Geometry::Point<Scalar,3> Point;
	typedef unsigned char Voxel;
	DatVolume();
	~DatVolume();
	void readDatVolume(const char* filename, Volume* volume);
};

#endif /*DATVOLUME_H_*/
