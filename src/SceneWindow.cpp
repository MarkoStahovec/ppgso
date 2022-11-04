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