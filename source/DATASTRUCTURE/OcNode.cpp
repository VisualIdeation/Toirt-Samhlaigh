/*
 * OcNode.cpp - Methods for OcNode class.
 *
 * Author: Patrick O'Leary
 * Created: November 11, 2007
 * Copyright 2007. All rights reserved.
 */
#include <algorithm>

#include <DATA/Volume.h>
#include <DATASTRUCTURE/BoundingBox.h>
#include <DATASTRUCTURE/BoundingSphere.h>
#include <DATASTRUCTURE/Brick.h>
#include <DATASTRUCTURE/ComparableOcNode.h>
#include <DATASTRUCTURE/OcNode.h>
#include <GRAPHIC/CFrustum.h>
#include <MATH/Matrix4x4.h>
#include <MATH/Vector4.h>
#include <SHADER/ShaderObject.h>
#include <UTILITY/Stringify.h>

/*
 * OcNode - Constructor for OcNode.
 */
OcNode::OcNode(void) :
    brick(NULL), child(NULL), cost(0.0), costStructure(0.01), costRender(0.0), delta(0.0), displayed(false), empty(false),
            emptyChildren(0), leaf(false), level(0), maximumLevel(-1), maximumPriorityQueueTest(false), parent(NULL),
            percentageOfEmptyVolume(0.0), ratioOfVisibility(0.1), ratioOfVisibilityTest(false), ratioOfVisibilityThreshold(1.0),
            renderingCostFunctionTest(true), renderingCost(0.2), sliceFactor(1.5), volumeBox(NULL), volumeSphere(NULL), which(-1) {
} // end OcNode()

/*
 * OcNode - Constructor for OcNode.
 *
 * parameter _brick - Brick*
 */
OcNode::OcNode(Brick* _brick) :
    child(NULL), cost(0.0), costStructure(0.01), costRender(0.0), empty(false), emptyChildren(0), level(0), maximumLevel(-1),
            maximumPriorityQueueTest(false), parent(NULL), percentageOfEmptyVolume(0.0), ratioOfVisibility(0.1),
            ratioOfVisibilityTest(false), renderingCostFunctionTest(true), renderingCost(0.2), which(-1) {
    brick = _brick;
    volume = brick->getVolume();
    ratioOfVisibilityThreshold = volume->getRatioOfVisibilityThreshold();
    sliceFactor = volume->getSliceFactor();
    column = brick->getColumn();
    row = brick->getRow();
    slab = brick->getSlab();
    width = brick->getWidth();
    height = brick->getHeight();
    depth = brick->getDepth();
    x = brick->getX();
    y = brick->getY();
    z = brick->getZ();
    volumeBox = brick->getVolumeBox();
    volumeSphere = brick->getVolumeSphere();
    textureBox = new BoundingBox(0, 0, 0, 1, 1, 1);
    calculateDelta(getMaximum(volumeBox, volumeBox->getMaximumIndex()), getMinimum(volumeBox, volumeBox->getMinimumIndex()));
    if ((width / 2 >= volume->getLeafSize() && height / 2 >= volume->getLeafSize() && depth / 2 >= volume->getLeafSize()) && (width
            / 2 >= 4 && height / 2 >= 4 && depth / 2 >= 4)) {
        leaf = false;
        displayed = false;
        createChildren();
    } else {
        leaf = true;
        displayed = true;
    }
    renderingCost = 1.0f / pow(2.0, 3.0);
} // end OcNode()

/*
 * ~OcNode - Destructor for OcNode.
 */
OcNode::~OcNode(void) {
    delete textureBox;
    delete volumeBox;
    delete volumeSphere;
    if (child != 0)
        delete[] child;
} // end ~OcNode()

/*
 * adjustTextureCoordinatesTraversal
 *
 * parameter dx - float
 * parameter dy - float
 * parameter dz - float
 */
void OcNode::adjustTextureCoordinatesTraversal(float dx, float dy, float dz) {
    textureBox->adjustCoordinates(dx, dy, dz);
    if (!isLeaf()) {
        for (int i = 0; i < 8; i++) {
            child[i].adjustTextureCoordinatesTraversal(dx, dy, dz);
        } // end for
    } // end if
} // end adjustTextureCoordinatesTraversal()

/*
 * calculateDelta - Calculate the slice step size.
 *
 * parameter _maximum - float
 * parameter _minimum - float
 */
void OcNode::calculateDelta(float _maximum, float _minimum) {
    delta = 0;
    if (width < height) {
        if (height < depth) {
            delta = (fabs(_maximum - _minimum) * 1.5f) / depth;
        } // end if
        else {
            delta = (fabs(_maximum - _minimum) * 1.5f) / height;
        } // end else
    } // end if
    else if (width < depth) {
        delta = (fabs(_maximum - _minimum) * 1.5f) / depth;
    } // end else if
    else {
        delta = (fabs(_maximum - _minimum) * 1.5f) / width;
    } // end else
    delta *= sliceFactor;
} // end calculateDelta()

/*
 * calculateEmptiness - Calculate the ratio of visibility.
 *
 * parameter alpha - float*
 */
void OcNode::calculateEmptiness(float* alpha) {
    emptyChildren = 0;
    ratioOfVisibility = 0.0f;
    float value = 0.0f;
    int w = volume->getWidth();
    int h = volume->getHeight();
    int d = volume->getDepth();
    for (int k = 0; k < depth; k++) {
        for (int j = 0; j < height; j++) {
            for (int i = 0; i < width; i++) {
                value = alpha[volume->getVoxel(((slab + (k)) * (w * h)) + ((row + (j)) * (w)) + (column + (i)))];
                ratioOfVisibility += value;
            } // end for
        } // end for
    } // end for
    if (ratioOfVisibility == 0.0f)
        empty = true;
    else
        empty = false;
} // end calculateEmptiness()

/*
 * calculateEmptiness - Calculate the ratio of visibility.
 *
 * parameter red - float*
 * parameter green - float*
 * parameter blue - float*
 */
void OcNode::calculateEmptiness(float* red, float* green, float* blue) {
    emptyChildren = 0;
    ratioOfVisibility = 0.0f;
    float value = 0.0f;
    int w = volume->getWidth();
    int h = volume->getHeight();
    int d = volume->getDepth();
    for (int k = 0; k < depth; k++) {
        for (int j = 0; j < height; j++) {
            for (int i = 0; i < width; i++) {
                value = 0.0f;
                value += red[volume->getRedVoxel(((slab + (k)) * (w * h)) + ((row + (j)) * (w)) + (column + (i)))];
                value += green[volume->getGreenVoxel(((slab + (k)) * (w * h)) + ((row + (j)) * (w)) + (column + (i)))];
                value += blue[volume->getBlueVoxel(((slab + (k)) * (w * h)) + ((row + (j)) * (w)) + (column + (i)))];
                ratioOfVisibility += value;
            } // end for
        } // end for
    } // end for
    if (ratioOfVisibility == 0.0f)
        empty = true;
    else
        empty = false;
} // end calculateEmptiness()

/*
 * calculateEmptiness2_5D - Calculate the ratio of visibility.
 *
 * parameter alpha - float*
 */
void OcNode::calculateEmptiness2_5D(float * alpha) {
    emptyChildren = 0;
    ratioOfVisibility = 0.0f;
    float value = 0.0f;
    int w = volume->getWidth();
    int h = volume->getHeight();
    int d = volume->getDepth();
    float maximumGradient = volume->getMaximumGradient();
    for (int k = 0; k < depth; k++) {
        for (int j = 0; j < height; j++) {
            for (int i = 0; i < width; i++) {
                int x = volume->getVoxel(((slab + (k)) * (w * h)) + ((row + (j)) * (w)) + (column + (i)));
                int y = 255 - int((log2(volume->getGradient(((slab + k) * w * h) + ((row + j) * w) + (column + i))) / log2(
                        maximumGradient)) * 255.0);
                value = alpha[y * 256 + x];
                ratioOfVisibility += value;
            } // end for
        } // end for
    } // end for
    if (ratioOfVisibility == 0.0f)
        empty = true;
    else
        empty = false;
} // end calculateEmptiness2_5D()

/*
 * calculatePercentageOfEmptyVolume - Calculate the ratio of visibility.
 *
 * parameter alpha - float*
 */
void OcNode::calculatePercentageOfEmptyVolume(float* alpha) {
    emptyChildren = 0;
    percentageOfEmptyVolume = 0.0f;
    ratioOfVisibility = 0.0f;
    float value = 0.0f;
    int w = volume->getWidth();
    int h = volume->getHeight();
    int d = volume->getDepth();
    for (int k = 0; k < depth; k++) {
        for (int j = 0; j < height; j++) {
            for (int i = 0; i < width; i++) {
                value = alpha[volume->getVoxel(((slab + (k)) * (w * h)) + ((row + (j)) * (w)) + (column + (i)))];
                ratioOfVisibility += value;
                if (value == 0.0f)
                    percentageOfEmptyVolume += 1.0f;
            } // end for
        } // end for
    } // end for
    percentageOfEmptyVolume *= ((volume->getScale(0)) * (volume->getScale(1)) * (volume->getScale(2)));
    percentageOfEmptyVolume /= ((w * volume->getScale(0)) * (h * volume->getScale(1)) * (d * volume->getScale(2)));
    if (ratioOfVisibility == 0.0f)
        empty = true;
    else
        empty = false;
} // end calculatePercentageOfEmptyVolume()

/*
 * calculatePercentageOfEmptyVolume2_5D - Calculate the ratio of visibility.
 *
 * parameter alpha - float*
 */
void OcNode::calculatePercentageOfEmptyVolume2_5D(float* alpha) {
    emptyChildren = 0;
    percentageOfEmptyVolume = 0.0f;
    ratioOfVisibility = 0.0f;
    float value = 0.0f;
    int w = volume->getWidth();
    int h = volume->getHeight();
    int d = volume->getDepth();
    float maximumGradient = volume->getMaximumGradient();
    for (int k = 0; k < depth; k++) {
        for (int j = 0; j < height; j++) {
            for (int i = 0; i < width; i++) {
                int x = volume->getVoxel(((slab + (k)) * (w * h)) + ((row + (j)) * (w)) + (column + (i)));
                int y = 255 - int((log2(volume->getGradient(((slab + k) * w * h) + ((row + j) * w) + (column + i))) / log2(
                        maximumGradient)) * 255.0);
                value = alpha[y * 256 + x];
                ratioOfVisibility += value;
                if (value == 0.0f)
                    percentageOfEmptyVolume += 1.0f;
            } // end for
        } // end for
    } // end for
    percentageOfEmptyVolume *= ((volume->getScale(0)) * (volume->getScale(1)) * (volume->getScale(2)));
    percentageOfEmptyVolume /= ((w * volume->getScale(0)) * (h * volume->getScale(1)) * (d * volume->getScale(2)));
    if (ratioOfVisibility == 0.0f)
        empty = true;
    else
        empty = false;
} // end calculatePercentageOfEmptyVolume2_5D()

