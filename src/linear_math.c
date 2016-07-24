#include "../header/linear_math.h"

#define TAU 2.0 * M_PI
#define ONE_DEG_IN_RAD (2.0 * M_PI) / 360.0  // 0.017444444
#define ONE_RAD_IN_DEG 360.0 / (2.0 * M_PI)  // 57.2957795

#define VEC2_SIZE 2
#define VEC3_SIZE 3
#define VEC4_SIZE 4
#define MAT3_SIZE 9
#define MAT4_SIZE 16
#define VERSOR_SIZE 4

#define __LINEAR_MATH_DEBUG 1

/*-----------------------------DEGUB MACRO------------------------------------*/

// define some debug assertion helper functions
#if __LINEAR_MATH_DEBUG
inline static void vec2SizeCheck(const _FLOAT *v) {
  // printf("MACRO: vec2_size_check\n");
  assert(sizeof(_FLOAT) * VEC2_SIZE == sizeof(*v) * VEC2_SIZE);
}

inline static void vec3SizeCheck(const _FLOAT *v) {
  // printf("MACRO: vec3_size_check\n");
  assert(sizeof(_FLOAT) * VEC3_SIZE == sizeof(*v) * VEC3_SIZE);
}

inline static void vec4SizeCheck(const _FLOAT *v) {
  // printf("MACRO: vec4_size_check\n");
  assert(sizeof(_FLOAT) * VEC4_SIZE == sizeof(*v) * VEC4_SIZE);
}

inline static void mat3SizeCheck(const _FLOAT *m) {
  // printf("MACRO: mat3_size_check\n");
  assert(sizeof(_FLOAT) * MAT3_SIZE == sizeof(*m) * MAT3_SIZE);
}

inline static void mat4SizeCheck(const _FLOAT *m) {
  // printf("MACRO: mat4_size_check\n");
  assert(sizeof(_FLOAT) * MAT4_SIZE == sizeof(*m) * MAT4_SIZE);
}

inline static void versorSizeCheck(const _FLOAT *ve) {
  // printf("MACRO: versor_size_check\n");
  assert(sizeof(_FLOAT) * VERSOR_SIZE != sizeof(ve));
}

#define __vec2_size_check(expr) vec2SizeCheck(expr)
#define __vec3_size_check(expr) vec3SizeCheck(expr)
#define __vec4_size_check(expr) vec4SizeCheck(expr)
#define __mat3_size_check(expr) mat3SizeCheck(expr)
#define __mat4_size_check(expr) mat4SizeCheck(expr)
#define __versor_size_check(expr) versorSizeCheck(expr)

#define __assert_ptr(expr) assert(expr)
#else
#define __vec2_size_check(expr) NULL
#define __vec3_size_check(expr) NULL
#define __vec4_size_check(expr) NULL
#define __mat3_size_check(expr) NULL
#define __mat4_size_check(expr) NULL
#define __versor_size_check(expr) NULL

#define __assert_ptr(expr) NULL
#endif

/*-----------------------------PRINT FUNCTIONS--------------------------------*/

void printVec2(const _FLOAT *v) {
  __vec2_size_check(v);
  printf("[%.4f, %.4f]\n", v[0], v[1]);
}

void printVec3(const _FLOAT *v) {
  __vec3_size_check(v);
  printf("[%.4f, %.4f, %.4f]\n", v[0], v[1], v[2]);
}

void printVec4(const _FLOAT *v) {
  __vec4_size_check(v);
  printf("[%.4f, %.4f, %.4f, %.4f]\n", v[0], v[1], v[2], v[3]);
}

void printMat3(const _FLOAT *m) {
  __mat3_size_check(m);
  printf("\n");
  printf("[%.4f][%.4f][%.4f]\n", m[0], m[3], m[6]);
  printf("[%.4f][%.4f][%.4f]\n", m[1], m[4], m[7]);
  printf("[%.4f][%.4f][%.4f]\n", m[2], m[5], m[8]);
}

void printMat4(const _FLOAT *m) {
  __mat4_size_check(m);
  printf("\n");
  printf("[%.4f][%.4f][%.4f][%.4f]\n", m[0], m[4], m[8], m[12]);
  printf("[%.4f][%.4f][%.4f][%.4f]\n", m[1], m[5], m[9], m[13]);
  printf("[%.4f][%.4f][%.4f][%.4f]\n", m[2], m[6], m[10], m[14]);
  printf("[%.4f][%.4f][%.4f][%.4f]\n", m[3], m[7], m[11], m[15]);
}

