//==============================================================================
//	
//	Matrix templated class with specializations for:
//
//	Matrix<T,2,2>
//	Matrix<T,3,3>
//	Matrix<T,4,4>
//
//==============================================================================


#ifndef MEM_MATRIX_H
#define MEM_MATRIX_H


//== INCLUDES ==================================================================

#include "MemPrerequisites.h"
#include "newmat.h"
#include "newmatap.h"


//== NAMESPACES ================================================================

MEM_BEGIN_NAMESPACE


//== PRIMARY TEMPLATE ==========================================================

#include "MemMatrixXxY.h"


//== TEMPLATE SPECIALIZATIONS ==================================================

// There are a few ways to do partial template specializations. Here we use
// a simple copy/paste paradigm to supply the specialized templates with the
// base class functionality. More sophisticated methods include the
// Curiously Recurring Template Pattern (CRTP) design pattern. See the homepage
// http://kaba.hilvi.org/pastel/techniques_specialization.htm for details.
//

#define S1 2
#define S2 2
#include "MemMatrixXxY.h"
#undef S1
#undef S2

#define S1 3
#define S2 3
#include "MemMatrixXxY.h"
#undef S1
#undef S2

#define S1 4
#define S2 4
#include "MemMatrixXxY.h"
#undef S1
#undef S2


//== TYPEDEFS ==================================================================

/** 2x2-element matrices */
typedef Matrix<signed int,2,2> Mat2i;
typedef Matrix<unsigned int,2,2> Mat2ui;
typedef Matrix<float,2,2> Mat2f;
typedef Matrix<double,2,2> Mat2d;

/** 3x3-element matrices */
typedef Matrix<signed int,3,3> Mat3i;
typedef Matrix<unsigned int,3,3> Mat3ui;
typedef Matrix<float,3,3> Mat3f;
typedef Matrix<double,3,3> Mat3d;

/** 4x4-element matrices */
typedef Matrix<signed int,4,4> Mat4i;
typedef Matrix<unsigned int,4,4> Mat4ui;
typedef Matrix<float,4,4> Mat4f;
typedef Matrix<double,4,4> Mat4d;


//==============================================================================
MEM_END_NAMESPACE
#endif
//==============================================================================