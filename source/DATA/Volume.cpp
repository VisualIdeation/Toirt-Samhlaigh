/*
 * Volume.cpp - Methods for Volume class.
 *
 * Author: Patrick O'Leary
 * Created: October 19, 2007
 * Copyright: 2007
 */
#include <cfloat>

#include <DATA/Volume.h>
#include <DATASTRUCTURE/BoundingBox.h>
#include <UTILITY/Stringify.h>

/*
 *  Volume - Constructor for Volume class.
 */
Volume::Volume() :
    blueByteVoxelsBase(NULL), blueVoxels(NULL), byteVoxelsBase(NULL), floatVoxelsBase(NULL), gradients(NULL), greenByteVoxelsBase(
            NULL), greenVoxels(NULL), intVoxelsBase(NULL), leafSize(4), maximumPriorityQueueSize(0), numberOfComponents(0),
            ratioOfVisibilityThreshold(1.0), redByteVoxelsBase(NULL), redVoxels(NULL), sliceFactor(1.5), textureSize(32), voxels(
                    NULL) {
    increments = new int[3];
    scale = new float[3];
    size = new int[3];
    sliceIndexDir = new int[3];
    initialize();
} // end Volume()

/*
 * ~Volume - Destructor for Volume class.
 */
Volume::~Volume() {
    voxels = NULL;
    blueVoxels = NULL;
    greenVoxels = NULL;
    redVoxels = NULL;
    if (hasBlueByteVoxelsBase()) {
        delete[] blueByteVoxelsBase;
    }
    if (hasByteVoxelsBase()) {
        delete[] byteVoxelsBase;
    }
    if (hasFloatVoxelsBase()) {
        delete[] floatVoxelsBase;
    }
    if (gradients != NULL)
        delete[] gradients;
    if (hasGreenByteVoxelsBase()) {
        delete[] greenByteVoxelsBase;
    }
    delete[] increments;
    if (hasIntVoxelsBase()) {
        delete[] intVoxelsBase;
    }
    if (hasRedByteVoxelsBase()) {
        delete[] redByteVoxelsBase;
    }
    delete[] size;
    delete[] scale;
    delete volumeBox;
} // end ~Volume

/*
 * calculateGradient
 */
void Volume::calculateGradient(void) {
    int depth = getDepth();
    int height = getHeight();
    int width = getWidth();
    maximumGradient = FLT_MIN;
    minimumGradient = FLT_MAX;
    gradients = new float[4 * (width * height * depth)];
    for (int i = 0; i < 4 * (width * height * depth); i++)
        gradients[i] = 0.0f;
    for (int k = 1; k < depth - 1; k++) {
        for (int j = 1; j < height - 1; j++) {
            for (int i = 1; i < width - 1; i++) {
                float x = float(getVoxel(i+1 + j * width + k * (width * height))) - float(getVoxel(i-1 + j * width + k * (width * height)));
                float y = float(getVoxel(i + (j+1) * width + k * (width * height))) - float(getVoxel(i + (j-1) * width + k * (width * height)));
                float z = float(getVoxel(i + j * width + (k+1) * (width * height))) - float(getVoxel(i + j * width + (k-1) * (width * height)));
                float m = x*x + y*y + z*z;
                gradients[4 * (i + j * width + k * (width * height)) + 0] = x;
                gradients[4 * (i + j * width + k * (width * height)) + 1] = y;
                gradients[4 * (i + j * width + k * (width * height)) + 2] = z;
                gradients[4 * (i + j * width + k * (width * height)) + 3] = m;
                if (maximumGradient < m) {
                    maximumGradient = m;
                }
                if (minimumGradient > m) {
                    minimumGradient = m;
                }
            } // end for
        } // end for
    } // end for
} // end calculateGradient()

/*
 * calculateMaximum
 *
 * parameter column - int
 * parameter row - int
 * parameter slab - int
 * parameter width - int
 * parameter height - int
 * parameter depth - int
 * return - int
 */
