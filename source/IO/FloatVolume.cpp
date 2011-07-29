/*
 * FloatVolume.cpp - Methods for FloatVolume class.
 * 
 * Author: Patrick O'Leary
 * Created: October 19, 2007
 * Copyright: 2007
 */

/* Vrui includes */
#include <Math/Math.h>
#include <Misc/File.h>

#include <DATA/Volume.h>
#include <IO/FloatVolume.h>

/* 
 * FloatVolume - Constructor for FloatVolume class.
 */
FloatVolume::FloatVolume() {
}

/*
 * ~FloatVolume - Destructor for FloatVolume class.
 */
FloatVolume::~FloatVolume() {
}

/*
 * readFloatVolume - Read a float volume volume renderering data set (.fvol).
 * 
 * parameter filename - const char*
 * parameter volume - Volume
 */
void FloatVolume::readFloatVolume(const char* filename, Volume* volume) {
	/* Open the volume file: */
	Misc::File floatVolumeFile(filename, "rb", Misc::File::BigEndian);
	/* Read the volume file header: */
	int size[3];
	floatVolumeFile.read(size, 3);
	volume->setSize(size);
	int borderSize=floatVolumeFile.read<int>();
	volume->setBorderSize(borderSize);
	/* Set the voxel block's position and size, and the slice center to the block's center: */
	Point origin=Point::origin;
	Size extent;
	Point center;
	for (int i=0; i<3; ++i) {
		extent[i]=Scalar(floatVolumeFile.read<float>());
		center[i]=origin[i]+extent[i]*Scalar(0.5);
	}
	volume->setOrigin(origin);
	volume->setExtent(extent);
	volume->setVolumeBox();
	volume->setCenter(center);
	/* Create a voxel array: */
	int numberOfVoxels=(size[0]+2*borderSize)*(size[1]+2*borderSize)*(size[2]+2*borderSize);
	volume->setNumberOfVoxels(numberOfVoxels);
	FloatVoxel* floatVoxelsBase=new FloatVoxel[numberOfVoxels];
	floatVolumeFile.read(floatVoxelsBase, numberOfVoxels);
	volume->setFloatVoxelsBase(floatVoxelsBase);
	/* Determine the voxel data's value range: */
	float _min, _max;
	_min=_max=floatVoxelsBase[0];
	for (int i=1; i<numberOfVoxels; ++i) {
		if (_min>floatVoxelsBase[i])
			_min=floatVoxelsBase[i];
		else if (_max<floatVoxelsBase[i])
			_max=floatVoxelsBase[i];
	}
	/* Convert the float data to unsigned char: */
	Voxel* voxelsBase=new Voxel[numberOfVoxels];
	for (int i=0; i<numberOfVoxels; ++i)
		voxelsBase[i]=(unsigned char)Math::floor((Scalar(floatVoxelsBase[i])-Scalar(_min))*Scalar(255)/(Scalar(_max)-Scalar(_min))
				+Scalar(0.5));
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
}
