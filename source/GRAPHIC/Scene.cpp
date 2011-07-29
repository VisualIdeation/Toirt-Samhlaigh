/*
 * Scene.cpp - Methods for Scene class.
 *
 * Author: Patrick O'Leary
 * Created: November 11, 2007
 * Copyright 2007. All rights reserved.
 */

#include <algorithm>
#include <cmath>

/* Vrui includes */
#include <Geometry/OrthogonalTransformation.h>
#include <Geometry/Point.h>
#include <GL/GLExtensionManager.h>
#include <GL/Extensions/GLARBTextureNonPowerOfTwo.h>
#include <GL/GLPolylineTube.h>
#include <Misc/Timer.h>

/* Vrui includes to use the Vrui interface */
#include <Vrui/Vrui.h>

#include <DATA/Volume.h>
#include <DATASTRUCTURE/BoundingBox.h>
#include <DATASTRUCTURE/Brick.h>
#include <DATASTRUCTURE/ComparableBrick.h>
#include <DATASTRUCTURE/ComparableOcNode.h>
#include <DATASTRUCTURE/OcNode.h>
#include <DATASTRUCTURE/OcTree.h>
#include <GRAPHIC/Scene.h>
#include <MATH/Matrix4x4.h>
#include <MATH/Point4.h>
#include <MATH/Vector4.h>
#include <SHADER/ShaderManager.h>
#include <SHADER/ShaderObject.h>
#include <UTILITY/Stringify.h>

Scene::DataItem::DataItem(void) {
}

Scene::DataItem::~DataItem(void) {
    delete shaderManager;
}

/*
 * Scene
 *
 * parameter _volume - Volume*
 * parameter _colormap - unsigned char*
 * parameter _alpha - float*
 * parameter _red - float*
 * parameter _green - float*
 * parameter _blue - float*
 * parameter _sliceColormap - unsigned char*
 */
Scene::Scene(Volume* _volume, unsigned char* _colormap, float* _alpha, float * _alpha2_5D, float* _red, float* _green,
        float* _blue, unsigned char* _sliceColormap) :
    alpha(_alpha), alpha2_5D(_alpha2_5D), animating(false), blue(_blue), blueScale(1.0), boundaryContribution(0.0),
            boundaryExponent(1.0), bricks(NULL),
            colormap(_colormap), colorMapChanged(true), dimension(1), edgeContribution(0.0), edgeExponent(1.0), edgeThreshold(0.1),
            focusAndContext(false), green(_green), greenScale(1.0), interactive(false), lighting(false), maximumPriorityQueueSizeChanged(false),
            maximumPriorityQueueTest(false), normalContribution(1.0), numberOfBricks(0), preintegrated(false),
            ratioOfVisibilityTest(false), red(_red), redScale(1.0), renderingCostFunctionTest(true), showDisplay(false),
            showOutline(true), showVolume(false), silhouetteContribution(0.0), silhouetteExponent(1.0), sliceColormap(
                    _sliceColormap), sliceColorMapChanged(true), toneContribution(0.0), toonContribution(0.0), volume(_volume) {
    if (volume->getNumberOfComponents() != 0) {
        rgbChanged = true;
        alphaChanged = false;
    } else {
        rgbChanged = false;
        alphaChanged = true;
    }
    Brick* bricks;
    int numberOfBricks;
    setCoolColor(0.0f, 0.0f, 1.0f);
    setWarmColor(1.0f, 0.0f, 0.0f);
    setToonColor(0.0f, 1.0f, 0.0f);
    setAmbientColor(0.0f, 0.0f, 0.0f, 1.0f);
    setDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
    setSpecularColor(1.0f, 1.0f, 1.0f, 1.0f);
    setLightPosition(0.0f, 0.0f, volume->getExtent(2) * 2.0f);
    setKAmbient(0.1f, 0.1f, 0.1f);
    setKDiffuse(0.6f, 0.6f, 0.6f);
    setKSpecular(0.2f, 0.2f, 0.2f);
    setShininess(100.0f);
} // end Scene()

/*
 * ~Scene - destructor for the Scene class
 */
Scene::~Scene(void) {
    alpha = NULL;
    blue = NULL;
    colormap = NULL;
    green = NULL;
    red = NULL;
    sliceColormap = NULL;
    volume = NULL;
    if (bricks != NULL)
        delete[] bricks;
} // end ~Scene()

/*
 * calculateSlicePlaneNormal
 *
 * parameter modelviewInverse - Matrix4x4*
 * return - Vector4*
 */
Vector4* Scene::calculateSlicePlaneNormal(Matrix4x4* modelviewInverse) const {
    /*
     * slicePlaneNormal = modelviewInverse * viewPlaneNormal
     * viewPlaneNormal = (0,0,1)
     */
    //Vector4* slicePlaneNormal = new Vector4(modelviewInverse->get(2, 0),modelviewInverse->get(2, 1), modelviewInverse->get(2, 2), 0.0f);
    /*
     * slicePlaneNormal = (modelviewInverse * xPlaneNormal) X (modelviewInverse * yPlaneNormal) Cross Product
     * xPlaneNormal = (1,0,0)
     * yPlaneNormal = (0,1,0)
     */
    Vector4* x = new Vector4(modelviewInverse->get(0, 0), modelviewInverse->get(0, 1), modelviewInverse->get(0, 2), 0.0f);
    Vector4* y = new Vector4(modelviewInverse->get(1, 0), modelviewInverse->get(1, 1), modelviewInverse->get(1, 2), 0.0f);
    Vector4* slicePlaneNormal = x->cross(y);
    delete x;
    delete y;
    slicePlaneNormal->unitize();
    return slicePlaneNormal;
} // end calculateSlicePlaneNormal()

/*
 * clearSpheres
 */
void Scene::clearSpheres(void) {
    spheres.clear();
} // end clearSpheres()

/*
 * createBricks
 */
void Scene::createBricks(void) {
    int numberOfColumns = volume->getWidth() / volume->getTextureSize();
    if (volume->getWidth() % volume->getTextureSize() != 0)
        numberOfColumns++;
    int width = volume->getWidth() / numberOfColumns;
    int numberOfRows = volume->getHeight() / volume->getTextureSize();
    if (volume->getHeight() % volume->getTextureSize() != 0)
        numberOfRows++;
    int height = volume->getHeight() / numberOfRows;
    int numberOfSlabs = volume->getDepth() / volume->getTextureSize();
    if (volume->getDepth() % volume->getTextureSize() != 0)
        numberOfSlabs++;
    int depth = volume->getDepth() / numberOfSlabs;
    numberOfBricks = numberOfColumns * numberOfRows * numberOfSlabs;
    bricks = new Brick[numberOfBricks];
    int which = 0;
    int slabRemainder = volume->getDepth() % numberOfSlabs;
    int _slab = 0;
    for (int k = 0; k < numberOfSlabs; k++) {
        int rowRemainder = volume->getHeight() % numberOfRows;
        int _depth = depth;
        if (slabRemainder > 0) {
            _depth++;
            slabRemainder--;
        }
        int _row = 0;
        for (int j = 0; j < numberOfRows; j++) {
            int columnRemainder = volume->getWidth() % numberOfColumns;
            int _height = height;
            if (rowRemainder > 0) {
                _height++;
                rowRemainder--;
            }
            int _column = 0;
            for (int i = 0; i < numberOfColumns; i++) {
                int _width = width;
                if (columnRemainder > 0) {
                    _width++;
                    columnRemainder--;
                }
                bricks[which].setBrick(_column, _row, _slab, _width, _height, _depth, volume);
                _column += _width;
                which++;
            }
            _row += _height;
        }
        _slab += _depth;
    }
    for (int i = 0; i < numberOfBricks; i++)
        bricks[i].adjustTextureCoordinates();
} // end createBricks()

/*
 * displayScene
 *
 * parameter glContextData - GLContextData
 */
void Scene::display(GLContextData& glContextData) const {
    DataItem* dataItem = glContextData.retrieveDataItem<DataItem> (this);
    pushGLState(glContextData);
    dataItem->cFrustum.updateFrustum();
    if (showDisplay) {
        // Get head position in physical coordinates (Cheaper than transforming every bounding box)
        Geometry::Point<double, 3> p = Vrui::getHeadPosition();
        Vrui::getNavigationTransformation().inverseTransform(p);
        float* point = new float[3];
        point[0] = p[0];
        point[1] = p[1];
        point[2] = p[2];
        // sort bricks and ocnodes
        std::vector<ComparableBrick> brickList(numberOfBricks);
        sortBricks(point, brickList);

        if (alphaChanged) {
            if (ratioOfVisibilityTest) {
                if (dimension == 1) {
                    for (int i = 0; i < numberOfBricks; i++)
                        bricks[i].setRatioOfVisibility(alpha);
                } else {
                    for (int i = 0; i < numberOfBricks; i++)
                        bricks[i].setRatioOfVisibility2_5D(alpha2_5D);
                }
            } else if (maximumPriorityQueueTest) {
                if (dimension == 1) {
                    for (int i = 0; i < numberOfBricks; i++)
                        bricks[i].setPercentageOfEmptyVolume(alpha);
                } else {
                    for (int i = 0; i < numberOfBricks; i++)
                        bricks[i].setPercentageOfEmptyVolume2_5D(alpha2_5D);
                }
                for (int i = 0; i < numberOfBricks; i++)
                    bricks[i].determineOcNodesToDisplay();
            } else if (renderingCostFunctionTest) {
                if (dimension == 1) {
                    for (int i = 0; i < numberOfBricks; i++)
                        bricks[i].setRenderingCostFunction(alpha);
                } else {
                    for (int i = 0; i < numberOfBricks; i++)
                        bricks[i].setRenderingCostFunction2_5D(alpha2_5D);
                }
            }
        }
        if (maximumPriorityQueueSizeChanged) {
            for (int i = 0; i < numberOfBricks; i++)
                bricks[i].determineOcNodesToDisplay();

        }

        if (rgbChanged) {
            if (ratioOfVisibilityTest) {
                for (int i = 0; i < numberOfBricks; i++)
                    bricks[i].setRatioOfVisibility(red, green, blue);
            } else if (maximumPriorityQueueTest) {
                for (int i = 0; i < numberOfBricks; i++) {
                    bricks[i].setPercentageOfEmptyVolume(red, green, blue);
                    bricks[i].determineOcNodesToDisplay();
                }
            } else if (renderingCostFunctionTest) {
                for (int i = 0; i < numberOfBricks; i++)
                    bricks[i].setRenderingCostFunction(red, green, blue);
            }
        }

        if (colorMapChanged) {
            if (!preintegrated && dimension == 1)
                update1DColorMap(dataItem);
            else {
                update2DColorMap(dataItem);
            }
        }

        if (sliceColorMapChanged) {
            update1DSliceColorMap(dataItem);
        }

        glDisable(GL_CULL_FACE);
        glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);

        drawGrid(dataItem, brickList);

        drawSlices(glContextData, dataItem, brickList);

        if (!spheres.empty() && !animating) {
            drawSpheres();
        }

        glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_FALSE);
        glEnable(GL_CULL_FACE);
        glDisable(GL_LIGHTING);

        // Add Point/Line Models Here

        glEnable(GL_LIGHTING);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glDepthMask(GL_FALSE);
        glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);

        //Misc::Timer timer;
        if (showVolume) {
            drawVolume(glContextData, dataItem, brickList, point);
        }
        //timer.elapse();
        //if(Vrui::getNodeIndex()==0) std::cout<<timer.getTime()*1000.0<<std::endl;

        glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_FALSE);
        glDepthMask(GL_TRUE);
        glDisable(GL_BLEND);
    }
    popGLState(glContextData);

} // end displayScene()

