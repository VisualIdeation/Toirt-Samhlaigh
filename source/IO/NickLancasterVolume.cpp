/*
 * NickLancasterVolume.cpp - Methods for NickLancasterVolume class.
 * 
 * Author: Patrick O'Leary
 * Created: October 24, 2007
 * Copyright: 2007
 */
#include <iostream>

/* Vrui includes */
#include <Math/Math.h>
#include <Misc/File.h>

#include <DATA/Volume.h>
#include <IO/NickLancasterVolume.h>
#include <UTILITY/StringTokenizer.h>
#include <UTILITY/Stringify.h>

/* 
 * NickLancasterVolume - Constructor for NickLancasterVolume class.
 */
NickLancasterVolume::NickLancasterVolume() {
}

/*
 * ~FloatVolume - Destructor for FloatVolume class.
 */
NickLancasterVolume::~NickLancasterVolume() {
}

/*
 * readNickLancasterVolume - Read a Nick Lancaster volume volume renderering data set (.nlv).
 * 
 * parameter filename - const char*
 * parameter volume - Volume
 */
void NickLancasterVolume::readNickLancasterVolume(const char* filename, Volume* volume) {
	Misc::File nickLancasterVolumeFile(filename, "rb", Misc::File::BigEndian);
	if (!readHeader(nickLancasterVolumeFile, volume)) {
		std::cout << "Error: Reading Nick Lancaster data set!" << std::endl;
	}
	const int* size = volume->getSize();
	int borderSize=0;
	volume->setBorderSize(borderSize);
	Point origin=volume->getOrigin();
	const Scalar* scale = volume->getScale();
	Size extent;
	Point center;
	for (int i=0; i<3; ++i) {
		extent[i]=Scalar(size[i])*scale[i];
		center[i]=origin[i]+extent[i]*Scalar(0.5);
	}
	volume->setExtent(extent);
	volume->setVolumeBox();
	volume->setCenter(center);
	int numberOfVoxels=(size[0]+2*borderSize)*(size[1]+2*borderSize) *(size[2] +2*borderSize);
	volume->setNumberOfVoxels(numberOfVoxels);
	IntVoxel* intVoxelsBase=new IntVoxel[numberOfVoxels];
	size_t nov = size_t(numberOfVoxels);
	nickLancasterVolumeFile.read(intVoxelsBase, nov);
	//volume->setIntVoxelsBase(intVoxelsBase);
	int _min, _max;
	_min=_max=intVoxelsBase[0];
	for (int i=1; i<numberOfVoxels; ++i) {
		if (_min>intVoxelsBase[i])
			_min=intVoxelsBase[i];
		if (_max<intVoxelsBase[i])
			_max=intVoxelsBase[i];
	}
	Voxel* voxelsBase=new Voxel[numberOfVoxels];
	for (int i=0; i<numberOfVoxels; i++) {
		voxelsBase[i]=(unsigned char)(((Scalar(intVoxelsBase[i])-Scalar(_min))/(Scalar(_max)-Scalar(_min)))*Scalar(255));
	}
	delete[] intVoxelsBase;
	volume->setByteVoxelsBase(voxelsBase);
	Voxel* voxels=voxelsBase+borderSize;
	int increments[3];
	increments[2]=1;
	for (int i=2; i>0; --i) {
		increments[i-1]=increments[i]*(size[i]+2*borderSize);
		voxels+=borderSize*increments[i-1];
	}
	volume->setIncrements(increments);
	volume->setVoxels(voxels);
	//volume->resize((unsigned char)128);
}