void printVersor(const _FLOAT *q) {
  printf("[%.4f ,%.4f, %.4f, %.4f]\n", q[0], q[1], q[2], q[3]);
}

/*------------------------------VECTOR FUNCTIONS------------------------------*/

_FLOAT *zeroVec3() {
  _FLOAT *vc = malloc(sizeof(_FLOAT) * VEC3_SIZE);
  assert(vc);
  for (int i = 0; i < VEC3_SIZE; i++) {
    vc[i] = 0.0f;
  }
  return vc;
}

_FLOAT *zeroVec4() {
  _FLOAT *vc = malloc(sizeof(_FLOAT) * VEC4_SIZE);
  assert(vc);
  for (int i = 0; i < VEC4_SIZE; i++) {
    vc[i] = 0.0f;
  }
  return vc;
}

void vecFree(_FLOAT *v) {
  __assert_ptr(v);
  free(v);
  v = NULL;
}

_FLOAT *vec3New(_FLOAT x, _FLOAT y, _FLOAT z) {
  _FLOAT *v = zeroVec3();
  v[0] = x;
  v[1] = y;
  v[2] = z;
  return v;
}

_FLOAT vec3Length(const _FLOAT *v) {
  __vec3_size_check(v);
  return sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
}

// squared length
_FLOAT vec3Length2(const _FLOAT *v) {
  __vec3_size_check(v);
  return v[0] * v[0] + v[1] * v[1] + v[2] * v[2];
}

_FLOAT *vec3Normalize(_FLOAT *v) {
  __vec3_size_check(v);
  _FLOAT *rv = zeroVec3();
  _FLOAT l = vec3Length(v);
  if (0.0f == l) {
    v[0] = v[1] = v[2] = 0.0f;
  }
  rv[0] = v[0] / l;
  rv[1] = v[1] / l;
  rv[2] = v[2] / l;
  return rv;
}

_FLOAT *vec3Add(const _FLOAT *first, const _FLOAT *second) {
  __vec3_size_check(first);
  __vec3_size_check(second);
  _FLOAT *vc = zeroVec3();
  vc[0] = first[0] + second[0];
  vc[1] = first[1] + second[1];
  vc[2] = first[2] + second[2];
  return vc;
}

void vec3AddOver(_FLOAT *first, const _FLOAT *second) {
  __vec3_size_check(first);
  __vec3_size_check(second);
  first[0] += second[0];
  first[1] += second[1];
  first[2] += second[2];
}

_FLOAT *vec3Sub(const _FLOAT *first, const _FLOAT *second) {
  __vec3_size_check(first);
  __vec3_size_check(second);
  _FLOAT *vc = zeroVec3();
  vc[0] = first[0] - second[0];
  vc[1] = first[1] - second[1];
  vc[2] = first[2] - second[2];
  return vc;
}

void vec3SubOver(_FLOAT *first, const _FLOAT *second) {
  __vec3_size_check(first);
  __vec3_size_check(second);
  first[0] -= second[0];
  first[1] -= second[1];
  first[2] -= second[2];
}

_FLOAT *vec3AddFloat(const _FLOAT *v, _FLOAT num) {
  __vec3_size_check(v);
  _FLOAT *vc = zeroVec3();
  vc[0] = v[0] + num;
  vc[1] = v[1] + num;
  vc[2] = v[2] + num;
  return vc;
}

void vec3AddFloatOver(_FLOAT *v, _FLOAT num) {
  __vec3_size_check(v);
  _FLOAT *vc = zeroVec3();
  vc[0] += num;
  vc[1] += num;
  vc[2] += num;
}

_FLOAT *vec3SubFloat(const _FLOAT *v, _FLOAT num) {
  __vec3_size_check(v);
  _FLOAT *vc = zeroVec3();
  vc[0] = v[0] - num;
  vc[1] = v[1] - num;
  vc[2] = v[2] - num;
  return vc;
}

void vec3SubFloatOver(_FLOAT *v, _FLOAT num) {
  __vec3_size_check(v);
  _FLOAT *vc = zeroVec3();
  vc[0] -= num;
  vc[1] -= num;
  vc[2] -= num;
}

