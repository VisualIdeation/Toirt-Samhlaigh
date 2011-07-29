/*
 * Scene
 *
 * Author: Patrick O'Leary
 * Created: November 6, 2007
 * Copyright 2007. All rights reserved.
 */
#ifndef SCENE_H_
#define SCENE_H_

#include <deque>
#include <string>
#include <vector>

#include <GL/gl.h>
#include <GL/glu.h>

/* Vrui includes */
#include <Geometry/Point.h>
#include <GL/GLContextData.h>
#include <GL/GLObject.h>
#include <GL/GLPolylineTube.h>

#include <GRAPHIC/CFrustum.h>

#define RED 0
#define GREEN 1
#define BLUE 2
#define ALPHA 3

/* GPUVertexShader */
static const char* GPUVertexShader = "uniform vec3 vBBox[8];    // updated per brick                           \n"
    "uniform vec3 tBBox[8];                                                   \n"
    "uniform float delta;                                                     \n"
    "uniform int front;        // updated per frame                           \n"
    "uniform vec3 slicePlaneNormal;                                           \n"
    "uniform int seq[64];      // never updated                               \n"
    "uniform int edge[48];                                                    \n"
    "                                                                         \n"
    "void main(void)                                                          \n"
    "{                                                                        \n"
    "    int xx = int(gl_Vertex.x);                                           \n"
    "    float yy = gl_Vertex.y;                                              \n"
    "    vec3 Position, p0, p1, sliceDelta, sPF, TexCoord0, t0, t1;           \n"
    "    Position = vec3(-1,-1,-1);                                           \n"
    "    TexCoord0 = vec3(-1,-1,-1);                                          \n"
    "    sliceDelta = slicePlaneNormal * delta;                               \n"
    "    sPF = vBBox[seq[front*8+7]] + sliceDelta * yy;                       \n"
    "    for(int e = 0; e < 4; e++) {                                         \n"
    "       p0 = vBBox[seq[front*8+edge[((xx * 4) + e)*2+0]]];                \n"
    "       p1 = vBBox[seq[front*8+edge[((xx * 4) + e)*2+1]]];                \n"
    "       float d = dot(slicePlaneNormal,(p1 - p0));                        \n"
    "       float tFront = (d!=0.0) ? dot(slicePlaneNormal,(sPF - p0)) / d : -1.0;         \n"
    "       if((tFront >= 0.0) && (tFront <= 1.0)) {                          \n"
    "          t0 = tBBox[seq[front*8+edge[((xx * 4) + e)*2+0]]];             \n"
    "          t1 = tBBox[seq[front*8+edge[((xx * 4) + e)*2+1]]];             \n"
    "          Position = p0 + (tFront * (p1 - p0));                          \n"
    "          TexCoord0 = t0 + (tFront * (t1 - t0));                         \n"
    "          break;                                                         \n"
    "       }                                                                 \n"
    "    }                                                                    \n"
    "	 vec4 pos = vec4(Position.x, Position.y, Position.z, 1.0);            \n"
    "    gl_Position = gl_ModelViewProjectionMatrix * pos;                    \n"
    "    gl_ClipVertex = gl_ModelViewMatrix * pos;                            \n"
    "	 vec4 tex0 = vec4(TexCoord0.x, TexCoord0.y, TexCoord0.z, 1.0);        \n"
    "    gl_TexCoord[0] = tex0;                                               \n"
    "}                                                                      \n\0";

/* GPUVertexShaderWithLighting */
static const char* GPUVertexShaderWithLighting = "uniform vec3 vBBox[8];    // updated per brick                           \n"
    "uniform vec3 tBBox[8];                                                   \n"
    "uniform float delta;                                                      \n"
    "uniform int front;        // updated per frame                           \n"
    "uniform vec3 slicePlaneNormal;                                           \n"
    "uniform int seq[64];      // never updated                               \n"
    "uniform int edge[48];                                                    \n"
    "                                                                         \n"
    "void main(void)                                                          \n"
    "{                                                                        \n"
    "    int xx = int(gl_Vertex.x);                                           \n"
    "    float yy = gl_Vertex.y;                                              \n"
    "    vec3 Position, p0, p1, sliceDelta, sPF, TexCoord0, t0, t1;           \n"
    "    Position = vec3(-1,-1,-1);                                           \n"
    "    TexCoord0 = vec3(-1,-1,-1);                                          \n"
    "    sliceDelta = slicePlaneNormal * delta;                               \n"
    "    sPF = vBBox[seq[front*8+7]] + sliceDelta * yy;                       \n"
    "    for(int e = 0; e < 4; e++) {                                         \n"
    "       p0 = vBBox[seq[front*8+edge[((xx * 4) + e)*2+0]]];                \n"
    "       p1 = vBBox[seq[front*8+edge[((xx * 4) + e)*2+1]]];                \n"
    "       float d = dot(slicePlaneNormal,(p1 - p0));                        \n"
    "       float tFront = (d!=0.0) ? dot(slicePlaneNormal,(sPF - p0)) / d : -1.0;         \n"
    "       if((tFront >= 0.0) && (tFront <= 1.0)) {                          \n"
    "          t0 = tBBox[seq[front*8+edge[((xx * 4) + e)*2+0]]];             \n"
    "          t1 = tBBox[seq[front*8+edge[((xx * 4) + e)*2+1]]];             \n"
    "          Position = p0 + (tFront * (p1 - p0));                          \n"
    "          TexCoord0 = t0 + (tFront * (t1 - t0));                         \n"
    "          break;                                                         \n"
    "       }                                                                 \n"
    "    }                                                                    \n"
    "	 vec4 pos = vec4(Position.x, Position.y, Position.z, 1.0);            \n"
    "    gl_Position = gl_ModelViewProjectionMatrix * pos;                    \n"
    "    gl_ClipVertex = gl_ModelViewMatrix * pos;                            \n"
    "	 vec4 tex0 = vec4(TexCoord0.x, TexCoord0.y, TexCoord0.z, 1.0);        \n"
    "    gl_TexCoord[0] = tex0;                                               \n"
    "	 gl_TexCoord[1] = gl_Position;                                        \n"
    "}                                                                      \n\0";

/* preIntegratedGPUVertexShader */
static const char* preIntegratedGPUVertexShader = "uniform vec3 vBBox[8];    // updated per brick                           \n"
    "uniform vec3 tBBox[8];                                                   \n"
    "uniform float delta;                                                      \n"
    "uniform int front;        // updated per frame                           \n"
    "uniform vec3 slicePlaneNormal;                                           \n"
    "uniform int seq[64];      // never updated                               \n"
    "uniform int edge[48];                                                    \n"
    "                                                                         \n"
    "void main(void)                                                          \n"
    "{                                                                        \n"
    "    int xx = int(gl_Vertex.x);                                           \n"
    "    float yy = gl_Vertex.y;                                              \n"
    "    vec3 Position, p0, p1, sliceDelta, sPF, sPB;                         \n"
    "    vec3 TexCoord0, TexCoord1, t0, t1;                                   \n"
    "    Position = vec3(-1,-1,-1);                                           \n"
    "    TexCoord0 = vec3(-1,-1,-1);                                          \n"
    "    TexCoord1 = vec3(-1,-1,-1);                                          \n"
    "    sliceDelta = slicePlaneNormal * delta;                               \n"
    "    sPF = vBBox[seq[front*8+7]] + sliceDelta * yy;                       \n"
    "    sPB = vBBox[seq[front*8+7]] + sliceDelta * (yy - 1.0);                 \n"
    "    for(int e = 0; e < 4; e++) {                                         \n"
    "       p0 = vBBox[seq[front*8+edge[((xx * 4) + e)*2+0]]];                \n"
    "       p1 = vBBox[seq[front*8+edge[((xx * 4) + e)*2+1]]];                \n"
    "       float d = dot(slicePlaneNormal,(p1 - p0));                        \n"
    "       float tFront = (d!=0.0) ? dot(slicePlaneNormal,(sPF - p0)) / d : -1.0;         \n"
    "       if((tFront >= 0.0) && (tFront <= 1.0)) {                          \n"
    "          t0 = tBBox[seq[front*8+edge[((xx * 4) + e)*2+0]]];             \n"
    "          t1 = tBBox[seq[front*8+edge[((xx * 4) + e)*2+1]]];             \n"
    "          Position = p0 + (tFront * (p1 - p0));                          \n"
    "          TexCoord0 = t0 + (tFront * (t1 - t0));                         \n"
    "          float tBack = (d!=0.0) ? dot(slicePlaneNormal,(sPB - p0)) / d : -1.0;       \n"
    "          if((tBack >= 0.0) && (tBack <= 1.0)) {                         \n"
    "             TexCoord1 = t0 + (tBack * (t1 - t0));                       \n"
    "          }                                                              \n"
    "          else {                                                         \n"
    "             TexCoord1 = t0 + (tFront * (t1 - t0));                      \n"
    "          }                                                              \n"
    "          break;                                                         \n"
    "       }                                                                 \n"
    "    }                                                                    \n"
    "	 vec4 pos = vec4(Position.x, Position.y, Position.z, 1.0);            \n"
    "    gl_Position = gl_ModelViewProjectionMatrix * pos;                    \n"
    "    gl_ClipVertex = gl_ModelViewMatrix * pos;                            \n"
    "	 vec4 tex0 = vec4(TexCoord0.x, TexCoord0.y, TexCoord0.z, 1.0);        \n"
    "    gl_TexCoord[0] = tex0;                                               \n"
    "	 vec4 tex1 = vec4(TexCoord1.x, TexCoord1.y, TexCoord1.z, 1.0);        \n"
    "    gl_TexCoord[1] = tex1;                                               \n"
    "}                                                                      \n\0";