/*
 * calculatePercentageOfEmptyVolume - Calculate the ratio of visibility.
 *
 * parameter red - float*
 * parameter green - float*
 * parameter blue - float*
 */
void OcNode::calculatePercentageOfEmptyVolume(float* red, float* green, float* blue) {
    percentageOfEmptyVolume = 0.0f;
    ratioOfVisibility = 0.0f;
    float value = 0.0f;
    int w = volume->getWidth();
    int h = volume->getHeight();
    int d = volume->getDepth();
    int typeOfComponents = volume->getTypeOfComponents();
    if (typeOfComponents == 7) {
        for (int k = 0; k < depth; k++) {
            for (int j = 0; j < height; j++) {
                for (int i = 0; i < width; i++) {
                    value = red[volume->getRedVoxel(((slab + (k)) * (w * h)) + ((row + (j)) * (w)) + (column + (i)))];
                    value += green[volume->getGreenVoxel(((slab + (k)) * (w * h)) + ((row + (j)) * (w)) + (column + (i)))];
                    value += blue[volume->getBlueVoxel(((slab + (k)) * (w * h)) + ((row + (j)) * (w)) + (column + (i)))];
                    ratioOfVisibility += value;
                    if (value == 0.0f)
                        percentageOfEmptyVolume += 1.0f;
                } // end for
            } // end for
        } // end for
    } else if (typeOfComponents == 6) {
        for (int k = 0; k < depth; k++) {
            for (int j = 0; j < height; j++) {
                for (int i = 0; i < width; i++) {
                    value = red[volume->getRedVoxel(((slab + (k)) * (w * h)) + ((row + (j)) * (w)) + (column + (i)))];
                    value += green[volume->getGreenVoxel(((slab + (k)) * (w * h)) + ((row + (j)) * (w)) + (column + (i)))];
                    ratioOfVisibility += value;
                    if (value == 0.0f)
                        percentageOfEmptyVolume += 1.0f;
                } // end for
            } // end for
        } // end for
    } else if (typeOfComponents == 5) {
        for (int k = 0; k < depth; k++) {
            for (int j = 0; j < height; j++) {
                for (int i = 0; i < width; i++) {
                    value = red[volume->getRedVoxel(((slab + (k)) * (w * h)) + ((row + (j)) * (w)) + (column + (i)))];
                    value += blue[volume->getBlueVoxel(((slab + (k)) * (w * h)) + ((row + (j)) * (w)) + (column + (i)))];
                    ratioOfVisibility += value;
                    if (value == 0.0f)
                        percentageOfEmptyVolume += 1.0f;
                } // end for
            } // end for
        } // end for
    } else if (typeOfComponents == 4) {
        for (int k = 0; k < depth; k++) {
            for (int j = 0; j < height; j++) {
                for (int i = 0; i < width; i++) {
                    value = red[volume->getRedVoxel(((slab + (k)) * (w * h)) + ((row + (j)) * (w)) + (column + (i)))];
                    ratioOfVisibility += value;
                    if (value == 0.0f)
                        percentageOfEmptyVolume += 1.0f;
                } // end for
            } // end for
        } // end for
    } else if (typeOfComponents == 3) {
        for (int k = 0; k < depth; k++) {
            for (int j = 0; j < height; j++) {
                for (int i = 0; i < width; i++) {
                    value = green[volume->getGreenVoxel(((slab + (k)) * (w * h)) + ((row + (j)) * (w)) + (column + (i)))];
                    value += blue[volume->getBlueVoxel(((slab + (k)) * (w * h)) + ((row + (j)) * (w)) + (column + (i)))];
                    ratioOfVisibility += value;
                    if (value == 0.0f)
                        percentageOfEmptyVolume += 1.0f;
                } // end for
            } // end for
        } // end for
    } else if (typeOfComponents == 2) {
        for (int k = 0; k < depth; k++) {
            for (int j = 0; j < height; j++) {
                for (int i = 0; i < width; i++) {
                    value = green[volume->getGreenVoxel(((slab + (k)) * (w * h)) + ((row + (j)) * (w)) + (column + (i)))];
                    ratioOfVisibility += value;
                    if (value == 0.0f)
                        percentageOfEmptyVolume += 1.0f;
                } // end for
            } // end for
        } // end for
    } else if (typeOfComponents == 1) {
        for (int k = 0; k < depth; k++) {
            for (int j = 0; j < height; j++) {
                for (int i = 0; i < width; i++) {
                    value = blue[volume->getBlueVoxel(((slab + (k)) * (w * h)) + ((row + (j)) * (w)) + (column + (i)))];
                    ratioOfVisibility += value;
                    if (value == 0.0f)
                        percentageOfEmptyVolume += 1.0f;
                } // end for
            } // end for
        } // end for
    }
    percentageOfEmptyVolume *= ((volume->getScale(0)) * (volume->getScale(1)) * (volume->getScale(2)));
    percentageOfEmptyVolume /= ((w * volume->getScale(0)) * (h * volume->getScale(1)) * (d * volume->getScale(2)));
} // end calculatePercentageOfEmptyVolume()

/*
 * calculateRatioOfVisibility - Calculate the ratio of visibility.
 *
 * parameter alpha - float*
 */
void OcNode::calculateRatioOfVisibility(float* alpha) {
    ratioOfVisibility = 0.0f;
    int w = volume->getWidth();
    int h = volume->getHeight();
    int d = volume->getDepth();
    for (int k = 0; k < depth; k++) {
        for (int j = 0; j < height; j++) {
            for (int i = 0; i < width; i++) {
                ratioOfVisibility += alpha[volume->getVoxel(((slab + (k)) * (w * h)) + ((row + (j)) * (w)) + (column + (i)))];
            } // end for
        } // end for
    } // end for
    ratioOfVisibility /= ((width) * (height) * (depth));
} // end calculateRatioOfVisibility()

/*
 * calculateRatioOfVisibility2_5D - Calculate the ratio of visibility.
 *
 * parameter alpha - float*
 */
void OcNode::calculateRatioOfVisibility2_5D(float* alpha) {
    ratioOfVisibility = 0.0f;
    float value = 0.0f;
    int w = volume->getWidth();
    int h = volume->getHeight();
    int d = volume->getDepth();
    float maximumGradient = volume->getMaximumGradient();
    for (int k = 0; k < depth; k++) {
        for (int j = 0; j < height; j++) {
            for (int i = 0; i < width; i++) {
                int x = volume->getVoxel(((slab + (k)) * (w * h)) + ((row + (j)) * (w)) + (column + (i)));
                int y = 255 - int((log2(volume->getGradient(((slab + k) * w * h) + ((row + j) * w) + (column + i))) / log2(
                        maximumGradient)) * 255.0);
                value = alpha[y * 256 + x];
                ratioOfVisibility += value;
            } // end for
        } // end for
    } // end for
    ratioOfVisibility /= ((width) * (height) * (depth));
} // end calculateRatioOfVisibility2_5D()

/*
 * calculateRatioOfVisibility - Calculate the ratio of visibility.
 *
 * parameter red - float*
 * parameter green - float*
 * parameter blue - float*
 */
void OcNode::calculateRatioOfVisibility(float* red, float* green, float* blue) {
    ratioOfVisibility = 0.0f;
    int w = volume->getWidth();
    int h = volume->getHeight();
    int d = volume->getDepth();
    int typeOfComponents = volume->getTypeOfComponents();
    if (typeOfComponents == 7) {
        for (int k = 0; k < depth; k++) {
            for (int j = 0; j < height; j++) {
                for (int i = 0; i < width; i++) {
                    ratioOfVisibility += red[volume->getRedVoxel(((slab + (k)) * (w * h)) + ((row + (j)) * (w)) + (column + (i)))];
                    ratioOfVisibility += green[volume->getGreenVoxel(((slab + (k)) * (w * h)) + ((row + (j)) * (w))
                            + (column + (i)))];
                    ratioOfVisibility
                            += blue[volume->getBlueVoxel(((slab + (k)) * (w * h)) + ((row + (j)) * (w)) + (column + (i)))];
                } // end for
            } // end for
        } // end for
        ratioOfVisibility /= (3 * (width) * (height) * (depth));
    } else if (typeOfComponents == 6) {
        for (int k = 0; k < depth; k++) {
            for (int j = 0; j < height; j++) {
                for (int i = 0; i < width; i++) {
                    ratioOfVisibility += red[volume->getRedVoxel(((slab + (k)) * (w * h)) + ((row + (j)) * (w)) + (column + (i)))];
                    ratioOfVisibility += green[volume->getGreenVoxel(((slab + (k)) * (w * h)) + ((row + (j)) * (w))
                            + (column + (i)))];
                } // end for
            } // end for
        } // end for
        ratioOfVisibility /= (2 * (width) * (height) * (depth));
    } else if (typeOfComponents == 5) {
        for (int k = 0; k < depth; k++) {
            for (int j = 0; j < height; j++) {
                for (int i = 0; i < width; i++) {
                    ratioOfVisibility += red[volume->getRedVoxel(((slab + (k)) * (w * h)) + ((row + (j)) * (w)) + (column + (i)))];
                    ratioOfVisibility
                            += blue[volume->getBlueVoxel(((slab + (k)) * (w * h)) + ((row + (j)) * (w)) + (column + (i)))];
                } // end for
            } // end for
        } // end for
        ratioOfVisibility /= (2 * (width) * (height) * (depth));
    } else if (typeOfComponents == 4) {
        for (int k = 0; k < depth; k++) {
            for (int j = 0; j < height; j++) {
                for (int i = 0; i < width; i++) {
                    ratioOfVisibility += red[volume->getRedVoxel(((slab + (k)) * (w * h)) + ((row + (j)) * (w)) + (column + (i)))];
                } // end for
            } // end for
        } // end for
        ratioOfVisibility /= ((width) * (height) * (depth));
    } else if (typeOfComponents == 3) {
        for (int k = 0; k < depth; k++) {
            for (int j = 0; j < height; j++) {
                for (int i = 0; i < width; i++) {
                    ratioOfVisibility += green[volume->getGreenVoxel(((slab + (k)) * (w * h)) + ((row + (j)) * (w))
                            + (column + (i)))];
                    ratioOfVisibility
                            += blue[volume->getBlueVoxel(((slab + (k)) * (w * h)) + ((row + (j)) * (w)) + (column + (i)))];
                } // end for
            } // end for
        } // end for
        ratioOfVisibility /= (2 * (width) * (height) * (depth));
    } else if (typeOfComponents == 2) {
        for (int k = 0; k < depth; k++) {
            for (int j = 0; j < height; j++) {
                for (int i = 0; i < width; i++) {
                    ratioOfVisibility += green[volume->getGreenVoxel(((slab + (k)) * (w * h)) + ((row + (j)) * (w))
                            + (column + (i)))];
                } // end for
            } // end for
        } // end for
        ratioOfVisibility /= ((width) * (height) * (depth));
    } else if (typeOfComponents == 1) {
        for (int k = 0; k < depth; k++) {
            for (int j = 0; j < height; j++) {
                for (int i = 0; i < width; i++) {
                    ratioOfVisibility
                            += blue[volume->getBlueVoxel(((slab + (k)) * (w * h)) + ((row + (j)) * (w)) + (column + (i)))];
                } // end for
            } // end for
        } // end for
        ratioOfVisibility /= ((width) * (height) * (depth));
    }
} // end calculateRatioOfVisibility()

