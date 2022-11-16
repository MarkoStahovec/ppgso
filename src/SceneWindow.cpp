#include "SceneWindow.h"

void SceneWindow::update(float dTime) {
    auto i = std::begin(Renderable_objects);
    while (i != std::end(Renderable_objects)) {
        // Update object and remove from list if needed
        auto obj = i->get();
        if (!obj->update(dTime, *this))
            i = Renderable_objects.erase(i);
        else
            ++i;
    }

    camera->update();
}

void SceneWindow::render() {
    // Simply render all objects
    for (auto &object: Renderable_objects) {
        object->render(*this);
    }
}


float SceneWindow::get_Y(float x, float z, unsigned char * heightMap){
    int i = (x+100)*19.2/2-1;
    int j = (z+100)*19.2/2-1;

    if(i < 0) i=0;
    if(j < 0) j=0;
    if(i >= 1920) i=0;
    if(j >= 1920) j=0;

    float y = heightMap[3 * (i * 1920 + j)];
    if (y>180) y *= 0.95;

    return y * 0.0809787;
}