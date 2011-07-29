/*
 * Brick.cpp - Methods for Brick class.
 *
 * Author: Patrick O'Leary
 * Created: December 31, 2007
 * Copyright 2007. All rights reserved.
 */
#include <queue>

#include <DATA/Volume.h>
#include <DATASTRUCTURE/Brick.h>
#include <DATASTRUCTURE/BoundingBox.h>
#include <DATASTRUCTURE/BoundingSphere.h>
#include <DATASTRUCTURE/ComparableOcNode.h>
#include <DATASTRUCTURE/OcNode.h>
#include <DATASTRUCTURE/OcTree.h>
#include <GRAPHIC/CFrustum.h>
#include <MATH/Matrix4x4.h>
#include <SHADER/ShaderObject.h>

Brick::DataItem::DataItem(void) {
    texture3DName = 0;
    downSamplingTexture3DName = 0;
}

Brick::DataItem::~DataItem(void) {
    texture3DName = 0;
    downSamplingTexture3DName = 0;
}

Brick::Brick(void) :
    borderSize(1), column(0), depth(0), downSamplingChanged(false), empty(false), height(0), interactive(false),
            maximumPriorityQueueSize(0), ocTree(NULL), row(0), showBricks(false), showXSlice(false), showYSlice(false), showZSlice(
                    false), slab(0), sliceFactor(1.5), slicingScale(2.0), volume(NULL), volumeBox(NULL), volumeSphere(NULL), width(
                    0), x(0), xSlice(0), xStep(2), y(0), ySlice(0), yStep(2), z(0), zSlice(0), zStep(2) {
}

Brick::~Brick(void) {
    volume = 0;
    delete ocTree;
}

/*
 * adjustTextureCoordinates
 */
void Brick::adjustTextureCoordinates(void) {
    float dx = 0.0f; // 1.0f/float(width + 2 * borderSize)/2.0f;
    float dy = 0.0f; // 1.0f/float(height + 2 * borderSize)/2.0f;
    float dz = 0.0f; // 1.0f/float(depth + 2 * borderSize)/2.0f;
    ocTree->getRoot()->adjustTextureCoordinatesTraversal(dx, dy, dz);
} // end adjustTextureCoordinates()

/*
 * create3DTexture
 *
 * return - GLuint
 */
GLuint Brick::create3DTexture(void) const {
    unsigned char* voxels = new unsigned char[(width) * (height) * (depth)];
    int w = volume->getWidth();
    int h = volume->getHeight();
    for (int k = 0; k < depth; k++) {
        for (int j = 0; j < height; j++) {
            for (int i = 0; i < width; i++) {
                voxels[(k * ((width) * (height))) + (j *( width)) +i] = volume->getVoxel(((slab + (k)) * (w * h)) + ((row + (j))
                        * (w)) + (column + (i)));
            } // end for
        } // end for
    } // end for
    GLuint name;
    glEnable(GL_TEXTURE_3D);
    glGenTextures(1, &name);
    glBindTexture(GL_TEXTURE_3D, name);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // GL_CLAMP_TO_BORDER
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage3D(GL_TEXTURE_3D, 0, GL_LUMINANCE_ALPHA, width, height, depth, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, voxels);
    glBindTexture(GL_TEXTURE_3D, 0);
    glDisable(GL_TEXTURE_3D);
    delete[] voxels;
    return name;
} // end create3DTexture()

/*
 * create3DTexture
 *
 * parameter bordersize - int
 * return - GLuint
 */
GLuint Brick::create3DTexture(int bordersize) const {
    unsigned char *voxels = new unsigned char[(width+(2*bordersize))*
                                              (height+(2*bordersize))*
                                              (depth+(2*bordersize))];

    fill3DTexture (bordersize, voxels);

    GLuint name;
    glEnable(GL_TEXTURE_3D);
    glGenTextures(1, &name);
    glBindTexture(GL_TEXTURE_3D, name);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER); // GL_CLAMP_TO_BORDER GL_CLAMP_TO_EDGE GL_CLAMP
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // GL_LINEAR GL_NEAREST
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage3D(GL_TEXTURE_3D, 0, GL_LUMINANCE_ALPHA,
            (width+(2*bordersize)), (height+(2*bordersize)),
            (depth+(2*bordersize)), bordersize,
            GL_LUMINANCE, GL_UNSIGNED_BYTE, voxels);
    glBindTexture(GL_TEXTURE_3D, 0);
    glDisable(GL_TEXTURE_3D);
    delete[] voxels;
    return name;
} // end create3DTexture()

/*
 * createDownSampling3DTexture
 *
 * return - GLuint
 */
GLuint Brick::createDownSampling3DTexture(void) const {
    unsigned char* voxels = new unsigned char[width * height * depth];
    int w = volume->getWidth();
    int h = volume->getHeight();
    for (int k = 0; k < depth; k++) {
        for (int j = 0; j < height; j++) {
            for (int i = 0; i < width; i++) {
                voxels[(k * ((width) * (height))) + (j *( width)) +i] = volume->getVoxel(((slab + (k)) * (w * h)) + ((row + (j))
                        * (w)) + (column + (i)));
            } // end for
        } // end for
    } // end for
    int newWidth = int(width / xStep);
    int newHeight = int(height / yStep);
    int newDepth = int(depth / zStep);
    unsigned char* downSamplingVoxels = new unsigned char[newWidth * newHeight * newDepth];
    int ktmp;
    int jtmp;
    int itmp;
    for (int k = 0; k < newDepth; k++) {
        k == 0 ? ktmp = 0 : ktmp = k * zStep - 1;
        for (int j = 0; j < newHeight; j++) {
            j == 0 ? jtmp = 0 : jtmp = j * yStep - 1;
            for (int i = 0; i < newWidth; i++) {
                i == 0 ? itmp = 0 : itmp = i * xStep - 1;
                downSamplingVoxels[(k * ((newWidth) * (newHeight))) + (j *( newWidth)) +i] = voxels[(ktmp * ((width) * (height)))
                        + (jtmp *( width)) +itmp];
            } // end for
        } // end for
    } // end for
    delete[] voxels;
    GLuint name;
    glEnable(GL_TEXTURE_3D);
    glGenTextures(1, &name);
    glBindTexture(GL_TEXTURE_3D, name);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // GL_CLAMP_TO_BORDER
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage3D(GL_TEXTURE_3D, 0, GL_LUMINANCE_ALPHA, newWidth, newHeight, newDepth, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE,
            downSamplingVoxels);
    glBindTexture(GL_TEXTURE_3D, 0);
    glDisable(GL_TEXTURE_3D);
    delete[] downSamplingVoxels;
    return name;
} // end createDownSampling3DTexture()

/*
 * createDownSamplingMultiComponent3DTexture
 *
 * return - GLuint
 */
