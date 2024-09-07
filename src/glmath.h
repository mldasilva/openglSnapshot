#ifndef glmath_h
#define glmath_h

#include <math.h>
#include <glm.hpp> //v 1.0.1
#include <ext.hpp>

#include <iostream>
using namespace std;
using namespace glm;

struct vertex
{
	vec3 position;
	vec3 normal;
	vec2 texUV;
};

// Overload the << operator
static ostream& operator<<(ostream& os, const vec2& vec) {
    os << "(" << vec.x << ", " << vec.y <<")";
    return os;
}

// Overload the << operator
static ostream& operator<<(ostream& os, const vec3& vec) {
    os << "(" << vec.x << ", " << vec.y << ", " << vec.z << ")";
    return os;
}

// Overload the << operator
static ostream& operator<<(ostream& os, const vec4& vec) {
    os << "(" << vec.x << ", " << vec.y << ", " << vec.z << ", " << vec.w << ")";
    return os;
}

#endif