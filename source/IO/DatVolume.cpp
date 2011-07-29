/*
 * DatVolume.cpp - Methods for DatVolume class.
 * 
 * Author: Patrick O'Leary
 * Created: March 4, 2008
 * Copyright: 2008. All rights reserved.
 */

#include <iostream>

/* Vrui includes */
#include <Misc/File.h>

#include <DATA/Volume.h>
#include <IO/DatVolume.h>

/*
 * DatVolume - Constructor for DatVolume class.
 */
DatVolume::DatVolume() {
} // end DatVolume()

/*i = (s[ 0 ] << 8) +s[ 1 ];
 * ~DatVolume - Destructor for DatVolume class.
 */
DatVolume::~DatVolume() {
} // end ~DatVolume()

/*
 * readDatVolume - Read a byte raw volume renderering data set (.raw).
 * 
 * parameter filename - const char*
 * parameter volume - Volume
 */
void DatVolume::readDatVolume(const char* filename, Volume* volume) {
	/* Open the volume file: */
	Misc::File datVolumeFile(filename, "rb", Misc::File::LittleEndian);
	Voxel * dimensions = new Voxel[6];
	datVolumeFile.read(dimensions, 6);
	int * size = new int[3];
	size[0] = (dimensions[1] << 8) +dimensions[0];
	size[1] = (dimensions[3] << 8) +dimensions[2];
	size[2] = (dimensions[5] << 8) +dimensions[4];
	volume->setSize(size);
	const Scalar* scale = volume->getScale();
	if (size[0]!=0&&size[1]!=0&&size[2]!=0) {
		int borderSize = 0;
		volume->setBorderSize(borderSize);
		/* Set the voxel block's position and size, and the slice center to the block's center: */
		Point origin=Point::origin;
		Size extent;
		Point center;
		for (int i=0; i<3; ++i) {
			extent[i]=Scalar(size[i]-1)*scale[i];
			center[i]=origin[i]+extent[i]*Scalar(0.5);
		}
		volume->setOrigin(origin);
		volume->setExtent(extent);
		volume->setVolumeBox();
		volume->setCenter(center);
		/* Create a voxel array: */
		int numberOfVoxels=(size[0]+2*borderSize)*(size[1]+2*borderSize) *(size[2]+2*borderSize);
		volume->setNumberOfVoxels(numberOfVoxels);
		Voxel* tmpVoxelsBase=new Voxel[numberOfVoxels*2];
		datVolumeFile.read(tmpVoxelsBase, numberOfVoxels*2);
		int * intVoxelsBase=new int[numberOfVoxels];
		for (int i=0; i<numberOfVoxels; ++i)
			intVoxelsBase[i] = (tmpVoxelsBase[2*i+1] << 8) +tmpVoxelsBase[2*i];
		volume->setIntVoxelsBase(intVoxelsBase);
		delete[] tmpVoxelsBase;
		int _min, _max;
		_min=_max=intVoxelsBase[0];
		for (int i=1; i<numberOfVoxels; ++i) {
			if (_min>intVoxelsBase[i])
				_min=intVoxelsBase[i];
			else if (_max<intVoxelsBase[i])
				_max=intVoxelsBase[i];
		}
		/* Convert the float data to unsigned char: */
		Voxel* voxelsBase=new Voxel[numberOfVoxels];
		for (int i=0; i<numberOfVoxels; ++i)
			voxelsBase[i]=(unsigned char)(((intVoxelsBase[i]-_min)*255)/(_max-_min));
		volume->setByteVoxelsBase(voxelsBase);
		/* Calculate the address of the voxel block: */
		Voxel* voxels=voxelsBase+borderSize;
		int increments[3];
		increments[2]=1;
		for (int i=2; i>0; --i) {
			increments[i-1]=increments[i]*(size[i]+2*borderSize);
			voxels+=borderSize*increments[i-1];
		}
		volume->setIncrements(increments);
		volume->setVoxels(voxels);
	} else {
		std::cout << "Error: size was not read properly for DatVolume volume!" << std::endl;
	}
} // end readDatVolume()
