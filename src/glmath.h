#ifndef glmath_h
#define glmath_h

#include <math.h>
#include <glm.hpp>

#include <iostream>

// Overload the << operator
ostream& operator<<(std::ostream& os, const glm::vec2& vec) {
    os << "(" << vec.x << ", " << vec.y <<")";
    return os;
}

// Overload the << operator
std::ostream& operator<<(std::ostream& os, const glm::vec3& vec) {
    os << "(" << vec.x << ", " << vec.y << ", " << vec.z << ")";
    return os;
}

// Overload the << operator
std::ostream& operator<<(std::ostream& os, const glm::vec4& vec) {
    os << "(" << vec.x << ", " << vec.y << ", " << vec.z << ", " << vec.w << ")";
    return os;
}

#endif