/*
 * drawBricks
 *
 * parameter glContextData - GLContextData &
 * parameter cFrustum - const CFrustum &
 * parameter shaderObject - ShaderObject*
 * parameter brickList - const std::vector<ComparableBrick> &
 * parameter point - float *
 */
void Scene::drawBricks(GLContextData & glContextData, const CFrustum & cFrustum, ShaderObject* shaderObject, const std::vector<
        ComparableBrick> & brickList, float * point, int maximumIndex, int minimumIndex, Matrix4x4 * modelviewInverse) const {
    for (int i = 0; i < numberOfBricks; i++) {
        if (!bricks[brickList[i].getWhich()].isEmpty())
            bricks[brickList[i].getWhich()].drawVolume(glContextData, cFrustum, shaderObject, point, minimumIndex, maximumIndex,
                    modelviewInverse);

    }
} // end drawBricks()

/*
 * drawGrid
 *
 * parameter dataItem - DataItem*
 * parameter brickList - const std::vector<ComparableBrick> &
 */
void Scene::drawGrid(DataItem* dataItem, const std::vector<ComparableBrick> & brickList) const {
    for (int i = 0; i < numberOfBricks; i++) {
        if (!bricks[brickList[i].getWhich()].isEmpty())
            bricks[brickList[i].getWhich()].drawGrid(dataItem->cFrustum);
    }
    if (showOutline) {
        glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
        drawOutline();
    }
} // end drawGrid()

/*
 * drawOutline
 */
void Scene::drawOutline(void) const {
    glDisable(GL_LIGHTING);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    //glCallList(Box);
    glBegin(GL_QUADS);
    // Front Face
    glNormal3f(0.0f, 0.0f, 1.0f);
    glVertex3f(volume->getVolumeBox()->getX(2), volume->getVolumeBox()->getY(2), volume->getVolumeBox()->getZ(2));
    glVertex3f(volume->getVolumeBox()->getX(3), volume->getVolumeBox()->getY(3), volume->getVolumeBox()->getZ(3));
    glVertex3f(volume->getVolumeBox()->getX(7), volume->getVolumeBox()->getY(7), volume->getVolumeBox()->getZ(7));
    glVertex3f(volume->getVolumeBox()->getX(6), volume->getVolumeBox()->getY(6), volume->getVolumeBox()->getZ(6));
    // Back Face
    glNormal3f(0.0f, 0.0f, -1.0f);
    glVertex3f(volume->getVolumeBox()->getX(0), volume->getVolumeBox()->getY(0), volume->getVolumeBox()->getZ(0));
    glVertex3f(volume->getVolumeBox()->getX(4), volume->getVolumeBox()->getY(4), volume->getVolumeBox()->getZ(4));
    glVertex3f(volume->getVolumeBox()->getX(5), volume->getVolumeBox()->getY(5), volume->getVolumeBox()->getZ(5));
    glVertex3f(volume->getVolumeBox()->getX(1), volume->getVolumeBox()->getY(1), volume->getVolumeBox()->getZ(1));
    // Top Face
    glNormal3f(0.0f, 1.0f, 0.0f);
    glVertex3f(volume->getVolumeBox()->getX(4), volume->getVolumeBox()->getY(4), volume->getVolumeBox()->getZ(4));
    glVertex3f(volume->getVolumeBox()->getX(6), volume->getVolumeBox()->getY(6), volume->getVolumeBox()->getZ(6));
    glVertex3f(volume->getVolumeBox()->getX(7), volume->getVolumeBox()->getY(7), volume->getVolumeBox()->getZ(7));
    glVertex3f(volume->getVolumeBox()->getX(5), volume->getVolumeBox()->getY(5), volume->getVolumeBox()->getZ(5));
    // Bottom Face
    glNormal3f(0.0f, -1.0f, 0.0f);
    glVertex3f(volume->getVolumeBox()->getX(0), volume->getVolumeBox()->getY(0), volume->getVolumeBox()->getZ(0));
    glVertex3f(volume->getVolumeBox()->getX(1), volume->getVolumeBox()->getY(1), volume->getVolumeBox()->getZ(1));
    glVertex3f(volume->getVolumeBox()->getX(5), volume->getVolumeBox()->getY(5), volume->getVolumeBox()->getZ(5));
    glVertex3f(volume->getVolumeBox()->getX(4), volume->getVolumeBox()->getY(4), volume->getVolumeBox()->getZ(4));
    // Right face
    glNormal3f(1.0f, 0.0f, 0.0f);
    glVertex3f(volume->getVolumeBox()->getX(1), volume->getVolumeBox()->getY(1), volume->getVolumeBox()->getZ(1));
    glVertex3f(volume->getVolumeBox()->getX(5), volume->getVolumeBox()->getY(5), volume->getVolumeBox()->getZ(5));
    glVertex3f(volume->getVolumeBox()->getX(7), volume->getVolumeBox()->getY(7), volume->getVolumeBox()->getZ(7));
    glVertex3f(volume->getVolumeBox()->getX(3), volume->getVolumeBox()->getY(3), volume->getVolumeBox()->getZ(3));
    // Left Face
    glNormal3f(-1.0f, 0.0f, 0.0f);
    glVertex3f(volume->getVolumeBox()->getX(0), volume->getVolumeBox()->getY(0), volume->getVolumeBox()->getZ(0));
    glVertex3f(volume->getVolumeBox()->getX(2), volume->getVolumeBox()->getY(2), volume->getVolumeBox()->getZ(2));
    glVertex3f(volume->getVolumeBox()->getX(6), volume->getVolumeBox()->getY(6), volume->getVolumeBox()->getZ(6));
    glVertex3f(volume->getVolumeBox()->getX(4), volume->getVolumeBox()->getY(4), volume->getVolumeBox()->getZ(4));
    glEnd();
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glEnable(GL_LIGHTING);
} // end drawOutline()

/*
 * drawSlices
 *
 * parameter glContextData - GLContextData &
 * parameter dataItem - DataItem *
 * parameter brickList - const std::vector<ComparableBrick> &
 */
void Scene::drawSlices(GLContextData & glContextData, DataItem * dataItem, const std::vector<ComparableBrick> & brickList) const {
    glEnable(GL_TEXTURE_3D);
    glEnable(GL_TEXTURE_1D);
    glPolygonMode(GL_FRONT, GL_FILL);
    glPolygonMode(GL_BACK, GL_FILL);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_1D, dataItem->sliceColormap1DName);
    dataItem->sliceShaderObject->begin();
    dataItem->sliceShaderObject->sendUniform1i("sliceColormap1D", 1);
    if (volume->getNumberOfComponents() != 0) {
        dataItem->sliceShaderObject->sendUniform1f("redScale", GLfloat(redScale));
        dataItem->sliceShaderObject->sendUniform1f("greenScale", GLfloat(greenScale));
        dataItem->sliceShaderObject->sendUniform1f("blueScale", GLfloat(blueScale));
    }
    for (int i = 0; i < numberOfBricks; i++) {
        if (!bricks[brickList[i].getWhich()].isEmpty())
            bricks[brickList[i].getWhich()].drawSlices(glContextData, dataItem->cFrustum, dataItem->sliceShaderObject);
    }
    dataItem->sliceShaderObject->end();
    glBindTexture(GL_TEXTURE_1D, 0);
    glDisable(GL_TEXTURE_1D);
    glDisable(GL_TEXTURE_3D);
} // end drawSlices()

/*
 * drawSpheres
 */
void Scene::drawSpheres(void) const {
    for (int i = 0; i < spheres.size(); i++) {
        glPushMatrix();
        //pushGLState(glContextData);
        glTranslatef(spheres[i][0], spheres[i][1], spheres[i][2]);
        GLUquadricObj* gluQuadricObj = gluNewQuadric();
        gluQuadricDrawStyle(gluQuadricObj, GLU_FILL);
        gluQuadricNormals(gluQuadricObj, GLU_SMOOTH);
        gluSphere(gluQuadricObj, 3.0, 20, 20);
        gluDeleteQuadric(gluQuadricObj);
        //popGLState(glContextData);
        glPopMatrix();
    }
    if (!perspective) {
        glLineWidth(3.0f);
        for (int i = 0; i < spheres.size() - 1; i++) {
            glBegin(GL_LINE_STRIP);
            Geometry::Vector<double, 3> a(spheres[i][0], spheres[i][1], spheres[i][2]);
            Geometry::Vector<double, 3> b(spheres[i + 1][0], spheres[i + 1][1], spheres[i + 1][2]);
            double aMagnitude = sqrt(a[0] * a[0] + a[1] * a[1] + a[2] * a[2]);
            double bMagnitude = sqrt(b[0] * b[0] + b[1] * b[1] + b[2] * b[2]);
            for (int j = 100; j >= 0; j--) {
                double magnitude = (j * aMagnitude + (100 - j) * bMagnitude) / 100;
                Geometry::Vector<double, 3> vertex((j * a[0] + (100 - j) * b[0]) / 100, (j * a[1] + (100 - j) * b[1]) / 100, (j
                        * a[2] + (100 - j) * b[2]) / 100);
                vertex.normalize();
                vertex *= magnitude;
                glVertex3f(vertex[0], vertex[1], vertex[2]);
            }
            glEnd();
        }
        glLineWidth(1.0f);
    }
} // end drawSpheres()

