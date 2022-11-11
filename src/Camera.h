//
// Created by A on 04/11/2022.
//

#ifndef PPGSO_CAMERA_H
#define PPGSO_CAMERA_H

#pragma once

#include <memory>

#include <glm/glm.hpp>
#include <ppgso/ppgso.h>

class Camera {
public:
    // TODO: Add parameters
    glm::mat4 viewMatrix;
    glm::mat4 projectionMatrix;

    glm::vec3 cameraPos{2, 4, 5};
    glm::vec3 center{0, 0, 0};
    glm::vec3 upwards{0, 1, 0};

    glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f,  0.0f);
    //float cameraSpeed = 0.2;

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