/* preIntegratedGPUVertexShaderWithLighting */
static const char* preIntegratedGPUVertexShaderWithLighting =
        "uniform vec3 vBBox[8];    // updated per brick                           \n"
            "uniform vec3 tBBox[8];                                                   \n"
            "uniform float delta;                                                      \n"
            "uniform int front;        // updated per frame                           \n"
            "uniform vec3 slicePlaneNormal;                                           \n"
            "uniform int seq[64];      // never updated                               \n"
            "uniform int edge[48];                                                    \n"
            "                                                                         \n"
            "void main(void)                                                          \n"
            "{                                                                        \n"
            "    int xx = int(gl_Vertex.x);                                           \n"
            "    float yy = gl_Vertex.y;                                              \n"
            "    vec3 Position, p0, p1, sliceDelta, sPF, sPB;                         \n"
            "    vec3 TexCoord0, TexCoord1, t0, t1;                                   \n"
            "    Position = vec3(-1,-1,-1);                                           \n"
            "    TexCoord0 = vec3(-1,-1,-1);                                          \n"
            "    TexCoord1 = vec3(-1,-1,-1);                                          \n"
            "    sliceDelta = slicePlaneNormal * delta;                               \n"
            "    sPF = vBBox[seq[front*8+7]] + sliceDelta * yy;                       \n"
            "    sPB = vBBox[seq[front*8+7]] + sliceDelta * (yy - 1.0);                 \n"
            "    for(int e = 0; e < 4; e++) {                                         \n"
            "       p0 = vBBox[seq[front*8+edge[((xx * 4) + e)*2+0]]];                \n"
            "       p1 = vBBox[seq[front*8+edge[((xx * 4) + e)*2+1]]];                \n"
            "       float d = dot(slicePlaneNormal,(p1 - p0));                        \n"
            "       float tFront = (d!=0.0) ? dot(slicePlaneNormal,(sPF - p0)) / d : -1.0;         \n"
            "       if((tFront >= 0.0) && (tFront <= 1.0)) {                          \n"
            "          t0 = tBBox[seq[front*8+edge[((xx * 4) + e)*2+0]]];             \n"
            "          t1 = tBBox[seq[front*8+edge[((xx * 4) + e)*2+1]]];             \n"
            "          Position = p0 + (tFront * (p1 - p0));                          \n"
            "          TexCoord0 = t0 + (tFront * (t1 - t0));                         \n"
            "          float tBack = (d!=0.0) ? dot(slicePlaneNormal,(sPB - p0)) / d : -1.0;       \n"
            "          if((tBack >= 0.0) && (tBack <= 1.0)) {                         \n"
            "             TexCoord1 = t0 + (tBack * (t1 - t0));                       \n"
            "          }                                                              \n"
            "          else {                                                         \n"
            "             TexCoord1 = t0 + (tFront * (t1 - t0));                      \n"
            "          }                                                              \n"
            "          break;                                                         \n"
            "       }                                                                 \n"
            "    }                                                                    \n"
            "	 vec4 pos = vec4(Position.x, Position.y, Position.z, 1.0);            \n"
            "    gl_Position = gl_ModelViewProjectionMatrix * pos;                    \n"
            "    gl_ClipVertex = gl_ModelViewMatrix * pos;                            \n"
            "	 vec4 tex0 = vec4(TexCoord0.x, TexCoord0.y, TexCoord0.z, 1.0);        \n"
            "    gl_TexCoord[0] = tex0;                                               \n"
            "	 vec4 tex1 = vec4(TexCoord1.x, TexCoord1.y, TexCoord1.z, 1.0);        \n"
            "    gl_TexCoord[1] = tex1;                                               \n"
            "	 gl_TexCoord[2] = gl_Position;                                        \n"
            "}                                                                      \n\0";

/* vertexShader */
static const char* vertexShader = "void main(void)                                     \n"
    "{                                                           \n"
    "	gl_TexCoord[0] = gl_MultiTexCoord0;                      \n"
    "	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;  \n"
    "   gl_ClipVertex = gl_ModelViewMatrix * gl_Vertex;          \n"
    "}                                                         \n\0";

/* vertexShaderWithLighting */
static const char* vertexShaderWithLighting = "void main(void)                                     \n"
    "{                                                           \n"
    "	gl_TexCoord[0] = gl_MultiTexCoord0;                      \n"
    "	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;  \n"
    "   gl_ClipVertex = gl_ModelViewMatrix * gl_Vertex;          \n"
    "	gl_TexCoord[1] = gl_Position;                            \n"
    "}                                                         \n\0";

/* fragmentShaderWithLighting */
static const char* focusAndContextFragmentShader = "uniform sampler3D volume;                                     \n"
    "uniform sampler1D colormap1D;                                 \n"
    "uniform vec3 planeNormal;                                     \n"
    "uniform float planeOffset;                                    \n"
    "                                                              \n"
    "void main (void)                                              \n"
    "{                                                             \n"
    "   float x = texture3D(volume, gl_TexCoord[0].xyz).x;         \n"
    "   vec4 c = texture1D(colormap1D, x);                         \n"
    "   float delta = abs(dot(planeNormal,gl_TexCoord[1].xyz)-planeOffset);     \n"
    "   c.a *= 1.0 - 0.9 * float(delta > 0.5);                   \n"
    "   gl_FragColor = c;                                          \n"
    "}                                                           \n\0";

/* fragmentShader */
static const char* fragmentShader = "uniform sampler3D volume;                         \n"
    "uniform sampler1D colormap1D;                                 \n"
    "                                                              \n"
    "void main (void)                                              \n"
    "{                                                             \n"
    "	float x = texture3D(volume, vec3(gl_TexCoord[0])).x;       \n"
    "	vec4 y = texture1D(colormap1D, x);                         \n"
    "	gl_FragColor = y;                                          \n"
    "}                                                           \n\0";

/* fragmentShader2D */
static const char* fragmentShader2D = "#define DELTA 0.01                                            \n"
    "uniform sampler3D volume;                                     \n"
    "uniform sampler2D colormap2D;                                 \n"
    "uniform float maximumGradient;                                        \n"
    "                                                              \n"
    "void main (void)                                              \n"
    "{                                                             \n"
    "   float x = texture3D(volume, gl_TexCoord[0].xyz).x;         \n"
    "   vec3 a, b;                                                 \n"
    "   a.x  = texture3D(volume, gl_TexCoord[0].xyz - vec3(DELTA,0.0,0.0)).x;\n"
    "   b.x  = texture3D(volume, gl_TexCoord[0].xyz + vec3(DELTA,0.0,0.0)).x;\n"
    "   a.y  = texture3D(volume, gl_TexCoord[0].xyz - vec3(0.0,DELTA,0.0)).x;\n"
    "   b.y  = texture3D(volume, gl_TexCoord[0].xyz + vec3(0.0,DELTA,0.0)).x;\n"
    "   a.z  = texture3D(volume, gl_TexCoord[0].xyz - vec3(0.0,0.0,DELTA)).x;\n"
    "   b.z  = texture3D(volume, gl_TexCoord[0].xyz + vec3(0.0,0.0,DELTA)).x;\n"
    "   vec3 gradient = b * 255.0 - a * 255.0;                     \n"
    "   float c = log2(dot(gradient, gradient))/log2(maximumGradient);     \n"
    "   c = 1.0 - c;                                               \n"
    "   vec2 coordinate = vec2(x,c);                               \n"
    "   vec4 color = texture2D(colormap2D, coordinate);            \n"
    "   gl_FragColor = color;                                      \n"
    "}                                                           \n\0";