GLuint Brick::createDownSamplingMultiComponent3DTexture(void) const {
    unsigned char* voxels = new unsigned char[3 * (width) * (height) * (depth)];
    int w = volume->getWidth();
    int h = volume->getHeight();
    if (volume->hasRedByteVoxelsBase()) {
        for (int k = 0; k < depth; k++) { // red
            for (int j = 0; j < height; j++) {
                for (int i = 0; i < width; i++) {
                    voxels[3 * ((k * ((width) * (height))) + (j *( width)) +i) + 0] = volume->getRedVoxel(((slab + (k)) * (w * h))
                            + ((row + (j)) * (w)) + (column + (i)));
                } // end for
            } // end for
        } // end for
    } else {
        for (int k = 0; k < depth; k++) { // red
            for (int j = 0; j < height; j++) {
                for (int i = 0; i < width; i++) {
                    voxels[3 * ((k * ((width) * (height))) + (j *( width)) +i) + 0] = 0;
                } // end for
            } // end for
        } // end for
    }
    if (volume->hasGreenByteVoxelsBase()) {
        for (int k = 0; k < depth; k++) { // red
            for (int j = 0; j < height; j++) {
                for (int i = 0; i < width; i++) {
                    voxels[3 * ((k * ((width) * (height))) + (j *( width)) +i) + 1] = volume->getGreenVoxel(
                            ((slab + (k)) * (w * h)) + ((row + (j)) * (w)) + (column + (i)));
                } // end for
            } // end for
        } // end for
    } else {
        for (int k = 0; k < depth; k++) { // red
            for (int j = 0; j < height; j++) {
                for (int i = 0; i < width; i++) {
                    voxels[3 * ((k * ((width) * (height))) + (j *( width)) +i) + 1] = 0;
                } // end for
            } // end for
        } // end for
    }
    if (volume->hasBlueByteVoxelsBase()) {
        for (int k = 0; k < depth; k++) { // red
            for (int j = 0; j < height; j++) {
                for (int i = 0; i < width; i++) {
                    voxels[3 * ((k * ((width) * (height))) + (j *( width)) +i) + 2] = volume->getBlueVoxel(((slab + (k)) * (w * h))
                            + ((row + (j)) * (w)) + (column + (i)));
                } // end for
            } // end for
        } // end for
    } else {
        for (int k = 0; k < depth; k++) { // red
            for (int j = 0; j < height; j++) {
                for (int i = 0; i < width; i++) {
                    voxels[3 * ((k * ((width) * (height))) + (j *( width)) +i) + 2] = 0;
                } // end for
            } // end for
        } // end for
    }
    int newWidth = int(width / xStep);
    int newHeight = int(height / yStep);
    int newDepth = int(depth / zStep);
    unsigned char* downSamplingVoxels = new unsigned char[3 * newWidth * newHeight * newDepth];
    int ktmp;
    int jtmp;
    int itmp;
    for (int k = 0; k < newDepth; k++) {
        k == 0 ? ktmp = 0 : ktmp = k * zStep - 1;
        for (int j = 0; j < newHeight; j++) {
            j == 0 ? jtmp = 0 : jtmp = j * yStep - 1;
            for (int i = 0; i < newWidth; i++) {
                i == 0 ? itmp = 0 : itmp = i * xStep - 1;
                downSamplingVoxels[3 * ((k * ((newWidth) * (newHeight))) + (j *( newWidth)) +i) + 0] = voxels[3 * ((ktmp * ((width)
                        * (height))) + (jtmp *( width)) +itmp) + 0];
                downSamplingVoxels[3 * ((k * ((newWidth) * (newHeight))) + (j *( newWidth)) +i) + 1] = voxels[3 * ((ktmp * ((width)
                        * (height))) + (jtmp *( width)) +itmp) + 1];
                downSamplingVoxels[3 * ((k * ((newWidth) * (newHeight))) + (j *( newWidth)) +i) + 2] = voxels[3 * ((ktmp * ((width)
                        * (height))) + (jtmp *( width)) +itmp) + 2];
            } // end for
        } // end for
    } // end for
    delete[] voxels;
    GLuint name;
    glEnable(GL_TEXTURE_3D);
    glGenTextures(1, &name);
    glBindTexture(GL_TEXTURE_3D, name);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // GL_CLAMP_TO_BORDER
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage3D(GL_TEXTURE_3D, 0, GL_RGB, newWidth, newHeight, newDepth, 0, GL_RGB, GL_UNSIGNED_BYTE, downSamplingVoxels);
    glBindTexture(GL_TEXTURE_3D, 0);
    glDisable(GL_TEXTURE_3D);
    delete[] downSamplingVoxels;
    return name;
} // end createDownSamplingMultiComponent3DTexture()

/*
 * createMultiComponent3DTexture
 *
 * return - GLuint
 */
GLuint Brick::createMultiComponent3DTexture(void) const {
    unsigned char* voxels = new unsigned char[3 * (width) * (height) * (depth)];
    int w = volume->getWidth();
    int h = volume->getHeight();
    if (volume->hasRedByteVoxelsBase()) {
        for (int k = 0; k < depth; k++) { // red
            for (int j = 0; j < height; j++) {
                for (int i = 0; i < width; i++) {
                    voxels[3 * ((k * ((width) * (height))) + (j *( width)) +i) + 0] = volume->getRedVoxel(((slab + (k)) * (w * h))
                            + ((row + (j)) * (w)) + (column + (i)));
                } // end for
            } // end for
        } // end for
    } else {
        for (int k = 0; k < depth; k++) { // red
            for (int j = 0; j < height; j++) {
                for (int i = 0; i < width; i++) {
                    voxels[3 * ((k * ((width) * (height))) + (j *( width)) +i) + 0] = 0;
                } // end for
            } // end for
        } // end for
    }
    if (volume->hasGreenByteVoxelsBase()) {
        for (int k = 0; k < depth; k++) { // red
            for (int j = 0; j < height; j++) {
                for (int i = 0; i < width; i++) {
                    voxels[3 * ((k * ((width) * (height))) + (j *( width)) +i) + 1] = volume->getGreenVoxel(
                            ((slab + (k)) * (w * h)) + ((row + (j)) * (w)) + (column + (i)));
                } // end for
            } // end for
        } // end for
    } else {
        for (int k = 0; k < depth; k++) { // red
            for (int j = 0; j < height; j++) {
                for (int i = 0; i < width; i++) {
                    voxels[3 * ((k * ((width) * (height))) + (j *( width)) +i) + 1] = 0;
                } // end for
            } // end for
        } // end for
    }
    if (volume->hasBlueByteVoxelsBase()) {
        for (int k = 0; k < depth; k++) { // red
            for (int j = 0; j < height; j++) {
                for (int i = 0; i < width; i++) {
                    voxels[3 * ((k * ((width) * (height))) + (j *( width)) +i) + 2] = volume->getBlueVoxel(((slab + (k)) * (w * h))
                            + ((row + (j)) * (w)) + (column + (i)));
                } // end for
            } // end for
        } // end for
    } else {
        for (int k = 0; k < depth; k++) { // red
            for (int j = 0; j < height; j++) {
                for (int i = 0; i < width; i++) {
                    voxels[3 * ((k * ((width) * (height))) + (j *( width)) +i) + 2] = 0;
                } // end for
            } // end for
        } // end for
    }
    GLuint name;
    glEnable(GL_TEXTURE_3D);
    glGenTextures(1, &name);
    glBindTexture(GL_TEXTURE_3D, name);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // GL_CLAMP_TO_BORDER
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage3D(GL_TEXTURE_3D, 0, GL_RGB, width, height, depth, 0, GL_RGB, GL_UNSIGNED_BYTE, voxels);
    glBindTexture(GL_TEXTURE_3D, 0);
    glDisable(GL_TEXTURE_3D);
    delete[] voxels;
    return name;
} // end createMultiComponent3DTexture()

/*
 * createMultiComponent3DTexture
 *
 * parameter bordersize - int
 * return - GLuint
 */