/*
 * drawVolume
 *
 * parameter glContextData - GLContextData &
 * parameter dataItem - DataItem*
 * parameter brickList - const std::vector<ComparableBrick> &
 * parameter point - float *
 */
void Scene::drawVolume(GLContextData & glContextData, DataItem* dataItem, const std::vector<ComparableBrick> & brickList,
        float * point) const {
    float* data = new float[16];
    Matrix4x4* modelviewInverse;
    glGetFloatv(GL_MODELVIEW_MATRIX, data);
    Matrix4x4 modelview(data);
    delete[] data;
    modelviewInverse = modelview.inverse();
    BoundingBox* rotatedBox = new BoundingBox(bricks[0].getOcTree()->getRoot()->getVolumeBox()->getCorners());
    rotatedBox->transform(modelview);
    rotatedBox->findExtrema();
    int minimumIndex = rotatedBox->getMinimumIndex();
    delete rotatedBox;
    int maximumIndex = oppositeCorner[minimumIndex];
    Vector4* slicePlaneNormal = calculateSlicePlaneNormal(modelviewInverse);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    // glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_DEPTH_TEST);
    glDepthMask(false);
    glDisable(GL_CULL_FACE);
    glDisable(GL_LIGHT0);
    glDisable(GL_LIGHTING);
    glEnable(GL_TEXTURE_3D);
    glPolygonMode(GL_FRONT, GL_FILL);
    glPolygonMode(GL_BACK, GL_FILL);
    if (focusAndContext) {
        volumeShaderFocusAndContext(glContextData, dataItem, brickList, point, maximumIndex, minimumIndex, slicePlaneNormal,
                                modelviewInverse);
    } else if (dimension == 1) {
        if (preintegrated) {
            glEnable(GL_TEXTURE_2D);
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, dataItem->colormap2DName);
            if (lighting) {
                volumeShaderPreIntegratedWithLighting(glContextData, dataItem, brickList, point, maximumIndex, minimumIndex,
                        slicePlaneNormal, modelviewInverse);
            } else {
                volumeShaderPreIntegrated(glContextData, dataItem, brickList, point, maximumIndex, minimumIndex, slicePlaneNormal,
                        modelviewInverse);
            }
            glBindTexture(GL_TEXTURE_2D, 0);
            glDisable(GL_TEXTURE_2D);
        } else {
            glEnable(GL_TEXTURE_1D);
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_1D, dataItem->colormap1DName);
            if (lighting) {
                volumeShaderWithLighting(glContextData, dataItem, brickList, point, maximumIndex, minimumIndex, slicePlaneNormal,
                        modelviewInverse);
            } else {
                volumeShader(glContextData, dataItem, brickList, point, maximumIndex, minimumIndex, slicePlaneNormal,
                        modelviewInverse);
            }
            glBindTexture(GL_TEXTURE_1D, 0);
            glDisable(GL_TEXTURE_1D);
        }
    } else {
        glEnable(GL_TEXTURE_2D);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, dataItem->colormap2DName);
        if (lighting) {
            volumeShader2DWithLighting(glContextData, dataItem, brickList, point, maximumIndex, minimumIndex, slicePlaneNormal,
                    modelviewInverse);
        } else {
            volumeShader2D(glContextData, dataItem, brickList, point, maximumIndex, minimumIndex, slicePlaneNormal,
                    modelviewInverse);
        }
        glBindTexture(GL_TEXTURE_2D, 0);
        glDisable(GL_TEXTURE_2D);
    }
    glDisable(GL_TEXTURE_3D);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);
    delete slicePlaneNormal;
    delete modelviewInverse;
} // end drawVolume()

/*
 * setAmbientColor
 *
 * parameter r - float
 * parameter g - float
 * parameter b - float
 * parameter a - float
 */
void Scene::setAmbientColor(float r, float g, float b, float a) {
    ambientColor[0] = r;
    ambientColor[1] = g;
    ambientColor[2] = b;
    ambientColor[3] = a;
} // end setAmbientColor()

/*
 * setAnimating
 *
 * parameter _animating - bool
 */
void Scene::setAnimating(bool _animating) {
    animating = _animating;
} // end setAnimating()

/*
 * setAlphaChanged
 *
 * parameter _alphaChanged - bool
 */
void Scene::setAlphaChanged(bool _alphaChanged) {
    alphaChanged = _alphaChanged;
} // end setAlphaChanged()

/*
 * setBlueScale
 *
 * parameter _blueScale - float
 */
void Scene::setBlueScale(float _blueScale) {
    blueScale = _blueScale;
} // end setBlueScale()

/*
 * setBoundaryContribution
 *
 * parameter boundaryContribution - float
 */
void Scene::setBoundaryContribution(float boundaryContribution) {
    this->boundaryContribution = boundaryContribution;
} // end setBoundaryContribution()

/*
 * setBoundaryExponent
 *
 * parameter boundaryExponent - float
 */
void Scene::setBoundaryExponent(float boundaryExponent) {
    this->boundaryExponent = boundaryExponent;
} // end setBoundaryExponent()

/*
 * setColorMapChanged
 *
 * parameter _colorMapChanged - bool
 */
void Scene::setColorMapChanged(bool _colorMapChanged) {
    colorMapChanged = _colorMapChanged;
} // end setColorMapChanged()

/*
 * setCoolColor
 *
 * parameter r - float
 * parameter g - float
 * parameter b - float
 */
void Scene::setCoolColor(float r, float g, float b) {
    coolColor[0] = r;
    coolColor[1] = g;
    coolColor[2] = b;
} // end setCoolColor()

/*
 * setDiffuseColor
 *
 * parameter r - float
 * parameter g - float
 * parameter b - float
 * parameter a - float
 */
void Scene::setDiffuseColor(float r, float g, float b, float a) {
    diffuseColor[0] = r;
    diffuseColor[1] = g;
    diffuseColor[2] = b;
    diffuseColor[3] = a;
} // end setDiffuseColor()

/*
 * getDimension
 *
 * return - int
 */
int Scene::getDimension(void) const {
    return dimension;
} // end getDimension()

/*
 * setDimension
 *
 * parameter dimension - int
 */
void Scene::setDimension(int dimension) {
    this->dimension = dimension;
} // end setDimension()

/*
 * setDownSamplingChanged
 *
 * parameter downSamplingChanged - bool
 */
void Scene::setDownSamplingChanged(bool downSamplingChanged) {
    for (int i = 0; i < numberOfBricks; i++)
        bricks[i].setDownSamplingChanged(downSamplingChanged);
} // end setDownSamplingChanged()

/*
 * setEdgeContribution
 *
 * parameter edgeContribution - float
 */
void Scene::setEdgeContribution(float edgeContribution) {
    this->edgeContribution = edgeContribution;
} // end setEdgeContribution()

/*
 * setEdgeExponent
 *
 * parameter edgeExponent - float
 */
void Scene::setEdgeExponent(float edgeExponent) {
    this->edgeExponent = edgeExponent;
} // end setEdgeExponent()

/*
 * setEdgeThreshold
 *
 * parameter edgeThreshold - float
 */
void Scene::setEdgeThreshold(float edgeThreshold) {
    this->edgeThreshold = edgeThreshold;
} // end setEdgeThreshold()

/*
 * setGreenScale
 *
 * parameter _greenScale - float
 */
void Scene::setGreenScale(float _greenScale) {
    greenScale = _greenScale;
} // end setGreenScale()

/*
 * getInteractive
 *
 * return - bool
 */
bool Scene::getInteractive(void) const {
    return interactive;
} // end getInteractive()

/*
 * setInteractive
 *
 * parameter interactive - bool
 */
void Scene::setInteractive(bool interactive) {
    this->interactive = interactive;
    for (int i = 0; i < numberOfBricks; i++)
        bricks[i].setInteractive(interactive);
} // end setInteractive()

/*
 * setKAmbient
 *
 * parameter r - float
 * parameter g - float
 * parameter b - float
 */
void Scene::setKAmbient(float r, float g, float b) {
    kAmbient[0] = r;
    kAmbient[1] = g;
    kAmbient[2] = b;
} // end setKAmbient()

/*
 * setKDiffuse
 *
 * parameter r - float
 * parameter g - float
 * parameter b - float
 */
void Scene::setKDiffuse(float r, float g, float b) {
    kDiffuse[0] = r;
    kDiffuse[1] = g;
    kDiffuse[2] = b;
} // end setKDiffuse()

/*
 * setKSpecular
 *
 * parameter r - float
 * parameter g - float
 * parameter b - float
 */
void Scene::setKSpecular(float r, float g, float b) {
    kSpecular[0] = r;
    kSpecular[1] = g;
    kSpecular[2] = b;
} // end setKSpecular()

/*
 * getLighting
 *
 * return - bool
 */
bool Scene::getLighting(void) const {
    return lighting;
} // end getLighting()

/*
 * setLighting
 *
 * parameter lighting - bool
 */
void Scene::setLighting(bool lighting) {
    this->lighting = lighting;
} // end setLighting()

/*
 * setLightPosition
 *
 * parameter xLightPosition - float
 * parameter yLightPosition - float
 * parameter zLightPosition - float
 */
void Scene::setLightPosition(float xLightPosition, float yLightPosition, float zLightPosition) {
    this->lightPosition[0] = xLightPosition;
    this->lightPosition[1] = yLightPosition;
    this->lightPosition[2] = zLightPosition;
} // end setLightPosition()

