/*
 * ByteRaw.cpp - Methods for ByteRaw class.
 * 
 * Author: Patrick O'Leary
 * Created: October 21, 2007
 * Copyright: 2007
 */

#include <iostream>

/* Vrui includes */
#include <Misc/File.h>

#include <DATA/Volume.h>
#include <IO/ByteRaw.h>

/*
 * ByteRaw - Constructor for ByteRaw class.
 */
ByteRaw::ByteRaw() {
}

/*
 * ~ByteRaw - Destructor for ByteRaw class.
 */
ByteRaw::~ByteRaw() {
}

/*
 * readByteRaw - Read a byte raw volume renderering data set (.raw).
 * 
 * parameter filename - const char*
 * parameter volume - Volume
 */
void ByteRaw::readByteRaw(const char* filename, Volume* volume) {
	/* Open the volume file: */
	Misc::File byteRawFile(filename, "rb", Misc::File::LittleEndian);
	const int* size = volume->getSize();
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
		Voxel* voxelsBase=new Voxel[numberOfVoxels];
		byteRawFile.read(voxelsBase, numberOfVoxels);
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
		std::cout << "Error: size was not set on command-line for ByteRaw volume!" << std::endl;
	}
}