/* fragmentShader2DWithLighting */
static const char* fragmentShader2DWithLighting = "#define DELTA 0.01                                            \n"
    "#define THRESHOLD 0.1                                         \n"
    "uniform sampler3D volume;                                     \n"
    "uniform sampler2D colormap2D;                                 \n"
    "uniform float maximumGradient;                                \n"
    "uniform vec4 lightPosition;                                   \n"
    "uniform vec4 eyePosition;                                     \n"
    "uniform vec3 ambientColor;                                    \n"
    "uniform vec3 diffuseColor;                                    \n"
    "uniform vec3 specularColor;                                   \n"
    "uniform vec3 kAmbient;                                        \n"
    "uniform vec3 kDiffuse;                                        \n"
    "uniform vec3 kSpecular;                                       \n"
    "uniform float shininess;                                      \n"
    "uniform float normalContribution;                             \n"
    "uniform float toneContribution;                               \n"
    "uniform vec3 coolColor;                                       \n"
    "uniform vec3 warmColor;                                       \n"
    "uniform float toonContribution;                               \n"
    "uniform vec3 toonColor;                                       \n"
    "uniform float boundaryContribution;                           \n"
    "uniform float boundaryExponent;                               \n"
    "uniform float silhouetteContribution;                         \n"
    "uniform float silhouetteExponent;                             \n"
    "uniform float edgeContribution;                               \n"
    "uniform float edgeExponent;                                   \n"
    "uniform float edgeThreshold;                                  \n"
    "                                                              \n"
    "vec3 normalShading(vec3 N, vec3 V, vec3 L, float diffuseLight)\n"
    "{                                                             \n"
    "// calculate halfway vector                                   \n"
    "    vec3 H = normalize(L + V);                                \n"
    "// compute the ambient term                                   \n"
    "    vec3 ambient = kAmbient * ambientColor;                   \n"
    "// compute the diffuse term                                   \n"
    "    vec3 diffuse = kDiffuse * diffuseColor * diffuseLight;    \n"
    "// compute the specular term                                  \n"
    "    float specularLight = pow(max(dot(H, N), 0.0), shininess);\n"
    "    if (specularLight <= 0.0) specularLight = 0.0;            \n"
    "    vec3 specular = kSpecular * specularColor * specularLight;\n"
    "    return ambient + diffuse + specular;                      \n"
    "}                                                             \n"
    "                                                              \n"
    "vec3 toneShading(float diffuseLight)                          \n"
    "{                                                             \n"
    "    vec3 tone = (((1.0+diffuseLight)*0.5)*warmColor + (1.0-((1.0+diffuseLight)*0.5))*coolColor);\n"
    "    return tone;                                              \n"
    "}                                                             \n"
    "                                                              \n"
    "vec3 toonShading(float diffuseLight)                          \n"
    "{                                                             \n"
    "    vec3 color = toonColor - vec3(1.0, 1.0, 1.0);             \n"
    "    vec3 color1 = vec3(toonColor - .75*color);                \n"
    "    vec3 color2 = vec3(toonColor - .50*color);                \n"
    "    vec3 color3 = vec3(toonColor - .25*color);                \n"
    "    if (diffuseLight > 0.95) return color1;                   \n"
    "    else if (diffuseLight > 0.50) return color2;              \n"
    "    else if (diffuseLight > 0.25) return color3;              \n"
    "    else return toonColor;                                    \n"
    "}                                                             \n"
    "                                                              \n"
    "float boundaryShading(float gradient)                         \n"
    "{                                                             \n"
    "    return pow(gradient,boundaryExponent);  \n"
    "}                                                             \n"
    "                                                              \n"
    "float silhouetteShading(float angleVN)                        \n"
    "{                                                             \n"
    "    return pow(1.0-abs(angleVN),silhouetteExponent);          \n"
    "}                                                             \n"
    "                                                              \n"
    "vec3 edgeShading(vec3 color, float angleVN)                   \n"
    "{                                                             \n"
    "    float edgeValue = pow(1.0-abs(angleVN),edgeExponent);     \n"
    "    if (edgeValue > edgeThreshold) {                          \n"
    "      float weight = pow((edgeValue-edgeThreshold)/(1.0-edgeThreshold),4.0);\n"
    "      return mix(color, vec3(0.0,0.0,0.0), weight); \n"
    "    }                                                         \n"
    "    return color;                                             \n"
    "}                                                             \n"
    "                                                              \n"
    "void main (void)                                              \n"
    "{                                                             \n"
    "   float x = texture3D(volume, gl_TexCoord[0].xyz).x;         \n"
    "   vec3 a, b;                                                 \n"
    "   a.x  = texture3D(volume, gl_TexCoord[0].xyz - vec3(DELTA,0.0,0.0)).x;\n"
    "   b.x  = texture3D(volume, gl_TexCoord[0].xyz + vec3(DELTA,0.0,0.0)).x;\n"
    "   a.y  = texture3D(volume, gl_TexCoord[0].xyz - vec3(0.0,DELTA,0.0)).x;\n"
    "   b.y  = texture3D(volume, gl_TexCoord[0].xyz + vec3(0.0,DELTA,0.0)).x;\n"
    "   a.z  = texture3D(volume, gl_TexCoord[0].xyz - vec3(0.0,0.0,DELTA)).x;\n"
    "   b.z  = texture3D(volume, gl_TexCoord[0].xyz + vec3(0.0,0.0,DELTA)).x;\n"
    "   vec3 gradient = b * 255.0 - a * 255.0;                     \n"
    "   float gradientMagnitude = log2(dot(gradient, gradient))/log2(maximumGradient);     \n"
    "   gradientMagnitude = 1.0 - gradientMagnitude;                                               \n"
    "   vec2 coordinate = vec2(x,gradientMagnitude);                               \n"
    "   vec4 color = texture2D(colormap2D, coordinate);            \n"
    "   if (color.a > THRESHOLD) {                                 \n"
    "     vec3 N = normalize(gradient);                              \n"
    "     vec3 L = normalize((gl_ModelViewProjectionMatrix * lightPosition).xyz - gl_TexCoord[1].xyz); \n"
    "     vec3 V = normalize((gl_ModelViewProjectionMatrix * eyePosition).xyz - gl_TexCoord[1].xyz);   \n"
    "     float diffuseLight = dot(L, N);                          \n"
    "     color.rgb += normalContribution * normalShading(N, V, L, diffuseLight);   \n"
    "     color.rgb += toneContribution * toneShading(diffuseLight);  \n"
    "     color.rgb += toonContribution * toonShading(diffuseLight);  \n"
    "     color.a += boundaryContribution * boundaryShading(1.0 - gradientMagnitude);\n"
    "     float angleVN = dot(V, N);                              \n"
    "     color.a += silhouetteContribution * silhouetteShading(angleVN);\n"
    "      if (edgeContribution > 0.0) {                           \n"
    "         color.rgb = edgeShading(color.rgb, angleVN);         \n"
    "      }                                                       \n"
    "   }                                                          \n"
    "   gl_FragColor = color;                                      \n"
    "}                                                           \n\0";

