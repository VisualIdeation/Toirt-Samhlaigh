/*
 * OcTree.cpp - Methods for OcTree class.
 *
 * Author: Patrick O'Leary
 * Created: November 11, 2007
 * Copyright 2007. All rights reserved.
 */

#include <GL/gl.h>
#include <GL/glu.h>

/* Vrui includes */
#include <Geometry/HVector.h>
#include <Geometry/ProjectiveTransformation.h>
#include <GL/GLGeometryWrappers.h>
#include <GL/GLTransformationWrappers.h>
#include <Geometry/Vector.h>

#include <DATASTRUCTURE/BoundingBox.h>
#include <DATASTRUCTURE/Brick.h>
#include <DATASTRUCTURE/ComparableOcNode.h>
#include <DATASTRUCTURE/OcNode.h>
#include <DATASTRUCTURE/OcTree.h>
#include <MATH/Matrix4x4.h>
#include <SHADER/ShaderObject.h>

/*
 * OcTree - Constructor for OcTree.
 *
 * parameter _brick - Brick*
 */
OcTree::OcTree(Brick* _brick) {
	root = new OcNode(_brick);
	showOcNodes = false;
} // end OcTree()

/*
 * ~OcTree - Destructor for OcTree.
 */
OcTree::~OcTree(void) {
	delete root;
} // end ~OcTree()

/*
 * drawGrid
 *
 * parameter cFrustum - const CFrustum &
 */
void OcTree::drawGrid(const CFrustum & cFrustum) {
	glDisable(GL_LIGHTING);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	bool testFrustum = true;
	if (showOcNodes) {
		root->drawGridTraversal(cFrustum, testFrustum);
	}
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_LIGHTING);
}

/*
 * drawVolume
 *
 * parameter cFrustum - const CFrustum &
 * parameter shaderObject - ShaderObject*
 * parameter point - float *
 * parameter minimumIndex - int
 * parameter maximumIndex - int
 * parameter modelviewInverse - const Matrix4x4 *
 */
void OcTree::drawVolume(const CFrustum & cFrustum, ShaderObject* shaderObject, float * point, int minimumIndex, int maximumIndex, const Matrix4x4 * modelviewInverse) {
	bool testFrustum = true;
	root->drawVolumeTraversal(cFrustum, point, shaderObject, minimumIndex, maximumIndex, modelviewInverse, true);
} // end drawVolume()

/*
 * setMaximumPriorityQueueTest
 *
 * parameter _maximumPriorityQueueTest - bool
 */
void OcTree::setMaximumPriorityQueueTest(bool _maximumPriorityQueueTest) {
	root->maximumPriorityQueueTestTraversal(_maximumPriorityQueueTest);
} // end setMaximumPriorityQueueTest()

/*
 * setPercentageOfEmptyVolume
 *
 * parameter alpha - float*
 */
void OcTree::setPercentageOfEmptyVolume(float* alpha) {
	root->percentageOfEmptyVolumeTraversal(alpha);
} // end setPercentageOfEmptyVolume()

/*
 * setPercentageOfEmptyVolume
 *
 * parameter red - float*
 * parameter green - float*
 * parameter blue - float*
 */
void OcTree::setPercentageOfEmptyVolume(float* red, float* green, float* blue) {
	root->percentageOfEmptyVolumeTraversal(red, green, blue);
} // end setPercentageOfEmptyVolume()

/*
 * setPercentageOfEmptyVolume2_5D
 *
 * parameter alpha - float*
 */
void OcTree::setPercentageOfEmptyVolume2_5D(float* alpha) {
    root->percentageOfEmptyVolumeTraversal2_5D(alpha);
} // end setPercentageOfEmptyVolume2_5D()

/*
 * setRatioOfVisibility
 *
 * parameter alpha - float*
 */
void OcTree::setRatioOfVisibility(float* alpha) {
	root->ratioOfVisibilityTraversal(alpha);
} // end setRatioOfVisibility()

/*
 * setRatioOfVisibility
 *
 * parameter red - float*
 * parameter green - float*
 * parameter blue - float*
 */
void OcTree::setRatioOfVisibility(float* red, float* green, float* blue) {
	root->ratioOfVisibilityTraversal(red, green, blue);
} // end setRatioOfVisibility()

/*
 * setRatioOfVisibility2_5D
 *
 * parameter alpha - float*
 */
void OcTree::setRatioOfVisibility2_5D(float* alpha) {
    root->ratioOfVisibilityTraversal2_5D(alpha);
} // end setRatioOfVisibility2_5D()

/*
 * setRatioOfVisibilityTest
 *
 * parameter _ratioOfVisibilityTest - bool
 */
void OcTree::setRatioOfVisibilityTest(bool _ratioOfVisibilityTest) {
	root->ratioOfVisibilityTestTraversal(_ratioOfVisibilityTest);
} // end setRatioOfVisibilityTest()

/*
 * setRatioOfVisibilityThreshold
 *
 * parameter _ratioOfVisibilityThreshold - float
 */
void OcTree::setRatioOfVisibilityThreshold(float _ratioOfVisibilityThreshold) {
	root->ratioOfVisibilityThresholdTraversal(_ratioOfVisibilityThreshold);
} // end setRatioOfVisibilityThreshold()

/*
 * setRenderingCost
 *
 * parameter _renderingCost - float
 */
void OcTree::setRenderingCost(float _renderingCost) {
	root->renderingCostTraversal(_renderingCost);
} // end setRenderingCost()

/*
 * setRenderingCostFunction
 *
 * parameter alpha - float*
 */
void OcTree::setRenderingCostFunction(float* alpha) {
	root->rendereringCostFuctionTraversal(alpha);
} // end setRenderingCostFunction()

/*
 * setRenderingCostFunction
 *
 * parameter red - float *
 * parameter green - float *
 * parameter blue - float *
 */
void OcTree::setRenderingCostFunction(float * red, float * green, float * blue) {
    root->rendereringCostFuctionTraversal(red, green, blue);
} // end setRenderingCostFunction()

/*
 * setRenderingCostFunction2_5D
 *
 * parameter alpha - float*
 */
void OcTree::setRenderingCostFunction2_5D(float* alpha) {
    root->rendereringCostFuctionTraversal2_5D(alpha);
} // end setRenderingCostFunction2_5D()

/*
 * setRenderingCostFunctionTest
 *
 * parameter _renderingCostFunctionTest - bool
 */
void OcTree::setRenderingCostFunctionTest(bool _renderingCostFunctionTest) {
	root->renderingCostFunctionTestTraversal(_renderingCostFunctionTest);
} // end setRenderingCostFunctionTest()

/*
 * getRootclass Volume;
 *
 * return - OcNode*
 */
OcNode* OcTree::getRoot(void) {
	return root;
} // end getRoot()

/*
 * getShowOcNodes
 *
 * return - bool
 */
bool OcTree::getShowOcNodes(void) {
	return showOcNodes;
} // end getShowOcNodes()

/*
 * setShowOcNodes
 *
 * parameter _showOcNodes - bool
 */
void OcTree::setShowOcNodes(bool _showOcNodes) {
	showOcNodes = _showOcNodes;
} // end setShowOcNodes()

/*
 * setSliceFactor
 *
 * parameter _sliceFactor - float
 */
void OcTree::setSliceFactor(float _sliceFactor) {
	root->sliceFactorTraversal(_sliceFactor);
} // end setSliceFactor()

/* toString
 *
 * return - std::string
 */
std::string OcTree::toString(void) {
	if (root!=NULL) {
		return root->toString();
	} else
		return " ";
} // end toString()

