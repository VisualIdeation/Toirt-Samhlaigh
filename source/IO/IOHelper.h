/*
 * IOHelper.h - Class of helpers for I/O.
 *
 * Author: Patrick O'Leary
 * Created: October 19, 2007
 * Copyright: 2007
 *
 * Changes:
 *
 *     12Apr2009    Phil McDonald: added SEG-Y file support
 */
#ifndef IOHELPER_H_
#define IOHELPER_H_

/* file type defines */
#define UNKNOWN_FILE -1
#define BYTE_VOLUME_FILE 0
#define DAT_VOLUME_FILE 1
#define FLOAT_VOLUME_FILE 2
#define RAW_VOLUME_FILE 3
#define NL_VOLUME_FILE 4
#define SEGY3D_VOLUME_FILE 5
#define SEGY_SLICE_FILE 6

#include <iostream>
#include <stdexcept>

/* Vrui includes */
#include <Misc/ThrowStdErr.h>
#include <Misc/Timer.h>

/* Vrui includes to use the Vrui interface */
#include <Vrui/Vrui.h>

#include <DATA/Volume.h>
#include <IO/ByteRaw.h>
#include <IO/ByteVolume.h>
#include <IO/DatVolume.h>
#include <IO/FloatVolume.h>
#include <IO/MultiComponentByteRaw.h>
#include <IO/NickLancasterVolume.h>
#include <IO/SegyVolume.h>