int Volume::calculateMaximum(int column, int row, int slab, int width, int height, int depth) {
    int maximum = 0;
    int w = getWidth();
    int h = getHeight();
    for (int k = 0; k < depth; k++) {
        for (int j = 0; j < height; j++) {
            for (int i = 0; i < width; i++) {
                int tmp = int(voxels[((slab + k) * (w * h)) + ((row + j) * (w)) + (column + i)]);

                if (maximum < tmp) {
                    maximum = tmp;
                } // end if
            } // end for
        } // end for
    } // end for
    return maximum;
} // end calculateMaximum()

/*
 * calculateMinimum
 *
 * parameter column - int
 * parameter row - int
 * parameter slab - int
 * parameter width - int
 * parameter height - int
 * parameter depth - int
 * return - int
 */
int Volume::calculateMinimum(int column, int row, int slab, int width, int height, int depth) {
    int minimum = 255;
    int w = getWidth();
    int h = getHeight();
    for (int k = 0; k < depth; k++) {
        for (int j = 0; j < height; j++) {
            for (int i = 0; i < width; i++) {
                int tmp = int(voxels[((slab + k) * (w * h)) + ((row + j) * (w)) + (column + i)]);

                if (minimum > tmp) {
                    minimum = tmp;
                } // end if
            } // end for
        } // end for
    } // end for
    return minimum;
} // end calculateMinimum()

/*
 * getBlueByteVoxelsBase
 *
 * return - unsigned char*
 */
const unsigned char* Volume::getBlueByteVoxelsBase(void) const {
    return blueByteVoxelsBase;
} // end getBlueByteVoxelsBase()

/*
 * hasBlueByteVoxelsBase
 *
 * return - bool
 */
bool Volume::hasBlueByteVoxelsBase(void) const {
    return blueByteVoxelsBase != NULL;
} // end hasBlueByteVoxelsBase()

/*
 * setBlueByteVoxelsBase
 *
 * parameter _blueByteVoxelsBase - unsigned char*
 */
void Volume::setBlueByteVoxelsBase(unsigned char* _blueByteVoxelsBase) {
    blueByteVoxelsBase = _blueByteVoxelsBase;
} // end setBlueByteVoxelsBase()

/*
 * getBlueVoxel
 *
 * parameter i - int
 * return - unsigned char
 */
unsigned char Volume::getBlueVoxel(int i) const {
    return blueVoxels[i];
} // getBlueVoxel()

/*
 * getBlueVoxel
 *
 * parameter i - int
 * parameter j - int
 * parameter k - int
 * return - unsigned char
 */
unsigned char Volume::getBlueVoxel(int i, int j, int k) const {
    return blueVoxels[i * increments[0] + j * increments[1] + k];
} // getBlueVoxel()

/*
 * getBlueVoxelPtr
 *
 * parameter i - int
 * parameter j - int
 * parameter k - int
 * return - unsigned char*
 */
const unsigned char * Volume::getBlueVoxelPtr(int i, int j, int k) const {
    return blueVoxels + (i * increments[0] + j * increments[1] + k);
} // end getBlueVoxelPtr()

/*
 * getBlueVoxels
 *
 * return - const unsigned char*
 */
const unsigned char* Volume::getBlueVoxels(void) const {
    return blueVoxels;
} // end getBlueVoxels()

/*
 * hasBlueVoxels
 *
 * return - bool
 */
bool Volume::hasBlueVoxels(void) const {
    return blueVoxels != NULL;
} // end hasBlueVoxels()

/*
 * setBlueVoxels
 *
 * parameter _blueVoxels - unsigned char*
 */
void Volume::setBlueVoxels(unsigned char* _blueVoxels) {
    blueVoxels = _blueVoxels;
} // end setBlueVoxels()

/*
 * getBorderSize
 *
 * return - int
 */
int Volume::getBorderSize(void) const {
    return borderSize;
} // end getBorderSize()

/*
 * setBorderSize
 *
 * parameter _borderSize - int
 */
void Volume::setBorderSize(int _borderSize) {
    borderSize = _borderSize;
} // end setBorderSize()

/*
 * getByteVoxelsBase
 *
 * return - unsigned char*
 */
