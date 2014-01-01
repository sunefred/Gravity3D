//== INCLUDES ==================================================================

#include "MemMath.h"


//== NAMESPACES ================================================================

MEM_BEGIN_NAMESPACE


//== STATIC CONSTANTS (NONE TEMPLATE) ==========================================

const float Math::PI = 3.14159265359f; // single has only seven digit precision
const float Math::TWOPI = 6.28318530718f;
const float Math::HALFPI = 1.570796326795f;
const float Math::RAD2DEG = 180.0f/PI;
const float Math::DEG2RAD = PI/180.0f;

const float Math::POSINFF =  std::numeric_limits<float>::max();
const float Math::NEGINFF = -std::numeric_limits<float>::max();


//== IMPLEMENTATION ============================================================



//==============================================================================
MEM_END_NAMESPACE
//==============================================================================