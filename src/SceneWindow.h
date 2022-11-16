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

    unsigned char * heightMap;

    void update(float time);
    void render();
    // 0-255 ~ 0-21
    float get_Y(float x, float z, unsigned char * heightMap);


};

#endif //PPGSO_SCENEWINDOW_H
