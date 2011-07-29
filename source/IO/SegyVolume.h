/*
 * SegyVolume.h - Class for SEG-Y volume renderering data set (.segy, .sgy, .segy3D, .sgy3D).
 *
 * Author: Phil McDonald
 * Created: April 13, 2009
 * Copyright: 2009
 */
#ifndef SEGYVOLUME_H_
#define SEGYVOLUME_H_

#include <string>

/* Vrui includes */
#include <Geometry/ComponentArray.h>
#include <Geometry/Point.h>

#include <IO/segy.h>

// begin Forward Declarations
class Volume;
// end Forward Declarations

class SegyVolume {
public:
    typedef float Scalar;
    typedef Geometry::ComponentArray<Scalar,3> Size;
    typedef Geometry::Point<Scalar,3> Point;
    typedef unsigned char Voxel;
    typedef float FloatVoxel;
    SegyVolume();
    ~SegyVolume();
    void readSegyVolume (const char *filename, Volume *volume);
    void readSegySlice (const char *filename, Volume *volume);
private:
    void finishSegyVolume (int nslices, SegyData **segy_slices, Volume *volume,
                           int iz_clip);
};

#endif /*SEGYVOLUME_H_*/