_FLOAT *vec3MultFloat(const _FLOAT *v, _FLOAT num) {
  __vec3_size_check(v);
  _FLOAT *vc = zeroVec3();
  vc[0] = v[0] * num;
  vc[1] = v[1] * num;
  vc[2] = v[2] * num;
  return vc;
}

_FLOAT *vec3DevFloat(const _FLOAT *v, _FLOAT num) {
  __vec3_size_check(v);
  assert(num);
  _FLOAT *vc = zeroVec3();
  vc[0] = v[0] / num;
  vc[1] = v[1] / num;
  vc[2] = v[2] / num;
  return vc;
}

void vec3MultFloatOver(_FLOAT *v, _FLOAT num) {
  __vec3_size_check(v);
  _FLOAT *vc = zeroVec3();
  vc[0] *= num;
  vc[1] *= num;
  vc[2] *= num;
}

void vec3DevFloatOver(_FLOAT *v, _FLOAT num) {
  __vec3_size_check(v);
  assert(num);
  _FLOAT *vc = zeroVec3();
  vc[0] /= num;
  vc[1] /= num;
  vc[2] /= num;
}

_FLOAT *vec3Copy(const _FLOAT *source) {
  __vec3_size_check(source);
  _FLOAT *vc = zeroVec3();
  for (int i = 0; i < VEC3_SIZE; i++) {
    vc[i] = source[i];
  }
  return vc;
}

_FLOAT vec3Dot(const _FLOAT *a, const _FLOAT *b) {
  __vec3_size_check(a);
  __vec3_size_check(b);
  return a[0] * b[0] + a[1] * b[1] + a[2] * b[2];
}

_FLOAT *vec3Cross(const _FLOAT *a, const _FLOAT *b) {
  __vec3_size_check(a);
  __vec3_size_check(b);
  _FLOAT *vc = zeroVec3();
  vc[0] = a[1] * b[2] - a[2] * b[1];
  vc[1] = a[2] * b[0] - a[0] * b[2];
  vc[2] = a[0] * b[1] - a[1] * b[0];
  return vc;
}

_FLOAT getSquaredDist(const _FLOAT *from, const _FLOAT *to) {
  __vec3_size_check(from);
  __vec3_size_check(to);
  _FLOAT x = (to[0] - from[0]) * (to[0] - from[0]);
  _FLOAT y = (to[1] - from[1]) * (to[1] - from[1]);
  _FLOAT z = (to[2] - from[2]) * (to[2] - from[2]);
  return x + y + z;
}

_FLOAT distance(const _FLOAT *from, const _FLOAT *to) {
  __vec3_size_check(from);
  __vec3_size_check(to);
  _FLOAT x = (to[0] - from[0]) * (to[0] - from[0]);
  _FLOAT y = (to[1] - from[1]) * (to[1] - from[1]);
  _FLOAT z = (to[2] - from[2]) * (to[2] - from[2]);
  return sqrt(x + y + z);
}

//  converts an un-normalised direction into a heading in degrees
// Note: i suspect that the z is backwards here but i've used it in
// several places like this.
_FLOAT directionToHeading(const _FLOAT *d) {
  __vec3_size_check(d);
  return atan2(-d[0], -d[2]) * ONE_RAD_IN_DEG;
}

_FLOAT *headingToDirection(_FLOAT degrees) {
  _FLOAT rad = degrees * ONE_DEG_IN_RAD;
  _FLOAT *vc = zeroVec3();
  vc[0] = -sinf(rad);
  vc[1] = 0.0f;
  vc[2] = -cosf(rad);
  return vc;
  // return vec3(-sinf(rad), 0.0f, -cosf(rad));
}

/*-----------------------------MATRIX FUNCTIONS-------------------------------*/

_FLOAT *zeroMat3() {
  _FLOAT *mat = malloc(sizeof(_FLOAT) * MAT3_SIZE);
  assert(mat);
  for (int i = 0; i < MAT3_SIZE; i++) {
    mat[i] = 0.0f;
  }
  return mat;
}

_FLOAT *identityMat3() {
  _FLOAT *mat = zeroMat3();
  mat[0] = 1.0f;
  mat[4] = 1.0f;
  mat[8] = 1.0f;
  return mat;
  // return mat3(1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
}

_FLOAT *zeroMat4() {
  _FLOAT *mat = malloc(sizeof(_FLOAT) * MAT4_SIZE);
  assert(mat);
  for (int i = 0; i < MAT4_SIZE; i++) {
    mat[i] = 0.0f;
  }
  return mat;
}

