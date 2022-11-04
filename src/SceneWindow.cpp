#include "SceneWindow.h"

void SceneWindow::update(float time) {
    // Use iterator to update all objects so we can remove while iterating
    auto i = std::begin(Renderable_objects);

    while (i != std::end(Renderable_objects)) {
        // Update and remove from list if needed
        auto obj = i->get();
        if (!obj->update(time,*this))
            i = Renderable_objects.erase(i); // NOTE: no need to call destructors as we store shared pointers in the scene
        else
            ++i;
    }

    camera->update();
}

void SceneWindow::render() {
    // Simply render all objects
    for ( auto& obj : Renderable_objects )
        obj->render(*this);
}