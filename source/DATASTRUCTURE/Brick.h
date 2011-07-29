/*
 *  Brick
 *
 *       6*--------*7
 *       /|       /|
 *      / |      / |
 *     /  |     /  |
 *    /  2*----/---*3
 *  4*--------*5  /
 wZ *   |  /     |  /
 *   | /      | /
 *   |/       |/
 *  0*--------*1
 *
 * Author:     Patrick O'Leary
 * Created:    December 31, 2007
 * Copyright 2007. All rights reserved.
 */
#ifndef BRICK_H_
#define BRICK_H_

#include <vector>

#include <GL/gl.h>

/* Vrui includes */
#include <GL/GLContextData.h>
#include <GL/GLObject.h>

// begin Forward Declarations
class BoundingBox;
class BoundingSphere;
class ComparableOcNode;
class CFrustum;
class Matrix4x4;
class OcTree;
class ShaderObject;
class Volume;
// end Forward Declarations

class Brick: public GLObject {
protected:
    struct DataItem: public GLObject::DataItem {
        GLuint texture3DName;
        GLuint downSamplingTexture3DName;
        DataItem(void);
        virtual ~DataItem(void);
    };
public:
    friend class DataItem;
    Brick(void);
    ~Brick(void);
    void adjustTextureCoordinates(void);
    GLuint create3DTexture(void) const;
    GLuint create3DTexture(int bordersize) const;
    GLuint createDownSampling3DTexture(void) const;
    GLuint createDownSamplingMultiComponent3DTexture(void) const;
    GLuint createMultiComponent3DTexture(void) const;
    GLuint createMultiComponent3DTexture(int bordersize) const;
    void determineOcNodesToDisplay(void);
    void drawBrick(void) const;
    void drawGrid(const CFrustum & cFrustum) const;
    void drawSlices(GLContextData & glContextData, const CFrustum & cFrustum, ShaderObject* shaderObject);
    void drawVolume(GLContextData & glContextData, const CFrustum & cFrustum, ShaderObject* shaderObject, float * point, int minimumIndex, int maximumIndex,
            const Matrix4x4 * modelviewInverse);
    void drawXSlice(const CFrustum & cFrustum);
    void drawYSlice(const CFrustum & cFrustum);
    void drawZSlice(const CFrustum & cFrustum);
    void setBrick(int _column, int _row, int _slab, int _width, int _height, int _depth, Volume* _volume);
    void setDownSamplingChanged(bool downSamplingChanged);
    bool isEmpty(void);
    void setEmpty(bool _empty);
    void setMaximumPriorityQueueSize(int _maximumPriorityQueueSize);
    void setMaximumPriorityQueueTest(bool _maximumPriorityQueueTest);
    void setPercentageOfEmptyVolume(float* _alpha);
    void setPercentageOfEmptyVolume(float* _red, float* _green, float* _blue);
    void setPercentageOfEmptyVolume2_5D(float* _alpha);
    void setRatioOfVisibility(float* _alpha);
    void setRatioOfVisibility(float* _red, float* _green, float* _blue);
    void setRatioOfVisibility2_5D(float* _alpha);
    void setRatioOfVisibilityTest(bool _ratioOfVisibilityTest);
    void setRatioOfVisibilityThreshold(float _ratioOfVisibilityThreshold);
    void setRenderingCost(float _renderingCost);
    void setRenderingCostFunction(float* _alpha);
    void setRenderingCostFunction(float* _red, float* _green, float* _blue);
    void setRenderingCostFunction2_5D(float* _alpha);
    void setRenderingCostFunctionTest(bool _renderingCostFunctionTest);
    void setShowBricks(bool _showBricks);
    void setShowOcNodes(bool _showOcNodes);
    void setShowVolume(bool _showVolume);
    void setShowXSlice(bool _showXSlice);
    void setShowYSlice(bool _showYSlice);
    void setShowZSlice(bool _showZSlice);
    void setSliceFactor(float _sliceFactor);
    int getColumn(void);
    int getDepth(void);
    int getHeight(void);
    void setInteractive(bool _interactive);
    OcTree* getOcTree(void);
    int getRow(void);
    int getSlab(void);
    void setSlicingScale(float slicingScale);
    Volume* getVolume(void);
    BoundingBox* getVolumeBox(void);
    BoundingSphere* getVolumeSphere(void);
    void setVolumeSphere(void);
    int getWidth(void);
    float getX(void);
    int getXSlice(void);
    void setXSlice(int _xSlice);
    void setXStep(int xStep);
    float getY(void);
    int getYSlice(void);
    void setYSlice(int _ySlice);
    void setYStep(int yStep);
    float getZ(void);
    int getZSlice(void);
    void setZSlice(int _zSlice);
    void setZStep(int zStep);
    void initContext(GLContextData& glContextData) const;
    void initialize(DataItem * dataItem) const;
    void update3DTexture(GLuint texture3DName);
private:
    int borderSize;
    int column;
    int depth;
    bool downSamplingChanged;
    bool empty;
    int height;
    bool interactive;
    int maximumPriorityQueueSize;
    OcTree* ocTree;
    int row;
    bool showBricks;
    bool showXSlice;
    bool showYSlice;
    bool showZSlice;
    int slab;
    float sliceFactor;
    float slicingScale;
    Volume* volume;
    BoundingBox* volumeBox;
    BoundingSphere* volumeSphere;
    int width;
    float x;
    int xSlice;
    int xStep;
    float y;
    int ySlice;
    int yStep;
    float z;
    int zSlice;
    int zStep;
    void fill3DTexture (int bordersize, unsigned char *voxels) const;
    void fillMultiComponent3DTexture (int bordersize, unsigned char *voxels,
                                      int icomp) const;
};

#endif /*BRICK_H_*/