_FLOAT *identityMat4() {
  _FLOAT *mat = zeroMat4();
  mat[0] = 1.0f;
  mat[5] = 1.0f;
  mat[10] = 1.0f;
  mat[15] = 1.0f;
  return mat;
  // return mat4(1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
  // 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);
}

void mat4ComponentsAssign(_FLOAT *target, const _FLOAT *source) {
  __mat4_size_check(target);
  __mat4_size_check(source);
  for (int i = 0; i < MAT4_SIZE; i++) {
    target[i] = source[i];
  }
}

void matFree(_FLOAT *m) {
  __assert_ptr(m);
  free(m);
  m = NULL;
}

/* mat4 array layout
 0  4  8 12
 1  5  9 13
 2  6 10 14
 3  7 11 15
*/

_FLOAT *mat4MulVec4(const _FLOAT *m, const _FLOAT *v) {
  // 0x + 4y + 8z + 12w
  __mat4_size_check(m);
  __vec4_size_check(v);
  _FLOAT *vc = zeroVec4();
  vc[0] = m[0] * v[0] + m[4] * v[1] + m[8] * v[2] + m[12] * v[3];
  // 1x + 5y + 9z + 13w
  vc[1] = m[1] * v[0] + m[5] * v[1] + m[9] * v[2] + m[13] * v[3];
  // 2x + 6y + 10z + 14w
  vc[2] = m[2] * v[0] + m[6] * v[1] + m[10] * v[2] + m[14] * v[3];
  // 3x + 7y + 11z + 15w
  vc[3] = m[3] * v[0] + m[7] * v[1] + m[11] * v[2] + m[15] * v[3];
  return vc;
}

_FLOAT *mat4MulMat4(const _FLOAT *first, const _FLOAT *second) {
  __mat4_size_check(first);
  __mat4_size_check(second);
  _FLOAT *mat = zeroMat4();
  int r_index = 0;
  for (int col = 0; col < 4; col++) {
    for (int row = 0; row < 4; row++) {
      _FLOAT sum = 0.0f;
      for (int i = 0; i < 4; i++) {
        // first * second is not equal to second * first
        // in this case, we use rows in first mat4 to multiply columns in the
        // seconds mat4
        sum += first[i + col * 4] * second[row + i * 4];
      }
      mat[r_index] = sum;
      r_index++;
    }
  }
  return mat;
}

_FLOAT *mat4Copy(const _FLOAT *m) {
  __mat4_size_check(m);
  _FLOAT *mat = zeroMat4();
  for (int i = 0; i < MAT4_SIZE; i++) {
    mat[i] = m[i];
  }
  return mat;
}

// returns a scalar value with the determinant for a 4x4 matrix see
// http://www.euclideanspace.com/maths/algebra/matrix/
// functions/determinant/fourD/index.htm for more detail
_FLOAT determinant(const _FLOAT *m) {
  __mat4_size_check(m);
  _FLOAT dv = m[12] * m[9] * m[6] * m[3] - m[8] * m[13] * m[6] * m[3] -
              m[12] * m[5] * m[10] * m[3] + m[4] * m[13] * m[10] * m[3] +
              m[8] * m[5] * m[14] * m[3] - m[4] * m[9] * m[14] * m[3] -
              m[12] * m[9] * m[2] * m[7] + m[8] * m[13] * m[2] * m[7] +
              m[12] * m[1] * m[10] * m[7] - m[0] * m[13] * m[10] * m[7] -
              m[8] * m[1] * m[14] * m[7] + m[0] * m[9] * m[14] * m[7] +
              m[12] * m[5] * m[2] * m[11] - m[4] * m[13] * m[2] * m[11] -
              m[12] * m[1] * m[6] * m[11] + m[0] * m[13] * m[6] * m[11] +
              m[4] * m[1] * m[14] * m[11] - m[0] * m[5] * m[14] * m[11] -
              m[8] * m[5] * m[2] * m[15] + m[4] * m[9] * m[2] * m[15] +
              m[8] * m[1] * m[6] * m[15] - m[0] * m[9] * m[6] * m[15] -
              m[4] * m[1] * m[10] * m[15] + m[0] * m[5] * m[10] * m[15];
  return dv;
}

// returns a 16-element array that is the inverse of a 16-element array (4x4
// matrix). see
// http://www.euclideanspace.com/maths/algebra/matrix/functions/
// inverse/fourD/index.htm for more detail