/*
 * calculateSampleDistance - Calculate the overall sample distance.
 *
 * parameter modelviewInverse - Matrix4x4*
 * parameter _minimum - float
 * parameter _maximum - float
 * return - float
 */
float OcNode::calculateSampleDistance(const Matrix4x4 * modelviewInverse, float _minimum, float _maximum) {
    /*
     * Calculate the the _minimum and _maximum x-, y- or z-position of the
     * rotated bounding box.
     *
     * Equivalent to but quicker than:
     *
     *  _maximumD = (_maximum, 0, 0), (0, _maximum, 0) or (0, 0, _maximum);
     *
     *  _minimumD = (_minimum, 0, 0), (0, _minimum, 0) or (0, 0, _minimum);
     *
     *   maximumV = modelviewInverse * _maximumD;
     *
     *   minimumV = modelviewInverse * _minimumD;
     */
    float* point = new float[3];
    for (int i = 0; i < 3; i++) {
        point[i] = (modelviewInverse->get(2, i) * _maximum) - (modelviewInverse->get(2, i) * _minimum);
    }
    float sampleDistance = float(sqrt(double(point[0] * point[0] + point[1] * point[1] + point[2] * point[2])));
    delete[] point;
    return sampleDistance;
} // end calculateSampleDistance()

/*
 * createChildren
 */
void OcNode::createChildren(void) {
    child = new OcNode[8];
    for (int i = 0; i < 8; i++) {
        child[i].setChild(level + 1, i, this, brick, maximumLevel);
    } // end for
} // end createChildren()

/*
 * drawGrid
 */
void OcNode::drawGrid(void) {
    glDisable(GL_LIGHTING);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    //glCallList(Box);
    glBegin(GL_QUADS);
    // Front Face
    glNormal3f(0.0f, 0.0f, 1.0f);
    glVertex3f(getVolumeBox()->getX(2), getVolumeBox()->getY(2), getVolumeBox()->getZ(2));
    glVertex3f(getVolumeBox()->getX(3), getVolumeBox()->getY(3), getVolumeBox()->getZ(3));
    glVertex3f(getVolumeBox()->getX(7), getVolumeBox()->getY(7), getVolumeBox()->getZ(7));
    glVertex3f(getVolumeBox()->getX(6), getVolumeBox()->getY(6), getVolumeBox()->getZ(6));
    // Back Face
    glNormal3f(0.0f, 0.0f, -1.0f);
    glVertex3f(getVolumeBox()->getX(0), getVolumeBox()->getY(0), getVolumeBox()->getZ(0));
    glVertex3f(getVolumeBox()->getX(4), getVolumeBox()->getY(4), getVolumeBox()->getZ(4));
    glVertex3f(getVolumeBox()->getX(5), getVolumeBox()->getY(5), getVolumeBox()->getZ(5));
    glVertex3f(getVolumeBox()->getX(1), getVolumeBox()->getY(1), getVolumeBox()->getZ(1));
    // Top Face
    glNormal3f(0.0f, 1.0f, 0.0f);
    glVertex3f(getVolumeBox()->getX(4), getVolumeBox()->getY(4), getVolumeBox()->getZ(4));
    glVertex3f(getVolumeBox()->getX(6), getVolumeBox()->getY(6), getVolumeBox()->getZ(6));
    glVertex3f(getVolumeBox()->getX(7), getVolumeBox()->getY(7), getVolumeBox()->getZ(7));
    glVertex3f(getVolumeBox()->getX(5), getVolumeBox()->getY(5), getVolumeBox()->getZ(5));
    // Bottom Face
    glNormal3f(0.0f, -1.0f, 0.0f);
    glVertex3f(getVolumeBox()->getX(0), getVolumeBox()->getY(0), getVolumeBox()->getZ(0));
    glVertex3f(getVolumeBox()->getX(1), getVolumeBox()->getY(1), getVolumeBox()->getZ(1));
    glVertex3f(getVolumeBox()->getX(5), getVolumeBox()->getY(5), getVolumeBox()->getZ(5));
    glVertex3f(getVolumeBox()->getX(4), getVolumeBox()->getY(4), getVolumeBox()->getZ(4));
    // Right face
    glNormal3f(1.0f, 0.0f, 0.0f);
    glVertex3f(getVolumeBox()->getX(1), getVolumeBox()->getY(1), getVolumeBox()->getZ(1));
    glVertex3f(getVolumeBox()->getX(5), getVolumeBox()->getY(5), getVolumeBox()->getZ(5));
    glVertex3f(getVolumeBox()->getX(7), getVolumeBox()->getY(7), getVolumeBox()->getZ(7));
    glVertex3f(getVolumeBox()->getX(3), getVolumeBox()->getY(3), getVolumeBox()->getZ(3));
    // Left Face
    glNormal3f(-1.0f, 0.0f, 0.0f);
    glVertex3f(getVolumeBox()->getX(0), getVolumeBox()->getY(0), getVolumeBox()->getZ(0));
    glVertex3f(getVolumeBox()->getX(2), getVolumeBox()->getY(2), getVolumeBox()->getZ(2));
    glVertex3f(getVolumeBox()->getX(6), getVolumeBox()->getY(6), getVolumeBox()->getZ(6));
    glVertex3f(getVolumeBox()->getX(4), getVolumeBox()->getY(4), getVolumeBox()->getZ(4));
    glEnd();
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glEnable(GL_LIGHTING);
} // end drawGrid()

/*
 * drawGridTraversal
 *
 * parameter _level - int
 * parameter cFrustum - const CFrustum &
 * parameter testChildren - bool
 */
void OcNode::drawGridTraversal(int _level, const CFrustum & cFrustum, bool testChildren) {
    if (level == _level) {
        glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
        drawGrid();
    } else {
        for (int i = 0; i < 8; i++) {
            child[i].drawGridTraversal(_level, cFrustum, testChildren);
        } // end for
    } // end if
} // end drawGridTraversal()

/*
 * drawGridTraversal
 *
 * parameter cFrustum - const CFrustum &
 * parameter testChildren - bool
 */
void OcNode::drawGridTraversal(const CFrustum & cFrustum, bool testChildren) {
    /*
     if (testChildren) {
     // test the sphere first
     switch (cFrustum->sphereLocation(volumeSphere->getCenter(), volumeSphere->getRadius())) {
     case FULL_OUTSIDE:
     return;
     case FULL_INSIDE:
     testChildren = false;
     break;
     case PARTIAL_INSIDE:
     // check if the box is in view
     float* fMinimum = volumeBox->getMinimum(0);
     float* fMaximum = volumeBox->getMaximum(7);
     switch (cFrustum->rightParallelepipedLocation(fMinimum, fMaximum)) {
     case FULL_INSIDE:
     testChildren = false;
     break;
     case FULL_OUTSIDE:
     return;
     }
     delete fMinimum;
     delete fMaximum;
     break;
     }
     }
     */
    if (ratioOfVisibility != 0.0f) {
        if (ratioOfVisibilityTest) {
            if (isLeaf() || (ratioOfVisibility >= ratioOfVisibilityThreshold /* && !testChildren */)) {
                glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
                drawGrid();
            } else {
                for (int i = 0; i < 8; i++) {
                    child[i].drawGridTraversal(cFrustum, testChildren);
                } // end for
            } // end if
        } else if (maximumPriorityQueueTest) {
            if (isLeaf() || displayed) {
                glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
                drawGrid();
            } else {
                for (int i = 0; i < 8; i++) {
                    child[i].drawGridTraversal(cFrustum, testChildren);
                } // end for
            } // end if
        } else if (renderingCostFunctionTest) {
            if (isLeaf() || (cost < costToRenderChildren)) {
                glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
                drawGrid();
            } else {
                for (int i = 0; i < 8; i++) {
                    child[i].drawGridTraversal(cFrustum, testChildren);
                } // end for
            } // end if
        }
    } // end if
} // end drawGridTraversal()

/*
 * drawPreIntegratedViewAlignedSlices - Draw the proxy geometry.
 *
 * parameter slicePlaneNormal - const Vector4 &
 * parameter minimumIndex - int
 * parameter maximumIndex - int
 * parameter modelviewInverse - const Matrix4x4 *
 */
void OcNode::drawPreIntegratedViewAlignedSlices(const Vector4 & slicePlaneNormal, int minimumIndex, int maximumIndex,
        const Matrix4x4 * modelviewInverse) {
    Vector4 slicePointFront(volumeBox->getCorner(minimumIndex));
    Vector4 slicePointBack(volumeBox->getCorner(minimumIndex));
    float _minimum = getMinimum(volumeBox, minimumIndex);
    float _maximum = getMaximum(volumeBox, maximumIndex);
    float sampleDistance = calculateSampleDistance(modelviewInverse, _minimum, _maximum);
    Vector4* sliceDelta = Vector4::multiply(slicePlaneNormal, delta);
    int samples = (int) ((sampleDistance) / delta);
    // Calculate edge intersections between the plane and the boxes
    Vector4* verts = new Vector4[6]; // for edge intersections
    Vector4* sFront = new Vector4[6]; // for texture intersections
    Vector4* sBack = new Vector4[6]; // for texture intersections
    slicePointBack.subtract(sliceDelta);
    for (int i = 0; i <= samples; i++) {
        for (int j = 0; j < 6; j++) {
            viewAlignedVertex(j, maximumIndex, slicePointFront, slicePointBack, slicePlaneNormal, sFront, sBack, verts);
        } // end for
        // Draw slice and texture map it
        glBegin(GL_POLYGON);
        glMultiTexCoord3f(0, sFront[0].getX(), sFront[0].getY(), sFront[0].getZ());
        glMultiTexCoord3f(1, sBack[0].getX(), sBack[0].getY(), sBack[0].getZ());
        glVertex3f(verts[0].getX(), verts[0].getY(), verts[0].getZ());
        glMultiTexCoord3f(0, sFront[1].getX(), sFront[1].getY(), sFront[1].getZ());
        glMultiTexCoord3f(1, sBack[1].getX(), sBack[1].getY(), sBack[1].getZ());
        glVertex3f(verts[1].getX(), verts[1].getY(), verts[1].getZ());
        glMultiTexCoord3f(0, sFront[2].getX(), sFront[2].getY(), sFront[2].getZ());
        glMultiTexCoord3f(1, sBack[2].getX(), sBack[2].getY(), sBack[2].getZ());
        glVertex3f(verts[2].getX(), verts[2].getY(), verts[2].getZ());
        glMultiTexCoord3f(0, sFront[3].getX(), sFront[3].getY(), sFront[3].getZ());
        glMultiTexCoord3f(1, sBack[3].getX(), sBack[3].getY(), sBack[3].getZ());
        glVertex3f(verts[3].getX(), verts[3].getY(), verts[3].getZ());
        glMultiTexCoord3f(0, sFront[4].getX(), sFront[4].getY(), sFront[4].getZ());
        glMultiTexCoord3f(1, sBack[4].getX(), sBack[4].getY(), sBack[4].getZ());
        glVertex3f(verts[4].getX(), verts[4].getY(), verts[4].getZ());
        glMultiTexCoord3f(0, sFront[5].getX(), sFront[5].getY(), sFront[5].getZ());
        glMultiTexCoord3f(1, sBack[5].getX(), sBack[5].getY(), sBack[5].getZ());
        glVertex3f(verts[5].getX(), verts[5].getY(), verts[5].getZ());
        glEnd();
        // increment the slice plane by the slice distance
        slicePointFront.add(sliceDelta);
        slicePointBack.add(sliceDelta);
    } // end for
    glFlush();
    delete sliceDelta;
    delete[] verts;
    delete[] sFront;
    delete[] sBack;
} // end drawPreIntegratedViewAlignedSlices()