/* fragmentShaderWithLighting */
static const char* fragmentShaderWithLighting = "#define DELTA 0.01                                            \n"
    "#define THRESHOLD 0.1                                         \n"
    "uniform sampler3D volume;                                     \n"
    "uniform sampler1D colormap1D;                                 \n"
    "uniform vec4 lightPosition;                                   \n"
    "uniform vec4 eyePosition;                                     \n"
    "uniform vec3 ambientColor;                                    \n"
    "uniform vec3 diffuseColor;                                    \n"
    "uniform vec3 specularColor;                                   \n"
    "uniform vec3 kAmbient;                                        \n"
    "uniform vec3 kDiffuse;                                        \n"
    "uniform vec3 kSpecular;                                       \n"
    "uniform float shininess;                                      \n"
    "uniform float normalContribution;                             \n"
    "uniform float toneContribution;                               \n"
    "uniform vec3 coolColor;                                       \n"
    "uniform vec3 warmColor;                                       \n"
    "uniform float toonContribution;                               \n"
    "uniform vec3 toonColor;                                       \n"
    "uniform float boundaryContribution;                           \n"
    "uniform float boundaryExponent;                               \n"
    "uniform float silhouetteContribution;                         \n"
    "uniform float silhouetteExponent;                             \n"
    "uniform float edgeContribution;                               \n"
    "uniform float edgeExponent;                                   \n"
    "uniform float edgeThreshold;                                  \n"
    "                                                              \n"
    "vec3 normalShading(vec3 N, vec3 V, vec3 L, float diffuseLight)\n"
    "{                                                             \n"
    "// calculate halfway vector                                   \n"
    "    vec3 H = normalize(L + V);                                \n"
    "// compute the ambient term                                   \n"
    "    vec3 ambient = kAmbient * ambientColor;                   \n"
    "// compute the diffuse term                                   \n"
    "    vec3 diffuse = kDiffuse * diffuseColor * diffuseLight;    \n"
    "// compute the specular term                                  \n"
    "    float specularLight = pow(max(dot(H, N), 0.0), shininess);\n"
    "    if (specularLight <= 0.0) specularLight = 0.0;            \n"
    "    vec3 specular = kSpecular * specularColor * specularLight;\n"
    "    return ambient + diffuse + specular;                      \n"
    "}                                                             \n"
    "                                                              \n"
    "vec3 toneShading(float diffuseLight)                          \n"
    "{                                                             \n"
    "    vec3 tone = (((1.0+diffuseLight)*0.5)*warmColor + (1.0-((1.0+diffuseLight)*0.5))*coolColor);\n"
    "    return tone;                                              \n"
    "}                                                             \n"
    "                                                              \n"
    "vec3 toonShading(float diffuseLight)                          \n"
    "{                                                             \n"
    "    vec3 color = toonColor - vec3(1.0, 1.0, 1.0);             \n"
    "    vec3 color1 = vec3(toonColor - .75*color);                \n"
    "    vec3 color2 = vec3(toonColor - .50*color);                \n"
    "    vec3 color3 = vec3(toonColor - .25*color);                \n"
    "    if (diffuseLight > 0.95) return color1;                   \n"
    "    else if (diffuseLight > 0.50) return color2;              \n"
    "    else if (diffuseLight > 0.25) return color3;              \n"
    "    else return toonColor;                                    \n"
    "}                                                             \n"
    "                                                              \n"
    "float boundaryShading(float gradient)                         \n"
    "{                                                             \n"
    "    return pow(gradient,boundaryExponent);                    \n"
    "}                                                             \n"
    "                                                              \n"
    "float silhouetteShading(float angleVN)                        \n"
    "{                                                             \n"
    "    return pow(1.0-abs(angleVN),silhouetteExponent);          \n"
    "}                                                             \n"
    "                                                              \n"
    "vec3 edgeShading(vec3 color, float angleVN)                   \n"
    "{                                                             \n"
    "    float edgeValue = pow(1.0-abs(angleVN),edgeExponent);     \n"
    "    if (edgeValue > edgeThreshold) {                          \n"
    "      float weight = pow((edgeValue-edgeThreshold)/(1.0-edgeThreshold),4.0);\n"
    "      return mix(color, vec3(0.0,0.0,0.0), weight); \n"
    "    }                                                         \n"
    "    return color;                                             \n"
    "}                                                             \n"
    "                                                              \n"
    "void main (void)                                              \n"
    "{                                                             \n"
    "   float x = texture3D(volume, gl_TexCoord[0].xyz).x;         \n"
    "   vec4 c = texture1D(colormap1D, x);                         \n"
    "   if (c.a > THRESHOLD) {                                     \n"
    "      vec3 sample1, sample2;                                  \n"
    "      sample1.x  = texture3D(volume, gl_TexCoord[0].xyz - vec3(DELTA,0.0,0.0)).x;\n"
    "      sample2.x  = texture3D(volume, gl_TexCoord[0].xyz + vec3(DELTA,0.0,0.0)).x;\n"
    "      sample1.y  = texture3D(volume, gl_TexCoord[0].xyz - vec3(0.0,DELTA,0.0)).y;\n"
    "      sample2.y  = texture3D(volume, gl_TexCoord[0].xyz + vec3(0.0,DELTA,0.0)).y;\n"
    "      sample1.z  = texture3D(volume, gl_TexCoord[0].xyz - vec3(0.0,0.0,DELTA)).z;\n"
    "      sample2.z  = texture3D(volume, gl_TexCoord[0].xyz + vec3(0.0,0.0,DELTA)).z;\n"
    "      vec3 N = normalize(sample2 - sample1);                  \n"
    "      vec3 L = normalize((gl_ModelViewProjectionMatrix * lightPosition).xyz - gl_TexCoord[1].xyz); \n"
    "      vec3 V = normalize((gl_ModelViewProjectionMatrix * eyePosition).xyz - gl_TexCoord[1].xyz);   \n"
    "      float diffuseLight = dot(L, N);                         \n"
    "      c.rgb += normalContribution * normalShading(N, V, L, diffuseLight);   \n"
    "      c.rgb += toneContribution * toneShading(diffuseLight);  \n"
    "      c.rgb += toonContribution * toonShading(diffuseLight);  \n"
    "      float gradient = dot(N, N);                             \n"
    "      c.a += boundaryContribution * boundaryShading(gradient);\n"
    "      float angleVN = dot(V, N);                              \n"
    "      c.a += silhouetteContribution * silhouetteShading(angleVN);\n"
    "      if (edgeContribution > 0.0) {                           \n"
    "         c.rgb = edgeShading(c.rgb, angleVN);                 \n"
    "      }                                                       \n"
    "   }                                                          \n"
    "   gl_FragColor = c;                                          \n"
    "}                                                           \n\0";

/* multiComponentFragmentShader */
static const char* multiComponentFragmentShader = "uniform sampler3D volume;                         \n"
    "uniform sampler1D colormap1D;                                 \n"
    "uniform float redScale;                                       \n"
    "uniform float greenScale;                                     \n"
    "uniform float blueScale;                                      \n"
    "                                                              \n"
    "void main (void)                                              \n"
    "{                                                             \n"
    "	vec3 c = texture3D(volume, vec3(gl_TexCoord[0])).xyz;      \n"
    "	float r = texture1D(colormap1D, c.x).x * redScale;         \n"
    "	float g = texture1D(colormap1D, c.y).y * greenScale;       \n"
    "	float b = texture1D(colormap1D, c.z).z * blueScale;        \n"
    "	vec4 y = vec4(r, g, b, r+g+b);                             \n"
    "	gl_FragColor = y;                                          \n"
    "}                                                           \n\0";