_FLOAT *inverse(_FLOAT *m) {
  __mat4_size_check(m);
  _FLOAT det = determinant(m);
  // there is no inverse if determinant is zero (not likely unless scale is
  // broken)
  if (0.0f == det) {
    printf("WARNING. matrix has no determinant. can not invert\n");
    return m;
  }

  _FLOAT inv_det = 1.0f / det;
  _FLOAT *mat = zeroMat4();
  mat[0] = inv_det *
           (m[9] * m[14] * m[7] - m[13] * m[10] * m[7] + m[13] * m[6] * m[11] -
            m[5] * m[14] * m[11] - m[9] * m[6] * m[15] + m[5] * m[10] * m[15]);
  mat[1] = inv_det *
           (m[13] * m[10] * m[3] - m[9] * m[14] * m[3] - m[13] * m[2] * m[11] +
            m[1] * m[14] * m[11] + m[9] * m[2] * m[15] - m[1] * m[10] * m[15]);
  mat[2] = inv_det *
           (m[5] * m[14] * m[3] - m[13] * m[6] * m[3] + m[13] * m[2] * m[7] -
            m[1] * m[14] * m[7] - m[5] * m[2] * m[15] + m[1] * m[6] * m[15]);
  mat[3] = inv_det *
           (m[9] * m[6] * m[3] - m[5] * m[10] * m[3] - m[9] * m[2] * m[7] +
            m[1] * m[10] * m[7] + m[5] * m[2] * m[11] - m[1] * m[6] * m[11]);
  mat[4] = inv_det *
           (m[12] * m[10] * m[7] - m[8] * m[14] * m[7] - m[12] * m[6] * m[11] +
            m[4] * m[14] * m[11] + m[8] * m[6] * m[15] - m[4] * m[10] * m[15]);
  mat[5] = inv_det *
           (m[8] * m[14] * m[3] - m[12] * m[10] * m[3] + m[12] * m[2] * m[11] -
            m[0] * m[14] * m[11] - m[8] * m[2] * m[15] + m[0] * m[10] * m[15]);
  mat[6] = inv_det *
           (m[12] * m[6] * m[3] - m[4] * m[14] * m[3] - m[12] * m[2] * m[7] +
            m[0] * m[14] * m[7] + m[4] * m[2] * m[15] - m[0] * m[6] * m[15]);
  mat[7] = inv_det *
           (m[4] * m[10] * m[3] - m[8] * m[6] * m[3] + m[8] * m[2] * m[7] -
            m[0] * m[10] * m[7] - m[4] * m[2] * m[11] + m[0] * m[6] * m[11]);
  mat[8] = inv_det *
           (m[8] * m[13] * m[7] - m[12] * m[9] * m[7] + m[12] * m[5] * m[11] -
            m[4] * m[13] * m[11] - m[8] * m[5] * m[15] + m[4] * m[9] * m[15]);
  mat[9] = inv_det *
           (m[12] * m[9] * m[3] - m[8] * m[13] * m[3] - m[12] * m[1] * m[11] +
            m[0] * m[13] * m[11] + m[8] * m[1] * m[15] - m[0] * m[9] * m[15]);
  mat[10] = inv_det *
            (m[4] * m[13] * m[3] - m[12] * m[5] * m[3] + m[12] * m[1] * m[7] -
             m[0] * m[13] * m[7] - m[4] * m[1] * m[15] + m[0] * m[5] * m[15]);
  mat[11] = inv_det *
            (m[8] * m[5] * m[3] - m[4] * m[9] * m[3] - m[8] * m[1] * m[7] +
             m[0] * m[9] * m[7] + m[4] * m[1] * m[11] - m[0] * m[5] * m[11]);
  mat[12] = inv_det *
            (m[12] * m[9] * m[6] - m[8] * m[13] * m[6] - m[12] * m[5] * m[10] +
             m[4] * m[13] * m[10] + m[8] * m[5] * m[14] - m[4] * m[9] * m[14]);
  mat[13] = inv_det *
            (m[8] * m[13] * m[2] - m[12] * m[9] * m[2] + m[12] * m[1] * m[10] -
             m[0] * m[13] * m[10] - m[8] * m[1] * m[14] + m[0] * m[9] * m[14]);
  mat[14] = inv_det *
            (m[12] * m[5] * m[2] - m[4] * m[13] * m[2] - m[12] * m[1] * m[6] +
             m[0] * m[13] * m[6] + m[4] * m[1] * m[14] - m[0] * m[5] * m[14]);
  mat[15] = inv_det *
            (m[4] * m[9] * m[2] - m[8] * m[5] * m[2] + m[8] * m[1] * m[6] -
             m[0] * m[9] * m[6] - m[4] * m[1] * m[10] + m[0] * m[5] * m[10]);

  return mat;
}

