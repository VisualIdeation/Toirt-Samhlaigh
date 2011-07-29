/*
 *  OcTree
 *
 * Author: Patrick O'Leary
 * Created: November 5, 2007
 * Copyright 2007. All rights reserved.
 */
#ifndef OCTREE_H_
#define OCTREE_H_

#include <vector>

// begin Forward Declarations
class Brick;
class CFrustum;
class ComparableOcNode;
class Matrix4x4;
class OcNode;
class ShaderObject;
// end Forward Declarations

class OcTree {
public:
    OcTree(Brick* _brick);
    ~OcTree(void);
    void drawGrid(const CFrustum & cFrustum);
    void drawVolume(const CFrustum & cFrustum, ShaderObject* shaderObject, float * point, int minimumIndex, int maximumIndex,
            const Matrix4x4 * modelviewInverse);
    void setMaximumPriorityQueueTest(bool _maximumPriorityQueueTest);
    void setPercentageOfEmptyVolume(float* alpha);
    void setPercentageOfEmptyVolume(float* red, float* green, float* blue);
    void setPercentageOfEmptyVolume2_5D(float* alpha);
    void setRatioOfVisibility(float* alpha);
    void setRatioOfVisibility(float* red, float* green, float* blue);
    void setRatioOfVisibility2_5D(float* alpha);
    void setRatioOfVisibilityTest(bool _ratioOfVisibilityTest);
    void setRatioOfVisibilityThreshold(float _ratioOfVisibilityThreshold);
    void setRenderingCost(float _renderingCost);
    void setRenderingCostFunction(float* alpha);
    void setRenderingCostFunction(float* red, float* green, float* blue);
    void setRenderingCostFunction2_5D(float* alpha);
    void setRenderingCostFunctionTest(bool _renderingCostFunctionTest);
    OcNode* getRoot(void);
    bool getShowOcNodes(void);
    void setShowOcNodes(bool _showOcNodes);
    void setSliceFactor(float _sliceFactor);
    std::string toString(void);
private:
    OcNode* root;
    bool showOcNodes;
};

#endif /*OCTREE_H_*/