/*
 * drawShadowedViewAlignedSlices - Draw the proxy geometry
 *
 * parameter slicePlaneNormal - const Vector4 &
 * parameter minimumIndex - int
 * parameter maximumIndex - int
 * parameter modelviewInverse - const Matrix4x4 *
 */
void OcNode::drawShadowedViewAlignedSlices(const Vector4 & slicePlaneNormal, int minimumIndex, int maximumIndex,
        const Matrix4x4 * modelviewInverse) {
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE_MINUS_DST_ALPHA, GL_ONE);
    glEnable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);
    Vector4 slicePointFront(volumeBox->getCorner(minimumIndex));
    Vector4 slicePointBack(volumeBox->getCorner(minimumIndex));
    float _minimum = getMinimum(volumeBox, minimumIndex);
    float _maximum = getMaximum(volumeBox, maximumIndex);
    float sampleDistance = calculateSampleDistance(modelviewInverse, _minimum, _maximum);
    Vector4* sliceDelta = Vector4::multiply(slicePlaneNormal, delta);
    int samples = (int) ((sampleDistance) / delta);
    Vector4* verts = new Vector4[6]; // for edge intersections
    Vector4* sFront = new Vector4[6]; // for texture intersections
    Vector4* sBack = new Vector4[6]; // for texture intersections
    slicePointBack.add(sliceDelta);
    for (int i = 0; i <= samples; i++) {
        for (int j = 0; j < 6; j++) {
            viewAlignedVertex(j, maximumIndex, slicePointFront, slicePointBack, slicePlaneNormal, sFront, sBack, verts);
        } // end for
        glBegin(GL_POLYGON);
        glMultiTexCoord3f(0, sFront[0].getX(), sFront[0].getY(), sFront[0].getZ());
        glMultiTexCoord3f(1, sBack[0].getX(), sBack[0].getY(), sBack[0].getZ());
        glVertex3f(verts[0].getX(), verts[0].getY(), verts[0].getZ());
        glMultiTexCoord3f(0, sFront[1].getX(), sFront[1].getY(), sFront[1].getZ());
        glMultiTexCoord3f(1, sBack[1].getX(), sBack[1].getY(), sBack[1].getZ());
        glVertex3f(verts[1].getX(), verts[1].getY(), verts[1].getZ());
        glMultiTexCoord3f(0, sFront[2].getX(), sFront[2].getY(), sFront[2].getZ());
        glMultiTexCoord3f(1, sBack[2].getX(), sBack[2].getY(), sBack[2].getZ());
        glVertex3f(verts[2].getX(), verts[2].getY(), verts[2].getZ());
        glMultiTexCoord3f(0, sFront[3].getX(), sFront[3].getY(), sFront[3].getZ());
        glMultiTexCoord3f(1, sBack[3].getX(), sBack[3].getY(), sBack[3].getZ());
        glVertex3f(verts[3].getX(), verts[3].getY(), verts[3].getZ());
        glMultiTexCoord3f(0, sFront[4].getX(), sFront[4].getY(), sFront[4].getZ());
        glMultiTexCoord3f(1, sBack[4].getX(), sBack[4].getY(), sBack[4].getZ());
        glVertex3f(verts[4].getX(), verts[4].getY(), verts[4].getZ());
        glMultiTexCoord3f(0, sFront[5].getX(), sFront[5].getY(), sFront[5].getZ());
        glMultiTexCoord3f(1, sBack[5].getX(), sBack[5].getY(), sBack[5].getZ());
        glVertex3f(verts[5].getX(), verts[5].getY(), verts[5].getZ());
        glEnd();
        slicePointFront.subtract(sliceDelta);
        slicePointBack.subtract(sliceDelta);
    } // end for
    glFlush();
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);
    delete sliceDelta;
    delete[] verts;
    delete[] sFront;
    delete[] sBack;
} // end drawShadowedViewAlignedSlices()

/*
 * drawViewAlignedSlices - Draw the proxy geometry
 *
 * parameter slicePlaneNormal - const Vector4 &
 * parameter minimumIndex - int
 * parameter maximumIndex - int
 * parameter modelviewInverse - const Matrix4x4 *
 */
void OcNode::drawViewAlignedSlices(const Vector4 & slicePlaneNormal, int minimumIndex, int maximumIndex,
        const Matrix4x4 * modelviewInverse) {
    Vector4 slicePoint(volumeBox->getCorner(minimumIndex));
    float _minimum = getMinimum(volumeBox, minimumIndex);
    float _maximum = getMaximum(volumeBox, maximumIndex);
    float sampleDistance = calculateSampleDistance(modelviewInverse, _minimum, _maximum);
    Vector4* sliceDelta = Vector4::multiply(slicePlaneNormal, delta);
    int samples = (int) ((sampleDistance) / delta);
    // Calculate edge intersections between the plane and the boxes
    Vector4* verts = new Vector4[6]; // for edge intersections
    Vector4* tverts = new Vector4[6]; // for texture intersections
    for (int i = 0; i <= samples; i++) {
        for (int j = 0; j < 6; j++) {
            viewAlignedVertex(j, maximumIndex, slicePoint, slicePlaneNormal, tverts, verts);
        } // end for
        // Draw slice and texture map it
        glBegin(GL_POLYGON);
        glTexCoord3f(tverts[0].getX(), tverts[0].getY(), tverts[0].getZ());
        glVertex3f(verts[0].getX(), verts[0].getY(), verts[0].getZ());
        glTexCoord3f(tverts[1].getX(), tverts[1].getY(), tverts[1].getZ());
        glVertex3f(verts[1].getX(), verts[1].getY(), verts[1].getZ());
        glTexCoord3f(tverts[2].getX(), tverts[2].getY(), tverts[2].getZ());
        glVertex3f(verts[2].getX(), verts[2].getY(), verts[2].getZ());
        glTexCoord3f(tverts[3].getX(), tverts[3].getY(), tverts[3].getZ());
        glVertex3f(verts[3].getX(), verts[3].getY(), verts[3].getZ());
        glTexCoord3f(tverts[4].getX(), tverts[4].getY(), tverts[4].getZ());
        glVertex3f(verts[4].getX(), verts[4].getY(), verts[4].getZ());
        glTexCoord3f(tverts[5].getX(), tverts[5].getY(), tverts[5].getZ());
        glVertex3f(verts[5].getX(), verts[5].getY(), verts[5].getZ());
        glEnd();
        // increment the slice plane by the slice distance
        slicePoint.add(sliceDelta);
    } // end for
    glFlush();
    delete sliceDelta;
    delete[] verts;
    delete[] tverts;
} // end drawViewAlignedSlices()

/*
 * drawViewAlignedSlicesGPU - Draw the proxy geometry
 *
 * parameter minimumIndex - int
 * parameter maximumIndex - int
 * parameter modelviewInverse - const Matrix4x4 *
 */
void OcNode::drawViewAlignedSlicesGPU(int minimumIndex, int maximumIndex, const Matrix4x4 * modelviewInverse) {
    float _minimum = getMinimum(volumeBox, minimumIndex);
    float _maximum = getMaximum(volumeBox, maximumIndex);
    float sampleDistance = calculateSampleDistance(modelviewInverse, _minimum, _maximum);
    int samples = (int) ((sampleDistance) / delta);
    for (int sample = 0; sample <= samples; sample++) {
        glBegin(GL_POLYGON);
        glVertex2i(0, sample);
        glVertex2i(1, sample);
        glVertex2i(2, sample);
        glVertex2i(3, sample);
        glVertex2i(4, sample);
        glVertex2i(5, sample);
        glEnd();
    } // end for
    glFlush();
} // end drawViewAlignedSlicesGPU()

/*
 * drawVolume
 *
 * parameter minimumIndex - int
 * parameter maximumIndex - int
 * parameter slicePlaneNormal - const Vector4 &
 * parameter modelviewInverse - const Matrix4x4 *
 */
void OcNode::drawVolume(int minimumIndex, int maximumIndex, const Vector4 & slicePlaneNormal, const Matrix4x4 * modelviewInverse) {
    drawPreIntegratedViewAlignedSlices(slicePlaneNormal, minimumIndex, maximumIndex, modelviewInverse);
} // end drawVolume()

/*
 * drawVolume
 *
 * parameter shaderObject - ShaderObject*
 * parameter minimumIndex - int
 * parameter maximumIndex - int
 * parameter modelviewInverse - const Matrix4x4 *
 */
void OcNode::drawVolume(ShaderObject* shaderObject, int minimumIndex, int maximumIndex, const Matrix4x4 * modelviewInverse) {
    /* GPU Accelerated */
    shaderObject->sendUniform1f("delta", GLfloat(delta));
    float* p;
    GLfloat* vBBox = new GLfloat[24];
    GLfloat* tBBox = new GLfloat[24];
    for (int i = 0; i < 8; i++) {
        p = volumeBox->getCorner(i);
        vBBox[i * 3 + 0] = GLfloat(p[0]);
        vBBox[i * 3 + 1] = GLfloat(p[1]);
        vBBox[i * 3 + 2] = GLfloat(p[2]);
        delete[] p;
        p = textureBox->getCorner(i);
        tBBox[i * 3 + 0] = GLfloat(p[0]);
        tBBox[i * 3 + 1] = GLfloat(p[1]);
        tBBox[i * 3 + 2] = GLfloat(p[2]);
        delete[] p;
    }
    shaderObject->sendUniform3fv("vBBox", 8, vBBox);
    shaderObject->sendUniform3fv("tBBox", 8, tBBox);
    /* END GPU Accelerated */
    drawViewAlignedSlicesGPU(minimumIndex, maximumIndex, modelviewInverse);
    //drawPreIntegratedViewAlignedSlices(slicePlaneNormal, minimumIndex, maximumIndex, modelviewInverse);
    delete[] vBBox;
    delete[] tBBox;
} // end drawVolume()