/*
 * setMaximumPriorityQueueSize
 *
 * parameter _maximumPriorityQueueSize - int
 */
void Scene::setMaximumPriorityQueueSize(int _maximumPriorityQueueSize) {
    for (int i = 0; i < numberOfBricks; i++)
        bricks[i].setMaximumPriorityQueueSize(_maximumPriorityQueueSize);
} // end setMaximumPriorityQueueSize()

/*
 * setMaximumPriorityQueueSizeChanged
 *
 * parameter _maximumPriorityQueueSizeChanged - bool
 */
void Scene::setMaximumPriorityQueueSizeChanged(bool _maximumPriorityQueueSizeChanged) {
    maximumPriorityQueueSizeChanged = _maximumPriorityQueueSizeChanged;
} // end setMaximumPriorityQueueSizeChanged()

/*
 * setMaximumPriorityQueueTest
 *
 * parameter _maximumPriorityQueueTest - bool
 */
void Scene::setMaximumPriorityQueueTest(bool _maximumPriorityQueueTest) {
    maximumPriorityQueueTest = _maximumPriorityQueueTest;
    for (int i = 0; i < numberOfBricks; i++)
        bricks[i].setMaximumPriorityQueueTest(_maximumPriorityQueueTest);
} // end setMaximumPriorityQueueTest()

/*
 * setNormalContribution
 *
 * parameter normalContribution - float
 */
void Scene::setNormalContribution(float normalContribution) {
    this->normalContribution = normalContribution;
} // end setNormalContribution()

/*
 * setPerspective
 *
 * parameter _perspective - bool
 */
void Scene::setPerspective(bool _perspective) {
    perspective = _perspective;
} // end setPerspective()

/*
 * getPreintegrated
 *
 * return - bool
 */
bool Scene::getPreintegrated(void) const {
    return preintegrated;
} // end getPreintegrated()

/*
 * setPreintegrated
 *
 * parameter preintegrated - bool
 */
void Scene::setPreintegrated(bool preintegrated) {
    this->preintegrated = preintegrated;
} // end setPreintegrated()

/*
 * setPlaneNormal
 *
 * parameter planeNormal - float[3]
 */
void Scene::setPlaneNormal(float planeNormal[3]) {
    this->planeNormal[0] = planeNormal[0];
    this->planeNormal[1] = planeNormal[2];
    this->planeNormal[2] = planeNormal[0];
} // end setPlaneNormal()

/*
 * setPlaneOffset
 *
 * parameter planeOffset - float
 */
void Scene::setPlaneOffset(float planeOffset) {
    this->planeOffset = planeOffset;
} // end setPlaneOffset()

/*
 * setRatioOfVisibilityTest
 *
 * parameter _ratioOfVisibilityTest - bool
 */
void Scene::setRatioOfVisibilityTest(bool _ratioOfVisibilityTest) {
    ratioOfVisibilityTest = _ratioOfVisibilityTest;
    for (int i = 0; i < numberOfBricks; i++)
        bricks[i].setRatioOfVisibilityTest(_ratioOfVisibilityTest);
} // end setRatioOfVisibilityTest()

/*
 * setRatioOfVisibilityThreshold
 *
 * parameter _ratioOfVisibilityThreshold - float
 */
void Scene::setRatioOfVisibilityThreshold(float _ratioOfVisibilityThreshold) {
    for (int i = 0; i < numberOfBricks; i++)
        bricks[i].setRatioOfVisibilityThreshold(_ratioOfVisibilityThreshold);
} // end setRatioOfVisibilityThreshold()

/*
 * setRedScale
 *
 * parameter _redScale - float
 */
void Scene::setRedScale(float _redScale) {
    redScale = _redScale;
} // end setRedScale()

/*
 * setRenderingCost
 *
 * parameter _renderingCost - float
 */
void Scene::setRenderingCost(float _renderingCost) {
    for (int i = 0; i < numberOfBricks; i++)
        bricks[i].setRenderingCost(_renderingCost);
} // end setRenderingCost()

/*
 * setRenderingCostFunctionTest
 *
 * parameter _renderingCostFunctionTest - bool
 */
void Scene::setRenderingCostFunctionTest(bool _renderingCostFunctionTest) {
    renderingCostFunctionTest = _renderingCostFunctionTest;
    for (int i = 0; i < numberOfBricks; i++)
        bricks[i].setRenderingCostFunctionTest(_renderingCostFunctionTest);
} // end setRenderingCostFunctionTest()

/*
 * setRGBChanged
 *
 * parameter _rgbChanged - bool
 */
void Scene::setRGBChanged(bool _rgbChanged) {
    rgbChanged = _rgbChanged;
} // end setRGBChanged()

/*
 * setShininess
 *
 * parameter shininess - float
 */
void Scene::setShininess(float shininess) {
    this->shininess = shininess;
} // end setShininess()

/*
 * setShowBricks
 *
 * parameter _showBricks - bool
 */
void Scene::setShowBricks(bool _showBricks) {
    for (int i = 0; i < numberOfBricks; i++)
        bricks[i].setShowBricks(_showBricks);
} // end setShowBricks()

/*
 * setShowDisplay
 *
 * parameter showDisplay - bool
 */
void Scene::setShowDisplay(bool showDisplay) {
    this->showDisplay = showDisplay;
} // end setShowDisplay()

/*
 * setShowOcNodes
 *
 * parameter _showOcNodes - bool
 */
void Scene::setShowOcNodes(bool _showOcNodes) {
    for (int i = 0; i < numberOfBricks; i++)
        bricks[i].setShowOcNodes(_showOcNodes);
} // end setShowOcNodeLeaves()

/*
 * setShowOutline
 *
 * parameter _showOutline - bool
 */
void Scene::setShowOutline(bool _showOutline) {
    showOutline = _showOutline;
} // end setShowOutline()

/*
 * setShowVolume
 *
 * parameter _showVolume - bool
 */
void Scene::setShowVolume(bool _showVolume) {
    showVolume = _showVolume;
} // end setShowVolume()

/*
 * setShowXSlice
 *
 * parameter _showXSlice - bool
 */
void Scene::setShowXSlice(bool _showXSlice) {
    for (int i = 0; i < numberOfBricks; i++)
        bricks[i].setShowXSlice(_showXSlice);
} // end setShowXSlice()

/*
 * setShowYSlice
 *
 * parameter _showYSlice - bool
 */
void Scene::setShowYSlice(bool _showYSlice) {
    for (int i = 0; i < numberOfBricks; i++)
        bricks[i].setShowYSlice(_showYSlice);
} // end setShowYSlice()

/*
 * setShowZSlice
 *
 * parameter _showZSlice - bool
 */
void Scene::setShowZSlice(bool _showZSlice) {
    for (int i = 0; i < numberOfBricks; i++)
        bricks[i].setShowZSlice(_showZSlice);
} // end setShowZSlice()

/*
 * setSilhouetteContribution
 *
 * parameter silhouetteContribution - float
 */
void Scene::setSilhouetteContribution(float silhouetteContribution) {
    this->silhouetteContribution = silhouetteContribution;
} // end setSilhouetteContribution()

/*
 * setSilhouetteExponent
 *
 * parameter silhouetteExponent - float
 */
void Scene::setSilhouetteExponent(float silhouetteExponent) {
    this->silhouetteExponent = silhouetteExponent;
} // end setSilhouetteExponent()

/*
 * setSliceColorMapChanged
 *
 * parameter _sliceColorMapChanged - bool
 */
void Scene::setSliceColorMapChanged(bool _sliceColorMapChanged) {
    sliceColorMapChanged = _sliceColorMapChanged;
} // end setSliceColorMapChanged()

/*
 * setSliceFactor
 *
 * parameter _sliceFactor - float
 */
void Scene::setSliceFactor(float _sliceFactor) {
    for (int i = 0; i < numberOfBricks; i++)
        bricks[i].setSliceFactor(_sliceFactor);
} // end setSliceFactor()

/*
 * setSlicingScale
 *
 * parameter slicingScale - float
 */
void Scene::setSlicingScale(float slicingScale) {
    for (int i = 0; i < numberOfBricks; i++)
        bricks[i].setSlicingScale(slicingScale);
} // end setSlicingScale()

/*
 * setSpecularColor
 *
 * parameter r - float
 * parameter g - float
 * parameter b - float
 * parameter a - float
 */
void Scene::setSpecularColor(float r, float g, float b, float a) {
    specularColor[0] = r;
    specularColor[1] = g;
    specularColor[2] = b;
    specularColor[3] = a;
} // end setSpecularColor()

/*
 * setSphere
 *
 * parameter sphere - Geometry::Point<double,3>
 */
void Scene::setSphere(Geometry::Point<double, 3> sphere) {
    spheres.push_back(sphere);
} // end setSphere()

/*
 * setToneContribution
 *
 * parameter toneContribution - float
 */
void Scene::setToneContribution(float toneContribution) {
    this->toneContribution = toneContribution;
} // end setToneContribution()

/*
 * setToonColor
 *
 * parameter r - float
 * parameter g - float
 * parameter b - float
 */
void Scene::setToonColor(float r, float g, float b) {
    toonColor[0] = r;
    toonColor[1] = g;
    toonColor[2] = b;
} // end setToonColor()

/*
 * setToonContribution
 *
 * parameter toonContribution - float
 */
void Scene::setToonContribution(float toonContribution) {
    this->toonContribution = toonContribution;
} // end setToonContribution()

/*
 * setWarmColor
 *
 * parameter r - float
 * parameter g - float
 * parameter b - float
 */
void Scene::setWarmColor(float r, float g, float b) {
    warmColor[0] = r;
    warmColor[1] = g;
    warmColor[2] = b;
} // end setWarmColor()

/*
 * setXSlice
 *
 * parameter _xSlice - int
 */
void Scene::setXSlice(int _xSlice) {
    for (int i = 0; i < numberOfBricks; i++)
        bricks[i].setXSlice(_xSlice);
} // end setXSlice()

/*
 * setXStep
 *
 * parameter xStep - int
 */