/* fragmentShaderWithLighting */
static const char* multiComponentFragmentShaderWithLighting = "#define DELTA 0.01                                            \n"
    "#define THRESHOLD 0.1                                         \n"
    "uniform sampler3D volume;                                     \n"
    "uniform sampler1D colormap1D;                                 \n"
    "uniform float redScale;                                       \n"
    "uniform float greenScale;                                     \n"
    "uniform float blueScale;                                      \n"
    "uniform vec4 lightPosition;                                   \n"
    "uniform vec4 eyePosition;                                     \n"
    "uniform vec3 ambientColor;                                    \n"
    "uniform vec3 diffuseColor;                                    \n"
    "uniform vec3 specularColor;                                   \n"
    "uniform vec3 kAmbient;                                        \n"
    "uniform vec3 kDiffuse;                                        \n"
    "uniform vec3 kSpecular;                                       \n"
    "uniform float shininess;                                      \n"
    "uniform float normalContribution;                             \n"
    "uniform float toneContribution;                               \n"
    "uniform vec3 coolColor;                                       \n"
    "uniform vec3 warmColor;                                       \n"
    "uniform float toonContribution;                               \n"
    "uniform vec3 toonColor;                                       \n"
    "uniform float boundaryContribution;                           \n"
    "uniform float boundaryExponent;                               \n"
    "uniform float silhouetteContribution;                         \n"
    "uniform float silhouetteExponent;                             \n"
    "uniform float edgeContribution;                               \n"
    "uniform float edgeExponent;                                   \n"
    "uniform float edgeThreshold;                                  \n"
    "                                                              \n"
    "vec3 normalShading(vec3 N, vec3 V, vec3 L, float diffuseLight)\n"
    "{                                                             \n"
    "// calculate halfway vector                                   \n"
    "    vec3 H = normalize(L + V);                                \n"
    "// compute the ambient term                                   \n"
    "    vec3 ambient = kAmbient * ambientColor;                   \n"
    "// compute the diffuse term                                   \n"
    "    vec3 diffuse = kDiffuse * diffuseColor * diffuseLight;    \n"
    "// compute the specular term                                  \n"
    "    float specularLight = pow(max(dot(H, N), 0.0), shininess);\n"
    "    if (specularLight <= 0.0) specularLight = 0.0;            \n"
    "    vec3 specular = kSpecular * specularColor * specularLight;\n"
    "    return ambient + diffuse + specular;                      \n"
    "}                                                             \n"
    "                                                              \n"
    "vec3 toneShading(float diffuseLight)                          \n"
    "{                                                             \n"
    "    vec3 tone = (((1.0+diffuseLight)*0.5)*warmColor + (1.0-((1.0+diffuseLight)*0.5))*coolColor);\n"
    "    return tone;                                              \n"
    "}                                                             \n"
    "                                                              \n"
    "vec3 toonShading(float diffuseLight)                          \n"
    "{                                                             \n"
    "    vec3 color = toonColor - vec3(1.0, 1.0, 1.0);             \n"
    "    vec3 color1 = vec3(toonColor - .75*color);                \n"
    "    vec3 color2 = vec3(toonColor - .50*color);                \n"
    "    vec3 color3 = vec3(toonColor - .25*color);                \n"
    "    if (diffuseLight > 0.95) return color1;                   \n"
    "    else if (diffuseLight > 0.50) return color2;              \n"
    "    else if (diffuseLight > 0.25) return color3;              \n"
    "    else return toonColor;                                    \n"
    "}                                                             \n"
    "                                                              \n"
    "float boundaryShading(float gradient)                         \n"
    "{                                                             \n"
    "    return pow(gradient,boundaryExponent);                    \n"
    "}                                                             \n"
    "                                                              \n"
    "float silhouetteShading(float angleVN)                        \n"
    "{                                                             \n"
    "    return pow(1.0-abs(angleVN),silhouetteExponent);          \n"
    "}                                                             \n"
    "                                                              \n"
    "vec3 edgeShading(vec3 color, float angleVN)                   \n"
    "{                                                             \n"
    "    float edgeValue = pow(1.0-abs(angleVN),edgeExponent);     \n"
    "    if (edgeValue > edgeThreshold) {                          \n"
    "      float weight = pow((edgeValue-edgeThreshold)/(1.0-edgeThreshold),4.0);\n"
    "      return mix(color, vec3(0.0,0.0,0.0), weight); \n"
    "    }                                                         \n"
    "    return color;                                             \n"
    "}                                                             \n"
    "                                                              \n"
    "void main (void)                                              \n"
    "{                                                             \n"
    "   vec3 c = texture3D(volume, vec3(gl_TexCoord[0])).xyz;      \n"
    "   float r = texture1D(colormap1D, c.x).x * redScale;         \n"
    "   float g = texture1D(colormap1D, c.y).y * greenScale;       \n"
    "   float b = texture1D(colormap1D, c.z).z * blueScale;        \n"
    "   vec4 color = vec4(r, g, b, r+g+b);                         \n"
    "   if (color.a > THRESHOLD) {                                 \n"
    "      vec3 sample1, sample2;                                  \n"
    "      sample1.x  = texture3D(volume, gl_TexCoord[0].xyz - vec3(DELTA,0.0,0.0)).x;\n"
    "      sample2.x  = texture3D(volume, gl_TexCoord[0].xyz + vec3(DELTA,0.0,0.0)).x;\n"
    "      sample1.y  = texture3D(volume, gl_TexCoord[0].xyz - vec3(0.0,DELTA,0.0)).y;\n"
    "      sample2.y  = texture3D(volume, gl_TexCoord[0].xyz + vec3(0.0,DELTA,0.0)).y;\n"
    "      sample1.z  = texture3D(volume, gl_TexCoord[0].xyz - vec3(0.0,0.0,DELTA)).z;\n"
    "      sample2.z  = texture3D(volume, gl_TexCoord[0].xyz + vec3(0.0,0.0,DELTA)).z;\n"
    "      vec3 N = normalize(sample2 - sample1);                  \n"
    "      vec3 L = normalize((gl_ModelViewProjectionMatrix * lightPosition).xyz - gl_TexCoord[1].xyz); \n"
    "      vec3 V = normalize((gl_ModelViewProjectionMatrix * eyePosition).xyz - gl_TexCoord[1].xyz);   \n"
    "      float diffuseLight = dot(L, N);                         \n"
    "      color.rgb += normalContribution * normalShading(N, V, L, diffuseLight);   \n"
    "      color.rgb += toneContribution * toneShading(diffuseLight);  \n"
    "      color.rgb += toonContribution * toonShading(diffuseLight);  \n"
    "      float gradient = dot(N, N);                             \n"
    "      color.a += boundaryContribution * boundaryShading(gradient);\n"
    "      float angleVN = dot(V, N);                              \n"
    "      color.a += silhouetteContribution * silhouetteShading(angleVN);\n"
    "      if (edgeContribution > 0.0) {                           \n"
    "         color.rgb = edgeShading(color.rgb, angleVN);         \n"
    "      }                                                       \n"
    "   }                                                          \n"
    "   gl_FragColor = color;                                      \n"
    "}                                                           \n\0";

/* preIntegratedFragmentShader */
static const char* preIntegratedFragmentShader = "uniform sampler3D volume;                                       \n"
    "uniform sampler2D colormap2D;                                 \n"
    "                                                              \n"
    "void main (void)                                              \n"
    "{                                                             \n"
    "	float x = texture3D(volume, vec3(gl_TexCoord[0])).x;       \n"
    "	float y = texture3D(volume, vec3(gl_TexCoord[1])).y;       \n"
    "	vec2 coord = vec2(x, y);                                   \n"
    "	vec4 c = texture2D(colormap2D, coord);                     \n"
    "	gl_FragColor = c;                                          \n"
    "}                                                           \n\0";