/*
 * drawVolumeTraversal
 *
 * parameter _level - int
 * parameter cFrustum - const CFrustum &
 * parameter point - float *
 * parameter shaderObject - ShaderObject*
 * parameter minimumIndex - int
 * parameter maximumIndex - int
 * parameter modelviewInverse - const Matrix4x4 *
 * parameter testChildren - bool
 */
void OcNode::drawVolumeTraversal(int _level, const CFrustum & cFrustum, float* point, ShaderObject* shaderObject, int minimumIndex,
        int maximumIndex, const Matrix4x4 * modelviewInverse, bool testChildren) {
    if (level == _level) {
        drawVolume(shaderObject, minimumIndex, maximumIndex, modelviewInverse);
    } else {
        std::vector<ComparableOcNode> ocNodeList(8);
        sortChildren(point, ocNodeList);
        for (int i = 0; i < 8; i++) {
            child[ocNodeList[i].getWhich()].drawVolumeTraversal(_level, cFrustum, point, shaderObject, minimumIndex, maximumIndex,
                    modelviewInverse, testChildren);
        } // end for
    } // end if
} // end drawVolumeTraversal()

/*
 * drawVolumeTraversal
 *
 * parameter cFrustum - const CFrustum &
 * parameter point - float *
 * parameter shaderObject - ShaderObject*
 * parameter minimumIndex - int
 * parameter maximumIndex - int
 * parameter modelviewInverse - const Matrix4x4 *
 * parameter testChildren - bool
 */
void OcNode::drawVolumeTraversal(const CFrustum & cFrustum, float* point, ShaderObject* shaderObject, int minimumIndex,
        int maximumIndex, const Matrix4x4 * modelviewInverse, bool testChildren) {
    /*
     if (testChildren) {
     // test the sphere first
     switch (cFrustum->sphereLocation(volumeSphere->getCenter(), volumeSphere->getRadius())) {
     case FULL_OUTSIDE:
     return;
     case FULL_INSIDE:
     testChildren = false;
     break;
     case PARTIAL_INSIDE:
     // check if the box is in view
     float* fMinimum = volumeBox->getMinimum(0);
     float* fMaximum = volumeBox->getMaximum(7);
     switch (cFrustum->rightParallelepipedLocation(fMinimum, fMaximum)) {
     case FULL_INSIDE:
     testChildren = false;
     break;
     case FULL_OUTSIDE:
     return;
     }
     delete fMinimum;
     delete fMaximum;
     break;
     }
     }
     */
    if (ratioOfVisibility != 0.0f) {
        if (ratioOfVisibilityTest) {
            if (isLeaf() || (ratioOfVisibility >= ratioOfVisibilityThreshold /* && !testChildren */)) {
                drawVolume(shaderObject, minimumIndex, maximumIndex, modelviewInverse);
            } else {
                std::vector<ComparableOcNode> ocNodeList(8);
                sortChildren(point, ocNodeList);
                for (int i = 0; i < 8; i++) {
                    child[ocNodeList[i].getWhich()].drawVolumeTraversal(cFrustum, point, shaderObject, minimumIndex, maximumIndex,
                            modelviewInverse, testChildren);
                } // end for
            } // end if
        } else if (maximumPriorityQueueTest) {
            if (isLeaf() || displayed) {
                drawVolume(shaderObject, minimumIndex, maximumIndex, modelviewInverse);
            } else {
                std::vector<ComparableOcNode> ocNodeList(8);
                sortChildren(point, ocNodeList);
                for (int i = 0; i < 8; i++) {
                    child[ocNodeList[i].getWhich()].drawVolumeTraversal(cFrustum, point, shaderObject, minimumIndex, maximumIndex,
                            modelviewInverse, testChildren);
                } // end for
            } // end if
        } else if (renderingCostFunctionTest) {
            if (isLeaf() || (cost < costToRenderChildren)) {
                drawVolume(shaderObject, minimumIndex, maximumIndex, modelviewInverse);
            } else {
                std::vector<ComparableOcNode> ocNodeList(8);
                sortChildren(point, ocNodeList);
                for (int i = 0; i < 8; i++) {
                    child[ocNodeList[i].getWhich()].drawVolumeTraversal(cFrustum, point, shaderObject, minimumIndex, maximumIndex,
                            modelviewInverse, testChildren);
                } // end for
            } // end if
        }
    } // end if
} // end drawVolumeTraversal()

/*
 * drawVolumeTraversal
 *
 * parameter cFrustum - const CFrustum &
 * parameter point - float *
 * parameter minimumIndex - int
 * parameter maximumIndex - int
 * parameter slicePlaneNormal - const Vector4 &
 * parameter modelviewInverse - const Matrix4x4 *
 * parameter testChildren - bool
 */
void OcNode::drawVolumeTraversal(const CFrustum & cFrustum, float * point, int minimumIndex, int maximumIndex,
        const Vector4 & slicePlaneNormal, const Matrix4x4 * modelviewInverse, bool testChildren) {
    /*
     if (testChildren) {
     // test the sphere first
     switch (cFrustum->sphereLocation(volumeSphere->getCenter(), volumeSphere->getRadius())) {
     case FULL_OUTSIDE:
     return;
     case FULL_INSIDE:
     testChildren = false;
     break;
     case PARTIAL_INSIDE:
     // check if the box is in view
     float* fMinimum = volumeBox->getMinimum(0);
     float* fMaximum = volumeBox->getMaximum(7);
     switch (cFrustum->rightParallelepipedLocation(fMinimum, fMaximum)) {
     case FULL_INSIDE:
     testChildren = false;
     break;
     case FULL_OUTSIDE:
     return;
     }
     delete fMinimum;
     delete fMaximum;
     break;
     }
     }
     */
    if (ratioOfVisibility != 0.0f) {
        if (ratioOfVisibilityTest) {
            if (isLeaf() || (ratioOfVisibility >= ratioOfVisibilityThreshold /* && !testChildren */)) {
                drawVolume(minimumIndex, maximumIndex, slicePlaneNormal, modelviewInverse);
            } else {
                std::vector<ComparableOcNode> ocNodeList(8);
                sortChildren(point, ocNodeList);
                for (int i = 0; i < 8; i++) {
                    child[ocNodeList[i].getWhich()].drawVolumeTraversal(cFrustum, point, minimumIndex, maximumIndex,
                            slicePlaneNormal, modelviewInverse, testChildren);
                } // end for
            } // end if
        } else {
            if (isLeaf() || displayed) {
                drawVolume(minimumIndex, maximumIndex, slicePlaneNormal, modelviewInverse);
            } else {
                std::vector<ComparableOcNode> ocNodeList(8);
                sortChildren(point, ocNodeList);
                for (int i = 0; i < 8; i++) {
                    child[ocNodeList[i].getWhich()].drawVolumeTraversal(cFrustum, point, minimumIndex, maximumIndex,
                            slicePlaneNormal, modelviewInverse, testChildren);
                } // end for
            } // end if
        } // end if
    } // end if
} // end drawVolumeTraversal()

/*
 * setCenter
 */
void OcNode::setCenter(void) {
    setX(float(volumeBox->getCenter(0)));
    setY(float(volumeBox->getCenter(1)));
    setZ(float(volumeBox->getCenter(2)));
} // end setCenter()

/*
 * getChildren
 *
 * return - OcNode*
 */
OcNode* OcNode::getChildren(void) {
    return child;
} // end getChildren()

/*
 * setChild
 *
 * parameter _level -  int
 * parameter _which - int
 * parameter _parent - OcNode*
 * parameter _brick - Brick*
 * parameter _maximumLevel - int
 */
void OcNode::setChild(int _level, int _which, OcNode* _parent, Brick* _brick, int _maximumLevel) {
    level = _level;
    which = _which;
    parent = _parent;
    brick = _brick;
    maximumLevel = _maximumLevel;
    volume = brick->getVolume();
    ratioOfVisibilityThreshold = volume->getRatioOfVisibilityThreshold();
    sliceFactor = volume->getSliceFactor();
    child = 0;
    displayed = false;
    setWeight();
    setDimensions();
    setVolumeBox();
    setVolumeSphere();
    setTextureBox();
    setLocations();
    setCenter();
    delta = parent->getDelta();
    if ((width / 2 >= volume->getLeafSize() && height / 2 >= volume->getLeafSize() && depth / 2 >= volume->getLeafSize()) && (width
            / 2 >= 4 && height / 2 >= 4 && depth / 2 >= 4)) {
        leaf = false;
        displayed = false;
        createChildren();
    } else {
        leaf = true;
        displayed = true;
    }
    if (level < 3)
        renderingCost = 1.0f / pow(2.0, (3.0 - double(level)));
    else
        renderingCost = 0.5f;
}
// end setChild()

/*
 * getColumn
 *
 * return - int
 */
int OcNode::getColumn(void) {
    return column;
} // end getColumn()

/*
 * setColumn
 */
void OcNode::setColumn(void) {
    column = which % 2;
    //column *= (volume->getWidth() / int(pow(double(2), double(level))));
    // Do we need to handle remainders
    if ((parent->getWidth() % 2) != 0) {
        column *= (parent->getWidth() / 2) + 1;
    } else
        column *= parent->getWidth() / 2;
    // column *= (brick->getWidth() / int(pow(double(2), double(level))));
    column += parent->getColumn();
} // end setColumn()

/*
 * getCost
 *
 * return - float
 */
float OcNode::getCost(void) {
    return cost;
} // end getCost()

/*
 * getCostRender
 *
 * return - float
 */
float OcNode::getCostRender(void) {
    return costRender;
} // end getCostRender()

/*
 * getCostStructure
 *
 * return - float
 */
float OcNode::getCostStructure(void) {
    return costStructure;
} // end getCostStructure()

/*
 * getDelta
 *
 * return - float
 */
float OcNode::getDelta(void) {
    return delta;
} // end getDelta()

/*
 * getDepth
 *
 * return - int
 */
int OcNode::getDepth(void) {
    return depth;
} // end getDepth()

/*
 * setDepth
 */
void OcNode::setDepth(void) {
    if ((parent->getDepth() % 2) != 0 && (((which / 2) == 0) || ((which / 2) == 2)))
        depth = (parent->getDepth() / 2) + 1;
    else
        depth = parent->getDepth() / 2;
} // end setDepth()

