/*
 * ByteVolume.cpp - Methods for ByteVolume class.
 * 
 * Author: Patrick O'Leary
 * Created: October 19, 2007
 * Copyright: 2007
 */
#include <iostream>

/* Vrui includes */
#include <Misc/File.h>

#include <DATA/Volume.h>
#include <IO/ByteVolume.h>

/*
 * ByteVolume - Constructor for ByteVolume class.
 */
ByteVolume::ByteVolume() {
}

/*
 * ~ByteVolume - Destructor for ByteVolume class.
 */
ByteVolume::~ByteVolume() {
}

/*
 * readByteVolume - Read a byte volume volume renderering data set (.vol).
 * 
 * parameter filename - const char*
 * parameter volume - Volume
 */
void ByteVolume::readByteVolume(const char* filename, Volume* volume) {
	/* Open the volume file: */
	Misc::File byteVolumeFile(filename, "rb", Misc::File::BigEndian);
	/* Read the volume file header: */
	int size[3];
	byteVolumeFile.read(size, 3);
	volume->setSize(size);
	int borderSize=byteVolumeFile.read<int>();
	volume->setBorderSize(borderSize);
	/* Set the voxel block's position and size, and the slice center to the block's center: */
	Point origin=Point::origin;
	Size extent;
	Point center;
	for (int i=0; i<3; ++i) {
		extent[i]=Scalar(byteVolumeFile.read<float>());
		center[i]=origin[i]+extent[i]*Scalar(0.5);
	}
	volume->setOrigin(origin);
	volume->setExtent(extent);
	volume->setVolumeBox();
	volume->setCenter(center);
	/* Create a voxel array: */
	int numberOfVoxels=(size[0]+2*borderSize)*(size[1]+2*borderSize) *(size[2]+2*borderSize);
	volume->setNumberOfVoxels(numberOfVoxels);
	Voxel* voxelsBase=new Voxel[numberOfVoxels];
	byteVolumeFile.read(voxelsBase, numberOfVoxels);
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
