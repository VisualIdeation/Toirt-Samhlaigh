/*
 * MultiComponentByteRaw.cpp - Methods for MultiComponentByteRaw class.
 * 
 * Author: Patrick O'Leary
 * Created: October 21, 2007
 * Copyright: 2007
 */

#include <iostream>

/* Vrui includes */
#include <Misc/File.h>

#include <DATA/Volume.h>
#include <IO/MultiComponentByteRaw.h>

/*
 * MultiComponentByteRaw - Constructor for MultiComponentByteRaw class.
 */
MultiComponentByteRaw::MultiComponentByteRaw() {
}

/*
 * ~MultiComponentByteRaw - Destructor for MultiComponentByteRaw class.
 */
MultiComponentByteRaw::~MultiComponentByteRaw() {
}

/*
 * readMultiComponentByteRaw - Read a byte raw volume renderering data set (.raw).
 * 
 * parameter filename - const char*
 * parameter volume - Volume
 * parameter typeOfComponents - int
 */
void MultiComponentByteRaw::readMultiComponentByteRaw(const char* filename,
		Volume* volume, int typeOfComponents) {
	/* Open the volume component file */
	Misc::File byteRawFile(filename, "rb", Misc::File::LittleEndian);
	const int* size = volume->getSize();
	const Scalar* scale = volume->getScale();
	volume->setNumberOfComponents(1);
	volume->setTypeOfComponents(typeOfComponents);
	if (size[0]!=0&&size[1]!=0&&size!=0) {
		int borderSize = 0;
		volume->setBorderSize(borderSize);
		/* Set the voxel block's position and size, and the slice center to the block's center */
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
		int numberOfVoxels=(size[0]+2*borderSize)*(size[1]+2*borderSize)
				*(size[2]+2*borderSize);
		volume->setNumberOfVoxels(numberOfVoxels);
		Voxel* voxelsBase=new Voxel[numberOfVoxels];
		byteRawFile.read(voxelsBase, numberOfVoxels);
		/* Calculate the address of the voxel block */
		if (typeOfComponents == 4) { // Red Component
			volume->setRedByteVoxelsBase(voxelsBase);
			Voxel* redVoxels=voxelsBase+borderSize;
			int increments[3];
			increments[2]=1;
			for (int i=2; i>0; --i) {
				increments[i-1]=increments[i]*(size[i]+2*borderSize);
				redVoxels+=borderSize*increments[i-1];
			}
			volume->setIncrements(increments);
			volume->setRedVoxels(redVoxels);
			volume->setVoxels(redVoxels); // remove when fully operational
		} else if (typeOfComponents == 2) { // Green Component
			volume->setGreenByteVoxelsBase(voxelsBase);
			Voxel* greenVoxels=voxelsBase+borderSize;
			int increments[3];
			increments[2]=1;
			for (int i=2; i>0; --i) {
				increments[i-1]=increments[i]*(size[i]+2*borderSize);
				greenVoxels+=borderSize*increments[i-1];
			}
			volume->setIncrements(increments);
			volume->setGreenVoxels(greenVoxels);
			volume->setVoxels(greenVoxels); // remove when fully operational
		} else if (typeOfComponents == 1) { // Blue Component
			volume->setBlueByteVoxelsBase(voxelsBase);
			Voxel* blueVoxels=voxelsBase+borderSize;
			int increments[3];
			increments[2]=1;
			for (int i=2; i>0; --i) {
				increments[i-1]=increments[i]*(size[i]+2*borderSize);
				blueVoxels+=borderSize*increments[i-1];
			}
			volume->setIncrements(increments);
			volume->setBlueVoxels(blueVoxels);
			volume->setVoxels(blueVoxels); // remove when fully operational
		}
	} else {
		std::cout
				<< "Error: size was not set on command-line for MultiComponentByteRaw volume!"
				<< std::endl;
	}
}

/*
 * readMultiComponentByteRaw - Read a byte raw volume renderering data set (.raw).
 * 
 * parameter filenameA - const char*
 * parameter filenameB - const char*
 * parameter volume - Volume
 * parameter typeOfComponents - int
 */