/*
 * setDimensions
 */
// end setDimensions()

/*
 * isDisplayed
 *
 * return - bool
 */
bool OcNode::isDisplayed(void) {
    return displayed;
} // end isDisplayed()

/*
 * setDisplayed
 *
 * parameter _displayed - bool
 */
void OcNode::setDisplayed(bool _displayed) {
    displayed = _displayed;
} // end setDisplayed()

/*
 * getHeight
 *
 * return - int
 */
void OcNode::setDimensions(void) {
    setWidth();
    setHeight();
    setDepth();
}

/*
 * isEmpty
 *
 * return - unsigned char
 */
bool OcNode::isEmpty(void) {
    return empty;
} // end isEmpty()

/*
 * getEmptyChildren
 *
 * return - unsigned char
 */
unsigned char OcNode::getEmptyChildren(void) {
    return emptyChildren;
} // end getEmptyChildren()

/*
 * getHeight
 *
 * return - int
 */
int OcNode::getHeight(void) {
    return height;
} // end getHeight()

/*
 * setHeight
 */
void OcNode::setHeight(void) {
    if ((parent->getHeight() % 2) != 0 && which < 4)
        height = (parent->getHeight() / 2) + 1;
    else
        height = parent->getHeight() / 2;
} // end setHeight()

/*
 * isLeaf
 *
 * return - bool
 */
bool OcNode::isLeaf(void) {
    return leaf;
} // end isLeaf()

/*
 * setLeaf
 *
 * parameter _leaf - bool
 */
void OcNode::setLeaf(bool _leaf) {
    leaf = _leaf;
} // end setLeaf()

/*
 * getLevel
 *
 * return - int
 */
int OcNode::getLevel(void) {
    return level;
} // end getLevel()

/*
 * setLocations
 */
void OcNode::setLocations(void) {
    setColumn();
    setRow();
    setSlab();
} // end setLocations()

/*
 * getMaximum
 *
 * parameter _boundingBox - BoundingBox*
 * parameter _maximumIndex - int
 * return - float
 */
float OcNode::getMaximum(BoundingBox* _boundingBox, int _maximumIndex) {
    float* p = _boundingBox->getCorner(_maximumIndex);
    float z = (float) p[2];
    delete[] p;
    return z;
} // end getMaximum()

/*
 * getMaximum
 *
 * return - int
 */
int OcNode::getMaximum(void) {
    return maximum;
} // end getMaximum()

/*
 * setMaximum
 *
 * parameter _maximum - int
 */
void OcNode::setMaximum(int _maximum) {
    maximum = _maximum;
} // end setMaximum()

/*
 * getMinimum
 *
 * parameter _boundingBox - BoundingBox*
 * parameter _minimumIndex - int
 * return - float
 */
float OcNode::getMinimum(BoundingBox* _boundingBox, int _minimumIndex) {
    float* p = _boundingBox->getCorner(_minimumIndex);
    float z = (float) p[2];
    delete[] p;
    return z;
} // end getMinimum()

/*
 * getMinimum
 *
 * return - int
 */
int OcNode::getMinimum(void) {
    return minimum;
} // end getMinimum()

/*
 * setMinimum
 *
 * parameter _minimum - int
 */
void OcNode::setMinimum(int _minimum) {
    minimum = _minimum;
} // end setMinimum()

/*
 * getParent
 *
 * return - OcNode*
 */
OcNode* OcNode::getParent(void) {
    return parent;
} // end getParent()

/*
 * setParent
 *
 * parameter _parent - OcNode*
 */
void OcNode::setParent(OcNode* _parent) {
    parent = _parent;
} // end setParent()

/*
 * getPercentageOfEmptyVolume
 *
 * return - float
 */
float OcNode::getPercentageOfEmptyVolume(void) {
    return percentageOfEmptyVolume;
} // end getPercentageOfEmptyVolume()

/*
 * getRatioOfVisibility
 *
 * return - float
 */
float OcNode::getRatioOfVisibility(void) {
    return ratioOfVisibility;
} // end getRatioOfVisibility()

/*
 * getRow
 *
 * return - int
 */
int OcNode::getRow(void) {
    return row;
} // end getRow()

/*
 * setRow
 */
void OcNode::setRow(void) {
    row = which / (2 * 2);
    // row *= ((volume->getHeight()) / int(pow(double(2), double(level))));
    // Do we need to handle remainders
    if ((parent->getHeight() % 2) != 0)
        row *= (parent->getHeight() / 2) + 1;
    else
        row *= parent->getHeight() / 2;
    // row *= ((brick->getHeight()) / int(pow(double(2), double(level))));
    row += parent->getRow();
} // end setRow()

/*
 * getSlab
 *
 * return - int
 */
int OcNode::getSlab(void) {
    return slab;
} // end getSlab()

/*
 * setSlab
 */
void OcNode::setSlab(void) {
    if (which < 4) {
        slab = which / 2;
    } // end if
    else {
        slab = (which / 2) - 2;
    } // end else
    // slab *= ((volume->getDepth()) / int(pow(double(2), double(level))));
    // Do we need to handle remainders
    if ((parent->getDepth() % 2) != 0)
        slab *= (parent->getDepth() / 2) + 1;
    else
        slab *= parent->getDepth() / 2;
    // slab *= ((brick->getDepth()) / int(pow(double(2), double(level))));
    slab += parent->getSlab();
} // end setSlab()

/*
 * getTextureBox
 *
 * return - BoundingBox*
 */
BoundingBox* OcNode::getTextureBox(void) {
    return textureBox;
} // end getTextureBox()

/*
 * setTextureBox
 */
void OcNode::setTextureBox(void) {
    float* p0;
    float* p1;
    switch (which) {
        case 0:
            p0 = parent->textureBox->getCorner(0);
            p1 = parent->textureBox->getWeightedCenter(xWeight, yWeight, zWeight);
            break;
        case 1:
            p0 = parent->textureBox->getWeightedMidPoint(0, 1, xWeight, 0.5, 0.5);
            p1 = parent->textureBox->getWeightedFaceMidPoint(RightFace, xWeight, yWeight, zWeight);
            break;
        case 2:
            p0 = parent->textureBox->getWeightedMidPoint(0, 2, 0.5, 0.5, zWeight);
            p1 = parent->textureBox->getWeightedFaceMidPoint(BackFace, xWeight, yWeight, zWeight);
            break;
        case 3:
            p0 = parent->textureBox->getWeightedFaceMidPoint(BottomFace, xWeight, yWeight, zWeight);
            p1 = parent->textureBox->getWeightedMidPoint(3, 7, 0.5, yWeight, 0.5);
            break;
        case 4:
            p0 = parent->textureBox->getWeightedMidPoint(0, 4, 0.5, yWeight, 0.5);
            p1 = parent->textureBox->getWeightedFaceMidPoint(TopFace, xWeight, yWeight, zWeight);
            break;
        case 5:
            p0 = parent->textureBox->getWeightedFaceMidPoint(FrontFace, xWeight, yWeight, zWeight);
            p1 = parent->textureBox->getWeightedMidPoint(5, 7, 0.5, 0.5, zWeight);
            break;
        case 6:
            p0 = parent->textureBox->getWeightedFaceMidPoint(LeftFace, xWeight, yWeight, zWeight);
            p1 = parent->textureBox->getWeightedMidPoint(6, 7, xWeight, 0.5, 0.5);
            break;
        case 7:
            p0 = parent->textureBox->getWeightedCenter(xWeight, yWeight, zWeight);
            p1 = parent->textureBox->getCorner(7);
            break;
    } // end switch
    textureBox = new BoundingBox(p0[0], p0[1], p0[2], p1[0], p1[1], p1[2]);
    delete[] p0;
    delete[] p1;
} // end setTextureBox()

/*
 * setVolume
 *
 * parameter _volume - Volume*
 */
void OcNode::setVolume(Volume* _volume) {
    volume = _volume;
} // end setVolume()

/*
 * getVolume
 *
 * return - Volume*
 */
Volume* OcNode::getVolume(void) {
    return volume;
} // end getVolume()

/*
 * getVolumeBox
 *
 * return - BoundingBox*
 */
BoundingBox* OcNode::getVolumeBox(void) {
    return volumeBox;
} // end getVolumeBox()

/*
 * setVolumeBox
 */
void OcNode::setVolumeBox(void) {
    float* p0;
    float* p1;
    switch (which) {
        case 0:
            p0 = parent->volumeBox->getCorner(0);
            p1 = parent->volumeBox->getWeightedCenter(xWeight, yWeight, zWeight);
            break;
        case 1:
            p0 = parent->volumeBox->getWeightedMidPoint(0, 1, xWeight, 0.5, 0.5);
            p1 = parent->volumeBox->getWeightedFaceMidPoint(RightFace, xWeight, yWeight, zWeight);
            break;
        case 2:
            p0 = parent->volumeBox->getWeightedMidPoint(0, 2, 0.5, 0.5, zWeight);
            p1 = parent->volumeBox->getWeightedFaceMidPoint(BackFace, xWeight, yWeight, zWeight);
            break;
        case 3:
            p0 = parent->volumeBox->getWeightedFaceMidPoint(BottomFace, xWeight, yWeight, zWeight);
            p1 = parent->volumeBox->getWeightedMidPoint(3, 7, 0.5, yWeight, 0.5);
            break;
        case 4:
            p0 = parent->volumeBox->getWeightedMidPoint(0, 4, 0.5, yWeight, 0.5);
            p1 = parent->volumeBox->getWeightedFaceMidPoint(TopFace, xWeight, yWeight, zWeight);
            break;
        case 5:
            p0 = parent->volumeBox->getWeightedFaceMidPoint(FrontFace, xWeight, yWeight, zWeight);
            p1 = parent->volumeBox->getWeightedMidPoint(5, 7, 0.5, 0.5, zWeight);
            break;
        case 6:
            p0 = parent->volumeBox->getWeightedFaceMidPoint(LeftFace, xWeight, yWeight, zWeight);
            p1 = parent->volumeBox->getWeightedMidPoint(6, 7, xWeight, 0.5, 0.5);
            break;
        case 7:
            p0 = parent->volumeBox->getWeightedCenter(xWeight, yWeight, zWeight);
            p1 = parent->volumeBox->getCorner(7);
            break;
    } // end switch
    volumeBox = new BoundingBox(p0[0], p0[1], p0[2], p1[0], p1[1], p1[2]);
    delete[] p0;
    delete[] p1;
} // end setVolumeBox()

/*
 * getVolumeSphere
 *
 * return - BoundingSphere*
 */
BoundingSphere* OcNode::getVolumeSphere(void) {
    return volumeSphere;
} // end getVolumeSphere()

/*
 * setVolumeSphere
 */
void OcNode::setVolumeSphere(void) {
    float _radius = volumeBox->getRadius();
    volumeSphere = new BoundingSphere(x, y, z, _radius);
} // end setVolumeSphere()