GLuint Brick::createMultiComponent3DTexture(int bordersize) const {
    unsigned char *voxels = new unsigned char[3*(width+(2*bordersize))*
                                                (height+(2*bordersize))*
                                                (depth+(2*bordersize))];

    if (volume->hasRedByteVoxelsBase())
    {
        fillMultiComponent3DTexture (bordersize, voxels, 0);
    }
    if (volume->hasGreenByteVoxelsBase())
    {
        fillMultiComponent3DTexture (bordersize, voxels, 1);
    }
    if (volume->hasBlueByteVoxelsBase())
    {
        fillMultiComponent3DTexture (bordersize, voxels, 2);
    }

    GLuint name;
    glEnable(GL_TEXTURE_3D);
    glGenTextures(1, &name);
    glBindTexture(GL_TEXTURE_3D, name);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // GL_CLAMP_TO_BORDER
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage3D(GL_TEXTURE_3D, 0, GL_RGB,
            (width+(2*bordersize)), (height+(2*bordersize)),
            (depth+(2*bordersize)), bordersize,
            GL_RGB, GL_UNSIGNED_BYTE, voxels);
    glBindTexture(GL_TEXTURE_3D, 0);
    glDisable(GL_TEXTURE_3D);
    delete[] voxels;
    return name;
} // end createMultiComponent3DTexture()

/*
 * determineOcNodesToDisplay
 */
void Brick::determineOcNodesToDisplay(void) {
    ComparableOcNode comparableOcNode;
    comparableOcNode.minimum(); // use the standard less operator<
    std::priority_queue<ComparableOcNode> maximumPriorityQueue;
    OcNode * ocNode = ocTree->getRoot();
    if (ocNode->getRatioOfVisibility() != 0.0f) {
        setEmpty(false);
        ocNode->setDisplayed(true);
        maximumPriorityQueue.push(ComparableOcNode(0, ocNode->getPercentageOfEmptyVolume(), ocNode));
        while (maximumPriorityQueue.size() < maximumPriorityQueueSize && !maximumPriorityQueue.empty()) {
            maximumPriorityQueue.pop();
            if (!ocNode->isLeaf()) {
                ocNode->setDisplayed(false);
                OcNode * child = ocNode->getChildren();
                for (int i = 0; i < 8; i++) {
                    if (child[i].getRatioOfVisibility() != 0.0f) {
                        child[i].setDisplayed(true);
                        maximumPriorityQueue.push(ComparableOcNode(i, child[i].getPercentageOfEmptyVolume(), &child[i]));
                    } else
                        child[i].setDisplayed(false);
                }
            }
            ocNode = ((const ComparableOcNode &) maximumPriorityQueue.top()).getOcNode();
        }
    } else {
        setEmpty(true);
        ocNode->setDisplayed(false);
    }
} // end determineOcNodesToDisplay()

/*
 * drawBrick
 */
void Brick::drawBrick(void) const {
    glDisable(GL_LIGHTING);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glBegin(GL_QUADS);
    // Front Face
    glNormal3f(0.0f, 0.0f, 1.0f);
    glVertex3f(volumeBox->getX(2), volumeBox->getY(2), volumeBox->getZ(2));
    glVertex3f(volumeBox->getX(3), volumeBox->getY(3), volumeBox->getZ(3));
    glVertex3f(volumeBox->getX(7), volumeBox->getY(7), volumeBox->getZ(7));
    glVertex3f(volumeBox->getX(6), volumeBox->getY(6), volumeBox->getZ(6));
    // Back Face
    glNormal3f(0.0f, 0.0f, -1.0f);
    glVertex3f(volumeBox->getX(0), volumeBox->getY(0), volumeBox->getZ(0));
    glVertex3f(volumeBox->getX(4), volumeBox->getY(4), volumeBox->getZ(4));
    glVertex3f(volumeBox->getX(5), volumeBox->getY(5), volumeBox->getZ(5));
    glVertex3f(volumeBox->getX(1), volumeBox->getY(1), volumeBox->getZ(1));
    // Top Face
    glNormal3f(0.0f, 1.0f, 0.0f);
    glVertex3f(volumeBox->getX(4), volumeBox->getY(4), volumeBox->getZ(4));
    glVertex3f(volumeBox->getX(6), volumeBox->getY(6), volumeBox->getZ(6));
    glVertex3f(volumeBox->getX(7), volumeBox->getY(7), volumeBox->getZ(7));
    glVertex3f(volumeBox->getX(5), volumeBox->getY(5), volumeBox->getZ(5));
    // Bottom Face
    glNormal3f(0.0f, -1.0f, 0.0f);
    glVertex3f(volumeBox->getX(0), volumeBox->getY(0), volumeBox->getZ(0));
    glVertex3f(volumeBox->getX(1), volumeBox->getY(1), volumeBox->getZ(1));
    glVertex3f(volumeBox->getX(5), volumeBox->getY(5), volumeBox->getZ(5));
    glVertex3f(volumeBox->getX(4), volumeBox->getY(4), volumeBox->getZ(4));
    // Right face
    glNormal3f(1.0f, 0.0f, 0.0f);
    glVertex3f(volumeBox->getX(1), volumeBox->getY(1), volumeBox->getZ(1));
    glVertex3f(volumeBox->getX(5), volumeBox->getY(5), volumeBox->getZ(5));
    glVertex3f(volumeBox->getX(7), volumeBox->getY(7), volumeBox->getZ(7));
    glVertex3f(volumeBox->getX(3), volumeBox->getY(3), volumeBox->getZ(3));
    // Left Face
    glNormal3f(-1.0f, 0.0f, 0.0f);
    glVertex3f(volumeBox->getX(0), volumeBox->getY(0), volumeBox->getZ(0));
    glVertex3f(volumeBox->getX(2), volumeBox->getY(2), volumeBox->getZ(2));
    glVertex3f(volumeBox->getX(6), volumeBox->getY(6), volumeBox->getZ(6));
    glVertex3f(volumeBox->getX(4), volumeBox->getY(4), volumeBox->getZ(4));
    glEnd();
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glEnable(GL_LIGHTING);
} // end drawBrick()

/*
 * drawGrid
 *
 * parameter cFrustum - const CFrustum &
 */
void Brick::drawGrid(const CFrustum & cFrustum) const {
    // test the sphere first
    /*
     switch (cFrustum->sphereLocation(volumeSphere->getCenter(), volumeSphere->getRadius())) {
     case FULL_OUTSIDE:
     return;
     case FULL_INSIDE:
     break;
     case PARTIAL_INSIDE:
     // check if the box is in view
     float* fMinimum = volumeBox->getMinimum(0);
     float* fMaximum = volumeBox->getMaximum(7);
     switch (cFrustum->rightParallelepipedLocation(fMinimum, fMaximum)) {
     case FULL_INSIDE:
     break;
     case FULL_OUTSIDE:
     return;
     }
     delete fMinimum;
     delete fMaximum;
     break;
     }
     */
    if (interactive) {
        if (ocTree->getShowOcNodes()) {
            glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
            ocTree->getRoot()->drawGrid();
        }
    } else
        ocTree->drawGrid(cFrustum);
    if (showBricks) {
        glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
        drawBrick();
    }
} // end drawGrid()

/*
 * drawSlices
 *
 * parameter glContextData - GLContextData &
 * parameter cFrustum - const CFrustum &
 * parameter shaderObject - ShaderObject*
 */