void Scene::setXStep(int xStep) {
    for (int i = 0; i < numberOfBricks; i++)
        bricks[i].setXStep(xStep);
} // end setXStep()

/*
 * setYSlice
 *
 * parameter _ySlice - int
 */
void Scene::setYSlice(int _ySlice) {
    for (int i = 0; i < numberOfBricks; i++)
        bricks[i].setYSlice(_ySlice);
} // end setYSlice()

/*
 * setYStep
 *
 * parameter yStep - int
 */
void Scene::setYStep(int yStep) {
    for (int i = 0; i < numberOfBricks; i++)
        bricks[i].setYStep(yStep);
} // end setYStep()

/*
 * setZSlice
 *
 * parameter _zSlice - int
 */
void Scene::setZSlice(int _zSlice) {
    for (int i = 0; i < numberOfBricks; i++)
        bricks[i].setZSlice(_zSlice);
} // end setZSlice()

/*
 * setZStep
 *
 * parameter zStep - int
 */
void Scene::setZStep(int zStep) {
    for (int i = 0; i < numberOfBricks; i++)
        bricks[i].setZStep(zStep);
} // end setZStep()

/*
 * initContext
 *
 * parameter glContextData - GLContextData&
 */
void Scene::initContext(GLContextData& glContextData) const {
    DataItem* dataItem = new DataItem();
    glContextData.addDataItem(this, dataItem);
    dataItem->textureNonPowerOfTwo = GLARBTextureNonPowerOfTwo::isSupported();
    if (dataItem->textureNonPowerOfTwo) {
        GLARBTextureNonPowerOfTwo::initExtension();
    } else {
        volume->resize();
    }
    glGetIntegerv(GL_MAX_3D_TEXTURE_SIZE, &dataItem->maximum3DTextureSize);
    dataItem->shaderManager = new ShaderManager();
    if (!dataItem->shaderManager->initializeGLSL()) {
        std::cout << "Error: Can't initialize GLSL" << std::endl;
    }
    if (volume->getNumberOfComponents() != 0) {
        dataItem->volumeShaderObject = dataItem->shaderManager->loadFromMemory(GPUVertexShader, multiComponentFragmentShader);
        dataItem->volumeShaderObject2D = dataItem->shaderManager->loadFromMemory(GPUVertexShader, multiComponentFragmentShader);
        dataItem->preIntegratedVolumeShaderObject = dataItem->shaderManager->loadFromMemory(preIntegratedGPUVertexShader,
                preIntegratedMultiComponentFragmentShader);
        dataItem->volumeShaderObjectWithLighting = dataItem->shaderManager->loadFromMemory(GPUVertexShaderWithLighting,
                multiComponentFragmentShaderWithLighting);
        dataItem->preIntegratedVolumeShaderObjectWithLighting = dataItem->shaderManager->loadFromMemory(
                preIntegratedGPUVertexShaderWithLighting, preIntegratedMultiComponentFragmentShaderWithLighting);
    } else {
        dataItem->volumeShaderObjectFocusAndContext = dataItem->shaderManager->loadFromMemory(GPUVertexShaderWithLighting, focusAndContextFragmentShader);
        dataItem->volumeShaderObject = dataItem->shaderManager->loadFromMemory(GPUVertexShader, fragmentShader);
        dataItem->volumeShaderObject2D = dataItem->shaderManager->loadFromMemory(GPUVertexShader, fragmentShader2D);
        dataItem->volumeShaderObject2DWithLighting = dataItem->shaderManager->loadFromMemory(GPUVertexShaderWithLighting,
                fragmentShader2DWithLighting);
        dataItem->volumeShaderObjectWithLighting = dataItem->shaderManager->loadFromMemory(GPUVertexShaderWithLighting,
                fragmentShaderWithLighting);
        dataItem->preIntegratedVolumeShaderObject = dataItem->shaderManager->loadFromMemory(preIntegratedGPUVertexShader,
                preIntegratedFragmentShader);
        dataItem->preIntegratedVolumeShaderObjectWithLighting = dataItem->shaderManager->loadFromMemory(
                preIntegratedGPUVertexShaderWithLighting, preIntegratedFragmentShaderWithLighting);
    }
    if (volume->getNumberOfComponents() != 0) {
        dataItem->sliceShaderObject = dataItem->shaderManager->loadFromMemory(vertexShader, sliceMultiComponentFragmentShader);
    } else {
        dataItem->sliceShaderObject = dataItem->shaderManager->loadFromMemory(vertexShader, sliceFragmentShader);
    }
    if (dataItem->volumeShaderObject == 0) {
        std::cout << "Error: can't init volume shader!\n";
        exit(-1);
    }
    if (dataItem->preIntegratedVolumeShaderObject == 0) {
        std::cout << "Error: can't init pre integrated volume shader!\n";
        exit(-1);
    }
    if (dataItem->sliceShaderObject == 0) {
        std::cout << "Error: can't init slice shader!\n";
        exit(-1);
    }
    initialize1DColorMap(dataItem);
    initialize2DColorMap(dataItem);
    initialize1DSliceColorMap(dataItem);
} // end initContext()

/*
 * initialize
 */
void Scene::initialize(void) {
    createBricks();
} // end initialize()

/*
 * initialize1DColorMap
 *
 * parameter dataItem - DataItem*
 */
void Scene::initialize1DColorMap(DataItem* dataItem) const {
    unsigned char * texels = new unsigned char[256 * 4];
    for (int i = 0; i < 256; i++) {
        texels[4 * i + RED] = colormap[4 * (i * 256 + i) + RED];
        texels[4 * i + GREEN] = colormap[4 * (i * 256 + i) + GREEN];
        texels[4 * i + BLUE] = colormap[4 * (i * 256 + i) + BLUE];
        texels[4 * i + ALPHA] = colormap[4 * (i * 256 + i) + ALPHA];
    }
    glEnable(GL_TEXTURE_1D);
    glGenTextures(1, &dataItem->colormap1DName);
    glBindTexture(GL_TEXTURE_1D, dataItem->colormap1DName);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage1D(GL_TEXTURE_1D, 0, GL_RGBA, 256, 0, GL_RGBA, GL_UNSIGNED_BYTE, texels);
    glBindTexture(GL_TEXTURE_1D, 0);
    glDisable(GL_TEXTURE_1D);
    delete[] texels;
} // end initialize1DColorMap()

/*
 * initialize2DColorMap
 *
 * parameter dataItem - DataItem*
 */
void Scene::initialize2DColorMap(DataItem* dataItem) const {
    glEnable(GL_TEXTURE_2D);
    glGenTextures(1, &dataItem->colormap2DName);
    glBindTexture(GL_TEXTURE_2D, dataItem->colormap2DName);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 256, 256, 0, GL_RGBA, GL_UNSIGNED_BYTE, colormap);
    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_TEXTURE_2D);
} // end initialize2DColormap()

/*
 * initialize1DSliceColorMap
 *
 * parameter dataItem - DataItem*
 */
void Scene::initialize1DSliceColorMap(DataItem* dataItem) const {
    unsigned char * texels = new unsigned char[256 * 4];
    for (int i = 0; i < 256; i++) {
        texels[4 * i + RED] = sliceColormap[4 * (i * 256 + i) + RED];
        texels[4 * i + GREEN] = sliceColormap[4 * (i * 256 + i) + GREEN];
        texels[4 * i + BLUE] = sliceColormap[4 * (i * 256 + i) + BLUE];
        texels[4 * i + ALPHA] = sliceColormap[4 * (i * 256 + i) + ALPHA];
    }
    glEnable(GL_TEXTURE_1D);
    glGenTextures(1, &dataItem->sliceColormap1DName);
    glBindTexture(GL_TEXTURE_1D, dataItem->sliceColormap1DName);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage1D(GL_TEXTURE_1D, 0, GL_RGBA, 256, 0, GL_RGBA, GL_UNSIGNED_BYTE, texels);
    glBindTexture(GL_TEXTURE_1D, 0);
    glDisable(GL_TEXTURE_1D);
    delete[] texels;
} // end initialize1DSliceColorMap()

/*
 * popGLState - Restore GL State
 *
 * parameter glContextData - GLContextData&
 */
void Scene::popGLState(GLContextData& glContextData) const {
    glPopAttrib();
}

/*
 * pushGLState - Save GL state
 *
 * parameter glContextData - GLContextData&
 */
void Scene::pushGLState(GLContextData& glContextData) const {
    glPushAttrib(GL_COLOR_BUFFER_BIT | GL_CURRENT_BIT | GL_DEPTH_BUFFER_BIT | GL_ENABLE_BIT | GL_POLYGON_BIT | GL_TEXTURE_BIT);
} // end pushGLState()

/*
 * sortBricks
 *
 * parameter point - float *
 * parameter brickList - std::vector<ComparableBrick> &
 */
void Scene::sortBricks(float * point, std::vector<ComparableBrick> & brickList) const {
    // Calculate minimum distance from head position to brick feature
    ComparableBrick comparableBrick;
    comparableBrick.maximum(); // set static variable greater to perform a maximum sort
    int i;
    for (i = 0; i < numberOfBricks; i++) {
        brickList[i] = ComparableBrick(i, bricks[i].getVolumeBox()->getMinimumDistance(point));
    }
    std::sort(brickList.begin(), brickList.end());
    // Handle ties in smallest keys (drawn last)
    i = 2;
    if (numberOfBricks > 1) {
        while (i <= numberOfBricks & brickList[numberOfBricks - i].getValue() == brickList[numberOfBricks - 1].getValue()) {
            if (bricks[brickList[numberOfBricks - i].getWhich()].getVolumeBox()->collision(point)) { // if head position inside bounding box
                float distance = brickList[numberOfBricks - 1].getValue();
                int which = brickList[numberOfBricks - 1].getWhich();
                brickList[numberOfBricks - 1].setValue(brickList[numberOfBricks - i].getValue());
                brickList[numberOfBricks - 1].setWhich(brickList[numberOfBricks - i].getWhich());
                brickList[numberOfBricks - i].setValue(distance);
                brickList[numberOfBricks - i].setWhich(which);
            }
            i++;
        }
    }
} // end sortBricks()

/*
 * toString
 *
 * return - std::string
 */