int NickLancasterVolume::readHeader(Misc::File nickLancasterVolumeFile, Volume* volume) {
	int sizeRead=0, scaleRead=0, originRead=0;
	char* buffer=new char[256];
	nickLancasterVolumeFile.gets(buffer, 256);
	if (strncmp("# vtk DataFile Version", buffer, 20)) {
		std::cout << "Error: Unrecognized file type: "<< buffer << std::endl;
		delete[] buffer;
		return 0;
	}
	nickLancasterVolumeFile.gets(buffer, 256);
	nickLancasterVolumeFile.gets(buffer, 256);
	if (!strncmp(LowerCase(buffer), "ascii", 5)) {
		std::cout << "Error: Unrecognized file type: "<< buffer << std::endl;
		delete[] buffer;
		return 0;
	} else if (!strncmp(LowerCase(buffer), "binary", 6)) {
	} else {
		std::cout << "Error: Unrecognized file type: "<< buffer<< std::endl;
		delete[] buffer;
		return 0;
	}
	nickLancasterVolumeFile.gets(buffer, 256); // Note: Only supporting structured_points
	std::string bufferString = buffer;
	StringTokenizer stringTokenizer(bufferString, " ");
	if ( !strncmp(LowerCase(stringTokenizer.nextToken()), "dataset", (unsigned long)7) ) {
		if (strncmp(LowerCase(stringTokenizer.nextToken()), "structured_points", 17) ) {
			std::cout << "Error: Cannot read dataset type not equal to structured_points!" << std::endl;
			delete[] buffer;
			return 0;
		}
		bool done=false;
		while (!done) {
			if (nickLancasterVolumeFile.gets(buffer, 256)==NULL) {
				std::cout << "Error: Unexpected EOF!" << std::endl;
				delete[] buffer;
				return 0;
			}
			std::string bufferString = buffer;
			StringTokenizer stringTokenizer(bufferString, " ");
			std::string token = stringTokenizer.nextToken();
			if (!strncmp(LowerCase(token), "dimensions", 10)) {
				int size[3];
				if (stringTokenizer.getNumberOfTokens()==3) {
					size[0]=stringTokenizer.nextIntToken();
					size[1]=stringTokenizer.nextIntToken();
					size[2]=stringTokenizer.nextIntToken();
					volume->setSize(size);
					sizeRead = 1;
				} else {
					std::cout << "Error: Cannot read dataset dimensions!" << std::endl;
					delete[] buffer;
					return 0;
				}
			} else if (!strncmp(LowerCase(token), "aspect-ratio", 12)||!strncmp(LowerCase(token), "spacing", 7)) {
				Scalar scale[3];
				if (stringTokenizer.getNumberOfTokens()==3) {
					scale[0]=stringTokenizer.nextFloatToken();
					scale[1]=stringTokenizer.nextFloatToken();
					scale[2]=stringTokenizer.nextFloatToken();
					volume->setScale(scale);
					scaleRead = 1;
				} else {
					std::cout << "Error: Cannot read dataset spacing or aspect_ratio (scale)!" << std::endl;
					delete[] buffer;
					return 0;
				}
			} else if (!strncmp(LowerCase(token), "origin", 6)) {
				Point origin=Point::origin;
				Scalar value;
				if (stringTokenizer.getNumberOfTokens()==3) {
					value=stringTokenizer.nextFloatToken();
					value=stringTokenizer.nextFloatToken();
					value=stringTokenizer.nextFloatToken();
					volume->setOrigin(origin);
					originRead = 1;
				} else {
					std::cout << "Error: Cannot read dataset origin!" << std::endl;
					delete[] buffer;
					return 0;
				}
			} else if (!strncmp(LowerCase(token), "point_data", 10)) {
				if (stringTokenizer.hasMoreTokens()) {
					int value = stringTokenizer.nextIntToken();
				} else {
					std::cout << "Error: Cannot read dataset number of points!" << std::endl;
					delete[] buffer;
					return 0;
				}
			} else if (!strncmp(LowerCase(token), "cell_data", 9)) {
				if (stringTokenizer.hasMoreTokens()) {
					int value = stringTokenizer.nextIntToken();
				} else {
					std::cout << "Error: Cannot read dataset number of cells!" << std::endl;
					delete[] buffer;
					return 0;
				}
			} else if (!strncmp(LowerCase(token), "scalars", 7)) {
				std::string field = stringTokenizer.nextToken();
				std::string datatype;
				token=stringTokenizer.nextToken();
				if ( !strncmp(LowerCase(token), "bit", 3) ) {
					std::cout << "Error: Cannot read scalar field of type bit!" << std::endl;
					delete[] buffer;
					return 0;
				} else if ( !strncmp(LowerCase(token), "char", 4) ) {
					std::cout << "Error: Cannot read scalar field of type char!" << std::endl;
					delete[] buffer;
					return 0;
				} else if ( !strncmp(LowerCase(token), "unsigned-char", 13) ) {
					std::cout << "Error: Cannot read scalar field of type unsigned_char!" << std::endl;
					delete[] buffer;
					return 0;
				} else if ( !strncmp(LowerCase(token), "short", 5) ) {
					std::cout << "Error: Cannot read scalar field of type short!" << std::endl;
					delete[] buffer;
					return 0;
				} else if ( !strncmp(LowerCase(token), "unsigned-short", 14) ) {
					std::cout << "Error: Cannot read scalar field of type unsigned_short!" << std::endl;
					delete[] buffer;
					return 0;
				} else if ( !strncmp(LowerCase(token), "int", 3) ) {
					datatype = "int";
				} else if ( !strncmp(LowerCase(token), "unsigned_int", 12) ) {
					std::cout << "Error: Cannot read scalar field of type unsigned_int!" << std::endl;
					delete[] buffer;
					return 0;
				} else if ( !strncmp(LowerCase(token), "long", 4) ) {
					std::cout << "Error: Cannot read scalar field of type long!" << std::endl;
					delete[] buffer;
					return 0;
				} else if ( !strncmp(LowerCase(token), "unsigned-long", 13) ) {
					std::cout << "Error: Cannot read scalar field of type unsigned_long!" << std::endl;
					delete[] buffer;
					return 0;
				} else if ( !strncmp(LowerCase(token), "float", 5) ) {
					std::cout << "Error: Cannot read scalar field of type float!" << std::endl;
					delete[] buffer;
					return 0;
				} else if ( !strncmp(LowerCase(token), "double", 6) ) {
					std::cout << "Error: Cannot read scalar field of type double!" << std::endl;
					delete[] buffer;
					return 0;
				}
			} else if (!strcmp(LowerCase(token), "lookup_table")) {
				token=stringTokenizer.nextToken();
				if (!strncmp(LowerCase(token), "default", 7)) {
					done=true;
				} else {
					std::cout << "Error: Cannot read a data set with a lookup_table other than default!" << std::endl;
					delete[] buffer;
					return 0;
				}
			} else {
				std::cout << "Unsupported attribute type: " << token << " " << LowerCase(token) <<"!" << std::endl;
				delete[] buffer;
				return 0;
			}
		}
	}
	if (!sizeRead||!scaleRead||!originRead) {
		std::cout <<"Not all header data was read to form the file." << std::endl;
		delete[] buffer;
		return 0;
	}
	delete[] buffer;
	return 1;
}

const char* NickLancasterVolume::LowerCase(std::string _string) {
	for (size_t i = 0; i < _string.length(); ++i)
		_string[i] = tolower(_string[i]);
	return _string.c_str();
}