void Brick::drawSlices(GLContextData & glContextData, const CFrustum & cFrustum, ShaderObject* shaderObject) {
    DataItem * dataItem = glContextData.retrieveDataItem<DataItem> (this);
    if (downSamplingChanged) {
        glDeleteTextures(1, &dataItem->downSamplingTexture3DName);
        if (volume->getNumberOfComponents() != 0) {
            dataItem->downSamplingTexture3DName = createDownSamplingMultiComponent3DTexture();
        } else {
            dataItem->downSamplingTexture3DName = createDownSampling3DTexture();
        }
    }
    if (showXSlice || showYSlice || showZSlice) {
        glActiveTexture(GL_TEXTURE0);
        if (interactive)
            glBindTexture(GL_TEXTURE_3D, dataItem->downSamplingTexture3DName);
        else
            glBindTexture(GL_TEXTURE_3D, dataItem->texture3DName);
        shaderObject->sendUniform1i("volume", 0);
        if (showXSlice) {
            if (xSlice >= column && xSlice <= (column + width))
                drawXSlice(cFrustum);
        }
        if (showYSlice) {
            if (ySlice >= row && ySlice <= (row + height))
                drawYSlice(cFrustum);
        }
        if (showZSlice) {
            if (zSlice >= slab && zSlice <= (slab + depth))
                drawZSlice(cFrustum);
        }
        glBindTexture(GL_TEXTURE_3D, 0);
    }
} // end drawSlices()

/*
 * drawVolume
 *
 * parameter glContextData - GLContextData &
 * parameter cFrustum - const CFrustum &
 * parameter shaderObject - ShaderObject*
 * parameter point - float *
 * parameter minimumIndex - int
 * parameter maximumIndex - int
 * parameter modelviewInverse - const Matrix4x4 *
 */
void Brick::drawVolume(GLContextData & glContextData, const CFrustum & cFrustum, ShaderObject* shaderObject, float * point,
        int minimumIndex, int maximumIndex, const Matrix4x4 * modelviewInverse) {
    DataItem * dataItem = glContextData.retrieveDataItem<DataItem> (this);
    glActiveTexture(GL_TEXTURE0);
    if (interactive)
        glBindTexture(GL_TEXTURE_3D, dataItem->downSamplingTexture3DName);
    else
        glBindTexture(GL_TEXTURE_3D, dataItem->texture3DName);
    shaderObject->sendUniform1i("volume", 0);
    if (interactive) {
        ocTree->getRoot()->drawVolume(shaderObject, minimumIndex, maximumIndex, modelviewInverse);
    } else
        ocTree->drawVolume(cFrustum, shaderObject, point, minimumIndex, maximumIndex, modelviewInverse);
    glBindTexture(GL_TEXTURE_3D, 0);
} // end drawVolume()

/*
 * drawXSlice
 *
 * parameter cFrustum - const CFrustum &
 */
void Brick::drawXSlice(const CFrustum & cFrustum) {
    float xValue = float(xSlice) * volume->getScale(0);
    float xStart = float(column) * volume->getScale(0);
    float xEnd   = 0.0f;
    if ((column + width) >= volume->getWidth())
        xEnd = float(column + width - 1) * volume->getScale(0);
    else
        xEnd = float(column + width) * volume->getScale(0);
    if ((xValue >= xStart) && (xValue <= xEnd)) {
        float weight = (xValue - xStart) / (xEnd - xStart);
        float yStart = float(row) * volume->getScale(1);
        float yEnd = 0.0f;
        if ((row + height) >= volume->getHeight())
            yEnd = float(row + height - 1) * volume->getScale(1);
        else
            yEnd = float(row + height) * volume->getScale(1);
        float zStart = float(slab) * volume->getScale(2);
        float zEnd = 0.0f;
        if ((slab + depth) >= volume->getDepth())
            zEnd = float(slab + depth - 1) * volume->getScale(2);
        else
            zEnd = float(slab + depth) * volume->getScale(2);
        glBegin(GL_QUADS);
        glNormal3f(0.0f, 0.0f, -1.0f);
        glTexCoord3f(weight, 0.0f, 0.0f);
        glVertex3f(xValue, yStart, zStart);
        glTexCoord3f(weight, 1.0f, 0.0f);
        glVertex3f(xValue, yEnd, zStart);
        glTexCoord3f(weight, 1.0f, 1.0f);
        glVertex3f(xValue, yEnd, zEnd);
        glTexCoord3f(weight, 0.0f, 1.0f);
        glVertex3f(xValue, yStart, zEnd);
        glEnd();
    }
} // end drawXSlice()

/*
 * drawYSlice
 *
 * parameter cFrustum - const CFrustum &
 */
void Brick::drawYSlice(const CFrustum & cFrustum) {
    float yValue = float(ySlice) * volume->getScale(1);
    float yStart = float(row) * volume->getScale(1);
    float yEnd   = 0.0f;
    if ((row + height) >= volume->getHeight())
        yEnd = float(row + height - 1) * volume->getScale(1);
    else
        yEnd = float(row + height) * volume->getScale(1);
    if ((yValue >= yStart) && (yValue <= yEnd)) {
        float weight = (yValue - yStart) / (yEnd - yStart);
        float xStart = float(column) * volume->getScale(0);
        float xEnd = 0.0f;
        if ((column + width) >= volume->getWidth())
            xEnd = float(column + width - 1) * volume->getScale(0);
        else
            xEnd = float(column + width) * volume->getScale(0);
        float zStart = float(slab) * volume->getScale(2);
        float zEnd = 0.0f;
        if ((slab + depth) >= volume->getDepth())
            zEnd = float(slab + depth - 1) * volume->getScale(2);
        else
            zEnd = float(slab + depth) * volume->getScale(2);
        glBegin(GL_QUADS);
        glNormal3f(0.0f, 0.0f, -1.0f);
        glTexCoord3f(0.0f, weight, 0.0f);
        glVertex3f(xStart, yValue, zStart);
        glTexCoord3f(1.0f, weight, 0.0f);
        glVertex3f(xEnd, yValue, zStart);
        glTexCoord3f(1.0f, weight, 1.0f);
        glVertex3f(xEnd, yValue, zEnd);
        glTexCoord3f(0.0f, weight, 1.0f);
        glVertex3f(xStart, yValue, zEnd);
        glEnd();
    }
} // end drawYSlice()

/*
 * drawZSlice
 *
 * parameter cFrustum - const CFrustum &
 */
void Brick::drawZSlice(const CFrustum & cFrustum) {
    float zValue = float(zSlice) * volume->getScale(2);
    float zStart = float(slab) * volume->getScale(2);
    float zEnd   = 0.0f;
    if ((slab + depth) >= volume->getDepth())
        zEnd = float(slab + depth - 1) * volume->getScale(2);
    else
        zEnd = float(slab + depth) * volume->getScale(2);
    if ((zValue >= zStart) && (zValue <= zEnd)) {
        float weight = (zValue - zStart) / (zEnd - zStart);
        float xStart = float(column) * volume->getScale(0);
        float xEnd = 0.0f;
        if ((column + width) >= volume->getWidth())
            xEnd = float(column + width - 1) * volume->getScale(0);
        else
            xEnd = float(column + width) * volume->getScale(0);
        float yStart = float(row) * volume->getScale(1);
        float yEnd = 0.0f;
        if ((row + height) >= volume->getHeight())
            yEnd = float(row + height - 1) * volume->getScale(1);
        else
            yEnd = float(row + height) * volume->getScale(1);
        glBegin(GL_QUADS);
        glNormal3f(0.0f, 0.0f, -1.0f);
        glTexCoord3f(0.0f, 0.0f, weight);
        glVertex3f(xStart, yStart, zValue);
        glTexCoord3f(1.0f, 0.0f, weight);
        glVertex3f(xEnd, yStart, zValue);
        glTexCoord3f(1.0f, 1.0f, weight);
        glVertex3f(xEnd, yEnd, zValue);
        glTexCoord3f(0.0f, 1.0f, weight);
        glVertex3f(xStart, yEnd, zValue);
        glEnd();

    }
} // end drawZSlice()

/*
 * setBrick
 *
 * parameter _column - int
 * parameter _row - int
 * parameter _slab - int
 * parameter _width - int
 * parameter _height - int
 * parameter _depth - int
 * parameter _volume - Volume*
 */