std::string Scene::toString(void) {
    return " ";
} // end toString()_alpha

/*
 * update1DColorMap
 *
 * parameter dataItem - DataItem*
 */
void Scene::update1DColorMap(DataItem* dataItem) const {
    if (colorMapChanged) {
        unsigned char * texels = new unsigned char[256 * 4];
        for (int i = 0; i < 256; i++) {
            texels[4 * i + RED] = colormap[4 * (i * 256 + i) + RED];
            texels[4 * i + GREEN] = colormap[4 * (i * 256 + i) + GREEN];
            texels[4 * i + BLUE] = colormap[4 * (i * 256 + i) + BLUE];
            texels[4 * i + ALPHA] = colormap[4 * (i * 256 + i) + ALPHA];
        }
        glEnable(GL_TEXTURE_1D);
        glBindTexture(GL_TEXTURE_1D, dataItem->colormap1DName);
        glTexSubImage1D(GL_TEXTURE_1D, 0, 0, 256, GL_RGBA, GL_UNSIGNED_BYTE, texels);
        glBindTexture(GL_TEXTURE_1D, 0);
        glDisable(GL_TEXTURE_1D);
        delete[] texels;
    }
} // end update1DColorMap()

/*
 * update2DColorMap
 *
 * parameter dataItem - DataItem*
 */
void Scene::update2DColorMap(DataItem* dataItem) const {
    if (colorMapChanged) {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, dataItem->colormap2DName);
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 256, 256, GL_RGBA, GL_UNSIGNED_BYTE, colormap);
        glBindTexture(GL_TEXTURE_2D, 0);
        glDisable(GL_TEXTURE_2D);
    }
} // end update2DColorMap()

/*
 * update1DSliceColorMap
 *
 * parameter dataItem - DataItem*
 */
void Scene::update1DSliceColorMap(DataItem* dataItem) const {
    if (sliceColorMapChanged) {
        unsigned char * texels = new unsigned char[256 * 4];
        for (int i = 0; i < 256; i++) {
            texels[4 * i + RED] = sliceColormap[4 * (i * 256 + i) + RED];
            texels[4 * i + GREEN] = sliceColormap[4 * (i * 256 + i) + GREEN];
            texels[4 * i + BLUE] = sliceColormap[4 * (i * 256 + i) + BLUE];
            texels[4 * i + ALPHA] = sliceColormap[4 * (i * 256 + i) + ALPHA];
        }
        glEnable(GL_TEXTURE_1D);
        glBindTexture(GL_TEXTURE_1D, dataItem->sliceColormap1DName);
        glTexSubImage1D(GL_TEXTURE_1D, 0, 0, 256, GL_RGBA, GL_UNSIGNED_BYTE, texels);
        glBindTexture(GL_TEXTURE_1D, 0);
        glDisable(GL_TEXTURE_1D);
        delete[] texels;
    }
} // end update1DSliceColorMap()

/*
 * updateAlpha
 *
 * parameter _alpha - float*
 */
void Scene::updateAlpha(float* _alpha) {
    alpha = _alpha;
} // end updateAlpha()

/*
 * updateAlpha2_5D
 *
 * parameter _alpha2_5D - float*
 */
void Scene::updateAlpha2_5D(float * _alpha2_5D) {
    alpha2_5D = _alpha2_5D;
} // end updateAlpha2_5D()

/*
 * updateBlue
 *
 * parameter _blue - float*
 */
void Scene::updateBlue(float* _blue) {
    blue = _blue;
} // end updateBlue()

/*
 * updateColorMap
 *
 * parameter _colormap - unsigned char*
 */
void Scene::updateColorMap(unsigned char* _colormap) {
    colormap = _colormap;
} // end updateColorMap()

/*
 * updateGreen
 *
 * parameter _green - float*
 */
void Scene::updateGreen(float* _green) {
    green = _green;
} // end updateGreen()

/*
 * updateRed
 *
 * parameter _red - float*
 */
void Scene::updateRed(float* _red) {
    red = _red;
} // end updateRed()

/*
 * updateRGB
 *
 * parameter _red - float*
 * parameter _green - float*
 * parameter _blue - float*
 */
void Scene::updateRGB(float* _red, float* _green, float* _blue) {
    red = _red;
    green = _green;
    blue = _blue;
} // end updateRGB()

/*
 * updateSliceColorMap
 *
 * parameter _sliceColormap - unsigned char*
 */
void Scene::updateSliceColorMap(unsigned char* _sliceColormap) {
    sliceColormap = _sliceColormap;
} // end updateSliceColorMap()

/*
 * volumeShader
 *
 * parameter glContextData - GLContextData &
 * parameter dataItem - DataItem *
 * parameter brickList - const std::vector<ComparableBrick> &
 * parameter point - float *
 * parameter maximumIndex - int
 * parameter minimumIndex - int
 * parameter slicePlaneNormal - Vector4 *
 * parameter modelviewInverse - Matrix4x4 *
 */
void Scene::volumeShader(GLContextData & glContextData, DataItem * dataItem, const std::vector<ComparableBrick> & brickList,
        float * point, int maximumIndex, int minimumIndex, Vector4 * slicePlaneNormal, Matrix4x4 * modelviewInverse) const {
    dataItem->volumeShaderObject->begin();
    dataItem->volumeShaderObject->sendUniform1i("colormap1D", 1);
    dataItem->volumeShaderObject->sendUniform1iv("seq", 64, sequenceIndices);
    dataItem->volumeShaderObject->sendUniform1iv("edge", 48, edgeIndices);
    dataItem->volumeShaderObject->sendUniform1i("front", GLint(maximumIndex));
    dataItem->volumeShaderObject->sendUniform3f("slicePlaneNormal", GLfloat(slicePlaneNormal->getX()), GLfloat(
            slicePlaneNormal->getY()), GLfloat(slicePlaneNormal->getZ()));
    if (volume->getNumberOfComponents() != 0) {
        dataItem->volumeShaderObject->sendUniform1f("redScale", GLfloat(redScale));
        dataItem->volumeShaderObject->sendUniform1f("greenScale", GLfloat(greenScale));
        dataItem->volumeShaderObject->sendUniform1f("blueScale", GLfloat(blueScale));
    }
    drawBricks(glContextData, dataItem->cFrustum, dataItem->volumeShaderObject, brickList, point, maximumIndex, minimumIndex,
            modelviewInverse);
    dataItem->volumeShaderObject->end();
} // end volumeShader()

/*
 * volumeShader2D
 *
 * parameter glContextData - GLContextData &
 * parameter dataItem - DataItem *
 * parameter brickList - const std::vector<ComparableBrick> &
 * parameter point - float *
 * parameter maximumIndex - int
 * parameter minimumIndex - int
 * parameter slicePlaneNormal - Vector4 *
 * parameter modelviewInverse - Matrix4x4 *
 */
void Scene::volumeShader2D(GLContextData & glContextData, DataItem * dataItem, const std::vector<ComparableBrick> & brickList,
        float * point, int maximumIndex, int minimumIndex, Vector4 * slicePlaneNormal, Matrix4x4 * modelviewInverse) const {
    dataItem->volumeShaderObject2D->begin();
    dataItem->volumeShaderObject2D->sendUniform1i("colormap2D", 1);
    dataItem->volumeShaderObject2D->sendUniform1iv("seq", 64, sequenceIndices);
    dataItem->volumeShaderObject2D->sendUniform1iv("edge", 48, edgeIndices);
    dataItem->volumeShaderObject2D->sendUniform1i("front", GLint(maximumIndex));
    dataItem->volumeShaderObject2D->sendUniform1f("maximumGradient", GLfloat(volume->getMaximumGradient()));
    dataItem->volumeShaderObject2D->sendUniform3f("slicePlaneNormal", GLfloat(slicePlaneNormal->getX()), GLfloat(
            slicePlaneNormal->getY()), GLfloat(slicePlaneNormal->getZ()));
    if (volume->getNumberOfComponents() != 0) {
        dataItem->volumeShaderObject2D->sendUniform1f("redScale", GLfloat(redScale));
        dataItem->volumeShaderObject2D->sendUniform1f("greenScale", GLfloat(greenScale));
        dataItem->volumeShaderObject2D->sendUniform1f("blueScale", GLfloat(blueScale));
    }
    drawBricks(glContextData, dataItem->cFrustum, dataItem->volumeShaderObject2D, brickList, point, maximumIndex, minimumIndex,
            modelviewInverse);
    dataItem->volumeShaderObject2D->end();
} // end volumeShader2D()

/*
 * volumeShaderFocusAndContext
 *
 * parameter glContextData - GLContextData &
 * parameter dataItem - DataItem *
 * parameter brickList - const std::vector<ComparableBrick> &
 * parameter point - float *
 * parameter maximumIndex - int
 * parameter minimumIndex - int
 * parameter slicePlaneNormal - Vector4 *
 * parameter modelviewInverse - Matrix4x4 *
 */
void Scene::volumeShaderFocusAndContext(GLContextData & glContextData, DataItem * dataItem,
        const std::vector<ComparableBrick> & brickList, float * point, int maximumIndex, int minimumIndex,
        Vector4 * slicePlaneNormal, Matrix4x4 * modelviewInverse) const {
    dataItem->volumeShaderObjectFocusAndContext->begin();
    dataItem->volumeShaderObjectFocusAndContext->sendUniform1i("colormap1D", 1);
    dataItem->volumeShaderObjectFocusAndContext->sendUniform1iv("seq", 64, sequenceIndices);
    dataItem->volumeShaderObjectFocusAndContext->sendUniform1iv("edge", 48, edgeIndices);
    dataItem->volumeShaderObjectFocusAndContext->sendUniform1i("front", GLint(maximumIndex));
    dataItem->volumeShaderObjectFocusAndContext->sendUniform3f("slicePlaneNormal", GLfloat(slicePlaneNormal->getX()), GLfloat(
            slicePlaneNormal->getY()), GLfloat(slicePlaneNormal->getZ()));
    dataItem->volumeShaderObjectFocusAndContext->sendUniform3f("planeNormal", GLfloat(planeNormal[0]), GLfloat(planeNormal[1]), GLfloat(
            planeNormal[2]));
    dataItem->volumeShaderObjectFocusAndContext->sendUniform1f("planeOffset", GLfloat(planeOffset));
    drawBricks(glContextData, dataItem->cFrustum, dataItem->volumeShaderObjectWithLighting, brickList, point, maximumIndex,
            minimumIndex, modelviewInverse);
    dataItem->volumeShaderObjectFocusAndContext->end();
} // end volumeShaderFocusAndContext()