/* preIntegratedFragmentShaderWithLighting */
static const char* preIntegratedFragmentShaderWithLighting = "#define DELTA 0.01                                            \n"
    "#define THRESHOLD 0.1                                         \n"
    "uniform sampler3D volume;                                     \n"
    "uniform sampler2D colormap2D;                                 \n"
    "uniform vec4 lightPosition;                                   \n"
    "uniform vec4 eyePosition;                                     \n"
    "uniform vec3 ambientColor;                                    \n"
    "uniform vec3 diffuseColor;                                    \n"
    "uniform vec3 specularColor;                                   \n"
    "uniform vec3 kAmbient;                                        \n"
    "uniform vec3 kDiffuse;                                        \n"
    "uniform vec3 kSpecular;                                       \n"
    "uniform float shininess;                                      \n"
    "uniform float normalContribution;                             \n"
    "uniform float toneContribution;                               \n"
    "uniform vec3 coolColor;                                       \n"
    "uniform vec3 warmColor;                                       \n"
    "uniform float toonContribution;                               \n"
    "uniform vec3 toonColor;                                       \n"
    "uniform float boundaryContribution;                           \n"
    "uniform float boundaryExponent;                               \n"
    "uniform float silhouetteContribution;                         \n"
    "uniform float silhouetteExponent;                             \n"
    "uniform float edgeContribution;                               \n"
    "uniform float edgeExponent;                                   \n"
    "uniform float edgeThreshold;                                  \n"
    "                                                              \n"
    "vec3 normalShading(vec3 N, vec3 V, vec3 L, float diffuseLight)\n"
    "{                                                             \n"
    "// calculate halfway vector                                   \n"
    "    vec3 H = normalize(L + V);                                \n"
    "// compute the ambient term                                   \n"
    "    vec3 ambient = kAmbient * ambientColor;                   \n"
    "// compute the diffuse term                                   \n"
    "    vec3 diffuse = kDiffuse * diffuseColor * diffuseLight;    \n"
    "// compute the specular term                                  \n"
    "    float specularLight = pow(max(dot(H, N), 0.0), shininess);\n"
    "    if (specularLight <= 0.0) specularLight = 0.0;            \n"
    "    vec3 specular = kSpecular * specularColor * specularLight;\n"
    "    return ambient + diffuse + specular;                      \n"
    "}                                                             \n"
    "                                                              \n"
    "vec3 toneShading(float diffuseLight)                          \n"
    "{                                                             \n"
    "    vec3 tone = (((1.0+diffuseLight)*0.5)*warmColor + (1.0-((1.0+diffuseLight)*0.5))*coolColor);\n"
    "    return tone;                                              \n"
    "}                                                             \n"
    "                                                              \n"
    "vec3 toonShading(float diffuseLight)                          \n"
    "{                                                             \n"
    "    vec3 color = toonColor - vec3(1.0, 1.0, 1.0);             \n"
    "    vec3 color1 = vec3(toonColor - .75*color);                \n"
    "    vec3 color2 = vec3(toonColor - .50*color);                \n"
    "    vec3 color3 = vec3(toonColor - .25*color);                \n"
    "    if (diffuseLight > 0.95) return color1;                   \n"
    "    else if (diffuseLight > 0.50) return color2;              \n"
    "    else if (diffuseLight > 0.25) return color3;              \n"
    "    else return toonColor;                                    \n"
    "}                                                             \n"
    "                                                              \n"
    "float boundaryShading(float gradient)                         \n"
    "{                                                             \n"
    "    return pow(gradient,boundaryExponent);  \n"
    "}                                                             \n"
    "                                                              \n"
    "float silhouetteShading(float angleVN)                        \n"
    "{                                                             \n"
    "    return pow(1.0-abs(angleVN),silhouetteExponent);          \n"
    "}                                                             \n"
    "                                                              \n"
    "vec3 edgeShading(vec3 color, float angleVN)                   \n"
    "{                                                             \n"
    "    float edgeValue = pow(1.0-abs(angleVN),edgeExponent);     \n"
    "    if (edgeValue > edgeThreshold) {                          \n"
    "      float weight = pow((edgeValue-edgeThreshold)/(1.0-edgeThreshold),4.0);\n"
    "      return mix(color, vec3(0.0,0.0,0.0), weight); \n"
    "    }                                                         \n"
    "    return color;                                             \n"
    "}                                                             \n"
    "                                                              \n"
    "void main (void)                                              \n"
    "{                                                             \n"
    "	float x = texture3D(volume, vec3(gl_TexCoord[0])).x;       \n"
    "	float y = texture3D(volume, vec3(gl_TexCoord[1])).y;       \n"
    "	vec2 coord = vec2(x, y);                                   \n"
    "	vec4 c = texture2D(colormap2D, coord);                     \n"
    "   if (c.a > THRESHOLD) {                                     \n"
    "      vec3 sample1, sample2;                                  \n"
    "      sample1.x  = texture3D(volume, gl_TexCoord[0].xyz - vec3(DELTA,0.0,0.0)).x;\n"
    "      sample2.x  = texture3D(volume, gl_TexCoord[0].xyz + vec3(DELTA,0.0,0.0)).x;\n"
    "      sample1.y  = texture3D(volume, gl_TexCoord[0].xyz - vec3(0.0,DELTA,0.0)).x;\n"
    "      sample2.y  = texture3D(volume, gl_TexCoord[0].xyz + vec3(0.0,DELTA,0.0)).x;\n"
    "      sample1.z  = texture3D(volume, gl_TexCoord[0].xyz - vec3(0.0,0.0,DELTA)).x;\n"
    "      sample2.z  = texture3D(volume, gl_TexCoord[0].xyz + vec3(0.0,0.0,DELTA)).x;\n"
    "      vec3 N = normalize(sample2 - sample1);                  \n"
    "      vec3 L = normalize((gl_ModelViewProjectionMatrix * lightPosition).xyz - gl_TexCoord[1].xyz); \n"
    "      vec3 V = normalize((gl_ModelViewProjectionMatrix * eyePosition).xyz - gl_TexCoord[1].xyz);   \n"
    "      float diffuseLight = dot(L, N);                         \n"
    "      c.rgb += normalContribution * normalShading(N, V, L, diffuseLight);   \n"
    "      c.rgb += toneContribution * toneShading(diffuseLight);  \n"
    "      c.rgb += toonContribution * toonShading(diffuseLight);  \n"
    "      float gradient = dot(N, N);                             \n"
    "      c.a += boundaryContribution * boundaryShading(gradient);\n"
    "      float angleVN = dot(V, N);                              \n"
    "      c.a += silhouetteContribution * silhouetteShading(angleVN);\n"
    "      if (edgeContribution > 0.0) {                           \n"
    "         c.rgb = edgeShading(c.rgb, angleVN);                 \n"
    "      }                                                       \n"
    "   }                                                          \n"
    "	gl_FragColor = c;                                          \n"
    "}                                                           \n\0";

/* preIntegratedMultiComponentFragmentShader */
static const char* preIntegratedMultiComponentFragmentShader = "uniform sampler3D volume;                         \n"
    "uniform sampler2D colormap2D;                                 \n"
    "uniform float redScale;                                       \n"
    "uniform float greenScale;                                     \n"
    "uniform float blueScale;                                      \n"
    "                                                              \n"
    "void main (void)                                              \n"
    "{                                                             \n"
    "	vec3 front = texture3D(volume, vec3(gl_TexCoord[0])).xyz;  \n"
    "	vec3 back = texture3D(volume, vec3(gl_TexCoord[1])).xyz;   \n"
    "	vec2 rcoord = vec2(front.x, back.x);                       \n"
    "	float r = texture2D(colormap2D, rcoord).x * redScale;      \n"
    "	vec2 gcoord = vec2(front.y, back.y);                       \n"
    "	float g = texture2D(colormap2D, gcoord).y * greenScale;    \n"
    "	vec2 bcoord = vec2(front.z, back.z);                       \n"
    "	float b = texture2D(colormap2D, bcoord).z * blueScale;     \n"
    "	vec4 c = vec4(r, g, b, r+g+b);                             \n"
    "	gl_FragColor = c;                                          \n"
    "}                                                           \n\0";