void MultiComponentByteRaw::readMultiComponentByteRaw(const char* filenameA,
		const char* filenameB, Volume* volume, int typeOfComponents) {
	/* Open the volume component files */
	Misc::File byteRawFileA(filenameA, "rb", Misc::File::LittleEndian);
	Misc::File byteRawFileB(filenameB, "rb", Misc::File::LittleEndian);
	const int* size = volume->getSize();
	const Scalar* scale = volume->getScale();
	volume->setNumberOfComponents(2);
	volume->setTypeOfComponents(typeOfComponents);
	if (size[0]!=0&&size[1]!=0&&size!=0) {
		int borderSize = 0;
		volume->setBorderSize(borderSize);
		/* Set the voxel block's position and size, and the slice center to the block's center */
		Point origin=Point::origin;
		Size extent;
		Point center;
		for (int i=0; i<3; ++i) {
			extent[i]=Scalar(size[i])*scale[i];
			center[i]=origin[i]+extent[i]*Scalar(0.5);
		}
		volume->setOrigin(origin);
		volume->setExtent(extent);
		volume->setVolumeBox();
		volume->setCenter(center);
		/* Create a voxel arrays */
		int numberOfVoxels=(size[0]+2*borderSize)*(size[1]+2*borderSize)
				*(size[2]+2*borderSize);
		volume->setNumberOfVoxels(numberOfVoxels);
		/* Calculate the address of the voxel blocks */
		if (typeOfComponents == 6) { // Red and Green Components
			Voxel* redVoxelsBase=new Voxel[numberOfVoxels];
			byteRawFileA.read(redVoxelsBase, numberOfVoxels);
			volume->setRedByteVoxelsBase(redVoxelsBase);
			Voxel* redVoxels=redVoxelsBase+borderSize;
			int increments[3];
			increments[2]=1;
			for (int i=2; i>0; --i) {
				increments[i-1]=increments[i]*(size[i]+2*borderSize);
				redVoxels+=borderSize*increments[i-1];
			}
			volume->setRedVoxels(redVoxels);
			Voxel* greenVoxelsBase=new Voxel[numberOfVoxels];
			byteRawFileB.read(greenVoxelsBase, numberOfVoxels);
			volume->setGreenByteVoxelsBase(greenVoxelsBase);
			Voxel* greenVoxels=greenVoxelsBase+borderSize;
			for (int i=2; i>0; --i) {
				greenVoxels+=borderSize*increments[i-1];
			}
			volume->setGreenVoxels(greenVoxels);
			volume->setIncrements(increments);
			volume->setVoxels(greenVoxels); // remove when fully operational
		} else if (typeOfComponents == 5) { // Red and Blue Components
			Voxel* redVoxelsBase=new Voxel[numberOfVoxels];
			byteRawFileA.read(redVoxelsBase, numberOfVoxels);
			volume->setRedByteVoxelsBase(redVoxelsBase);
			Voxel* redVoxels=redVoxelsBase+borderSize;
			int increments[3];
			increments[2]=1;
			for (int i=2; i>0; --i) {
				increments[i-1]=increments[i]*(size[i]+2*borderSize);
				redVoxels+=borderSize*increments[i-1];
			}
			volume->setRedVoxels(redVoxels);
			Voxel* blueVoxelsBase=new Voxel[numberOfVoxels];
			byteRawFileB.read(blueVoxelsBase, numberOfVoxels);
			volume->setBlueByteVoxelsBase(blueVoxelsBase);
			Voxel* blueVoxels=blueVoxelsBase+borderSize;
			for (int i=2; i>0; --i) {
				blueVoxels+=borderSize*increments[i-1];
			}
			volume->setBlueVoxels(blueVoxels);
			volume->setIncrements(increments);
			volume->setVoxels(blueVoxels); // remove when fully operational
		} else if (typeOfComponents == 3) { // Green and Blue Components
			Voxel* greenVoxelsBase=new Voxel[numberOfVoxels];
			byteRawFileA.read(greenVoxelsBase, numberOfVoxels);
			volume->setGreenByteVoxelsBase(greenVoxelsBase);
			Voxel* greenVoxels=greenVoxelsBase+borderSize;
			int increments[3];
			increments[2]=1;
			for (int i=2; i>0; --i) {
				increments[i-1]=increments[i]*(size[i]+2*borderSize);
				greenVoxels+=borderSize*increments[i-1];
			}
			volume->setGreenVoxels(greenVoxels);
			Voxel* blueVoxelsBase=new Voxel[numberOfVoxels];
			byteRawFileB.read(blueVoxelsBase, numberOfVoxels);
			volume->setBlueByteVoxelsBase(blueVoxelsBase);
			Voxel* blueVoxels=blueVoxelsBase+borderSize;
			for (int i=2; i>0; --i) {
				blueVoxels+=borderSize*increments[i-1];
			}
			volume->setBlueVoxels(blueVoxels);
			volume->setIncrements(increments);
			volume->setVoxels(blueVoxels); // remove when fully operational
		}
	} else {
		std::cout
				<< "Error: size was not set on command-line for MultiComponentByteRaw volume!"
				<< std::endl;
	}
}