void Brick::setBrick(int _column, int _row, int _slab, int _width, int _height, int _depth, Volume* _volume) {
    column = _column;
    row = _row;
    slab = _slab;
    width = _width;
    height = _height;
    depth = _depth;
    volume = _volume;
    sliceFactor = volume->getSliceFactor();
    maximumPriorityQueueSize = volume->getMaximumPriorityQueueSize();
    float xOrigin = volume->getOrigin(0);
    float yOrigin = volume->getOrigin(1);
    float zOrigin = volume->getOrigin(2);
    float xExtent = volume->getExtent(0);
    float yExtent = volume->getExtent(1);
    float zExtent = volume->getExtent(2);
    float xStart = xOrigin + (xExtent - xOrigin) * (float(column) / float(volume->getWidth()));
    float xEnd = xOrigin + (xExtent - xOrigin) * (float(column + width) / float(volume->getWidth()));
    float yStart = yOrigin + (yExtent - yOrigin) * (float(row) / float(volume->getHeight()));
    float yEnd = yOrigin + (yExtent - yOrigin) * (float(row + height) / float(volume->getHeight()));
    float zStart = zOrigin + (zExtent - zOrigin) * (float(slab) / float(volume->getDepth()));
    float zEnd = zOrigin + (zExtent - zOrigin) * (float(slab + depth) / float(volume->getDepth()));
    volumeBox = new BoundingBox(xStart, yStart, zStart, xEnd, yEnd, zEnd);
    x = volumeBox->getCenterX();
    y = volumeBox->getCenterY();
    z = volumeBox->getCenterZ();
    setVolumeSphere();
    ocTree = new OcTree(this);
    setXSlice (0);
    setYSlice (0);
    setZSlice (0);

} // end setBrick()

/*
 * setDownSamplingChanged
 *
 * parameter downSamplingChanged - bool
 */
void Brick::setDownSamplingChanged(bool downSamplingChanged) {
    this->downSamplingChanged = downSamplingChanged;
} // end setDownSamplingChanged()

/*
 * isEmpty
 *
 * return - bool
 */
bool Brick::isEmpty(void) {
    return empty;
} // end isEmpty()

/*
 * setEmpty
 *
 * parameter _empty - bool
 */
void Brick::setEmpty(bool _empty) {
    empty - _empty;
} // end setEmpty()

/*
 * setMaximumPriorityQueueSize
 *
 * parameter _maximumPriorityQueueSize - int
 */
void Brick::setMaximumPriorityQueueSize(int _maximumPriorityQueueSize) {
    maximumPriorityQueueSize = _maximumPriorityQueueSize;
} // end setMaximumPriorityQueueSize()

/*
 * setMaximumPriorityQueueTest
 *
 * parameter _maximumPriorityQueueTest - bool
 */
void Brick::setMaximumPriorityQueueTest(bool _maximumPriorityQueueTest) {
    ocTree->setMaximumPriorityQueueTest(_maximumPriorityQueueTest);
} // end setMaximumPriorityQueueTest()

/*
 * setPercentageOfEmptyVolume
 *
 * parameter _alpha - float*
 */
void Brick::setPercentageOfEmptyVolume(float* _alpha) {
    ocTree->setPercentageOfEmptyVolume(_alpha);
} // end setPercentageOfEmptyVolume()

/*
 * setPercentageOfEmptyVolume2_5D
 *
 * parameter _alpha - float*
 */
void Brick::setPercentageOfEmptyVolume2_5D(float* _alpha) {
    ocTree->setPercentageOfEmptyVolume2_5D(_alpha);
} // end setPercentageOfEmptyVolume2_5D()

/*
 * setPercentageOfEmptyVolume
 *
 * parameter _red - float*
 * parameter _green - float*
 * parameter _blue - float*
 */
void Brick::setPercentageOfEmptyVolume(float* _red, float* _green, float* _blue) {
    ocTree->setPercentageOfEmptyVolume(_red, _green, _blue);
} // end setPercentageOfEmptyVolume()

/*
 * setRatioOfVisibility
 *
 * parameter _alpha - float*
 */
void Brick::setRatioOfVisibility(float* _alpha) {
    ocTree->setRatioOfVisibility(_alpha);
    if (ocTree->getRoot()->getRatioOfVisibility() == 0.0f)
        setEmpty(true);
    else
        setEmpty(false);
} // end setRatioOfVisibility()

/*
 * setRatioOfVisibility2_5D
 *
 * parameter _alpha - float*
 */
void Brick::setRatioOfVisibility2_5D(float* _alpha) {
    ocTree->setRatioOfVisibility2_5D(_alpha);
    if (ocTree->getRoot()->getRatioOfVisibility() == 0.0f)
        setEmpty(true);
    else
        setEmpty(false);
} // end setRatioOfVisibility2_5D()

/*
 * setRatioOfVisibility
 *
 * parameter _red - float*
 * parameter _green - float*
 * parameter _blue - float*
 */
void Brick::setRatioOfVisibility(float* _red, float* _green, float* _blue) {
    ocTree->setRatioOfVisibility(_red, _green, _blue);
    if (ocTree->getRoot()->getRatioOfVisibility() == 0.0f)
        setEmpty(true);
    else
        setEmpty(false);
} // end setRatioOfVisibility()

/*
 * setRatioOfVisibilityTest
 *
 * parameter _ratioOfVisibilityTest - bool
 */
void Brick::setRatioOfVisibilityTest(bool _ratioOfVisibilityTest) {
    ocTree->setRatioOfVisibilityTest(_ratioOfVisibilityTest);
} // end setRatioOfVisibilityTest()

/*
 * setRatioOfVisibilityThreshold
 *
 * parameter _ratioOfVisibilityThreshold - float
 */
void Brick::setRatioOfVisibilityThreshold(float _ratioOfVisibilityThreshold) {
    ocTree->setRatioOfVisibilityThreshold(_ratioOfVisibilityThreshold);
} // end setRatioOfVisibilityThreshold()

/*
 * setRenderingCost
 *
 * parameter _renderingCost - float
 */
void Brick::setRenderingCost(float _renderingCost) {
    ocTree->setRenderingCost(_renderingCost);
} // end setRenderingCost()

/*
 * setRenderingCostFunction
 *
 * parameter _alpha - float*
 */
void Brick::setRenderingCostFunction(float* _alpha) {
    ocTree->setRenderingCostFunction(_alpha);
} // end setRenderingCostFunction()

/*
 * setRenderingCostFunction
 *
 * parameter red - float*
 * parameter green - float*
 * parameter blue - float*
 */
void Brick::setRenderingCostFunction(float * red, float * green, float * blue) {
    ocTree->setRenderingCostFunction(red, green, blue);
} // end setRenderingCostFunction()

/*
 * setRenderingCostFunction2_5D
 *
 * parameter _alpha - float*
 */
void Brick::setRenderingCostFunction2_5D(float* _alpha) {
    ocTree->setRenderingCostFunction2_5D(_alpha);
} // end setRenderingCostFunction()

/*
 * setRenderingCostFunctionTest
 *
 * parameter _renderingCostFunctionTest - bool
 */
void Brick::setRenderingCostFunctionTest(bool _renderingCostFunctionTest) {
    ocTree->setRenderingCostFunctionTest(_renderingCostFunctionTest);
} // end setRenderingCostFunctionTest()

/*
 * setShowBricks
 *
 * parameter _showBricks - bool
 */
void Brick::setShowBricks(bool _showBricks) {
    showBricks = _showBricks;
} // end setShowBricks()

