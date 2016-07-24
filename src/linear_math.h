#ifndef LINEAR_MATH_H
#define LINEAR_MATH_H

#include <stdio.h>
#include <math.h>
//#include <stdbool.h>
#include <stdlib.h>
#include <assert.h>

//#include "debug.h"

//#define TAU 2.0 * M_PI
//#define ONE_DEG_IN_RAD (2.0 * M_PI) / 360.0 // 0.017444444
//#define ONE_RAD_IN_DEG 360.0 / (2.0 * M_PI) // 57.2957795
//
//#define VEC2_SIZE 2
//#define VEC3_SIZE 3
//#define VEC4_SIZE 4
//#define MAT3_SIZE 9
//#define MAT4_SIZE 16
//#define VERSOR_SIZE 4

//typedef float *vec2;
//typedef float *vec3;
//typedef float *vec4;
//typedef float *mat3;
//typedef float *mat4;
//typedef float *versor;

#define _FLOAT double

/*---------------------------PRINT FUNCTIONS------------------------------*/

void printVec2(const _FLOAT *v);
void printVec3(const _FLOAT *v);
void printVec4(const _FLOAT *v);
void printMat3(const _FLOAT *m);
void printMat4(const _FLOAT *m);
void printVersor(const _FLOAT *q);

/*----------------------------VECTOR FUNCTIONS----------------------------*/

_FLOAT *zeroVec3();
_FLOAT *zeroVec4();
void vecFree(_FLOAT *v);
_FLOAT *newVec3(_FLOAT x, _FLOAT y, _FLOAT z);
_FLOAT vec3Length(const _FLOAT *v);
_FLOAT vec3LengthSquare(const _FLOAT *v);
_FLOAT *vec3Normalize(_FLOAT *v);
_FLOAT *vec3Add(const _FLOAT *first, const _FLOAT *second);
void vec3AddOver(_FLOAT *first, const _FLOAT *second);
_FLOAT *vec3Sub(const _FLOAT *first, const _FLOAT *second);
void vec3SubOver(_FLOAT *first, const _FLOAT *second);
_FLOAT *vec3AddFloat(const _FLOAT *v, _FLOAT num);
void vec3AddFloatOver(_FLOAT *v, _FLOAT num);
_FLOAT *vec3SubFloat(const _FLOAT *v, _FLOAT num);
void vec3SubFloatOver(_FLOAT *v, _FLOAT num);
_FLOAT *vec3MultFloat(const _FLOAT *v, _FLOAT num);
_FLOAT *vec3DevFloat(const _FLOAT *v, _FLOAT num);
void vec3MultFloatOver(_FLOAT *v, _FLOAT num);
void vec3DevFloatOver(_FLOAT *v, _FLOAT num);
_FLOAT *vec3Copy(const _FLOAT *source);
_FLOAT vec3Dot(const _FLOAT *a, const _FLOAT *b);
_FLOAT *vec3Cross(const _FLOAT *a, const _FLOAT *b);
_FLOAT getSquaredDist(const _FLOAT *from, const _FLOAT *to);
_FLOAT distance(const _FLOAT *from, const _FLOAT *to);

_FLOAT directionToHeading(const _FLOAT *d);
_FLOAT *headingToDirection(_FLOAT degrees);

/*-----------------------------MATRIX FUNCTIONS---------------------------*/

_FLOAT *zeroMat3();
_FLOAT *identityMat3();
_FLOAT *zeroMat4();
_FLOAT *identityMat4();
void mat4ComponentsAssign(_FLOAT *target, const _FLOAT *source);
void matFree(_FLOAT *m);
_FLOAT *mat4MulVec4(const _FLOAT *m, const _FLOAT *v);
_FLOAT *mat4MulMat4(const _FLOAT *first, const _FLOAT *second);
_FLOAT *mat4Copy(const _FLOAT *m);

_FLOAT determinant(const _FLOAT *m);
_FLOAT *inverse(_FLOAT *m);
_FLOAT *transpose(const _FLOAT *m);

/*--------------------------AFFINE MATRIX FUNCTIONS-----------------------*/

_FLOAT *translate(const _FLOAT *m, const _FLOAT *v);
_FLOAT *rotateXdeg(const _FLOAT *m, _FLOAT deg);
_FLOAT *rotateYdeg(const _FLOAT *m, _FLOAT deg);
_FLOAT *rotateZdeg(const _FLOAT *m, _FLOAT deg);
_FLOAT *scale(const _FLOAT *m, const _FLOAT *v);

/*----------------------VIRTUAL CAMERA MATRIX FUNCTIONS-------------------*/

_FLOAT *lookAt(const _FLOAT *camPos, const _FLOAT *tarPos, const _FLOAT *up);
_FLOAT *perspective(_FLOAT fovy, _FLOAT aspect, _FLOAT near, _FLOAT far);

/*--------------------------HAMILTON FUNCTION-----------------------------*/

_FLOAT *zeroVersor();
void versorFree(_FLOAT *ve);
_FLOAT *versorDevFloat(const _FLOAT *ve, _FLOAT num);
_FLOAT *versorMulFloat(const _FLOAT *ve, _FLOAT num);
_FLOAT *versorNormalize(_FLOAT *q);
_FLOAT *versorMulVersor(const _FLOAT *first, const _FLOAT *second);
_FLOAT *versorAdd(const _FLOAT *first, const _FLOAT *second);
_FLOAT *quatFromAxisRad(_FLOAT radians, _FLOAT x, _FLOAT y, _FLOAT z);
_FLOAT *quatFromAxisDeg(_FLOAT degrees, _FLOAT x, _FLOAT y, _FLOAT z);
_FLOAT *quatToMat4(const _FLOAT *q);
_FLOAT versorDot(const _FLOAT *q, const _FLOAT *r);
_FLOAT *versorSlerp(_FLOAT *q, _FLOAT *r, _FLOAT t);

#endif