/*
 * readMultiComponentByteRaw - Read a byte raw volume renderering data set (.raw).
 * 
 * parameter redFilename - const char*
 * parameter greenFilename - const char*
 * parameter blueFilename - const char*
 * parameter volume - Volume
 */
void MultiComponentByteRaw::readMultiComponentByteRaw(const char* redFilename,
		const char* greenFilename, const char* blueFilename, Volume* volume) {
	/* Open the volume component files */
	Misc::File redByteRawFile(redFilename, "rb", Misc::File::LittleEndian);
	Misc::File greenByteRawFile(greenFilename, "rb", Misc::File::LittleEndian);
	Misc::File blueByteRawFile(blueFilename, "rb", Misc::File::LittleEndian);
	const int* size = volume->getSize();
	const Scalar* scale = volume->getScale();
	volume->setNumberOfComponents(3);
	volume->setTypeOfComponents(7);
	if (size[0]!=0&&size[1]!=0&&size!=0) {
		int borderSize = 0;
		volume->setBorderSize(borderSize);
		/* Set the voxel block's position and size, and the slice center to the block's center */
		Point origin=Point::origin;
		Size extent;
		Point center;
		for (int i=0; i<3; ++i) {
			extent[i]=Scalar(size[i])*scale[i];
			center[i]=origin[i]+extent[i]*Scalar(0.5);
		}
		volume->setOrigin(origin);
		volume->setExtent(extent);
		volume->setVolumeBox();
		volume->setCenter(center);
		/* Create a voxel array: */
		int numberOfVoxels=(size[0]+2*borderSize)*(size[1]+2*borderSize)
				*(size[2]+2*borderSize);
		volume->setNumberOfVoxels(numberOfVoxels);
		/* Calculate the address of the voxel block: */
		Voxel* redVoxelsBase=new Voxel[numberOfVoxels];
		redByteRawFile.read(redVoxelsBase, numberOfVoxels);
		volume->setRedByteVoxelsBase(redVoxelsBase);
		Voxel* redVoxels=redVoxelsBase+borderSize;
		int increments[3];
		increments[2]=1;
		for (int i=2; i>0; --i) {
			increments[i-1]=increments[i]*(size[i]+2*borderSize);
			redVoxels+=borderSize*increments[i-1];
		}
		volume->setRedVoxels(redVoxels);
		Voxel* greenVoxelsBase=new Voxel[numberOfVoxels];
		greenByteRawFile.read(greenVoxelsBase, numberOfVoxels);
		volume->setGreenByteVoxelsBase(greenVoxelsBase);
		Voxel* greenVoxels=greenVoxelsBase+borderSize;
		for (int i=2; i>0; --i) {
			greenVoxels+=borderSize*increments[i-1];
		}
		volume->setGreenVoxels(greenVoxels);
		Voxel* blueVoxelsBase=new Voxel[numberOfVoxels];
		blueByteRawFile.read(blueVoxelsBase, numberOfVoxels);
		volume->setBlueByteVoxelsBase(blueVoxelsBase);
		Voxel* blueVoxels=greenVoxelsBase+borderSize;
		for (int i=2; i>0; --i) {
			blueVoxels+=borderSize*increments[i-1];
		}
		volume->setBlueVoxels(blueVoxelsBase);
		volume->setIncrements(increments);
		volume->setVoxels(blueVoxels); // remove when fully operational
	} else {
		std::cout
				<< "Error: size was not set on command-line for MultiComponentByteRaw volume!"
				<< std::endl;
	}
}
