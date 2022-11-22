//
// Created by A on 04/11/2022.
//

#ifndef PPGSO_SCENEWINDOW_H
#define PPGSO_SCENEWINDOW_H

#include <map>
#include <list>
#include "Renderable.h"
#include "Camera.h"

class SceneWindow {
public:
    //std::unique_ptr<Camera> camera;
    std::list<std::unique_ptr<Renderable>> Renderable_objects;

    std::unique_ptr<Camera> camera;
    // Keyboard state
    std::map< int, int > keyboard;

    float current_frame_time;
    float last_frame_time;

    glm::vec3 gravity = {0,10,0};
    glm::vec3 wind = {0,0,0};

    glm::vec3 globalLightDirection = {0.25, 0.2, 0.5};
    glm::vec3 globalLightColor = {0.42,0.45,0.85};
    glm::vec3 globalLightAmbient = {0.5,0.5,0.5};
    glm::vec3 globalLightDiffuse = {0.8f, 0.8, 0.8f};
    glm::vec3 globalLightSpecular = {0.15,0.15,0.25};

    struct Lights {
        glm::vec3 position[100];
        glm::vec3 direction[100];

        float cutOff[100];
        float outerCutOff[100];
        bool isSpot[100];

        glm::vec3 color[100];
        float intensity[100];
        float radius[100];

        glm::vec3 ambient[100];
        glm::vec3 diffuse[100];
        glm::vec3 specular[100];
    };

    Lights lights;

    unsigned char * heightMap;

    void update(float time);
    void render();
    // 0-255 ~ 0-21
    float get_Y(float x, float z, unsigned char * heightMap);


};

#endif //PPGSO_SCENEWINDOW_H