/* preIntegratedFragmentShaderWithLighting */
static const char* preIntegratedMultiComponentFragmentShaderWithLighting = "#define DELTA 0.01                                            \n"
    "#define THRESHOLD 0.1                                         \n"
    "uniform sampler3D volume;                                     \n"
    "uniform sampler2D colormap2D;                                 \n"
    "uniform float redScale;                                       \n"
    "uniform float greenScale;                                     \n"
    "uniform float blueScale;                                      \n"
    "uniform vec4 lightPosition;                                   \n"
    "uniform vec4 eyePosition;                                     \n"
    "uniform vec3 ambientColor;                                    \n"
    "uniform vec3 diffuseColor;                                    \n"
    "uniform vec3 specularColor;                                   \n"
    "uniform vec3 kAmbient;                                        \n"
    "uniform vec3 kDiffuse;                                        \n"
    "uniform vec3 kSpecular;                                       \n"
    "uniform float shininess;                                      \n"
    "uniform float normalContribution;                             \n"
    "uniform float toneContribution;                               \n"
    "uniform vec3 coolColor;                                       \n"
    "uniform vec3 warmColor;                                       \n"
    "uniform float toonContribution;                               \n"
    "uniform vec3 toonColor;                                       \n"
    "uniform float boundaryContribution;                           \n"
    "uniform float boundaryExponent;                               \n"
    "uniform float silhouetteContribution;                         \n"
    "uniform float silhouetteExponent;                             \n"
    "uniform float edgeContribution;                               \n"
    "uniform float edgeExponent;                                   \n"
    "uniform float edgeThreshold;                                  \n"
    "                                                              \n"
    "vec3 normalShading(vec3 N, vec3 V, vec3 L, float diffuseLight)\n"
    "{                                                             \n"
    "// calculate halfway vector                                   \n"
    "    vec3 H = normalize(L + V);                                \n"
    "// compute the ambient term                                   \n"
    "    vec3 ambient = kAmbient * ambientColor;                   \n"
    "// compute the diffuse term                                   \n"
    "    vec3 diffuse = kDiffuse * diffuseColor * diffuseLight;    \n"
    "// compute the specular term                                  \n"
    "    float specularLight = pow(max(dot(H, N), 0.0), shininess);\n"
    "    if (specularLight <= 0.0) specularLight = 0.0;            \n"
    "    vec3 specular = kSpecular * specularColor * specularLight;\n"
    "    return ambient + diffuse + specular;                      \n"
    "}                                                             \n"
    "                                                              \n"
    "vec3 toneShading(float diffuseLight)                          \n"
    "{                                                             \n"
    "    vec3 tone = (((1.0+diffuseLight)*0.5)*warmColor + (1.0-((1.0+diffuseLight)*0.5))*coolColor);\n"
    "    return tone;                                              \n"
    "}                                                             \n"
    "                                                              \n"
    "vec3 toonShading(float diffuseLight)                          \n"
    "{                                                             \n"
    "    vec3 color = toonColor - vec3(1.0, 1.0, 1.0);             \n"
    "    vec3 color1 = vec3(toonColor - .75*color);                \n"
    "    vec3 color2 = vec3(toonColor - .50*color);                \n"
    "    vec3 color3 = vec3(toonColor - .25*color);                \n"
    "    if (diffuseLight > 0.95) return color1;                   \n"
    "    else if (diffuseLight > 0.50) return color2;              \n"
    "    else if (diffuseLight > 0.25) return color3;              \n"
    "    else return toonColor;                                    \n"
    "}                                                             \n"
    "                                                              \n"
    "float boundaryShading(float gradient)                         \n"
    "{                                                             \n"
    "    return pow(gradient,boundaryExponent);  \n"
    "}                                                             \n"
    "                                                              \n"
    "float silhouetteShading(float angleVN)                        \n"
    "{                                                             \n"
    "    return pow(1.0-abs(angleVN),silhouetteExponent);          \n"
    "}                                                             \n"
    "                                                              \n"
    "vec3 edgeShading(vec3 color, float angleVN)                   \n"
    "{                                                             \n"
    "    float edgeValue = pow(1.0-abs(angleVN),edgeExponent);     \n"
    "    if (edgeValue > edgeThreshold) {                          \n"
    "      float weight = pow((edgeValue-edgeThreshold)/(1.0-edgeThreshold),4.0);\n"
    "      return mix(color, vec3(0.0,0.0,0.0), weight); \n"
    "    }                                                         \n"
    "    return color;                                             \n"
    "}                                                             \n"
    "                                                              \n"
    "void main (void)                                              \n"
    "{                                                             \n"
    "  vec3 front = texture3D(volume, vec3(gl_TexCoord[0])).xyz;  \n"
    "   vec3 back = texture3D(volume, vec3(gl_TexCoord[1])).xyz;   \n"
    "   vec2 rcoord = vec2(front.x, back.x);                       \n"
    "   float r = texture2D(colormap2D, rcoord).x * redScale;      \n"
    "   vec2 gcoord = vec2(front.y, back.y);                       \n"
    "   float g = texture2D(colormap2D, gcoord).y * greenScale;    \n"
    "   vec2 bcoord = vec2(front.z, back.z);                       \n"
    "   float b = texture2D(colormap2D, bcoord).z * blueScale;     \n"
    "   vec4 c = vec4(r, g, b, r+g+b);                             \n"
    "   if (c.a > THRESHOLD) {                                     \n"
    "      vec3 sample1, sample2;                                  \n"
    "      sample1.x  = texture3D(volume, gl_TexCoord[0].xyz - vec3(DELTA,0.0,0.0)).x;\n"
    "      sample2.x  = texture3D(volume, gl_TexCoord[0].xyz + vec3(DELTA,0.0,0.0)).x;\n"
    "      sample1.y  = texture3D(volume, gl_TexCoord[0].xyz - vec3(0.0,DELTA,0.0)).x;\n"
    "      sample2.y  = texture3D(volume, gl_TexCoord[0].xyz + vec3(0.0,DELTA,0.0)).x;\n"
    "      sample1.z  = texture3D(volume, gl_TexCoord[0].xyz - vec3(0.0,0.0,DELTA)).x;\n"
    "      sample2.z  = texture3D(volume, gl_TexCoord[0].xyz + vec3(0.0,0.0,DELTA)).x;\n"
    "      vec3 N = normalize(sample2 - sample1);                  \n"
    "      vec3 L = normalize((gl_ModelViewProjectionMatrix * lightPosition).xyz - gl_TexCoord[1].xyz); \n"
    "      vec3 V = normalize((gl_ModelViewProjectionMatrix * eyePosition).xyz - gl_TexCoord[1].xyz);   \n"
    "      float diffuseLight = dot(L, N);                         \n"
    "      c.rgb += normalContribution * normalShading(N, V, L, diffuseLight);   \n"
    "      c.rgb += toneContribution * toneShading(diffuseLight);  \n"
    "      c.rgb += toonContribution * toonShading(diffuseLight);  \n"
    "      float gradient = dot(N, N);                             \n"
    "      c.a += boundaryContribution * boundaryShading(gradient);\n"
    "      float angleVN = dot(V, N);                              \n"
    "      c.a += silhouetteContribution * silhouetteShading(angleVN);\n"
    "      if (edgeContribution > 0.0) {                           \n"
    "         c.rgb = edgeShading(c.rgb, angleVN);                 \n"
    "      }                                                       \n"
    "   }                                                          \n"
    "   gl_FragColor = c;                                          \n"
    "}                                                           \n\0";

/* sliceFragmentShader */
static const char* sliceFragmentShader = "uniform sampler3D volume;                         \n"
    "uniform sampler1D sliceColormap1D;                            \n"
    "                                                              \n"
    "void main (void)                                              \n"
    "{                                                             \n"
    "	float x = texture3D(volume, vec3(gl_TexCoord[0])).x;       \n"
    "	vec4 color = texture1D(sliceColormap1D, x);                \n"
    "	gl_FragColor = color;                                      \n"
    "}                                                           \n\0";

/* sliceMultiComponentFragmentShader */
static const char* sliceMultiComponentFragmentShader = "uniform sampler3D volume;                           \n"
    "uniform sampler1D sliceColormap1D;                              \n"
    "uniform float redScale;                                         \n"
    "uniform float greenScale;                                       \n"
    "uniform float blueScale;                                        \n"
    "                                                                \n"
    "void main (void)                                                \n"
    "{                                                               \n"
    "	vec3 c = texture3D(volume, vec3(gl_TexCoord[0])).xyz;        \n"
    "	float r = texture1D(sliceColormap1D, c.x).x * redScale;      \n"
    "	float g = texture1D(sliceColormap1D, c.y).y * greenScale;    \n"
    "	float b = texture1D(sliceColormap1D, c.z).z * blueScale;     \n"
    "	vec4 y = vec4(r, g, b, r+g+b);                               \n"
    "	gl_FragColor = y;                                            \n"
    "}                                                             \n\0";

/* oppositeCorner */
static int oppositeCorner[8] = { 7, 6, 5, 4, 3, 2, 1, 0 };

/* edgeIndices */
static GLint edgeIndices[48] = { 0, 1, 1, 4, 4, 7, 1, 5, 1, 5, 0, 1, 1, 4, 4, 7, 0, 2, 2, 5, 5, 7, 2, 6, 2, 6, 0, 2, 2, 5, 5, 7, 0,
        3, 3, 6, 6, 7, 3, 4, 3, 4, 0, 3, 3, 6, 6, 7 };

/* sequenceIndices */
static GLint sequenceIndices[64] = { 0, 1, 2, 4, 5, 3, 6, 7, 1, 3, 0, 5, 7, 2, 4, 6, 2, 0, 3, 6, 4, 1, 7, 5, 3, 2, 1, 7, 6, 0, 5,
        4, 4, 5, 0, 6, 7, 1, 2, 3, 5, 7, 1, 4, 6, 3, 0, 2, 6, 4, 2, 7, 5, 0, 3, 1, 7, 6, 3, 5, 4, 2, 1, 0 };

// begin Forward Declarations
class Brick;
class ComparableBrick;
class ComparableOcNode;
class Matrix4x4;
class Point4;
class ShaderManager;
class ShaderObject;
class Vector4;
class Volume;
// end Forward Declarations