/*
 * setShowOcNodes
 *
 * parameter _showOcNodeLeaves - bool
 */
void Brick::setShowOcNodes(bool _showOcNodes) {
    ocTree->setShowOcNodes(_showOcNodes);
} // end setShowOcNodes()

/*
 * setShowXSlice
 *
 * parameter _showXSlice - bool
 */
void Brick::setShowXSlice(bool _showXSlice) {
    showXSlice = _showXSlice;
} // end setShowXSlice()

/*
 * setShowYSlice
 *
 * parameter _showYSlice - bool
 */
void Brick::setShowYSlice(bool _showYSlice) {
    showYSlice = _showYSlice;
} // end setShowYSlice()

/*
 * setShowZSlice
 *
 * parameter _showZSlice - bool
 */
void Brick::setShowZSlice(bool _showZSlice) {
    showZSlice = _showZSlice;
} // end setShowZSlice()

/*
 * setSliceFactor
 *
 * parameter _sliceFactor - float
 */
void Brick::setSliceFactor(float _sliceFactor) {
    this->sliceFactor = _sliceFactor;
    ocTree->setSliceFactor(_sliceFactor);
} // end setSliceFactor()

/*
 * setSlicingScale
 *
 * parameter slicingScale - float
 */
void Brick::setSlicingScale(float slicingScale) {
    this->slicingScale = slicingScale;
    if (interactive)
        ocTree->setSliceFactor(sliceFactor * slicingScale);
} // end setSlicingScale()

/*
 * getColumn
 *
 * return - int
 */
int Brick::getColumn(void) {
    return column;
} // end getColumn()

/*
 * getDepth
 *
 * return - int
 */
int Brick::getDepth(void) {
    return depth;
} // end getDepth()

/*
 * getHeight
 *
 * return - int
 */
int Brick::getHeight(void) {
    return height;
} // end getHeight()

/*
 * setInteractive
 *
 * parameter _interactive - bool
 */
void Brick::setInteractive(bool _interactive) {
    interactive = _interactive;
    if (interactive)
        ocTree->setSliceFactor(sliceFactor * slicingScale);
    else
        ocTree->setSliceFactor(sliceFactor);
} // end setInteractive()

/*
 * getOcTree
 *
 * return - OcTree*
 */
OcTree* Brick::getOcTree(void) {
    return ocTree;
} // end getOcTree()

/*
 * getRow
 *
 * return - int
 */
int Brick::getRow(void) {
    return row;
} // end getRow()

/*
 * getSlab
 *
 * return - int
 */
int Brick::getSlab(void) {
    return slab;
} // end getSlab()

/*
 * getVolume
 *
 * return - Volume*
 */
Volume* Brick::getVolume(void) {
    return volume;
} // end getVolume()

/*
 * getVolumeBox
 *
 * return - BoundingBox*
 */
BoundingBox* Brick::getVolumeBox(void) {
    return volumeBox;
} // end getVolumeBox()

/*
 * getVolumeSphere
 *
 * return - BoundingSphere*
 */
BoundingSphere* Brick::getVolumeSphere(void) {
    return volumeSphere;
} // end getVolumeSphere()

/*
 * setVolumeSphere
 */
void Brick::setVolumeSphere(void) {
    float _radius = volumeBox->getRadius();
    volumeSphere = new BoundingSphere(x, y, z, _radius);
} // end setVolumeSphere()

/*
 * getWidth
 *
 * return - int
 */
int Brick::getWidth(void) {
    return width;
} // end getWidth()

/*
 * getX
 *
 * return - float
 */
float Brick::getX(void) {
    return x;
} // end getX()

/*
 * getXSlice
 *
 * return - int
 */
int Brick::getXSlice(void) {
    return xSlice;
    if (volume->getSliceIndexDir(0) < 0)
        xSlice = (volume->getWidth() - 1) - xSlice;
} // end getXSlice()

/*
 * setXSlice
 *
 * parameter _xSlice - int
 */
void Brick::setXSlice(int _xSlice) {
    xSlice = _xSlice;
} // end setXSlice()

/*
 * setXStep
 *
 * parameter xStep - int
 */
void Brick::setXStep(int xStep) {
    this->xStep = xStep;
} // end setXStep()

/*
 * getY
 *
 * return - float
 */
float Brick::getY(void) {
    return y;
} // end getY()

/*
 * getYSlice
 *
 * return - int
 */
int Brick::getYSlice(void) {
    return ySlice;
} // end getYSlice()

/*
 * setYSlice
 *
 * parameter _ySlice - int
 */
void Brick::setYSlice(int _ySlice) {
    ySlice = _ySlice;
    if (volume->getSliceIndexDir(1) < 0)
        ySlice = (volume->getHeight() - 1) - ySlice;
} // end setYSlice()

/*
 * setYStep
 *
 * parameter yStep - int
 */
void Brick::setYStep(int yStep) {
    this->yStep = yStep;
} // end setYStep()

/*
 * getZ
 *
 * return - float
 */
float Brick::getZ(void) {
    return z;
} // end getZ()

/*
 * getZSlice
 *
 * return - int
 */
int Brick::getZSlice(void) {
    return zSlice;
} // end getZSlice()

/*
 * setZSlice
 *
 * parameter _zSlice - int
 */
void Brick::setZSlice(int _zSlice) {
    zSlice = _zSlice;
    if (volume->getSliceIndexDir(2) < 0)
        zSlice = (volume->getDepth() - 1) - zSlice;
} // end setZSlice()

/*
 * setZStep
 *
 * parameter zStep - int
 */
void Brick::setZStep(int zStep) {
    this->zStep = zStep;
} // end setZStep()

/*
 * initContext
 *
 * parameter glContextData - GLContextData&
 */
void Brick::initContext(GLContextData& glContextData) const {
    DataItem * dataItem = new DataItem();
    glContextData.addDataItem(this, dataItem);
    initialize(dataItem);
} // end initContext()

/*
 * initialize
 *
 * parameter dataItem - DataItem *
 */
void Brick::initialize(DataItem * dataItem) const {
    if (volume->getNumberOfComponents() != 0) {
        dataItem->texture3DName = createMultiComponent3DTexture(borderSize);
        dataItem->downSamplingTexture3DName = createDownSamplingMultiComponent3DTexture();
    } else {
        dataItem->texture3DName = create3DTexture(borderSize);
        dataItem->downSamplingTexture3DName = createDownSampling3DTexture();
    }
}
// end drawVolume()

/*
 * update3DTexture
 *
 * parameter texture3DName - GLuint
 */
void Brick::update3DTexture(GLuint texture3DName) {
    unsigned char* voxels = new unsigned char[(width) * (height) * (depth)];
    int w = volume->getWidth();
    int h = volume->getHeight();
    for (int k = 0; k < depth; k++) {
        for (int j = 0; j < height; j++) {
            for (int i = 0; i < width; i++) {
                voxels[(k * ((width) * (height))) + (j *( width)) +i] = volume->getVoxel(((slab + (k)) * (w * h)) + ((row + (j))
                        * (w)) + (column + (i)));
            } // end for
        } // end for
    } // end for
    glEnable(GL_TEXTURE_3D);
    glBindTexture(GL_TEXTURE_3D, texture3DName);
    glTexSubImage3D(GL_TEXTURE_3D, 0, 0, 0, 0, width, height, depth, GL_LUMINANCE, GL_UNSIGNED_BYTE, voxels);
    glBindTexture(GL_TEXTURE_3D, 0);
    glDisable(GL_TEXTURE_3D);
    delete[] voxels;
} // end updateBrick()

