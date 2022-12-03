//
// Created by A on 04/11/2022.
//

#ifndef PPGSO_CAMERA_H
#define PPGSO_CAMERA_H

#pragma once

#include <memory>

#include <glm/glm.hpp>
#include <ppgso/ppgso.h>

const unsigned int WINDOW_HEIGHT = 1920;
const unsigned int WINDOW_WIDTH = 1080;

class Camera {
public:
    // TODO: Add parameters
    glm::mat4 viewMatrix;
    glm::mat4 projectionMatrix;

    glm::vec3 cameraPos{100, 100, 20};
    glm::vec3 cameraFront = glm::vec3(0.0f, -0.15f, -1.0f);
    glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f,  0.0f);
    //float cameraSpeed = 0.2;

    bool isAnimating = false;
    glm::vec3 startPos;
    glm::vec3 endPos;
    glm::vec3 startPosLookAt;
    glm::vec3 endPosLookAt;
    float t;

    /// Representaiton of
    /// \param fov - Field of view (in degrees)
    /// \param ratio - Viewport ratio (width/height)
    /// \param near - Distance of the near clipping plane
    /// \param far - Distance of the far clipping plane
    Camera();

    /// Recalculate viewMatrix from position, rotation and scale
    void update();
};

#endif //PPGSO_CAMERA_H