/*
 * volumeShaderWithLighting
 *
 * parameter glContextData - GLContextData &
 * parameter dataItem - DataItem *
 * parameter brickList - const std::vector<ComparableBrick> &
 * parameter point - float *
 * parameter maximumIndex - int
 * parameter minimumIndex - int
 * parameter slicePlaneNormal - Vector4 *
 * parameter modelviewInverse - Matrix4x4 *
 */
void Scene::volumeShaderWithLighting(GLContextData & glContextData, DataItem * dataItem,
        const std::vector<ComparableBrick> & brickList, float * point, int maximumIndex, int minimumIndex,
        Vector4 * slicePlaneNormal, Matrix4x4 * modelviewInverse) const {
    dataItem->volumeShaderObjectWithLighting->begin();
    dataItem->volumeShaderObjectWithLighting->sendUniform1i("colormap1D", 1);
    dataItem->volumeShaderObjectWithLighting->sendUniform1iv("seq", 64, sequenceIndices);
    dataItem->volumeShaderObjectWithLighting->sendUniform1iv("edge", 48, edgeIndices);
    dataItem->volumeShaderObjectWithLighting->sendUniform1i("front", GLint(maximumIndex));
    dataItem->volumeShaderObjectWithLighting->sendUniform3f("slicePlaneNormal", GLfloat(slicePlaneNormal->getX()), GLfloat(
            slicePlaneNormal->getY()), GLfloat(slicePlaneNormal->getZ()));
    if (volume->getNumberOfComponents() != 0) {
        dataItem->volumeShaderObjectWithLighting->sendUniform1f("redScale", GLfloat(redScale));
        dataItem->volumeShaderObjectWithLighting->sendUniform1f("greenScale", GLfloat(greenScale));
        dataItem->volumeShaderObjectWithLighting->sendUniform1f("blueScale", GLfloat(blueScale));
    }
    dataItem->volumeShaderObjectWithLighting->sendUniform4f("eyePosition", GLfloat(point[0]), GLfloat(point[1]), GLfloat(point[2]),
            GLfloat(1.0f));
    dataItem->volumeShaderObjectWithLighting->sendUniform4f("lightPosition", GLfloat(lightPosition[0]), GLfloat(lightPosition[1]),
            GLfloat(lightPosition[2]), GLfloat(1.0f));
    dataItem->volumeShaderObjectWithLighting->sendUniform3f("ambientColor", GLfloat(ambientColor[0]), GLfloat(ambientColor[1]),
            GLfloat(ambientColor[2]));
    dataItem->volumeShaderObjectWithLighting->sendUniform3f("diffuseColor", GLfloat(diffuseColor[0]), GLfloat(diffuseColor[1]),
            GLfloat(diffuseColor[2]));
    dataItem->volumeShaderObjectWithLighting->sendUniform3f("specularColor", GLfloat(specularColor[0]), GLfloat(specularColor[1]),
            GLfloat(specularColor[2]));
    dataItem->volumeShaderObjectWithLighting->sendUniform3f("kAmbient", GLfloat(kAmbient[0]), GLfloat(kAmbient[1]), GLfloat(
            kAmbient[2]));
    dataItem->volumeShaderObjectWithLighting->sendUniform3f("kDiffuse", GLfloat(kDiffuse[0]), GLfloat(kDiffuse[1]), GLfloat(
            kDiffuse[2]));
    dataItem->volumeShaderObjectWithLighting->sendUniform3f("kSpecular", GLfloat(kSpecular[0]), GLfloat(kSpecular[1]), GLfloat(
            kSpecular[2]));
    dataItem->volumeShaderObjectWithLighting->sendUniform1f("shininess", GLfloat(shininess));
    dataItem->volumeShaderObjectWithLighting->sendUniform1f("normalContribution", GLfloat(normalContribution));
    dataItem->volumeShaderObjectWithLighting->sendUniform1f("toneContribution", GLfloat(toneContribution));
    dataItem->volumeShaderObjectWithLighting->sendUniform3f("coolColor", GLfloat(coolColor[0]), GLfloat(coolColor[1]), GLfloat(
            coolColor[2]));
    dataItem->volumeShaderObjectWithLighting->sendUniform3f("warmColor", GLfloat(warmColor[0]), GLfloat(warmColor[1]), GLfloat(
            warmColor[2]));
    dataItem->volumeShaderObjectWithLighting->sendUniform1f("toonContribution", GLfloat(toonContribution));
    dataItem->volumeShaderObjectWithLighting->sendUniform3f("toonColor", GLfloat(toonColor[0]), GLfloat(toonColor[1]), GLfloat(
            toonColor[2]));
    dataItem->volumeShaderObjectWithLighting->sendUniform1f("boundaryContribution", GLfloat(boundaryContribution));
    dataItem->volumeShaderObjectWithLighting->sendUniform1f("boundaryExponent", GLfloat(boundaryExponent));
    dataItem->volumeShaderObjectWithLighting->sendUniform1f("silhouetteContribution", GLfloat(silhouetteContribution));
    dataItem->volumeShaderObjectWithLighting->sendUniform1f("silhouetteExponent", GLfloat(silhouetteExponent));
    dataItem->volumeShaderObjectWithLighting->sendUniform1f("edgeContribution", GLfloat(edgeContribution));
    dataItem->volumeShaderObjectWithLighting->sendUniform1f("edgeExponent", GLfloat(edgeExponent));
    dataItem->volumeShaderObjectWithLighting->sendUniform1f("edgeThreshold", GLfloat(edgeThreshold));
    drawBricks(glContextData, dataItem->cFrustum, dataItem->volumeShaderObjectWithLighting, brickList, point, maximumIndex,
            minimumIndex, modelviewInverse);
    dataItem->volumeShaderObjectWithLighting->end();
} // end volumeShaderWithLighting()

/*
 * volumeShader2DWithLighting
 *
 * parameter glContextData - GLContextData &
 * parameter dataItem - DataItem *
 * parameter brickList - const std::vector<ComparableBrick> &
 * parameter point - float *
 * parameter maximumIndex - int
 * parameter minimumIndex - int
 * parameter slicePlaneNormal - Vector4 *
 * parameter modelviewInverse - Matrix4x4 *
 */
void Scene::volumeShader2DWithLighting(GLContextData & glContextData, DataItem * dataItem,
        const std::vector<ComparableBrick> & brickList, float * point, int maximumIndex, int minimumIndex,
        Vector4 * slicePlaneNormal, Matrix4x4 * modelviewInverse) const {
    dataItem->volumeShaderObject2DWithLighting->begin();
    dataItem->volumeShaderObject2DWithLighting->sendUniform1i("colormap2D", 1);
    dataItem->volumeShaderObject2DWithLighting->sendUniform1iv("seq", 64, sequenceIndices);
    dataItem->volumeShaderObject2DWithLighting->sendUniform1iv("edge", 48, edgeIndices);
    dataItem->volumeShaderObject2DWithLighting->sendUniform1i("front", GLint(maximumIndex));
    dataItem->volumeShaderObject2DWithLighting->sendUniform1f("maximumGradient", GLfloat(volume->getMaximumGradient()));
    dataItem->volumeShaderObject2DWithLighting->sendUniform3f("slicePlaneNormal", GLfloat(slicePlaneNormal->getX()), GLfloat(
            slicePlaneNormal->getY()), GLfloat(slicePlaneNormal->getZ()));
    dataItem->volumeShaderObject2DWithLighting->sendUniform4f("eyePosition", GLfloat(point[0]), GLfloat(point[1]),
            GLfloat(point[2]), GLfloat(1.0f));
    dataItem->volumeShaderObject2DWithLighting->sendUniform4f("lightPosition", GLfloat(lightPosition[0]),
            GLfloat(lightPosition[1]), GLfloat(lightPosition[2]), GLfloat(1.0f));
    dataItem->volumeShaderObject2DWithLighting->sendUniform3f("ambientColor", GLfloat(ambientColor[0]), GLfloat(ambientColor[1]),
            GLfloat(ambientColor[2]));
    dataItem->volumeShaderObject2DWithLighting->sendUniform3f("diffuseColor", GLfloat(diffuseColor[0]), GLfloat(diffuseColor[1]),
            GLfloat(diffuseColor[2]));
    dataItem->volumeShaderObject2DWithLighting->sendUniform3f("specularColor", GLfloat(specularColor[0]),
            GLfloat(specularColor[1]), GLfloat(specularColor[2]));
    dataItem->volumeShaderObject2DWithLighting->sendUniform3f("kAmbient", GLfloat(kAmbient[0]), GLfloat(kAmbient[1]), GLfloat(
            kAmbient[2]));
    dataItem->volumeShaderObject2DWithLighting->sendUniform3f("kDiffuse", GLfloat(kDiffuse[0]), GLfloat(kDiffuse[1]), GLfloat(
            kDiffuse[2]));
    dataItem->volumeShaderObject2DWithLighting->sendUniform3f("kSpecular", GLfloat(kSpecular[0]), GLfloat(kSpecular[1]), GLfloat(
            kSpecular[2]));
    dataItem->volumeShaderObject2DWithLighting->sendUniform1f("shininess", GLfloat(shininess));
    dataItem->volumeShaderObject2DWithLighting->sendUniform1f("normalContribution", GLfloat(normalContribution));
    dataItem->volumeShaderObject2DWithLighting->sendUniform1f("toneContribution", GLfloat(toneContribution));
    dataItem->volumeShaderObject2DWithLighting->sendUniform3f("coolColor", GLfloat(coolColor[0]), GLfloat(coolColor[1]), GLfloat(
            coolColor[2]));
    dataItem->volumeShaderObject2DWithLighting->sendUniform3f("warmColor", GLfloat(warmColor[0]), GLfloat(warmColor[1]), GLfloat(
            warmColor[2]));
    dataItem->volumeShaderObject2DWithLighting->sendUniform1f("toonContribution", GLfloat(toonContribution));
    dataItem->volumeShaderObject2DWithLighting->sendUniform3f("toonColor", GLfloat(toonColor[0]), GLfloat(toonColor[1]), GLfloat(
            toonColor[2]));
    dataItem->volumeShaderObject2DWithLighting->sendUniform1f("boundaryContribution", GLfloat(boundaryContribution));
    dataItem->volumeShaderObject2DWithLighting->sendUniform1f("boundaryExponent", GLfloat(boundaryExponent));
    dataItem->volumeShaderObject2DWithLighting->sendUniform1f("silhouetteContribution", GLfloat(silhouetteContribution));
    dataItem->volumeShaderObject2DWithLighting->sendUniform1f("silhouetteExponent", GLfloat(silhouetteExponent));
    dataItem->volumeShaderObject2DWithLighting->sendUniform1f("edgeContribution", GLfloat(edgeContribution));
    dataItem->volumeShaderObject2DWithLighting->sendUniform1f("edgeExponent", GLfloat(edgeExponent));
    dataItem->volumeShaderObject2DWithLighting->sendUniform1f("edgeThreshold", GLfloat(edgeThreshold));
    drawBricks(glContextData, dataItem->cFrustum, dataItem->volumeShaderObject2DWithLighting, brickList, point, maximumIndex,
            minimumIndex, modelviewInverse);
    dataItem->volumeShaderObject2DWithLighting->end();
} // end volumeShader2DWithLighting()