// returns a 16-element array flipped on the main diagonal
_FLOAT *transpose(const _FLOAT *m) {
  __mat4_size_check(m);
  _FLOAT *mat = zeroMat4();
  mat[0] = m[0];
  mat[1] = m[4];
  mat[2] = m[8];
  mat[3] = m[12];
  mat[4] = m[1];
  mat[5] = m[5];
  mat[6] = m[9];
  mat[7] = m[13];
  mat[8] = m[2];
  mat[9] = m[6];
  mat[10] = m[10];
  mat[11] = m[14];
  mat[12] = m[3];
  mat[13] = m[7];
  mat[14] = m[11];
  mat[15] = m[15];
  return mat;
}

/*--------------------------AFFINE MATRIX FUNCTIONS---------------------------*/

// translate a 4d matrix with xyz array
_FLOAT *translate(const _FLOAT *m, const _FLOAT *v) {
  __mat4_size_check(m);
  __vec3_size_check(v);
  _FLOAT *mat = identityMat4();
  mat[12] = v[0];
  mat[13] = v[1];
  mat[14] = v[2];
  _FLOAT *rm = mat4MulMat4(mat, m);
  matFree(mat);
  return rm;
}

// rotate around x axis by an angle in degrees
_FLOAT *rotateXdeg(const _FLOAT *m, _FLOAT deg) {
  __mat4_size_check(m);
  // convert to radians
  _FLOAT rad = deg * ONE_DEG_IN_RAD;
  _FLOAT *m_r = identityMat4();
  m_r[5] = cos(rad);
  m_r[9] = -sin(rad);
  m_r[6] = sin(rad);
  m_r[10] = cos(rad);

  _FLOAT *rm = mat4MulMat4(m_r, m);
  matFree(m_r);
  return rm;
}

// rotate around y axis by an angle in degrees
_FLOAT *rotateYdeg(const _FLOAT *m, _FLOAT deg) {
  __mat4_size_check(m);
  // convert to radians
  _FLOAT rad = deg * ONE_DEG_IN_RAD;
  _FLOAT *m_r = identityMat4();
  m_r[0] = cos(rad);
  m_r[8] = sin(rad);
  m_r[2] = -sin(rad);
  m_r[10] = cos(rad);

  _FLOAT *rm = mat4MulMat4(m_r, m);
  matFree(m_r);
  return rm;
}

// rotate around z axis by an angle in degrees
_FLOAT *rotateZdeg(const _FLOAT *m, _FLOAT deg) {
  __mat4_size_check(m);
  // convert to radians
  _FLOAT rad = deg * ONE_DEG_IN_RAD;
  _FLOAT *m_r = identityMat4();
  m_r[0] = cos(rad);
  m_r[4] = -sin(rad);
  m_r[1] = sin(rad);
  m_r[5] = cos(rad);

  _FLOAT *rm = mat4MulMat4(m_r, m);
  matFree(m_r);
  return rm;
}

// scale a matrix by [x, y, z]
_FLOAT *scale(const _FLOAT *m, const _FLOAT *v) {
  __mat4_size_check(m);
  __vec3_size_check(v);
  _FLOAT *a = identityMat4();
  a[0] = v[0];
  a[5] = v[1];
  a[10] = v[2];

  _FLOAT *rm = mat4MulMat4(a, m);
  matFree(a);
  return rm;
}

/*-----------------------VIRTUAL CAMERA MATRIX FUNCTIONS----------------------*/