/*
 * setWeight
 */
void OcNode::setWeight(void) {
    if ((parent->getDepth() % 2) != 0)
        zWeight = double((parent->getDepth() / 2) + 1) / double(parent->getDepth());
    else
        zWeight = 0.5;
    if ((parent->getHeight() % 2) != 0)
        yWeight = double((parent->getHeight() / 2) + 1) / double(parent->getHeight());
    else
        yWeight = 0.5;
    if ((parent->getWidth() % 2) != 0)
        xWeight = double((parent->getWidth() / 2) + 1) / double(parent->getWidth());
    else
        xWeight = 0.5;
} // end setWeight()

/*
 * getWhich
 *
 * return - int
 */
int OcNode::getWhich(void) {
    return which;
} // end getWhich()

/*
 * getWidth
 *
 * return - int
 */
int OcNode::getWidth(void) {
    return width;
} // end getWidth()

/*
 * setWidth
 */
void OcNode::setWidth(void) {
    if ((parent->getWidth() % 2) != 0 && (which % 2) == 0) {
        width = (parent->getWidth() / 2) + 1;
    } else
        width = parent->getWidth() / 2;
} // end setWidth()

/*
 * getX
 *
 * return - float
 */
float OcNode::getX(void) {
    return x;
} // end getX()

/*
 * setX
 *
 * parameter _x - float
 */
void OcNode::setX(float _x) {
    x = _x;
} // end setX()

/*
 * getY
 *
 * return - float
 */
float OcNode::getY(void) {
    return y;
} // end getY()

/*
 * setY
 *
 * parameter _y - float
 */
void OcNode::setY(float _y) {
    y = _y;
} // end setY()

/*
 * getZ
 *
 * return - float
 */
float OcNode::getZ(void) {
    return z;
} // end getZ()

/*
 * setZ
 *
 * parameter _z - float
 */
void OcNode::setZ(float _z) {
    z = _z;
} // end setZ()

/*
 * maximumPriorityQueueTestTraversal
 *
 * parameter _maximumPriorityQueueTest - bool
 */
void OcNode::maximumPriorityQueueTestTraversal(bool _maximumPriorityQueueTest) {
    maximumPriorityQueueTest = _maximumPriorityQueueTest;
    if (!isLeaf()) {
        for (int i = 0; i < 8; i++) {
            child[i].maximumPriorityQueueTestTraversal(_maximumPriorityQueueTest);
        } // end for
    }
} // end maximumPriorityQueueTestTraversal()

/*
 * percentageOfEmptyVolumeTraversal
 *
 * parameter alpha - float*
 */
void OcNode::percentageOfEmptyVolumeTraversal(float* alpha) {
    if (!isLeaf()) {
        for (int i = 0; i < 8; i++) {
            child[i].percentageOfEmptyVolumeTraversal(alpha);
        } // end for
    } else { // a Leaf
        calculatePercentageOfEmptyVolume(alpha);
    } // end if
    if (!isLeaf()) {
        percentageOfEmptyVolume = 0.0f;
        ratioOfVisibility = 0.0f;
        for (int i = 0; i < 8; i++) {
            percentageOfEmptyVolume += child[i].getPercentageOfEmptyVolume();
            ratioOfVisibility += child[i].getRatioOfVisibility();
        } // end for
    }
} // end percentageOfEmptyVolumeTraversal()

/*
 * percentageOfEmptyVolumeTraversal2_5D
 *
 * parameter alpha - float*
 */
void OcNode::percentageOfEmptyVolumeTraversal2_5D(float* alpha) {
    if (!isLeaf()) {
        for (int i = 0; i < 8; i++) {
            child[i].percentageOfEmptyVolumeTraversal2_5D(alpha);
        } // end for
    } else { // a Leaf
        calculatePercentageOfEmptyVolume2_5D(alpha);
    } // end if
    if (!isLeaf()) {
        percentageOfEmptyVolume = 0.0f;
        ratioOfVisibility = 0.0f;
        for (int i = 0; i < 8; i++) {
            percentageOfEmptyVolume += child[i].getPercentageOfEmptyVolume();
            ratioOfVisibility += child[i].getRatioOfVisibility();
        } // end for
    }
} // end percentageOfEmptyVolumeTraversal2_5D()

/*
 * percentageOfEmptyVolumeTraversal
 *
 * parameter red - float*
 * parameter green - float*
 * parameter blue - float*
 */
void OcNode::percentageOfEmptyVolumeTraversal(float* red, float* green, float* blue) {
    if (!isLeaf()) {
        for (int i = 0; i < 8; i++) {
            child[i].percentageOfEmptyVolumeTraversal(red, green, blue);
        } // end for
    } else { // a Leaf
        calculatePercentageOfEmptyVolume(red, green, blue);
    } // end if
    if (!isLeaf()) {
        percentageOfEmptyVolume = 0.0f;
        ratioOfVisibility = 0.0f;
        for (int i = 0; i < 8; i++) {
            percentageOfEmptyVolume += child[i].getPercentageOfEmptyVolume();
            ratioOfVisibility += child[i].getRatioOfVisibility();
        } // end for
    }
} // end percentageOfEmptyVolumeTraversal()

/*
 * ratioOfVisibilityTraversal
 *
 * parameter alpha - float*
 */
void OcNode::ratioOfVisibilityTraversal(float* alpha) {
    if (!isLeaf()) {
        for (int i = 0; i < 8; i++) {
            child[i].ratioOfVisibilityTraversal(alpha);
        } // end for
    } else { // a Leaf
        calculateRatioOfVisibility(alpha);
    } // end if
    if (!isLeaf()) {
        ratioOfVisibility = 0.0f;
        for (int i = 0; i < 8; i++) {
            ratioOfVisibility += child[i].getRatioOfVisibility();
        } // end for
        ratioOfVisibility /= 8;
    }
} // end ratioOfVisibilityTraversal()

/*
 * ratioOfVisibilityTraversal2_5D
 *
 * parameter alpha - float*
 */
void OcNode::ratioOfVisibilityTraversal2_5D(float* alpha) {
    if (!isLeaf()) {
        for (int i = 0; i < 8; i++) {
            child[i].ratioOfVisibilityTraversal2_5D(alpha);
        } // end for
    } else { // a Leaf
        calculateRatioOfVisibility2_5D(alpha);
    } // end if
    if (!isLeaf()) {
        ratioOfVisibility = 0.0f;
        for (int i = 0; i < 8; i++) {
            ratioOfVisibility += child[i].getRatioOfVisibility();
        } // end for
        ratioOfVisibility /= 8;
    }
} // end ratioOfVisibilityTraversal2_5D()

/*
 * ratioOfVisibilityTraversal
 *
 * parameter red - float*
 * parameter green - float*
 * parameter blue - float*
 */
void OcNode::ratioOfVisibilityTraversal(float* red, float* green, float* blue) {
    if (!isLeaf()) {
        for (int i = 0; i < 8; i++) {
            child[i].ratioOfVisibilityTraversal(red, green, blue);
        } // end for
    } else { // a Leaf
        calculateRatioOfVisibility(red, green, blue);
    } // end if
    if (!isLeaf()) {
        ratioOfVisibility = 0.0f;
        for (int i = 0; i < 8; i++) {
            ratioOfVisibility += child[i].getRatioOfVisibility();
        } // end for
        ratioOfVisibility /= 8;
    }
} // end ratioOfVisibilityTraversal()

/*
 * ratioOfVisibilityTestTraversal
 *
 * parameter _ratioOfVisibilityTest - bool
 */
void OcNode::ratioOfVisibilityTestTraversal(bool _ratioOfVisibilityTest) {
    ratioOfVisibilityTest = _ratioOfVisibilityTest;
    if (!isLeaf()) {
        for (int i = 0; i < 8; i++) {
            child[i].ratioOfVisibilityTestTraversal(_ratioOfVisibilityTest);
        } // end for
    }
} // end ratioOfVisibilityTestTraversal()

/*
 * ratioOfVisibilityThresholdTraversal
 *
 * parameter _ratioOfVisibilityThreshold - float
 */
void OcNode::ratioOfVisibilityThresholdTraversal(float _ratioOfVisibilityThreshold) {
    ratioOfVisibilityThreshold = _ratioOfVisibilityThreshold;
    if (!isLeaf()) {
        for (int i = 0; i < 8; i++) {
            child[i].ratioOfVisibilityThresholdTraversal(_ratioOfVisibilityThreshold);
        } // end for
    }
} // end ratioOfVisibilityThresholdTraversal()

/*
 * renderingCostTraversal
 *
 * parameter _renderingCost - float
 */
void OcNode::renderingCostTraversal(float _renderingCost) {
    //renderingCost = _renderingCost;
    if (!isLeaf()) {
        for (int i = 0; i < 8; i++) {
            child[i].renderingCostTraversal(_renderingCost);
        } // end for
    }
} // end renderingCostTraversal()

/*
 * renderingCostFunctionTestTraversal
 *
 * parameter _renderingCostFunctionTest - bool
 */
void OcNode::renderingCostFunctionTestTraversal(bool _renderingCostFunctionTest) {
    renderingCostFunctionTest = _renderingCostFunctionTest;
    if (!isLeaf()) {
        for (int i = 0; i < 8; i++) {
            child[i].renderingCostFunctionTestTraversal(_renderingCostFunctionTest);
        } // end for
    }
} // end renderingCostFunctionTestTraversal()

/*
 * rendereringCostFuctionTraversal
 *
 * parameter alpha - float*
 */
void OcNode::rendereringCostFuctionTraversal(float* alpha) {
    if (!isLeaf()) {
        for (int i = 0; i < 8; i++) {
            child[i].rendereringCostFuctionTraversal(alpha);
        } // end for
    } else { // a Leaf
        calculateEmptiness(alpha);
        if (isEmpty())
            costRender = 0.0f;
        else
            costRender = 1.0f;
        costToRenderChildren = 8.0f * costStructure + renderingCost * 8.0f;
        cost = std::min(level * costStructure + costRender, costToRenderChildren);
    } // end if
    if (!isLeaf()) {
        ratioOfVisibility = 0.0f;
        emptyChildren = 0;
        costToRenderChildren = 0.0f;
        for (int i = 0; i < 8; i++) {
            ratioOfVisibility += child[i].getRatioOfVisibility();
            costToRenderChildren += child[i].getLevel() * child[i].getCostStructure() + renderingCost * child[i].getCostRender();
            if (child[i].isEmpty()) {
                emptyChildren = emptyChildren + (1 << i);
            }
        } // end for
        if (emptyChildren == 255) {
            empty = true;
            costRender = 0.0f;
        } else {
            empty = false;
            costRender = 1.0f;
        }
        cost = std::min(level * costStructure + costRender, costToRenderChildren);
    }
} // end rendereringCostFuctionTraversal()

