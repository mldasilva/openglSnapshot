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

static glm::vec3 extractEulerAngles(const glm::mat4& mat) {
    glm::vec3 euler;

    // Extract the pitch (rotation around Y-axis)
    euler.y = std::asin(-mat[2][0]);

    if (std::cos(euler.y) > 0.0001) { // Avoid gimbal lock
        // Extract the yaw (rotation around Z-axis)
        euler.x = std::atan2(mat[2][1], mat[2][2]);
        // Extract the roll (rotation around X-axis)
        euler.z = std::atan2(mat[1][0], mat[0][0]);
    } else {
        // Gimbal lock case (cos(pitch) is close to zero)
        euler.x = std::atan2(-mat[1][2], mat[1][1]);
        euler.z = 0;
    }

    return euler;
}

static glm::vec3 calculateYawPitch(const glm::vec3& position, const glm::vec3& target) {
    // Calculate the direction vector
    glm::vec3 direction = glm::normalize(target - position);
    
    // Yaw: Rotation around the Y-axis (horizontal plane)
    float yaw = glm::atan(direction.z, direction.x); // atan2(z, x)
    
    // Pitch: Rotation around the X-axis (vertical plane)
    float pitch = glm::asin(direction.y); // asin(y), because direction is already normalized

    return glm::vec3(glm::degrees(pitch), glm::degrees(yaw), 0.0f); // Pitch, Yaw, Roll (roll usually not needed)
}

#endif