const unsigned char* Volume::getByteVoxelsBase(void) const {
    return byteVoxelsBase;
} // end getByteVoxelsBase()

/*
 * hasByteVoxelsBase
 *
 * return - bool
 */
bool Volume::hasByteVoxelsBase(void) const {
    return byteVoxelsBase != NULL;
} // end hasByteVoxelsBase()

/*
 * setByteVoxelsBase
 *
 * parameter _byteVoxelsBase - unsigned char*
 */
void Volume::setByteVoxelsBase(unsigned char* _byteVoxelsBase) {
    byteVoxelsBase = _byteVoxelsBase;
} // end setByteVoxelsBase()

/*
 * getDepth
 *
 * return - int
 */
int Volume::getDepth(void) {
    return size[2];
} // end getDepth()

/*
 * setDepth
 *
 * parameter depth - int
 */
void Volume::setDepth(int depth) {
    size[2] = depth;
} // end setDepth()

/*
 * getFloatVoxelsBase
 *
 * return - const float*
 */
const float* Volume::getFloatVoxelsBase(void) const {
    return floatVoxelsBase;
} // end getFloatVoxelsBase()

/*
 * hasFloatVoxelsBase
 *
 * return - bool
 */
bool Volume::hasFloatVoxelsBase(void) const {
    return floatVoxelsBase != NULL;
} // end hasFloatVoxelsBase()

/*
 * setFloatVoxelsBase
 *
 * parameter _floatVoxelsBase - float*
 */
void Volume::setFloatVoxelsBase(float* _floatVoxelsBase) {
    floatVoxelsBase = _floatVoxelsBase;
} // end setFloatVoxelsBase()

/*
 * getGradient
 *
 * parameter i - int
 * parameter j - int
 * parameter k - int
 * return - float
 */
float Volume::getGradient(int i, int j, int k) {
    return gradients[4 * (i * increments[0] + j * increments[1] + k) + 3];
} // getGradient()

/*
 * getGradient
 *
 * parameter i - int
 * return - float
 */
float Volume::getGradient(int i) {
    return gradients[4 * i + 3];
} // getGradient()

/*
 * getGreenByteVoxelsBase
 *
 * return - unsigned char*
 */
const unsigned char* Volume::getGreenByteVoxelsBase(void) const {
    return greenByteVoxelsBase;
} // end getGreenByteVoxelsBase()

/*
 * hasGreenByteVoxelsBase
 *
 * return - bool
 */
bool Volume::hasGreenByteVoxelsBase(void) const {
    return greenByteVoxelsBase != NULL;
} // end hasGreenByteVoxelsBase()

/*
 * setGreenByteVoxelsBase
 *
 * parameter _greenByteVoxelsBase - unsigned char*
 */
void Volume::setGreenByteVoxelsBase(unsigned char* _greenByteVoxelsBasee) {
    greenByteVoxelsBase = _greenByteVoxelsBasee;
} // end setGreenByteVoxelsBase()

/*
 * getGreenVoxel
 *
 * parameter i - int
 * return - unsigned char
 */
unsigned char Volume::getGreenVoxel(int i) const {
    return greenVoxels[i];
} // getGreenVoxel()

/*
 * getGreenVoxel
 *
 * parameter i - int
 * parameter j - int
 * parameter k - int
 * return - unsigned char
 */
unsigned char Volume::getGreenVoxel(int i, int j, int k) const {
    return greenVoxels[i * increments[0] + j * increments[1] + k];
} // getGreenVoxel()

/*
 * getGreenVoxelPtr
 *
 * parameter i - int
 * parameter j - int
 * parameter k - int
 * return - unsigned char*
 */
const unsigned char* Volume::getGreenVoxelPtr(int i, int j, int k) const {
    return greenVoxels + (i * increments[0] + j * increments[1] + k);
} // end getGreenVoxelPtr()

/*
 * getGreenVoxels
 *
 * return - const unsigned char*
 */
const unsigned char* Volume::getGreenVoxels(void) const {
    return greenVoxels;
} // end getGreenVoxels()