/*
 * fill3DTexture
 *
 * parameter voxels - unsigned char *
 * parameter bordersize - int
 */
void Brick::fill3DTexture (int bordersize, unsigned char *voxels) const
{
    int         i, j, k, i0, i1, j0, j1, k0, k1;
    int         w   = volume->getWidth();
    int         h   = volume->getHeight();
    int         d   = volume->getDepth();
    int         w2b = width  + (2 * bordersize);
    int         h2b = height + (2 * bordersize);
    int         d2b = depth  + (2 * bordersize);

#define VOXEL(K, J, I)        voxels[((K) * h2b * w2b) + ((J) * w2b) + (I)]

#define VOL_VOXEL(K, J, I)    volume->getVoxel (                               \
                                  ((slab   + ((K) - bordersize)) * w * h) +    \
                                  ((row    + ((J) - bordersize)) * w) +        \
                                  ((column + ((I) - bordersize))))

    k0 = 0;
    k1 = depth + bordersize;
    j0 = 0;
    j1 = height + bordersize;
    i0 = 0;
    i1 = width + bordersize;

//  Fill the core voxels with values from the volume
    for (k = bordersize; k < depth + bordersize; k++)
    {
        for (j = bordersize; j < height + bordersize; j++)
        {
            for (i = bordersize; i < width + bordersize; i++)
            {
                VOXEL(k,j,i) = VOL_VOXEL(k,j,i);
            } // end for i
        } // end for j
    } // end for k

//  Fill the face voxels
    if (slab == 0)
    {
        for (j = bordersize; j < height + bordersize; j++)
        {
            for (i = bordersize; i < width + bordersize; i++)
            {
                VOXEL(k0,j,i) = VOXEL((k0+1),j,i);
            } // end for i
        } // end for j
    }
    else
    {
        for (j = bordersize; j < height + bordersize; j++)
        {
            for (i = bordersize; i < width + bordersize; i++)
            {
                VOXEL(k0,j,i) = VOL_VOXEL((k0+bordersize-1),j,i);
            } // end for i
        } // end for j
    }

    if ((slab + depth) == d)
    {
        for (j = bordersize; j < height + bordersize; j++)
        {
            for (i = bordersize; i < width + bordersize; i++)
            {
                VOXEL(k1,j,i) = VOXEL((k1-1),j,i);
            } // end for i
        } // end for j
    }
    else
    {
        for (j = bordersize; j < height + bordersize; j++)
        {
            for (i = bordersize; i < width + bordersize; i++)
            {
                VOXEL(k1,j,i) = VOL_VOXEL(k1,j,i);
            } // end for i
        } // end for j
    }

    if (row == 0)
    {
        for (k = bordersize; k < depth + bordersize; k++)
        {
            for (i = bordersize; i < width + bordersize; i++)
            {
                VOXEL(k,j0,i) = VOXEL(k,(j0+1),i);
            } // end for i
        } // end for k
    }
    else
    {
        for (k = bordersize; k < depth + bordersize; k++)
        {
            for (i = bordersize; i < width + bordersize; i++)
            {
                VOXEL(k,j0,i) = VOL_VOXEL(k,(j0+bordersize-1),i);
            } // end for i
        } // end for k
    }

    if ((row + height) == h)
    {
        for (k = bordersize; k < depth + bordersize; k++)
        {
            for (i = bordersize; i < width + bordersize; i++)
            {
                VOXEL(k,j1,i) = VOXEL(k,(j1-1),i);
            } // end for i
        } // end for k
    }
    else
    {
        for (k = bordersize; k < depth + bordersize; k++)
        {
            for (i = bordersize; i < width + bordersize; i++)
            {
                VOXEL(k,j1,i) = VOL_VOXEL(k,j1,i);
            } // end for i
        } // end for k
    }

    if (column == 0)
    {
        for (k = bordersize; k < depth + bordersize; k++)
        {
            for (j = bordersize; j < height + bordersize; j++)
            {
                VOXEL(k,j,i0) = VOXEL(k,j,(i0+1));
            } // end for j
        } // end for k
    }
    else
    {
        for (k = bordersize; k < depth + bordersize; k++)
        {
            for (j = bordersize; j < height + bordersize; j++)
            {
                VOXEL(k,j,i0) = VOL_VOXEL(k,j,(i0+bordersize-1));
            } // end for j
        } // end for k
    }

    if ((column + width) == w)
    {
        for (k = bordersize; k < depth + bordersize; k++)
        {
            for (j = bordersize; j < height + bordersize; j++)
            {
                VOXEL(k,j,i1) = VOXEL(k,j,(i1-1));
            } // end for j
        } // end for k
    }
    else
    {
        for (k = bordersize; k < depth + bordersize; k++)
        {
            for (j = bordersize; j < height + bordersize; j++)
            {
                VOXEL(k,j,i) = VOL_VOXEL(k,j,i);
            } // end for j
        } // end for k
    }

//  Fill the edge and corner voxels
//  This method just copies values from the adjacent core voxels
//  rather than copying values from the volume, but it avoids
//  having to deal with all of the brick position conditions.
//  However, it is a bit better than filling with zeroes.
    for (i = bordersize; i < width + bordersize; i++)
    {
        VOXEL(k0,j0,i) = VOXEL((k0+1),(j0+1),i);
        VOXEL(k0,j1,i) = VOXEL((k0+1),(j1-1),i);
        VOXEL(k1,j0,i) = VOXEL((k1-1),(j0+1),i);
        VOXEL(k1,j1,i) = VOXEL((k1-1),(j1-1),i);
    }

    for (j = bordersize; j < height + bordersize; j++)
    {
        VOXEL(k0,j,i0) = VOXEL((k0+1),j,(i0+1));
        VOXEL(k0,j,i1) = VOXEL((k0+1),j,(i1-1));
        VOXEL(k1,j,i0) = VOXEL((k1-1),j,(i0+1));
        VOXEL(k1,j,i1) = VOXEL((k1-1),j,(i1-1));
    }

    for (k = bordersize; k < depth + bordersize; k++)
    {
        VOXEL(k,j0,i0) = VOXEL(k,(j0+1),(i0+1));
        VOXEL(k,j0,i1) = VOXEL(k,(j0+1),(i1-1));
        VOXEL(k,j1,i0) = VOXEL(k,(j1-1),(i0+1));
        VOXEL(k,j1,i1) = VOXEL(k,(j1-1),(i1-1));
    }

    VOXEL(k0,j0,i0) = VOXEL((k0+1),(j0+1),(i0+1));
    VOXEL(k0,j0,i1) = VOXEL((k0+1),(j0+1),(i1-1));
    VOXEL(k0,j1,i0) = VOXEL((k0+1),(j1-1),(i0+1));
    VOXEL(k0,j1,i1) = VOXEL((k0+1),(j1-1),(i1-1));
    VOXEL(k1,j0,i0) = VOXEL((k1-1),(j0+1),(i0+1));
    VOXEL(k1,j0,i1) = VOXEL((k1-1),(j0+1),(i1-1));
    VOXEL(k1,j1,i0) = VOXEL((k1-1),(j1-1),(i0+1));
    VOXEL(k1,j1,i1) = VOXEL((k1-1),(j1-1),(i1-1));
#undef VOL_VOXEL
#undef VOXEL

    return;
} // end fill3DTexture()

unsigned char getRed (Volume *volume, int ivoxel)
{
    return volume->getRedVoxel (ivoxel);
}

unsigned char getGrn (Volume *volume, int ivoxel)
{
    return volume->getGreenVoxel (ivoxel);
}

unsigned char getBlu (Volume *volume, int ivoxel)
{
    return volume->getBlueVoxel (ivoxel);
}