// returns a view matrix using the opengl lookAt style. COLUMN ORDER.
_FLOAT *lookAt(const _FLOAT *camPos, const _FLOAT *tarPos, const _FLOAT *up) {
  __vec3_size_check(camPos);
  __vec3_size_check(tarPos);
  __vec3_size_check(up);
  // inverse translation to make camera position at world origin
  _FLOAT *p = identityMat4();
  _FLOAT *q = translate(p, vec3New(-camPos[0], -camPos[1], -camPos[2]));
  // distance vector
  _FLOAT *d = vec3Sub(tarPos, camPos);
  // forward vector
  _FLOAT *f = vec3Normalize(d);
  // right vector
  _FLOAT *r = vec3Normalize(vec3Cross(f, up));
  // real up vector
  _FLOAT *u = vec3Normalize(vec3Cross(r, f));
  _FLOAT *ori = identityMat4();
  // note that the matrix is in cloumn major order
  ori[0] = r[0];
  ori[4] = r[1];
  ori[8] = r[2];
  ori[1] = u[0];
  ori[5] = u[1];
  ori[9] = u[2];
  ori[2] = -f[0];
  ori[6] = -f[1];
  ori[10] = -f[2];

  _FLOAT *rm = mat4MulMat4(ori, q);
  // clean up calculate resource
  matFree(p);
  matFree(q);
  vecFree(d);
  vecFree(f);
  vecFree(r);
  vecFree(u);
  matFree(ori);
  return rm;
  // return ori * p; // p * ori;
}

// returns a perspective function mimicking the opengl projection style.
_FLOAT *perspective(_FLOAT fovy, _FLOAT aspect, _FLOAT near, _FLOAT far) {
  _FLOAT fov_rad = fovy * ONE_DEG_IN_RAD;
  _FLOAT range = tan(fov_rad / 2.0f) * near;
  _FLOAT sx = (2.0f * near) / (range * aspect + range * aspect);
  _FLOAT sy = near / range;
  _FLOAT sz = -(far + near) / (far - near);
  _FLOAT pz = -(2.0f * far * near) / (far - near);
  _FLOAT *m = zeroMat4();  // make sure bottom-right corner is zero
  // note that the matrix is in cloumn major order
  m[0] = sx;
  m[5] = sy;
  m[10] = sz;
  m[14] = pz;
  m[11] = -1.0f;
  return m;
}

/*----------------------------HAMILTON FUNCTION-------------------------------*/

_FLOAT *zeroVersor() {
  _FLOAT *ve = malloc(sizeof(_FLOAT) * VERSOR_SIZE);
  assert(ve);
  for (int i = 0; i < VERSOR_SIZE; i++) {
    ve[i] = 0.0f;
  }
  return ve;
}

void versorFree(_FLOAT *ve) {
  __assert_ptr(ve);
  free(ve);
  ve = NULL;
}

_FLOAT *versorDevFloat(const _FLOAT *ve, _FLOAT num) {
  __versor_size_check(ve);
  assert(num);
  _FLOAT *result = zeroVersor();
  result[0] = ve[0] / num;
  result[1] = ve[1] / num;
  result[2] = ve[2] / num;
  result[3] = ve[3] / num;
  return result;
}

_FLOAT *versorMulFloat(const _FLOAT *ve, _FLOAT num) {
  __versor_size_check(ve);
  _FLOAT *result = zeroVersor();
  result[0] = ve[0] * num;
  result[1] = ve[1] * num;
  result[2] = ve[2] * num;
  result[3] = ve[3] * num;
  return result;
}

_FLOAT *versorNormalize(_FLOAT *q) {
  __versor_size_check(q);
  // norm(q) = q / magnitude (q)
  // magnitude (q) = sqrt (w*w + x*x...)
  // only compute sqrt if interior sum != 1.0
  _FLOAT sum = q[0] * q[0] + q[1] * q[1] + q[2] * q[2] + q[3] * q[3];
  // Note: floats have min 6 digits of precision
  const _FLOAT thresh = 0.0001f;
  if (fabs(1.0f - sum) < thresh) {
    return q;
  }
  _FLOAT mag = sqrt(sum);
  _FLOAT *rve = versorDevFloat(q, mag);
  return rve;
}

_FLOAT *versorMulVersor(const _FLOAT *first, const _FLOAT *second) {
  __versor_size_check(first);
  __versor_size_check(second);
  _FLOAT *result = zeroVersor();
  result[0] = first[0] * second[0] - first[1] * second[1] -
              first[2] * second[2] - first[3] * second[3];
  result[1] = first[0] * second[1] + first[1] * second[0] -
              first[2] * second[3] + first[3] * second[2];
  result[2] = first[0] * second[2] + first[1] * second[3] +
              first[2] * second[0] - first[3] * second[1];
  result[3] = first[0] * second[3] - first[1] * second[2] +
              first[2] * second[1] + first[3] * second[0];
  // re-normalise in case of mangling
  _FLOAT *rve = versorNormalize(result);
  versorFree(result);
  return rve;
}

