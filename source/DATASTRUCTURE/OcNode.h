/*
 *  OcNode
 *
 *       6*--------*7
 *       /|       /|
 *      / |      / |
 *     /  |     /  |
 *    /  2*----/---*3
 *  4*--------*5  /
 *   |  /     |  /
 *   | /      | /
 *   |/       |/
 *  0*--------*1
 *
 * Author:     Patrick O'Leary
 * Created:    November 5, 2007
 * Copyright 2007. All rights reserved.
 */
#ifndef OCNODE_H_
#define OCNODE_H_

#include <cmath>
#include <string>
#include <vector>

#include <GL/gl.h>

#define X 0
#define Y 1
#define Z 2

static int edges[12][2] = { { 0, 1 }, { 0, 2 }, { 1, 3 }, { 4, 0 }, { 1, 5 }, { 2, 3 }, { 4, 5 }, { 4, 6 }, { 5, 7 }, { 6, 7 }, {
        2, 6 }, { 3, 7 } };
static int edgeIndex[24][2] = { { 0, 1 }, { 1, 4 }, { 4, 7 }, { 1, 5 }, { 1, 5 }, { 0, 1 }, { 1, 4 }, { 4, 7 }, { 0, 2 }, { 2, 5 },
        { 5, 7 }, { 2, 6 }, { 2, 6 }, { 0, 2 }, { 2, 5 }, { 5, 7 }, { 0, 3 }, { 3, 6 }, { 6, 7 }, { 3, 4 }, { 3, 4 }, { 0, 3 }, {
                3, 6 }, { 6, 7 } };
static int sequence[8][8] = { { 0, 1, 2, 4, 5, 3, 6, 7 }, { 1, 3, 0, 5, 7, 2, 4, 6 }, { 2, 0, 3, 6, 4, 1, 7, 5 }, { 3, 2, 1, 7, 6,
        0, 5, 4 }, { 4, 5, 0, 6, 7, 1, 2, 3 }, { 5, 7, 1, 4, 6, 3, 0, 2 }, { 6, 4, 2, 7, 5, 0, 3, 1 }, { 7, 6, 3, 5, 4, 2, 1, 0 } };

// begin Forward Declarations
class BoundingSphere;
class Brick;
class BoundingBox;
class CFrustum;
class ComparableOcNode;
class Matrix4x4;
class ShaderObject;
class Vector4;
class Volume;
// end Forward Declarations