/*
 * hasGreenVoxels
 *
 * return - bool
 */
bool Volume::hasGreenVoxels(void) const {
    return greenVoxels != NULL;
} // end hasGreenVoxels()

/*
 * setGreenVoxels
 *
 * parameter _greenVoxels - unsigned char*
 */
void Volume::setGreenVoxels(unsigned char* _greenVoxels) {
    greenVoxels = _greenVoxels;
} // end setGreenVoxels()

/*
 * getHeight
 *
 * return - int
 */
int Volume::getHeight(void) {
    return size[1];
} // end getHeight()

/*
 * setHeight
 *
 * parameter height - int
 */
void Volume::setHeight(int height) {
    size[1] = height;
} // end setHeight()

/*
 * getIncrements
 *
 * return - const int*
 */
const int* Volume::getIncrements(void) {
    return increments;
} // end getIncrements()

/*
 * getIncrements
 *
 * parameter dimension - int
 * return - int
 */
int Volume::getIncrements(int dimension) const {
    return increments[dimension];
} // end getIncrements()

/*
 * setIncrements
 *
 * parameter _increments[3] - const int[3]
 */
void Volume::setIncrements(const int _increments[3]) {
    increments[0] = _increments[0];
    increments[1] = _increments[1];
    increments[2] = _increments[2];
} // end setIncrements()

/*
 * getIntVoxelsBase
 *
 * return - int*
 */
const int* Volume::getIntVoxelsBase(void) const {
    return intVoxelsBase;
} // end getIntVoxelsBase()

/*
 * hasIntVoxelsBase
 *
 * return - bool
 */
bool Volume::hasIntVoxelsBase(void) const {
    return intVoxelsBase != NULL;
} // end hasIntVoxelsBase()

/*
 * setIntVoxelsBase
 *
 * parameter _intVoxelsBase - int*
 */
void Volume::setIntVoxelsBase(int* _intVoxelsBase) {
    intVoxelsBase = _intVoxelsBase;
} // end setIntVoxelsBase()

/*
 * getLeafSize
 *
 * return - int
 */
int Volume::getLeafSize(void) {
    return leafSize;
} // end getLeafSize()

/*
 * setLeafSize
 *
 * parameter _leafSize - int
 */
void Volume::setLeafSize(int _leafSize) {
    leafSize = _leafSize;
} // end setLeafSize()

/*
 * getMaximumGradient
 *
 * return - float
 */
float Volume::getMaximumGradient(void) {
    return maximumGradient;
} // end getMaximumGradient()

/*
 * getMaximumPriorityQueueSize
 *
 * return - int
 */
int Volume::getMaximumPriorityQueueSize(void) {
    return maximumPriorityQueueSize;
} // end getMaximumPriorityQueueSize()

/*
 * setMaximumPriorityQueueSize
 *
 * parameter _maximumPriorityQueueSize - int
 */
void Volume::setMaximumPriorityQueueSize(int _maximumPriorityQueueSize) {
    maximumPriorityQueueSize = _maximumPriorityQueueSize;
} // end setMaximumPriorityQueueSize()

/*
 * getMinimumGradient
 *
 * return - float
 */
float Volume::getMinimumGradient(void) {
    return minimumGradient;
} // end getMinimumGradient()

/*
 * getNumberOfComponents
 *
 * return - int
 */
int Volume::getNumberOfComponents(void) const {
    return numberOfComponents;
} // end getNumberOfComponents()

/*
 * setNumberOfComponents
 *
 * parameter _numberOfComponents - int
 */
void Volume::setNumberOfComponents(int _numberOfComponents) {
    numberOfComponents = _numberOfComponents;
} // end setNumberOfComponents()

/*
 * getNumberOfVoxels
 *
 * return - int
 */
int Volume::getNumberOfVoxels(void) const {
    return numberOfVoxels;
} // end getNumberOfVoxels()

/*
 * setNumberOfVoxels
 *
 * parameter _numberOfVoxels - int
 */