/*
 * rendereringCostFuctionTraversal
 *
 * parameter red - float*
 * parameter green - float*
 * parameter blue - float*
 */
void OcNode::rendereringCostFuctionTraversal(float* red, float* green, float* blue) {
    if (!isLeaf()) {
        for (int i = 0; i < 8; i++) {
            child[i].rendereringCostFuctionTraversal(red, green, blue);
        } // end for
    } else { // a Leaf
        calculateEmptiness(red, green, blue);
        if (isEmpty())
            costRender = 0.0f;
        else
            costRender = 1.0f;
        costToRenderChildren = 8.0f * costStructure + renderingCost * 8.0f;
        cost = std::min(level * costStructure + costRender, costToRenderChildren);
    } // end if
    if (!isLeaf()) {
        ratioOfVisibility = 0.0f;
        emptyChildren = 0;
        costToRenderChildren = 0.0f;
        for (int i = 0; i < 8; i++) {
            ratioOfVisibility += child[i].getRatioOfVisibility();
            costToRenderChildren += child[i].getLevel() * child[i].getCostStructure() + renderingCost * child[i].getCostRender();
            if (child[i].isEmpty()) {
                emptyChildren = emptyChildren + (1 << i);
            }
        } // end for
        if (emptyChildren == 255) {
            empty = true;
            costRender = 0.0f;
        } else {
            empty = false;
            costRender = 1.0f;
        }
        cost = std::min(level * costStructure + costRender, costToRenderChildren);
    }
} // end rendereringCostFuctionTraversal()

/*
 * rendereringCostFuctionTraversal2_5D
 *
 * parameter alpha - float *
 */
void OcNode::rendereringCostFuctionTraversal2_5D(float* alpha) {
    if (!isLeaf()) {
        for (int i = 0; i < 8; i++) {
            child[i].rendereringCostFuctionTraversal2_5D(alpha);
        } // end for
    } else { // a Leaf
        calculateEmptiness2_5D(alpha);
        if (isEmpty())
            costRender = 0.0f;
        else
            costRender = 1.0f;
        costToRenderChildren = 8.0f * costStructure + renderingCost * 8.0f;
        cost = std::min(level * costStructure + costRender, costToRenderChildren);
    } // end if
    if (!isLeaf()) {
        ratioOfVisibility = 0.0f;
        emptyChildren = 0;
        costToRenderChildren = 0.0f;
        for (int i = 0; i < 8; i++) {
            ratioOfVisibility += child[i].getRatioOfVisibility();
            costToRenderChildren += child[i].getLevel() * child[i].getCostStructure() + renderingCost * child[i].getCostRender();
            if (child[i].isEmpty()) {
                emptyChildren = emptyChildren + (1 << i);
            }
        } // end for
        if (emptyChildren == 255) {
            empty = true;
            costRender = 0.0f;
        } else {
            empty = false;
            costRender = 1.0f;
        }
        cost = std::min(level * costStructure + costRender, costToRenderChildren);
    }
} // end rendereringCostFuctionTraversal2_5D()

/*
 * sliceFactorTraversal
 *
 * parameter _sliceFactor - float
 */
void OcNode::sliceFactorTraversal(float _sliceFactor) {
    sliceFactor = _sliceFactor;
    if (parent == NULL)
        calculateDelta(getMaximum(volumeBox, volumeBox->getMaximumIndex()), getMinimum(volumeBox, volumeBox->getMinimumIndex()));
    else
        delta = parent->getDelta();
    if (!isLeaf()) {
        for (int i = 0; i < 8; i++) {
            child[i].sliceFactorTraversal(_sliceFactor);
        } // end for
    } // end if
} // end sliceFactorTraversal()

/*
 * sortChildren
 *
 * parameter point - float *
 * parameter ocNodeList - std::vector<ComparableOcNode> &
 */
void OcNode::sortChildren(float * point, std::vector<ComparableOcNode> & ocNodeList) {
    // Calculate minimum distance from head position to ocNodes feature
    ComparableOcNode comparableOcNode;
    comparableOcNode.maximum(); // set static variable greater to perform a maximum sort
    int i;
    for (i = 0; i < 8; i++) {
        ocNodeList[i] = ComparableOcNode(i, child[i].getVolumeBox()->getMinimumDistance(point));
    }
    std::sort(ocNodeList.begin(), ocNodeList.end());
    // Handle ties in smallest keys (drawn last)
    i = 2;
    if (8 > 1) {
        while (i <= 8 & ocNodeList[8 - i].getValue() == ocNodeList[7].getValue()) {
            if (child[ocNodeList[8 - i].getWhich()].getVolumeBox()->collision(point)) { // if head position inside bounding box
                float distance = ocNodeList[7].getValue();
                int which = ocNodeList[7].getWhich();
                ocNodeList[7].setValue(ocNodeList[8 - i].getValue());
                ocNodeList[7].setWhich(ocNodeList[8 - i].getWhich());
                ocNodeList[8 - i].setValue(distance);
                ocNodeList[8 - i].setWhich(which);
            }
            i++;
        }
    }
} // end sortChildren()

/*
 * toString
 *
 * return - std::string
 */
std::string OcNode::toString(void) {
    std::string str = " ";
    for (int i = 0; i < level; i++) {
        str = str + "   ";
    } // end for
    return str + Stringify::toString(column) + " " + Stringify::toString(row) + " " + Stringify::toString(slab);
    /*
     return str + Stringify::toString(level) + " " + Stringify::toString(which) + "\n " + Stringify::toString(width) + " "
     + Stringify::toString(height) + " " + Stringify::toString(depth) + "\n " + Stringify::toString(column) + " "
     + Stringify::toString(row) + " " + Stringify::toString(slab) + "\n " + Stringify::toString(minimum) + " "
     + Stringify::toString(maximum) + "\n " + Stringify::toString(x) + " " + Stringify::toString(y) + " "
     + Stringify::toString(z);
     */
} // end toString()

/*
 * viewAlignedVertex - Find the vertex of the intersection polygon.
 *
 * parameter which - int
 * parameter frontIndex - int
 * parameter sp - const Vector4 &
 * parameter spn - const Vector4 &
 * parameter tverts - Vector4*
 * parameter verts - Vector4*
 */
void OcNode::viewAlignedVertex(int which, int frontIndex, const Vector4 & sp, const Vector4 & spn, Vector4* tverts, Vector4* verts) {
    for (int i = 0; i < 4; i++) {
        float* p0 = volumeBox->getCorner(sequence[frontIndex][edgeIndex[(which * 4) + i][0]]);
        float* p1 = volumeBox->getCorner(sequence[frontIndex][edgeIndex[(which * 4) + i][1]]);
        Vector4* spp0 = Vector4::subtract(sp, p0);
        Vector4* p1p0 = Vector4::subtract(p1, p0);
        float t = spp0->dot(spn) / p1p0->dot(spn);
        delete spp0;
        delete p1p0;
        if ((t >= 0) && (t <= 1)) {
            float* t0 = textureBox->getCorner(sequence[frontIndex][edgeIndex[(which * 4) + i][0]]);
            float* t1 = textureBox->getCorner(sequence[frontIndex][edgeIndex[(which * 4) + i][1]]);
            // Compute the line intersection
            float x = (float) (p0[0] + (t * (p1[0] - p0[0])));
            float y = (float) (p0[1] + (t * (p1[1] - p0[1])));
            float z = (float) (p0[2] + (t * (p1[2] - p0[2])));
            verts[which].setXYZW(x, y, z, 0.0f);
            // Compute the texture intersection
            x = (float) (t0[0] + (t * (t1[0] - t0[0])));
            y = (float) (t0[1] + (t * (t1[1] - t0[1])));
            z = (float) (t0[2] + (t * (t1[2] - t0[2])));
            tverts[which].setXYZW(x, y, z, 0.0f);
            delete[] t0;
            delete[] t1;
            break;
        } // end if
        delete[] p0;
        delete[] p1;
    } // end for
    /*
     * NOTE: Nothing to delete. p0, p1, t0 and t1 are pure pointers.
     */
} // end viewAlignedVertex()

/*
 * viewAlignedVertex - Find the vertex of the intersection polygon.
 *
 * parameter which - int
 * parameter frontIndex - int
 * parameter spF - const Vector4 &
 * parameter spB - const Vector4 &
 * parameter spn - const Vector4 &
 * parameter sFront - Vector4*
 * parameter sBack - Vector4*
 * parameter verts - Vector4*
 */
void OcNode::viewAlignedVertex(int which, int frontIndex, const Vector4 & spF, const Vector4 & spB, const Vector4 & spn,
        Vector4* sFront, Vector4* sBack, Vector4* verts) {
    for (int i = 0; i < 4; i++) {
        float* p0 = volumeBox->getCorner(sequence[frontIndex][edgeIndex[(which * 4) + i][0]]);
        float* p1 = volumeBox->getCorner(sequence[frontIndex][edgeIndex[(which * 4) + i][1]]);
        Vector4* spFp0 = Vector4::subtract(spF, p0);
        Vector4* spBp0 = Vector4::subtract(spB, p0);
        Vector4* p1p0 = Vector4::subtract(p1, p0);
        float tF = spFp0->dot(spn) / p1p0->dot(spn);
        float tB = spBp0->dot(spn) / p1p0->dot(spn);
        delete spFp0;
        delete spBp0;
        delete p1p0;
        if ((tF >= 0) && (tF <= 1)) {
            float* t0 = textureBox->getCorner(sequence[frontIndex][edgeIndex[(which * 4) + i][0]]);
            float* t1 = textureBox->getCorner(sequence[frontIndex][edgeIndex[(which * 4) + i][1]]);
            // Compute the line intersection
            float x = (float) (p0[0] + (tF * (p1[0] - p0[0])));
            float y = (float) (p0[1] + (tF * (p1[1] - p0[1])));
            float z = (float) (p0[2] + (tF * (p1[2] - p0[2])));
            verts[which].setXYZW(x, y, z, 0.0f);
            // Compute the texture intersection
            x = (float) (t0[0] + (tF * (t1[0] - t0[0])));
            y = (float) (t0[1] + (tF * (t1[1] - t0[1])));
            z = (float) (t0[2] + (tF * (t1[2] - t0[2])));
            sFront[which].setXYZW(x, y, z, 0.0f);
            // Compute the texture intersection
            x = (float) (t0[0] + (tB * (t1[0] - t0[0])));
            y = (float) (t0[1] + (tB * (t1[1] - t0[1])));
            z = (float) (t0[2] + (tB * (t1[2] - t0[2])));
            sBack[which].setXYZW(x, y, z, 0.0f);
            delete t0;
            delete t1;
            break;
        } // end if
        delete p0;
        delete p1;
    } // end for
    /*
     * NOTE: Nothing to delete. p0, p1, t0 and t1 are pure pointers.
     */
} // end viewAlignedVertex()