/*
 * fillMultiComponent3DTexture
 *
 * parameter voxels - unsigned char *
 * parameter bordersize - int
 * parameter icomp - int
 */
void Brick::fillMultiComponent3DTexture (int bordersize, unsigned char *voxels,
                                         int icomp) const
{
    unsigned char (*getVoxelFunc[])(Volume*, int) = { getRed, getGrn, getBlu };

    int         i, j, k, i0, i1, j0, j1, k0, k1;
    int         w   = volume->getWidth();
    int         h   = volume->getHeight();
    int         d   = volume->getDepth();
    int         w2b = width  + (2 * bordersize);
    int         h2b = height + (2 * bordersize);
    int         d2b = depth  + (2 * bordersize);

#define VOXEL(K, J, I)        voxels[((((K) * h2b * w2b) + ((J) * w2b) + (I)) \
                                     * 3) + icomp]

#define VOL_VOXEL(K, J, I)   getVoxelFunc[icomp] (volume,                     \
                                 ((slab   + ((K) - bordersize)) * w * h) +    \
                                 ((row    + ((J) - bordersize)) * w) +        \
                                 ((column + ((I) - bordersize))))


    k0 = 0;
    k1 = depth + bordersize;
    j0 = 0;
    j1 = height + bordersize;
    i0 = 0;
    i1 = width + bordersize;

//  Fill the core voxels with values from the volume
    for (k = bordersize; k < depth + bordersize; k++)
    {
        for (j = bordersize; j < height + bordersize; j++)
        {
            for (i = bordersize; i < width + bordersize; i++)
            {
                VOXEL(k,j,i) = VOL_VOXEL(k,j,i);
            } // end for i
        } // end for j
    } // end for k

//  Fill the face voxels
    if (slab == 0)
    {
        for (j = bordersize; j < height + bordersize; j++)
        {
            for (i = bordersize; i < width + bordersize; i++)
            {
                VOXEL(k0,j,i) = VOXEL((k0+1),j,i);
            } // end for i
        } // end for j
    }
    else
    {
        for (j = bordersize; j < height + bordersize; j++)
        {
            for (i = bordersize; i < width + bordersize; i++)
            {
                VOXEL(k0,j,i) = VOL_VOXEL((k0+bordersize-1),j,i);
            } // end for i
        } // end for j
    }

    if ((slab + depth) == d)
    {
        for (j = bordersize; j < height + bordersize; j++)
        {
            for (i = bordersize; i < width + bordersize; i++)
            {
                VOXEL(k1,j,i) = VOXEL((k1-1),j,i);
            } // end for i
        } // end for j
    }
    else
    {
        for (j = bordersize; j < height + bordersize; j++)
        {
            for (i = bordersize; i < width + bordersize; i++)
            {
                VOXEL(k1,j,i) = VOL_VOXEL(k1,j,i);
            } // end for i
        } // end for j
    }

    if (row == 0)
    {
        for (k = bordersize; k < depth + bordersize; k++)
        {
            for (i = bordersize; i < width + bordersize; i++)
            {
                VOXEL(k,j0,i) = VOXEL(k,(j0+1),i);
            } // end for i
        } // end for k
    }
    else
    {
        for (k = bordersize; k < depth + bordersize; k++)
        {
            for (i = bordersize; i < width + bordersize; i++)
            {
                VOXEL(k,j0,i) = VOL_VOXEL(k,(j0+bordersize-1),i);
            } // end for i
        } // end for k
    }

    if ((row + height) == h)
    {
        for (k = bordersize; k < depth + bordersize; k++)
        {
            for (i = bordersize; i < width + bordersize; i++)
            {
                VOXEL(k,j1,i) = VOXEL(k,(j1-1),i);
            } // end for i
        } // end for k
    }
    else
    {
        for (k = bordersize; k < depth + bordersize; k++)
        {
            for (i = bordersize; i < width + bordersize; i++)
            {
                VOXEL(k,j1,i) = VOL_VOXEL(k,j1,i);
            } // end for i
        } // end for k
    }

    if (column == 0)
    {
        for (k = bordersize; k < depth + bordersize; k++)
        {
            for (j = bordersize; j < height + bordersize; j++)
            {
                VOXEL(k,j,i0) = VOXEL(k,j,(i0+1));
            } // end for j
        } // end for k
    }
    else
    {
        for (k = bordersize; k < depth + bordersize; k++)
        {
            for (j = bordersize; j < height + bordersize; j++)
            {
                VOXEL(k,j,i0) = VOL_VOXEL(k,j,(i0+bordersize-1));
            } // end for j
        } // end for k
    }

    if ((column + width) == w)
    {
        for (k = bordersize; k < depth + bordersize; k++)
        {
            for (j = bordersize; j < height + bordersize; j++)
            {
                VOXEL(k,j,i1) = VOXEL(k,j,(i1-1));
            } // end for j
        } // end for k
    }
    else
    {
        for (k = bordersize; k < depth + bordersize; k++)
        {
            for (j = bordersize; j < height + bordersize; j++)
            {
                VOXEL(k,j,i) = VOL_VOXEL(k,j,i);
            } // end for j
        } // end for k
    }

//  Fill the edge and corner voxels
//  This method just copies values from the adjacent core voxels
//  rather than copying values from the volume, but it avoids
//  having to deal with all of the brick position conditions.
//  However, it is a bit better than filling with zeroes.
    for (i = bordersize; i < width + bordersize; i++)
    {
        VOXEL(k0,j0,i) = VOXEL((k0+1),(j0+1),i);
        VOXEL(k0,j1,i) = VOXEL((k0+1),(j1-1),i);
        VOXEL(k1,j0,i) = VOXEL((k1-1),(j0+1),i);
        VOXEL(k1,j1,i) = VOXEL((k1-1),(j1-1),i);
    }

    for (j = bordersize; j < height + bordersize; j++)
    {
        VOXEL(k0,j,i0) = VOXEL((k0+1),j,(i0+1));
        VOXEL(k0,j,i1) = VOXEL((k0+1),j,(i1-1));
        VOXEL(k1,j,i0) = VOXEL((k1-1),j,(i0+1));
        VOXEL(k1,j,i1) = VOXEL((k1-1),j,(i1-1));
    }

    for (k = bordersize; k < depth + bordersize; k++)
    {
        VOXEL(k,j0,i0) = VOXEL(k,(j0+1),(i0+1));
        VOXEL(k,j0,i1) = VOXEL(k,(j0+1),(i1-1));
        VOXEL(k,j1,i0) = VOXEL(k,(j1-1),(i0+1));
        VOXEL(k,j1,i1) = VOXEL(k,(j1-1),(i1-1));
    }

    VOXEL(k0,j0,i0) = VOXEL((k0+1),(j0+1),(i0+1));
    VOXEL(k0,j0,i1) = VOXEL((k0+1),(j0+1),(i1-1));
    VOXEL(k0,j1,i0) = VOXEL((k0+1),(j1-1),(i0+1));
    VOXEL(k0,j1,i1) = VOXEL((k0+1),(j1-1),(i1-1));
    VOXEL(k1,j0,i0) = VOXEL((k1-1),(j0+1),(i0+1));
    VOXEL(k1,j0,i1) = VOXEL((k1-1),(j0+1),(i1-1));
    VOXEL(k1,j1,i0) = VOXEL((k1-1),(j1-1),(i0+1));
    VOXEL(k1,j1,i1) = VOXEL((k1-1),(j1-1),(i1-1));
#undef VOL_VOXEL
#undef VOXEL

    return;
} // end fillMultiComponent3DTexture()