void Volume::setNumberOfVoxels(int _numberOfVoxels) {
    numberOfVoxels = _numberOfVoxels;
} // end setNumberOfVoxels()

/*
 * getRatioOfVisibilityThreshold
 *
 * return - float
 */
float Volume::getRatioOfVisibilityThreshold(void) const {
    return ratioOfVisibilityThreshold;
} // end getRatioOfVisibilityThreshold()

/*
 * setRatioOfVisibilityThreshold
 *
 * parameter _ratioOfVisibilityThreshold - float
 */
void Volume::setRatioOfVisibilityThreshold(float _ratioOfVisibilityThreshold) {
    ratioOfVisibilityThreshold = _ratioOfVisibilityThreshold;
} // end setRatioOfVisibilityThreshold()

/*
 * getRedByteVoxelsBase
 *
 * return - unsigned char*
 */
const unsigned char* Volume::getRedByteVoxelsBase(void) const {
    return redByteVoxelsBase;
} // end getRedByteVoxelsBase()

/*
 * hasRedByteVoxelsBase
 *
 * return - bool
 */
bool Volume::hasRedByteVoxelsBase(void) const {
    return redByteVoxelsBase != NULL;
} // end hasRedByteVoxelsBase()

/*
 * setRedByteVoxelsBase
 *
 * parameter _redByteVoxelsBase - unsigned char*
 */
void Volume::setRedByteVoxelsBase(unsigned char* _redByteVoxelsBase) {
    redByteVoxelsBase = _redByteVoxelsBase;
} // end setRedByteVoxelsBase()

/*
 * getRedVoxel
 *
 * parameter i - int
 * return - unsigned char
 */
unsigned char Volume::getRedVoxel(int i) const {
    return redVoxels[i];
} // getRedVoxel()

/*
 * getRedVoxel
 *
 * parameter i - int
 * parameter j - int
 * parameter k - int
 * return - unsigned char
 */
unsigned char Volume::getRedVoxel(int i, int j, int k) const {
    return redVoxels[i * increments[0] + j * increments[1] + k];
} // getRedVoxel()

/*
 * getRedVoxelPtr
 *
 * parameter i - int
 * parameter j - int
 * parameter k - int
 * return - unsigned char*
 */
const unsigned char* Volume::getRedVoxelPtr(int i, int j, int k) const {
    return redVoxels + (i * increments[0] + j * increments[1] + k);
} // end getRedVoxelPtr()

/*
 * getRedVoxels
 *
 * return - const unsigned char*
 */
const unsigned char* Volume::getRedVoxels(void) const {
    return redVoxels;
} // end getRedVoxels()

/*
 * hasRedVoxels
 *
 * return - bool
 */
bool Volume::hasRedVoxels(void) const {
    return redVoxels != NULL;
} // end hasRedVoxels()

/*
 * setRedVoxels
 *
 * parameter _redVoxels - unsigned char*
 */
void Volume::setRedVoxels(unsigned char* _redVoxels) {
    redVoxels = _redVoxels;
} // end setRedVoxels()

/*
 * getScale
 *
 * return - const float*
 */
const float* Volume::getScale(void) const {
    return scale;
} // end getScale()

/*
 * getScale
 *
 * parameter dimension - int
 * return - float
 */
float Volume::getScale(int dimension) {
    return scale[dimension];
} // end getScale()

/*
 * setScale
 *
 * parameter _scale[3] - float[3]
 */
void Volume::setScale(float _scale[3]) {
    scale[0] = _scale[0];
    scale[1] = _scale[1];
    scale[2] = _scale[2];
} // end setScale()

/*
 * getSize
 *
 * return - const int*
 */
const int* Volume::getSize(void) const {
    return size;
} // end getSize()

/*
 * getSize
 *
 * parameter dimension - int
 * return - int
 */
int Volume::getSize(int dimension) {
    return size[dimension];
} // end getSize()

/*
 * setSize
 *
 * parameter _size[3] - int[3]
 */
void Volume::setSize(int _size[3]) {
    size[0] = _size[0];
    size[1] = _size[1];
    size[2] = _size[2];
} // end setSize()

