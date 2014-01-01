//==============================================================================
//	
//	This is a quite basic linear algebra library. It focuses on the Matrix and
//	Vector math used in 3D applications. It implements inverse, determinant etc
//	for small dimensions and it has functions for creating transformation
//	matrices from translation, scale and rotation. And more.
//
//	The second function is a root solver for Polynomials using a stable
//	numeric method (bracket and bisect).
//
//	The third function is intersection tests for all common ray tracing tasks
//	such as line/triangle, line/mesh, line/sphere, line/aabox and more. These
//	functions heavily relies on the Polynomial root solver.
//
//==============================================================================

#ifndef MEM_H
#define MEM_H


//== INCLUDES ==================================================================

#include "MemMath.h"
#include "MemMatrix.h"
#include "MemPolynomial.h"
#include "MemVector.h"
#include "MemQuaternion.h"


//==============================================================================
#endif
//==============================================================================