class Scene: public GLObject {
protected:
    struct DataItem: public GLObject::DataItem {
        GLuint colormap1DName;
        GLuint colormap2DName;
        GLuint sliceColormap1DName;
        ShaderManager* shaderManager;
        ShaderObject* volumeShaderObjectFocusAndContext;
        ShaderObject* volumeShaderObject;
        ShaderObject* volumeShaderObject2D;
        ShaderObject* volumeShaderObject2DWithLighting;
        ShaderObject* volumeShaderObjectWithLighting;
        ShaderObject* preIntegratedVolumeShaderObject;
        ShaderObject* preIntegratedVolumeShaderObjectWithLighting;
        ShaderObject* sliceShaderObject;
        CFrustum cFrustum;
        bool textureNonPowerOfTwo;
        GLint maximum3DTextureSize;
        DataItem(void);
        ~DataItem(void);
    };
public:
    friend class DataItem;
    Scene(Volume* _volume, unsigned char* _colormap, float* _alpha, float * _alpha2_5D, float* _red, float* _green, float* _blue,
            unsigned char* _sliceColormap);
    ~Scene(void);
    Vector4* calculateSlicePlaneNormal(Matrix4x4* modelviewInverse) const;
    void clearSpheres(void);
    void createBricks(void);
    void display(GLContextData& glContextData) const;
    void drawBricks(GLContextData & glContextData, const CFrustum & cFrustum, ShaderObject * shaderObject, const std::vector<ComparableBrick> & brickList,
            float * point, int maximumIndex, int minimumIndex, Matrix4x4 * modelviewInverse) const;
    void drawGrid(DataItem* dataItem, const std::vector<ComparableBrick> & brickList) const;
    void drawOutline(void) const;
    void drawSlices(GLContextData & glContextData, DataItem* dataItem, const std::vector<ComparableBrick> & brickList) const;
    void drawSpheres(void) const;
    void drawVolume(GLContextData & glContextData, DataItem* dataItem, const std::vector<ComparableBrick> & brickList, float * point) const;
    void setAlphaChanged(bool _alphaChanged);
    void setAmbientColor(float r, float g, float b, float a);
    void setAnimating(bool _animating);
    void setBlueScale(float _blueScale);
    void setBoundaryContribution(float boundaryContribution);
    void setBoundaryExponent(float boundaryExponent);
    void setColorMapChanged(bool _colorMapChanged);
    void setCoolColor(float r, float g, float b);
    void setDiffuseColor(float r, float g, float b, float a);
    int getDimension(void) const;
    void setDimension(int dimension);
    void setDownSamplingChanged(bool downSamplingChanged);
    void setEdgeContribution(float edgeContribution);
    void setEdgeExponent(float edgeExponent);
    void setEdgeThreshold(float edgeThreshold);
    void setGreenScale(float _blueScale);
    bool getInteractive(void) const;
    void setInteractive(bool _interactiveShading);
    void setKAmbient(float r, float g, float b);
    void setKDiffuse(float r, float g, float b);
    void setKSpecular(float r, float g, float b);
    bool getLighting(void) const;
    void setLighting(bool lighting);
    void setLightPosition(float xLightPosition, float yLightPosition, float zLightPosition);
    void setMaximumPriorityQueueSize(int _maximumPriorityQueueSize);
    void setMaximumPriorityQueueSizeChanged(bool _maximumPriorityQueueSizeChanged);
    void setMaximumPriorityQueueTest(bool _maximumPriorityQueueTest);
    void setNormalContribution(float normalContribution);
    void setPerspective(bool _perspective);
    bool getPreintegrated(void) const;
    void setPreintegrated(bool preintegrated);
    void setPlaneNormal(float planeNormal[3]);
    void setPlaneOffset(float planeOffset);
    void setRatioOfVisibilityTest(bool _ratioOfVisibilityTest);
    void setRatioOfVisibilityThreshold(float _ratioOfVisibilityThreshold);
    void setRedScale(float _redScale);
    void setRenderingCost(float _renderingCost);
    void setRenderingCostFunctionTest(bool _renderingCostFunctionTest);
    void setRGBChanged(bool _rgbChanged);
    void setShininess(float shininess);
    void setShowBricks(bool _showBricks);
    void setShowDisplay(bool showDisplay);
    void setShowOcNodes(bool _showOcNodeLeaves);
    void setShowOutline(bool _showOutline);
    void setShowVolume(bool _showVolume);
    void setShowXSlice(bool _showXSlice);
    void setShowYSlice(bool _showYSlice);
    void setShowZSlice(bool _showZSlice);
    void setSilhouetteContribution(float silhouetteContribution);
    void setSilhouetteExponent(float silhouetteExponent);
    void setSliceColorMapChanged(bool _sliceColorMapChanged);
    void setSliceFactor(float _sliceFactor);
    void setSlicingScale(float slicingScale);
    void setSpecularColor(float r, float g, float b, float a);
    void setSphere(Geometry::Point<double,3> sphere);
    void setToneContribution(float toneContribution);
    void setToonContribution(float toonContribution);
    void setToonColor(float r, float g, float b);
    void setWarmColor(float r, float g, float b);
    void setXSlice(int _xSlice);
    void setXStep(int xStep);
    void setYSlice(int _ySlice);
    void setYStep(int yStep);
    void setZSlice(int _zSlice);
    void setZStep(int zStep);
    void initContext(GLContextData& glContextData) const;
    void initialize(void);
    void initialize1DColorMap(DataItem* dataItem) const;
    void initialize2DColorMap(DataItem* dataItem) const;
    void initialize1DSliceColorMap(DataItem* dataItem) const;
    void popGLState(GLContextData& glContextData) const;
    void pushGLState(GLContextData& glContextData) const;
    void sortBricks(float * point, std::vector<ComparableBrick> & brickList) const;
    std::string toString(void);
    void update1DColorMap(DataItem* dataItem) const;
    void update2DColorMap(DataItem* dataItem) const;
    void update1DSliceColorMap(DataItem* dataItem) const;
    void updateAlpha(float* _alpha);
    void updateAlpha2_5D(float * _alpha2_5D);
    void updateBlue(float* _blue);
    void updateColorMap(unsigned char* _colormap);
    void updateGreen(float* _green);
    void updateRed(float* _red);
    void updateRGB(float* _red, float* _green, float* _blue);
    void updateSliceColorMap(unsigned char* _sliceColormap);
private:
    float ambientColor[4];
    bool animating;
    float * alpha;
    float * alpha2_5D;
    bool alphaChanged;
    float* blue;
    float blueScale;
    float boundaryContribution;
    float boundaryExponent;
    Brick* bricks;
    unsigned char* colormap;
    bool colorMapChanged;
    float diffuseColor[4];
    int dimension; // one or two dimension transfer function
    float edgeContribution;
    float edgeExponent;
    float edgeThreshold;
    bool focusAndContext; // lighting
    float* green;
    float greenScale;
    bool interactive; // down sampled and not traversing octree
    float kAmbient[3];
    float kDiffuse[3];
    float kSpecular[3];
    bool lighting; // lighting
    float lightPosition[3];
    bool maximumPriorityQueueSizeChanged;
    bool maximumPriorityQueueTest;
    float normalContribution;
    int numberOfBricks;
    bool perspective;
    bool preintegrated;
    float planeNormal[3];
    float planeOffset;
    bool ratioOfVisibilityTest;
    float* red;
    float redScale;
    bool renderingCostFunctionTest;
    bool rgbChanged;
    bool showDisplay;
    bool showOutline;
    bool showVolume;
    unsigned char* sliceColormap;
    bool sliceColorMapChanged;
    typedef Geometry::Point<double,3> Point;
    float shininess;
    float silhouetteContribution;
    float silhouetteExponent;
    float specularColor[4];
    std::deque<Point> spheres;
    float toneContribution;
    float coolColor[3];
    float warmColor[3];
    float toonContribution;
    float toonColor[3];
    Volume* volume;
    void volumeShader(GLContextData & glContextData, DataItem * dataItem, const std::vector<ComparableBrick> & brickList, float * point, int maximumIndex,
            int minimumIndex, Vector4 * slicePlaneNormal, Matrix4x4 * modelviewInverse) const;
    void volumeShader2D(GLContextData & glContextData, DataItem * dataItem, const std::vector<ComparableBrick> & brickList, float * point, int maximumIndex,
            int minimumIndex, Vector4 * slicePlaneNormal, Matrix4x4 * modelviewInverse) const;
    void volumeShaderFocusAndContext(GLContextData & glContextData, DataItem * dataItem, const std::vector<ComparableBrick> & brickList, float * point,
                int maximumIndex, int minimumIndex, Vector4 * slicePlaneNormal, Matrix4x4 * modelviewInverse) const;
    void volumeShaderWithLighting(GLContextData & glContextData, DataItem * dataItem, const std::vector<ComparableBrick> & brickList, float * point,
            int maximumIndex, int minimumIndex, Vector4 * slicePlaneNormal, Matrix4x4 * modelviewInverse) const;
    void volumeShader2DWithLighting(GLContextData & glContextData, DataItem * dataItem, const std::vector<ComparableBrick> & brickList, float * point,
            int maximumIndex, int minimumIndex, Vector4 * slicePlaneNormal, Matrix4x4 * modelviewInverse) const;
    void volumeShaderPreIntegrated(GLContextData & glContextData, DataItem * dataItem, const std::vector<ComparableBrick> & brickList, float * point,
            int maximumIndex, int minimumIndex, Vector4 * slicePlaneNormal, Matrix4x4 * modelviewInverse) const;
    void volumeShaderPreIntegratedWithLighting(GLContextData & glContextData, DataItem * dataItem, const std::vector<ComparableBrick> & brickList, float * point,
            int maximumIndex, int minimumIndex, Vector4 * slicePlaneNormal, Matrix4x4 * modelviewInverse) const;
};

#endif /*SCENE_H_*/