/*
 * getSliceFactor
 *
 * return - float
 */
float Volume::getSliceFactor(void) const {
    return sliceFactor;
} // end getSliceFactor()

/*
 * setSliceFactor
 *
 * parameter _sliceFactor - float
 */
void Volume::setSliceFactor(float _sliceFactor) {
    sliceFactor = _sliceFactor;
} // end setSliceFactor()

/*
 * getTextureSize
 *
 * return - int
 */
int Volume::getTextureSize(void) {
    return textureSize;
} // end getTextureSize()

/*
 * setTextureSize
 *
 * parameter _textureSize - int
 */
void Volume::setTextureSize(int _textureSize) {
    textureSize = _textureSize;
} // end setTextureSize()

/*
 * getTypeOfComponents
 *
 * return - int
 */
int Volume::getTypeOfComponents(void) {
    return typeOfComponents;
} // end getTypeOfComponents()

/*
 * setTypeOfComponents
 *
 * parameter _typeOfComponents - int
 */
void Volume::setTypeOfComponents(int _typeOfComponents) {
    typeOfComponents = _typeOfComponents;
} // end setTypeOfComponents()

/*
 * getVolumeBox
 *
 * return - BoundingBox*
 */
BoundingBox* Volume::getVolumeBox(void) {
    return volumeBox;
}

/*
 * setVolumeBox
 */
void Volume::setVolumeBox(void) {
    volumeBox = new BoundingBox(getOrigin(0), getOrigin(1), getOrigin(2), getExtent(0), getExtent(1), getExtent(2));
}

/*
 * getVoxel
 *
 * parameter i - int
 * return - unsigned char
 */
unsigned char Volume::getVoxel(int i) const {
    return voxels[i];
} // getVoxel()

/*
 * getVoxel
 *
 * parameter i - int
 * parameter j - int
 * parameter k - int
 * return - unsigned char
 */
unsigned char Volume::getVoxel(int i, int j, int k) const {
    return voxels[i * increments[0] + j * increments[1] + k];
} // getVoxel()

/*
 * getVoxelPtr
 *
 * parameter i - int
 * parameter j - int
 * parameter k - int
 * return - unsigned char*
 */
const unsigned char* Volume::getVoxelPtr(int i, int j, int k) const {
    return voxels + (i * increments[0] + j * increments[1] + k);
} // end getVoxelPtr()

/*
 * getVoxels
 *
 * return - const unsigned char*
 */
const unsigned char* Volume::getVoxels(void) const {
    return voxels;
} // end getVoxels()

/*
 * hasVoxels
 *
 * return - bool
 */
bool Volume::hasVoxels(void) const {
    return voxels != NULL;
} // end hasVoxels()

/*
 * setVoxels
 *
 * parameter _voxels - unsigned char*
 */
void Volume::setVoxels(unsigned char* _voxels) {
    voxels = _voxels;
} // end setVoxels()

/*
 * getWidth
 *
 * return - int
 */
int Volume::getWidth(void) {
    return size[0];
} // end getWidth()

/*
 * setWidth
 *
 * parameter width - int
 */
void Volume::setWidth(int width) {
    size[0] = width;
} // end setWidth()

/*
 * initialize - Method to initialize Volume class.
 */
void Volume::initialize(void) {
    scale[0] = 1.0;
    scale[1] = 1.0;
    scale[2] = 1.0;
    sliceIndexDir[0] = sliceIndexDir[1] = sliceIndexDir[2] = 1;
} // end initialize()

/*
 * isPowerOfTwo
 *
 * parameter n - int
 * return - bool
 */
bool Volume::isPowerOfTwo(int n) {
    if (n <= 0) {
        return false;
    }
    int test = n & (n - 1);

    if (test == 0) {
        return true;
    } else {
        return false;
    }
} // end isPowerOfTwo()

/*
 * nextPowerOfTwo
 *
 * parameter n - int
 * return - int
 */
