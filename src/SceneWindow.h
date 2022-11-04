//
// Created by A on 04/11/2022.
//

#ifndef PPGSO_SCENEWINDOW_H
#define PPGSO_SCENEWINDOW_H

#include <memory>
#include <map>
#include <list>
#include "Renderable.h"
#include "Camera.h"

/*
 * Scene is an object that will aggregate all scene related data
 * Objects are stored in a list of objects
 * Keyboard and Mouse states are stored in a map and struct
 */
class SceneWindow {
public:
    //std::unique_ptr<Camera> camera;
    std::list<std::unique_ptr<Renderable>> Renderable_objects;

    std::unique_ptr<Camera> camera;

    void update(float time);
    void render();


};

#endif //PPGSO_SCENEWINDOW_H