class IOHelper {
public:
	IOHelper();
	~IOHelper();
	/*
	 * getFileExtension - Method to determine the file extension.
	 *
	 * parameter filename - const char*
	 */
	static int getFileExtension(const char* filename) {
		int type= UNKNOWN_FILE;
		for (const char* charPointer=filename; *charPointer!='\0'; ++charPointer)
			if (*charPointer=='.') {
				const char* extensionPointer=charPointer;
				if (strcasecmp(extensionPointer, ".vol")==0) {
					type = BYTE_VOLUME_FILE;
					break;
				} else if (strcasecmp(extensionPointer, ".dat")==0) {
					type = DAT_VOLUME_FILE;
					break;
				} else if (strcasecmp(extensionPointer, ".fvol")==0) {
					type = FLOAT_VOLUME_FILE;
					break;
				} else if (strcasecmp(extensionPointer, ".raw")==0) {
					type = RAW_VOLUME_FILE;
					break;
                } else if (strcasecmp(extensionPointer, ".nlv")==0) {
                    type = NL_VOLUME_FILE;
                    break;
                } else if (strcasecmp(extensionPointer, ".segy")==0) {
                    type = SEGY_SLICE_FILE;
                    break;
                } else if (strcasecmp(extensionPointer, ".sgy")==0) {
                    type = SEGY_SLICE_FILE;
                    break;
                } else if (strcasecmp(extensionPointer, ".segy3D")==0) {
                    type = SEGY3D_VOLUME_FILE;
                    break;
                } else if (strcasecmp(extensionPointer, ".sgy3D")==0) {
                    type = SEGY3D_VOLUME_FILE;
                    break;
				}
			}
		return type;
	}
	;
	/*
	 * readVolume - Method to read volume.
	 * 		throws std::runtime_error for I/O problems
	 *
	 * parameter filename - const char*
	 * parameter volume - Volume
	 */
	static void readVolume(const char* filename, Volume* volume) {
		int type = getFileExtension(filename);
		switch (type) {
		case UNKNOWN_FILE:
			Misc::throwStdErr("IOHelper::readVolume: Unknown file type");
			break;
		case BYTE_VOLUME_FILE: {
			try {
				ByteVolume byteVolume;
				Misc::Timer timer;
				byteVolume.readByteVolume(filename, volume);
				timer.elapse();
				if(Vrui::getNodeIndex()==0) std::cout<<"Time to load data set: "<<timer.getTime()*1000.0<<" ms"<<std::endl;
			}
			catch(std::runtime_error e) {
				Misc::throwStdErr("IOHelper::readVolume: Could not load data set due to exception %s",e.what());
			}
		}
			break;
		case DAT_VOLUME_FILE: {
			try {
				DatVolume datVolume;
				Misc::Timer timer;
				datVolume.readDatVolume(filename, volume);
				timer.elapse();
				if(Vrui::getNodeIndex()==0) std::cout<<"Time to load data set: "<<timer.getTime()*1000.0<<" ms"<<std::endl;
			}
			catch(std::runtime_error e) {
				Misc::throwStdErr("IOHelper::readVolume: Could not load data set due to exception %s",e.what());
			}
		}
			break;
		case FLOAT_VOLUME_FILE: {
			try {
				FloatVolume floatVolume;
				Misc::Timer timer;
				floatVolume.readFloatVolume(filename, volume);
				timer.elapse();
				if(Vrui::getNodeIndex()==0) std::cout<<"Time to load data set: "<<timer.getTime()*1000.0<<" ms"<<std::endl;
			}
			catch (std::runtime_error e) {
				Misc::throwStdErr("IOHelper::readVolume: Could not load data set due to exception %s",e.what());
			}
		}
			break;
		case RAW_VOLUME_FILE: {
			try {
				ByteRaw byteRaw;
				Misc::Timer timer;
				byteRaw.readByteRaw(filename, volume);
				timer.elapse();
				if(Vrui::getNodeIndex()==0) std::cout<<"Time to load data set: "<<timer.getTime()*1000.0<<" ms"<<std::endl;
			}
			catch(std::runtime_error e) {
				Misc::throwStdErr("IOHelper::readVolume: Could not load data set due to exception %s",e.what());
			}
		}
			break;
        case NL_VOLUME_FILE: {
            try {
                NickLancasterVolume nickLancasterVolume;
                Misc::Timer timer;
                nickLancasterVolume.readNickLancasterVolume(filename, volume);
                timer.elapse();
                if(Vrui::getNodeIndex()==0) std::cout<<"Time to load data set: "<<timer.getTime()*1000.0<<" ms"<<std::endl;
            }
            catch(std::runtime_error e) {
                Misc::throwStdErr("IOHelper::readVolume: Could not load data set due to exception %s",e.what());
            }
        }
            break;
        case SEGY_SLICE_FILE: {
            try {
                SegyVolume segyVolume;
                Misc::Timer timer;
                segyVolume.readSegySlice(filename, volume);
                timer.elapse();
                if(Vrui::getNodeIndex()==0) std::cout<<"Time to load data set: "<<timer.getTime()*1000.0<<" ms"<<std::endl;
            }
            catch(std::runtime_error e) {
                Misc::throwStdErr("IOHelper::readVolume: Could not load data set due to exception %s",e.what());
            }
        }
            break;
        case SEGY3D_VOLUME_FILE: {
            try {
                SegyVolume segyVolume;
                Misc::Timer timer;
                segyVolume.readSegyVolume(filename, volume);
                timer.elapse();
                if(Vrui::getNodeIndex()==0) std::cout<<"Time to load data set: "<<timer.getTime()*1000.0<<" ms"<<std::endl;
            }
            catch(std::runtime_error e) {
                Misc::throwStdErr("IOHelper::readVolume: Could not load data set due to exception %s",e.what());
            }
        }
            break;
		default:
			Misc::throwStdErr("IOHelper::readVolume: Unknown file type");
		}
	}
	;
	/*
	 * readVolume - Method to read volume.
	 * 		throws std::runtime_error for I/O problems
	 *
	 * parameter filename - const char*
	 * parameter volume - Volume
	 * parameter flag - int
	 */
	static void readVolume(const char* filename, Volume* volume, int flag) {
		int type = getFileExtension(filename);
		if (type == RAW_VOLUME_FILE) {
			try {
				MultiComponentByteRaw multiComponentByteRaw;
				Misc::Timer timer;
				multiComponentByteRaw.readMultiComponentByteRaw(filename, volume, flag);
				timer.elapse();
				if(Vrui::getNodeIndex()==0) std::cout<<"Time to load data set: "<<timer.getTime()*1000.0<<" ms"<<std::endl;
			}
			catch(std::runtime_error e) {
				Misc::throwStdErr("IOHelper::readVolume: Could not load data set due to exception %s",e.what());
			}
		} else {
			Misc::throwStdErr("IOHelper::readVolume: Unknown file type");
		}
	}
	;
	/*
	 * readVolume - Method to read volume.
	 * 		throws std::runtime_error for I/O problems
	 *
	 * parameter filenameA - const char*
	 * parameter filenameB - const char*
	 * parameter volume - Volume
	 * parameter flag - int
	 */
	static void readVolume(const char* filenameA, const char* filenameB, Volume* volume, int flag) {
		int typeA = getFileExtension(filenameA);
		int typeB = getFileExtension(filenameB);
		if (typeA == RAW_VOLUME_FILE && typeB == RAW_VOLUME_FILE) {
			try {
				MultiComponentByteRaw multiComponentByteRaw;
				Misc::Timer timer;
				multiComponentByteRaw.readMultiComponentByteRaw(filenameA, filenameB, volume, flag);
				timer.elapse();
				if(Vrui::getNodeIndex()==0) std::cout<<"Time to load data set: "<<timer.getTime()*1000.0<<" ms"<<std::endl;
			}
			catch(std::runtime_error e) {
				Misc::throwStdErr("IOHelper::readVolume: Could not load data set due to exception %s",e.what());
			}
		} else {
			Misc::throwStdErr("IOHelper::readVolume: Unknown file type");
		}
	}
	;
	/*
	 * readVolume - Method to read volume.
	 * 		throws std::runtime_error for I/O problems
	 *
	 * parameter redFilename - const char*
	 * parameter greenFilename - const char*
	 * parameter blueFilename - const char*
	 * parameter volume - Volume
	 * parameter flag - int
	 */
	static void readVolume(const char* redFilename, const char* greenFilename, const char* blueFilename, Volume* volume) {
		int redType = getFileExtension(redFilename);
		int greenType = getFileExtension(greenFilename);
		int blueType = getFileExtension(blueFilename);
		if (redType == RAW_VOLUME_FILE && greenType == RAW_VOLUME_FILE && blueType == RAW_VOLUME_FILE) {
			try {
				MultiComponentByteRaw multiComponentByteRaw;
				Misc::Timer timer;
				multiComponentByteRaw.readMultiComponentByteRaw(redFilename, greenFilename, blueFilename, volume);
				timer.elapse();
				if(Vrui::getNodeIndex()==0) std::cout<<"Time to load data set: "<<timer.getTime()*1000.0<<" ms"<<std::endl;
			}
			catch(std::runtime_error e) {
				Misc::throwStdErr("IOHelper::readVolume: Could not load data set due to exception %s",e.what());
			}
		} else {
			Misc::throwStdErr("IOHelper::readVolume: Unknown file type");
		}
	}
	;
};

#endif /*IOHELPER_H_*/
