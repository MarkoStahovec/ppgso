//
// Created by A on 04/11/2022.
//

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Camera.h"
#include <glm/gtx/euler_angles.hpp>

Camera::Camera() {

    float fov = 60.0f, ratio = 1.0f, near = 0.1f, far = 50.0f;

    projectionMatrix = glm::perspective(ppgso::PI * fov / 180, ratio, near, far);
}

void Camera::update() {
    // TODO: Update viewMatrix (hint: glm::lookAt)
    viewMatrix = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
}