_FLOAT *versorAdd(const _FLOAT *first, const _FLOAT *second) {
  __versor_size_check(first);
  __versor_size_check(second);
  _FLOAT *result = zeroVersor();
  result[0] = first[0] + second[0];
  result[1] = first[1] + second[1];
  result[2] = first[2] + second[2];
  result[3] = first[3] + second[3];
  // re-normalise in case of mangling
  _FLOAT *rve = versorNormalize(result);
  versorFree(result);
  return rve;
}

_FLOAT *quatFromAxisRad(_FLOAT radians, _FLOAT x, _FLOAT y, _FLOAT z) {
  _FLOAT *result = zeroVersor();
  result[0] = cos(radians / 2.0);
  result[1] = sin(radians / 2.0) * x;
  result[2] = sin(radians / 2.0) * y;
  result[3] = sin(radians / 2.0) * z;
  return result;
}

_FLOAT *quatFromAxisDeg(_FLOAT degrees, _FLOAT x, _FLOAT y, _FLOAT z) {
  return quatFromAxisRad(ONE_DEG_IN_RAD * degrees, x, y, z);
}

_FLOAT *quatToMat4(const _FLOAT *q) {
  __versor_size_check(q);
  _FLOAT w = q[0];
  _FLOAT x = q[1];
  _FLOAT y = q[2];
  _FLOAT z = q[3];

  _FLOAT *m = zeroMat4();
  m[0] = 1.0f - 2.0f * y * y - 2.0f * z * z;
  m[1] = 2.0f * x * y + 2.0f * w * z;
  m[2] = 2.0f * x * z - 2.0f * w * y;
  m[3] = 0.0f;
  m[4] = 2.0f * x * y - 2.0f * w * z;
  m[5] = 1.0f - 2.0f * x * x - 2.0f * z * z;
  m[6] = 2.0f * y * z + 2.0f * w * x;
  m[7] = 0.0f;
  m[8] = 2.0f * x * z + 2.0f * w * y;
  m[9] = 2.0f * y * z - 2.0f * w * x;
  m[10] = 1.0f - 2.0f * x * x - 2.0f * y * y;
  m[11] = 0.0f;
  m[12] = 0.0f;
  m[13] = 0.0f;
  m[14] = 0.0f;
  m[15] = 1.0;
  return m;
}

_FLOAT versorDot(const _FLOAT *q, const _FLOAT *r) {
  __versor_size_check(q);
  __versor_size_check(r);
  return q[0] * r[0] + q[1] * r[1] + q[2] * r[2] + q[3] * r[3];
}

_FLOAT *versorSlerp(_FLOAT *q, _FLOAT *r, _FLOAT t) {
  __versor_size_check(q);
  __versor_size_check(r);
  // angle between q0-q1
  _FLOAT cos_half_theta = versorDot(q, r);
  // as found here
  // http://stackoverflow.com/questions/2886606/flipping-issue-when-
  // interpolating-rotations-using-quaternions

  // if dot product is negative then one quaternion should be negated, to make
  // it take the short way around, rather than the long way
  // we had to recalculate the d.p. after this
  if (cos_half_theta < 0.0f) {
    for (int i = 0; i < 4; i++) {
      q[i] *= -1.0f;
    }
    cos_half_theta = versorDot(q, r);
  }
  // if qa=qb or qa=-qb then theta = 0 and we can return qa
  if (fabs(cos_half_theta) >= 1.0f) {
    return q;
  }
  // Calculate temporary values
  _FLOAT sin_half_theta = sqrt(1.0f - cos_half_theta * cos_half_theta);
  // if theta = 180 degrees then result is not fully defined
  // we could rotate around any axis normal to qa or qb
  _FLOAT *result = zeroVersor();
  if (fabs(sin_half_theta) < 0.001f) {
    for (int i = 0; i < 4; i++) {
      result[i] = (1.0f - t) * q[i] + t * r[i];
    }
    return result;
  }
  _FLOAT half_theta = acos(cos_half_theta);
  _FLOAT a = sin((1.0f - t) * half_theta) / sin_half_theta;
  _FLOAT b = sin(t * half_theta) / sin_half_theta;
  for (int i = 0; i < 4; i++) {
    result[i] = q[i] * a + r[i] * b;
  }
  return result;
}