/*
 * volumeShaderPreIntegrated
 *
 * parameter glContextData - GLContextData &
 * parameter dataItem - DataItem *
 * parameter brickList - const std::vector<ComparableBrick> &
 * parameter point - float *
 * parameter maximumIndex - int
 * parameter minimumIndex - int
 * parameter slicePlaneNormal - Vector4 *
 * parameter modelviewInverse - Matrix4x4 *
 */
void Scene::volumeShaderPreIntegrated(GLContextData & glContextData, DataItem * dataItem,
        const std::vector<ComparableBrick> & brickList, float * point, int maximumIndex, int minimumIndex,
        Vector4 * slicePlaneNormal, Matrix4x4 * modelviewInverse) const {
    dataItem->preIntegratedVolumeShaderObject->begin();
    dataItem->preIntegratedVolumeShaderObject->sendUniform1i("colormap2D", 1);
    dataItem->preIntegratedVolumeShaderObject->sendUniform1iv("seq", 64, sequenceIndices);
    dataItem->preIntegratedVolumeShaderObject->sendUniform1iv("edge", 48, edgeIndices);
    dataItem->preIntegratedVolumeShaderObject->sendUniform1i("front", GLint(maximumIndex));
    dataItem->preIntegratedVolumeShaderObject->sendUniform3f("slicePlaneNormal", GLfloat(slicePlaneNormal->getX()), GLfloat(
            slicePlaneNormal->getY()), GLfloat(slicePlaneNormal->getZ()));
    if (volume->getNumberOfComponents() != 0) {
        dataItem->volumeShaderObject->sendUniform1f("redScale", GLfloat(redScale));
        dataItem->volumeShaderObject->sendUniform1f("greenScale", GLfloat(greenScale));
        dataItem->volumeShaderObject->sendUniform1f("blueScale", GLfloat(blueScale));
    }
    drawBricks(glContextData, dataItem->cFrustum, dataItem->preIntegratedVolumeShaderObject, brickList, point, maximumIndex,
            minimumIndex, modelviewInverse);
    dataItem->preIntegratedVolumeShaderObject->end();
} // end volumeShaderPreIntegrated()

/*
 * volumeShaderPreIntegratedWithLighting
 *
 * parameter glContextData - GLContextData &
 * parameter dataItem - DataItem *
 * parameter brickList - const std::vector<ComparableBrick> &
 * parameter point - float *
 * parameter maximumIndex - int
 * parameter minimumIndex - int
 * parameter slicePlaneNormal - Vector4 *
 * parameter modelviewInverse - Matrix4x4 *
 */
void Scene::volumeShaderPreIntegratedWithLighting(GLContextData & glContextData, DataItem * dataItem, const std::vector<
        ComparableBrick> & brickList, float * point, int maximumIndex, int minimumIndex, Vector4 * slicePlaneNormal,
        Matrix4x4 * modelviewInverse) const {
    dataItem->preIntegratedVolumeShaderObjectWithLighting->begin();
    dataItem->preIntegratedVolumeShaderObjectWithLighting->sendUniform1i("colormap2D", 1);
    dataItem->preIntegratedVolumeShaderObjectWithLighting->sendUniform1iv("seq", 64, sequenceIndices);
    dataItem->preIntegratedVolumeShaderObjectWithLighting->sendUniform1iv("edge", 48, edgeIndices);
    dataItem->preIntegratedVolumeShaderObjectWithLighting->sendUniform1i("front", GLint(maximumIndex));
    dataItem->preIntegratedVolumeShaderObjectWithLighting->sendUniform3f("slicePlaneNormal", GLfloat(slicePlaneNormal->getX()),
            GLfloat(slicePlaneNormal->getY()), GLfloat(slicePlaneNormal->getZ()));
    if (volume->getNumberOfComponents() != 0) {
        dataItem->preIntegratedVolumeShaderObjectWithLighting->sendUniform1f("redScale", GLfloat(redScale));
        dataItem->preIntegratedVolumeShaderObjectWithLighting->sendUniform1f("greenScale", GLfloat(greenScale));
        dataItem->preIntegratedVolumeShaderObjectWithLighting->sendUniform1f("blueScale", GLfloat(blueScale));
    }
    dataItem->preIntegratedVolumeShaderObjectWithLighting->sendUniform4f("eyePosition", GLfloat(point[0]), GLfloat(point[1]),
            GLfloat(point[2]), GLfloat(1.0f));
    dataItem->preIntegratedVolumeShaderObjectWithLighting->sendUniform4f("lightPosition", GLfloat(lightPosition[0]), GLfloat(
            lightPosition[1]), GLfloat(lightPosition[2]), GLfloat(1.0f));
    dataItem->preIntegratedVolumeShaderObjectWithLighting->sendUniform3f("ambientColor", GLfloat(ambientColor[0]), GLfloat(
            ambientColor[1]), GLfloat(ambientColor[2]));
    dataItem->preIntegratedVolumeShaderObjectWithLighting->sendUniform3f("diffuseColor", GLfloat(diffuseColor[0]), GLfloat(
            diffuseColor[1]), GLfloat(diffuseColor[2]));
    dataItem->preIntegratedVolumeShaderObjectWithLighting->sendUniform3f("specularColor", GLfloat(specularColor[0]), GLfloat(
            specularColor[1]), GLfloat(specularColor[2]));
    dataItem->preIntegratedVolumeShaderObjectWithLighting->sendUniform3f("kAmbient", GLfloat(kAmbient[0]), GLfloat(kAmbient[1]),
            GLfloat(kAmbient[2]));
    dataItem->preIntegratedVolumeShaderObjectWithLighting->sendUniform3f("kDiffuse", GLfloat(kDiffuse[0]), GLfloat(kDiffuse[1]),
            GLfloat(kDiffuse[2]));
    dataItem->preIntegratedVolumeShaderObjectWithLighting->sendUniform3f("kSpecular", GLfloat(kSpecular[0]), GLfloat(kSpecular[1]),
            GLfloat(kSpecular[2]));
    dataItem->preIntegratedVolumeShaderObjectWithLighting->sendUniform1f("shininess", GLfloat(shininess));
    dataItem->preIntegratedVolumeShaderObjectWithLighting->sendUniform1f("normalContribution", GLfloat(normalContribution));
    dataItem->preIntegratedVolumeShaderObjectWithLighting->sendUniform1f("toneContribution", GLfloat(toneContribution));
    dataItem->preIntegratedVolumeShaderObjectWithLighting->sendUniform3f("coolColor", GLfloat(coolColor[0]), GLfloat(coolColor[1]),
            GLfloat(coolColor[2]));
    dataItem->preIntegratedVolumeShaderObjectWithLighting->sendUniform3f("warmColor", GLfloat(warmColor[0]), GLfloat(warmColor[1]),
            GLfloat(warmColor[2]));
    dataItem->preIntegratedVolumeShaderObjectWithLighting->sendUniform1f("toonContribution", GLfloat(toonContribution));
    dataItem->preIntegratedVolumeShaderObjectWithLighting->sendUniform3f("toonColor", GLfloat(toonColor[0]), GLfloat(toonColor[1]),
            GLfloat(toonColor[2]));
    dataItem->preIntegratedVolumeShaderObjectWithLighting->sendUniform1f("boundaryContribution", GLfloat(boundaryContribution));
    dataItem->preIntegratedVolumeShaderObjectWithLighting->sendUniform1f("boundaryExponent", GLfloat(boundaryExponent));
    dataItem->preIntegratedVolumeShaderObjectWithLighting->sendUniform1f("silhouetteContribution", GLfloat(silhouetteContribution));
    dataItem->preIntegratedVolumeShaderObjectWithLighting->sendUniform1f("silhouetteExponent", GLfloat(silhouetteExponent));
    dataItem->preIntegratedVolumeShaderObjectWithLighting->sendUniform1f("edgeContribution", GLfloat(edgeContribution));
    dataItem->preIntegratedVolumeShaderObjectWithLighting->sendUniform1f("edgeExponent", GLfloat(edgeExponent));
    dataItem->preIntegratedVolumeShaderObjectWithLighting->sendUniform1f("edgeThreshold", GLfloat(edgeThreshold));
    drawBricks(glContextData, dataItem->cFrustum, dataItem->preIntegratedVolumeShaderObjectWithLighting, brickList, point,
            maximumIndex, minimumIndex, modelviewInverse);
    dataItem->preIntegratedVolumeShaderObjectWithLighting->end();
} // end volumeShaderPreIntegratedWithLighting()
