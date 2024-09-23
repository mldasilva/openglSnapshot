#include "camera.h"

camera::camera(uint width, uint height)
{
    cout << "creating camera object" << endl;
    zoom = 50.0f;

    float halfWidth = (width / 2.0f) / zoom;
    float halfHeight = (height / 2.0f) / zoom;

    // Orthographic projection setup
    projection = glm::ortho(-halfWidth, halfWidth, -halfHeight, halfHeight, -1.0f, 100.0f);

    // View matrix setup (camera looking at the scene)
    position = glm::vec3(-15.0f, 15.0f, 15.0f);     // Camera position
    target = glm::vec3(0.0f, 0.0f, 0.0f);           // Look at the origin

    up = glm::vec3(0.0f, 1.0f, 0.0f);               // Up vector
    view = glm::lookAt(position, target, up);

    offset = position - target;
}

camera::~camera()
{
    cout << "deleting camera object" << endl;
}

void camera::rotate(float angleAddition)
{
    // Calculate the direction vector from the camera position to the target
    glm::vec3 direction = position - target;

    // Step 1: Apply rotation around the Y-axis
    // You can use glm::rotate to create a rotation matrix around the desired axis (e.g., Y-axis)
    glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(angleAddition), up);

    // Step 2: Apply the rotation to the direction vector
    glm::vec3 rotatedDirection = glm::vec3(rotationMatrix * glm::vec4(direction, 1.0f));

    // Step 3: Set the new camera position after rotation
    position = target + rotatedDirection;

    // Step 4: Create the view matrix using the new camera position
    view = glm::lookAt(position, target, up);

    // extra for calculating position when moving camera target to a vec3
    offset = position - target;
}

void camera::move(vec3 translate)
{
    target += translate;
    position += translate;
    view = glm::lookAt(position + translate, target + translate, up);
}

void camera::moveTo(vec3 target)
{
    camera::target = target;
    camera::position = target + offset;

    view = glm::lookAt(position, target, up);
}
