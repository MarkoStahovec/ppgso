//
// Created by A on 04/11/2022.
//

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Camera.h"
#include <glm/gtx/euler_angles.hpp>

Camera::Camera() {

    float fov = 60.0f, ratio = 1.0f, near = 0.1f, far = 500.0f;

    projectionMatrix = glm::perspective(ppgso::PI * fov / 180, ratio, near, far);
}

void Camera::update() {
    // TODO: Update viewMatrix (hint: glm::lookAt)
    if(isAnimating) {
        t += 0.1;
        cameraPos = glm::lerp(startPos, endPos, t / 30.0f);
        cameraFront = glm::lerp(startPosLookAt, endPosLookAt, t / 30.0f);
    }

    //viewMatrix = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
    viewMatrix = glm::lookAt(cameraPos, cameraFront, cameraUp);
}