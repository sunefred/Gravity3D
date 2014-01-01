//==============================================================================
//	
//	Vector templated class with specializations for:
//
//	Vector<T,2>
//	Vector<T,3>
//	Vector<T,4>
//
//==============================================================================

#ifndef MEM_VECTOR_H
#define MEM_VECTOR_H


//== INCLUDES ==================================================================

#include "MemPrerequisites.h"


//== NAMESPACES ================================================================

MEM_BEGIN_NAMESPACE


//== PRIMARY TEMPLATE ==========================================================

#include "MemVectorX.h"


//== TEMPLATE SPECIALIZATIONS ==================================================

// There are a few ways to do partial template specializations. Here we use
// a simple copy/paste paradigm to supply the specialized templates with the
// base class functionality. More sophisticated methods include the
// Curiously Recurring Template Pattern (CRTP) design pattern. See the homepage
// http://kaba.hilvi.org/pastel/techniques_specialization.htm for details.
//

#define S1 2
#include "MemVectorX.h"
#undef S1

#define S1 3
#include "MemVectorX.h"
#undef S1

#define S1 4
#include "MemVectorX.h"
#undef S1


//== TYPEDEFS ==================================================================

/** 2-element vectors */
typedef Vector<signed int, 2> Vec2i;
typedef Vector<unsigned int, 2> Vec2ui;
typedef Vector<float, 2> Vec2f;
typedef Vector<double, 2> Vec2d;

/** 3-element vectors */
typedef Vector<signed int, 3> Vec3i;
typedef Vector<unsigned int, 3> Vec3ui;
typedef Vector<float, 3> Vec3f;
typedef Vector<double, 3> Vec3d;

/** 4-element vectors */
typedef Vector<signed int, 4> Vec4i;
typedef Vector<unsigned int, 4> Vec4ui;
typedef Vector<float, 4> Vec4f;
typedef Vector<double, 4> Vec4d;


//==============================================================================
MEM_END_NAMESPACE
#endif
//==============================================================================