int Volume::nextPowerOfTwo(int n) {
    int number = 0;
    for (int i = 0; i < 31; i++) {
        number = 1 << i;
        if (number > n) {
            break;
        }
    }
    return number;
} // end nextPowerOfTwo()

/*
 * resize
 */
void Volume::resize(void) {
    int w = 0;
    int h = 0;
    int d = 0;
    int start_width = 0;
    int start_height = 0;
    int start_depth = 0;
    if (!isPowerOfTwo(getWidth())) {
        w = nextPowerOfTwo(getWidth());
        start_width = (w - getWidth()) / 2;
    }
    if (!isPowerOfTwo(getHeight())) {
        h = nextPowerOfTwo(getHeight());
        start_height = (h - getHeight()) / 2;
    }
    if (!isPowerOfTwo(getDepth())) {
        d = nextPowerOfTwo(getDepth());
        start_depth = (d - getDepth()) / 2;
    }
    if ((w != 0) || (h != 0) || (d != 0)) {
        if (w == 0) {
            w = getWidth();
        }
        if (h == 0) {
            h = getHeight();
        }
        if (d == 0) {
            d = getDepth();
        }
        int _size[3];
        _size[0] = w;
        _size[1] = h;
        _size[2] = d;
        int _borderSize = 0;
        int _numberOfVoxels = (_size[0] + 2 * _borderSize) * (_size[1] + 2 * _borderSize) * (_size[2] + 2 * _borderSize);
        unsigned char* _voxelsBase = new unsigned char[_numberOfVoxels];
        for (int k = 0; k < getDepth(); k++) {
            for (int j = 0; j < getHeight(); j++) {
                for (int i = 0; i < getWidth(); i++) {
                    _voxelsBase[((start_depth + k) * (w * h)) + ((start_height + j) * w) + (start_width + i)] = voxels[(k
                            * (getWidth() * getHeight())) + (j * getWidth()) + i];
                }
            }
        }
        Point _origin = Point::origin;
        Size _extent;
        Point _center;
        for (int i = 0; i < 3; ++i) {
            _extent[i] = Scalar(_size[i]) * scale[i];
            _center[i] = _origin[i] + _extent[i] * Scalar(0.5);
        }
        unsigned char* _voxels = _voxelsBase + borderSize;
        int _increments[3];
        _increments[2] = 1;
        for (int i = 2; i > 0; --i) {
            _increments[i - 1] = _increments[i] * (_size[i] + 2 * _borderSize);
            _voxels += _borderSize * _increments[i - 1];
        }
        setNumberOfVoxels(_numberOfVoxels);
        setSize(_size);
        setOrigin(_origin);
        setExtent(_extent);
        setCenter(_center);
        delete byteVoxelsBase;
        setByteVoxelsBase(_voxelsBase);
        setBorderSize(_borderSize);
        setIncrements(_increments);
        setVoxels(_voxels);
    }
} // end resize()

/*
 * resize
 */
