/*
 * Volume.h - Class for basic volume renderering data set.
 *
 * Author: Patrick O'Leary
 * Created: October 19, 2007
 * Copyright: 2007
 */
#ifndef VOLUME_H_
#define VOLUME_H_

#include <string>

/* Vrui includes */
#include <Geometry/ComponentArray.h>
#include <Geometry/Point.h>

#include <DATA/DataSet.h>
// begin Forward Declarations
class BoundingBox;
// end Forward Declarations

class Volume: public DataSet {
public:
    typedef float Scalar;
    typedef Geometry::ComponentArray<Scalar,3> Size;
    typedef Geometry::Point<Scalar,3> Point;
    explicit Volume();
    ~Volume();
    void calculateGradient(void);
    int calculateMaximum(int column, int row, int slab, int width, int height, int depth);
    int calculateMinimum(int column, int row, int slab, int width, int height, int depth);
    const unsigned char* getBlueByteVoxelsBase(void) const;
    bool hasBlueByteVoxelsBase(void) const;
    void setBlueByteVoxelsBase(unsigned char* _blueByteVoxelsBase);
    unsigned char getBlueVoxel(int i) const;
    unsigned char getBlueVoxel(int i, int j, int k) const;
    const unsigned char* getBlueVoxelPtr(int i, int j, int k) const;
    const unsigned char* getBlueVoxels(void) const;
    bool hasBlueVoxels(void) const;
    void setBlueVoxels(unsigned char* _blueVoxels);
    int getBorderSize(void) const;
    void setBorderSize(int _borderSize);
    const unsigned char* getByteVoxelsBase(void) const;
    bool hasByteVoxelsBase(void) const;
    void setByteVoxelsBase(unsigned char* _byteVoxelsBase);
    /*
     * getCenter
     *
     * return - Point&
     */
    const Point& getCenter(void) const {
        return center;
    }
    ; // end getCenter()
    /*
     * getCenter
     *
     * parameter dimension - int
     * return - float
     */
    float getCenter(int dimension) {
        return center[dimension];
    }
    ; // end getCenter()
    /*
     * setCenter
     *
     * parameter _center - Point&
     */
    void setCenter(const Point& _center) {
        center = _center;
    }
    ; // end setCenter
    int getDepth(void);
    void setDepth(int depth);
    /*
     * getExtent
     *
     * return - Size&
     */
    const Size& getExtent(void) const {
        return extent;
    }
    ; // end getExtent()
    /*
     * getExtent
     *
     * parameter dimension - int
     * return - float
     */
    float getExtent(int dimension) const {
        return extent[dimension];
    }
    ; // end getExtent()
    /*
     * setExtent
     *
     * parameter _extent - Size&
     */
    void setExtent(const Size& _extent) {
        extent = _extent;
    }
    ; // end setExtent()
    const float* getFloatVoxelsBase(void) const;
    bool hasFloatVoxelsBase(void) const;
    void setFloatVoxelsBase(float* _floatVoxelsBase);
    float getGradient(int i, int j, int k);
    float getGradient(int i);
    const unsigned char* getGreenByteVoxelsBase(void) const;
    bool hasGreenByteVoxelsBase(void) const;
    void setGreenByteVoxelsBase(unsigned char* _greenByteVoxelsBase);
    unsigned char getGreenVoxel(int i) const;
    unsigned char getGreenVoxel(int i, int j, int k) const;
    const unsigned char* getGreenVoxelPtr(int i, int j, int k) const;
    const unsigned char* getGreenVoxels(void) const;
    bool hasGreenVoxels(void) const;
    void setGreenVoxels(unsigned char* _greenVoxels);
    int getWidth(void);
    int getHeight(void);
    void setHeight(int height);
    int getLeafSize(void);
    void setLeafSize(int _leafSize);
    const int* getIncrements(void);
    int getIncrements(int dimension) const;
    void setIncrements(const int _increments[3]);
    const int* getIntVoxelsBase(void) const;
    bool hasIntVoxelsBase(void) const;
    void setIntVoxelsBase(int* _intVoxelsBase);
    /*
     * getMaximum
     *
     * return - Point
     */
    Point getMaximum(void) const {
        Point result = origin;
        for (int i = 0; i < 3; ++i)
            result[i] += extent[i] * float(1.0);
        return result;
    }
    ; // end getMaximum()
    float getMaximumGradient(void);
    int getMaximumPriorityQueueSize(void);
    void setMaximumPriorityQueueSize(int _maximumPriorityQueueSize);
    /*
     * getMinimum
     *
     * return - Point
     */
    Point getMinimum(void) const {
        return origin;
    }
    ; // end getMinimum()
    float getMinimumGradient(void);
    int getNumberOfComponents(void) const;
    void setNumberOfComponents(int _numberOfComponents);
    int getNumberOfVoxels(void) const;
    void setNumberOfVoxels(int _numberOfVoxels);
    /*
     * getOrigin
     *
     * return - const Point&
     */
    const Point& getOrigin(void) const {
        return origin;
    }
    ; // end getOrigin()
    /*
     * getOrigin
     *
     * parameter dimension - int
     * return - float
     */
    float getOrigin(int dimension) {
        return origin[dimension];
    }
    /*
     * setOrigin
     *
     * parameter _origin - const Point&
     */
    void setOrigin(const Point& _origin) {
        origin = _origin;
    }
    ; // end setOrigin()
    float getRatioOfVisibilityThreshold(void) const;
    void setRatioOfVisibilityThreshold(float _ratioOfVisibilityThreshold);
    const unsigned char* getRedByteVoxelsBase(void) const;
    bool hasRedByteVoxelsBase(void) const;
    void setRedByteVoxelsBase(unsigned char* _redByteVoxelsBase);
    unsigned char getRedVoxel(int i) const;
    unsigned char getRedVoxel(int i, int j, int k) const;
    const unsigned char* getRedVoxelPtr(int i, int j, int k) const;
    const unsigned char* getRedVoxels(void) const;
    bool hasRedVoxels(void) const;
    void setRedVoxels(unsigned char* _redVoxels);
    const float* getScale(void) const;
    float getScale(int dimension);
    void setScale(float _scale[3]);
    const int* getSize(void) const;
    int getSize(int dimension);
    void setSize(int _size[3]);
    float getSliceFactor(void) const;
    void setSliceFactor(float _sliceFactor);
    int getTextureSize(void);
    void setTextureSize(int _textureSize);
    int getTypeOfComponents(void);
    void setTypeOfComponents(int _typeOfComponents);
    BoundingBox* getVolumeBox(void);
    void setVolumeBox(void);
    unsigned char getVoxel(int i) const;
    unsigned char getVoxel(int i, int j, int k) const;
    const unsigned char* getVoxelPtr(int i, int j, int k) const;
    const unsigned char* getVoxels(void) const;
    bool hasVoxels(void) const;
    void setVoxels(unsigned char* _voxels);
    void setWidth(int width);
    bool isPowerOfTwo(int n);
    int nextPowerOfTwo(int n);
    void resize(unsigned char tmpByte);
    void resize(void);
    std::string toString();
    void setSliceIndexDir (int sliceIndDir[3]);
    void setSliceIndexDir (int dimension, int sliceIndDir);
    const int *getSliceIndexDir (void) const;
    int getSliceIndexDir (int dimension);

private:
    const unsigned char* blueByteVoxelsBase;
    const unsigned char* blueVoxels;
    int borderSize;
    const unsigned char* byteVoxelsBase;
    Point center;
    Size extent;
    const float* floatVoxelsBase;
    float* gradients;
    const unsigned char* greenByteVoxelsBase;
    const unsigned char* greenVoxels;
    int leafSize;
    int* increments;
    const int* intVoxelsBase;
    float maximumGradient;
    float minimumGradient;
    int maximumPriorityQueueSize;
    int numberOfComponents;
    int numberOfVoxels;
    Point origin;
    float ratioOfVisibilityThreshold;
    const unsigned char* redByteVoxelsBase;
    const unsigned char* redVoxels;
    float* scale;
    int* size;
    float sliceFactor;
    int textureSize;
    int typeOfComponents;
    BoundingBox* volumeBox;
    const unsigned char* voxels;
    void initialize(void);
    int *sliceIndexDir;
};

#endif /*VOLUME_H_*/