class OcNode {
public:
    explicit OcNode(void);
    OcNode(Brick* _brick);
    ~OcNode(void);
    void adjustTextureCoordinatesTraversal(float dx, float dy, float dz);
    void calculateDelta(float _maximum, float _minimum);
    void calculateEmptiness(float* alpha);
    void calculateEmptiness(float* red, float* green, float* blue);
    void calculateEmptiness2_5D(float* alpha);
    void calculatePercentageOfEmptyVolume(float* alpha);
    void calculatePercentageOfEmptyVolume(float* red, float* green, float* blue);
    void calculatePercentageOfEmptyVolume2_5D(float* alpha);
    void calculateRatioOfVisibility(float* alpha);
    void calculateRatioOfVisibility(float* red, float* green, float* blue);
    void calculateRatioOfVisibility2_5D(float* alpha);
    float calculateSampleDistance(const Matrix4x4 * modelviewInverse, float _minimum, float _maximum);
    void createChildren(void);
    void createChildren(int _minimumOcNodeSize);
    void createChildren(int _maximumTextureSize, int _minimumOcNodeSize);
    void drawGrid(void);
    void drawGridTraversal(int _level, const CFrustum & cFrustum, bool testChildren);
    void drawGridTraversal(const CFrustum & cFrustum, bool testChildren);
    void drawPreIntegratedViewAlignedSlices(const Vector4 & slicePlaneNormal, int minimumIndex, int maximumIndex,
            const Matrix4x4 * modelviewInverse);
    void drawShadowedViewAlignedSlices(const Vector4 & slicePlaneNormal, int minimumIndex, int maximumIndex,
            const Matrix4x4 * modelviewInverse);
    void drawViewAlignedSlices(const Vector4 & slicePlaneNormal, int minimumIndex, int maximumIndex,
            const Matrix4x4 * modelviewInverse);
    void drawViewAlignedSlicesGPU(int minimumIndex, int maximumIndex, const Matrix4x4 * modelviewInverse);
    void drawVolume(int minimumIndex, int maximumIndex, const Vector4 & slicePlaneNormal, const Matrix4x4 * modelviewInverse);
    void drawVolume(ShaderObject* shaderObject, int minimumIndex, int maximumIndex, const Matrix4x4 * modelviewInverse);
    void drawVolumeTraversal(int _level, const CFrustum & cFrustum, float* point, ShaderObject* shaderObject, int minimumIndex,
            int maximumIndex, const Matrix4x4 * modelviewInverse, bool testChildren);
    void drawVolumeTraversal(const CFrustum & cFrustum, float * point, ShaderObject* shaderObject, int minimumIndex,
            int maximumIndex, const Matrix4x4 * modelviewInverse, bool testChildren);
    void drawVolumeTraversal(const CFrustum & cFrustum, float * point, int minimumIndex, int maximumIndex,
            const Vector4 & slicePlaneNormal, const Matrix4x4 * modelviewInverse, bool testChildren);
    void setCenter(void);
    OcNode* getChildren(void);
    void setChild(int _level, int _which, OcNode* _parent, Brick* _brick, int _maximumLevel);
    void setChild(int _level, int _which, OcNode* _parent, Volume* _volume, int _maximumLevel);
    void setChild(int _level, int _which, OcNode* _parent, Volume* _volume, int _maximumLevel, int _minimumOcNodeSize);
    void setChild(int _level, int _which, OcNode* _parent, Volume* _volume, int _maximumLevel, int _maximumTextureSize,
            int _minimumOcNodeSize);
    int getColumn(void);
    void setColumn(void);
    float getCost(void);
    float getCostRender(void);
    float getCostStructure(void);
    float getDelta(void);
    int getDepth(void);
    void setDepth(void);
    void setDimensions(void);
    bool isDisplayed(void);
    void setDisplayed(bool _displayed);
    bool isEmpty(void);
    unsigned char getEmptyChildren(void);
    int getHeight(void);
    void setHeight(void);
    bool isLeaf(void);
    void setLeaf(bool _leaf);
    bool isTextureLeaf(void);
    void setTextureLeaf(bool _textureLeaf);
    int getLevel(void);
    void setLocations(void);
    float getMaximum(BoundingBox* boundingBox, int maximumIndex);
    int getMaximum(void);
    void setMaximum(int maximum);
    float getMinimum(BoundingBox* boundingBox, int minimumIndex);
    int getMinimum(void);
    void setMinimum(int minimum);
    OcNode* getParent(void);
    void setParent(OcNode* parent);
    float getPercentageOfEmptyVolume(void);
    float getRatioOfVisibility(void);
    int getRow(void);
    void setRow(void);
    int getSlab(void);
    void setSlab(void);
    BoundingBox* getTextureBox(void);
    void setTextureBox(void);
    Volume* getVolume(void);
    void setVolume(Volume* _volume);
    BoundingBox* getVolumeBox(void);
    void setVolumeBox(void);
    BoundingSphere* getVolumeSphere(void);
    void setVolumeSphere(void);
    void setWeight(void);
    int getWhich(void);
    int getWidth(void);
    void setWidth(void);
    float getX(void);
    void setX(float x);
    float getY(void);
    void setY(float y);
    float getZ(void);
    void setZ(float z);
    void maximumPriorityQueueTestTraversal(bool _maximumPriorityQueueTest);
    void percentageOfEmptyVolumeTraversal(float* alpha);
    void percentageOfEmptyVolumeTraversal(float* red, float* green, float* blue);
    void percentageOfEmptyVolumeTraversal2_5D(float* alpha);
    void ratioOfVisibilityTraversal(float* alpha);
    void ratioOfVisibilityTraversal(float* red, float* green, float* blue);
    void ratioOfVisibilityTraversal2_5D(float* alpha);
    void ratioOfVisibilityTestTraversal(bool _ratioOfVisibilityTest);
    void ratioOfVisibilityThresholdTraversal(float _ratioOfVisibilityThreshold);
    void renderingCostFunctionTestTraversal(bool _renderingCostFuctionTest);
    void renderingCostTraversal(float _renderingCost);
    void rendereringCostFuctionTraversal(float* alpha);
    void rendereringCostFuctionTraversal(float* red, float* green, float* blue);
    void rendereringCostFuctionTraversal2_5D(float* alpha);
    void sliceFactorTraversal(float _sliceFactor);
    void sortChildren(float * point, std::vector<ComparableOcNode> & ocNodeList);
    std::string toString(void);
    void viewAlignedVertex(int which, int frontIndex, const Vector4 & sp, const Vector4 & spn, Vector4* tverts, Vector4* verts);
    void viewAlignedVertex(int which, int frontIndex, const Vector4 & spF, const Vector4 & spB, const Vector4 & spn,
            Vector4* sFront, Vector4* sBack, Vector4* verts);
private:
    Brick* brick;
    OcNode* child;
    float cost;
    float costRender;
    float costStructure;
    float costToRenderChildren;
    int column;
    float delta;
    int depth;
    bool displayed;
    bool empty;
    unsigned char emptyChildren;
    int height;
    bool leaf;
    int level;
    int maximum;
    int maximumLevel;
    bool maximumPriorityQueueTest;
    int minimum;
    OcNode* parent;
    float percentageOfEmptyVolume;
    float ratioOfVisibility;
    bool ratioOfVisibilityTest;
    float ratioOfVisibilityThreshold;
    bool renderingCostFunctionTest;
    float renderingCost;
    int row;
    int slab;
    float sliceFactor;
    BoundingBox* textureBox;
    Volume* volume;
    BoundingBox* volumeBox;
    BoundingSphere* volumeSphere;
    int which;
    int width;
    float xWeight;
    float x;
    float y;
    float yWeight;
    float z;
    float zWeight;
};

#endif /*OCNODE_H_*/