void Volume::resize(unsigned char tmpByte) {
    int w = 0;
    int h = 0;
    int d = 0;
    int start_width = 0;
    int start_height = 0;
    int start_depth = 0;
    if (!isPowerOfTwo(getWidth())) {
        w = nextPowerOfTwo(getWidth());
        start_width = (w - getWidth()) / 2;
    }
    if (!isPowerOfTwo(getHeight())) {
        h = nextPowerOfTwo(getHeight());
        start_height = (h - getHeight()) / 2;
    }
    if (!isPowerOfTwo(getDepth())) {
        d = nextPowerOfTwo(getDepth());
        start_depth = (d - getDepth()) / 2;
    }
    if ((w != 0) || (h != 0) || (d != 0)) {
        if (w == 0) {
            w = getWidth();
        }
        if (h == 0) {
            h = getHeight();
        }
        if (d == 0) {
            d = getDepth();
        }
        int _size[3];
        _size[0] = w;
        _size[1] = h;
        _size[2] = d;
        int _borderSize = 0;
        int _numberOfVoxels = (_size[0] + 2 * _borderSize) * (_size[1] + 2 * _borderSize) * (_size[2] + 2 * _borderSize);
        unsigned char* _voxelsBase = new unsigned char[_numberOfVoxels];
        std::cout << Stringify::toString(_numberOfVoxels) << std::endl;
        for (int i = 0; i < _numberOfVoxels; i++)
            _voxelsBase[i] = tmpByte;
        for (int k = 0; k < getDepth(); k++) {
            for (int j = 0; j < getHeight(); j++) {
                for (int i = 0; i < getWidth(); i++) {
                    _voxelsBase[((start_depth + k) * (w * h)) + ((start_height + j) * w) + (start_width + i)] = voxels[(k
                            * (getWidth() * getHeight())) + (j * getWidth()) + i];
                }
            }
        }
        Point _origin = Point::origin;
        Size _extent;
        Point _center;
        for (int i = 0; i < 3; ++i) {
            _extent[i] = Scalar(_size[i]) * scale[i];
            _center[i] = _origin[i] + _extent[i] * Scalar(0.5);
        }
        unsigned char* _voxels = _voxelsBase + borderSize;
        int _increments[3];
        _increments[2] = 1;
        for (int i = 2; i > 0; --i) {
            _increments[i - 1] = _increments[i] * (_size[i] + 2 * _borderSize);
            _voxels += _borderSize * _increments[i - 1];
        }
        setNumberOfVoxels(_numberOfVoxels);
        setSize(_size);
        setOrigin(_origin);
        setExtent(_extent);
        setCenter(_center);
        delete byteVoxelsBase;
        setByteVoxelsBase(_voxelsBase);
        setBorderSize(_borderSize);
        setIncrements(_increments);
        setVoxels(_voxels);
    }
} // end resize()

/*
 * toString
 *
 * return - std::string
 */
std::string Volume::toString() {
    return "w " + Stringify::toString(getWidth()) + " h " + Stringify::toString(getHeight()) + " d " + Stringify::toString(
            getDepth()) + "\n border size " + Stringify::toString(getBorderSize()) + " number of voxes " + Stringify::toString(
            getNumberOfVoxels()) + "\n increments " + Stringify::toString(increments[0]) + " " + Stringify::toString(increments[1])
            + " " + Stringify::toString(increments[2]) + "\n size " + Stringify::toString(size[0]) + " " + Stringify::toString(
            size[1]) + " " + Stringify::toString(size[2]) + "\n origin " + Stringify::toString(origin[0]) + " "
            + Stringify::toString(origin[1]) + " " + Stringify::toString(origin[2]) + "\n center " + Stringify::toString(center[0])
            + " " + Stringify::toString(center[1]) + " " + Stringify::toString(center[2]) + "\n extent " + Stringify::toString(
            extent[0]) + " " + Stringify::toString(extent[1]) + " " + Stringify::toString(extent[2]) + "\n scale "
            + Stringify::toString(scale[0]) + " " + Stringify::toString(scale[1]) + " " + Stringify::toString(scale[2]);
} // end toString()

/*
 * getSliceIndexDir
 *
 * return - const float*
 */
const int* Volume::getSliceIndexDir(void) const {
    return sliceIndexDir;
} // end getSliceIndexDir()

/*
 * getSliceIndexDir
 *
 * parameter dimension - int
 * return - float
 */
int Volume::getSliceIndexDir(int dimension) {
    return sliceIndexDir[dimension];
} // end getSliceIndexDir()

/*
 * setSliceIndexDir
 *
 * parameter _sliceIndexDir[3] - float[3]
 */
void Volume::setSliceIndexDir(int _sliceIndexDir[3]) {
    sliceIndexDir[0] = _sliceIndexDir[0];
    sliceIndexDir[1] = _sliceIndexDir[1];
    sliceIndexDir[2] = _sliceIndexDir[2];
} // end setSliceIndexDir()

/*
 * setSliceIndexDir
 *
 * parameter dim - int
 * parameter _sliceIndexDir[3] - float[3]
 */
void Volume::setSliceIndexDir(int dim, int _sliceIndexDir) {
    sliceIndexDir[dim] = _sliceIndexDir;
} // end setSliceIndexDir()
