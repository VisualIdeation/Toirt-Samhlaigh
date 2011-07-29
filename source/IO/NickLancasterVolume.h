/*
 * NickLancasterVolume.h - Class for Nick Lancaster volume renderering data set (.nlv).
 * 
 * Author: Patrick O'Leary
 * Created: October 24, 2007
 * Copyright: 2007
 */
#ifndef NICKLANCASTERVOLUME_H_
#define NICKLANCASTERVOLUME_H_

#include <string>

/* Vrui includes */
#include <Geometry/ComponentArray.h>
#include <Geometry/Point.h>
#include <Misc/File.h>

// begin Forward Declarations
class Volume;
// end Forward Declarations

class NickLancasterVolume {
public:
	typedef float Scalar;
	typedef Geometry::ComponentArray<Scalar,3> Size;
	typedef Geometry::Point<Scalar,3> Point;
	typedef unsigned char Voxel;
	typedef int IntVoxel;
	NickLancasterVolume();
	~NickLancasterVolume();
	void readNickLancasterVolume(const char* filename, Volume* volume);
	int readHeader(Misc::File nickLancasterVolumeFile, Volume* volume);
	const char* LowerCase(std::string _string);
};

#endif /*NICKLANCASTERVOLUME_H_*/
