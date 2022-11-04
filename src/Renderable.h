#pragma once

#include <memory>
#include <list>
#include <map>

#include <glm/glm.hpp>


class SceneWindow;

class Renderable {
public:
    Renderable() = default;

    Renderable(const Renderable &) = default;

    Renderable(Renderable &&) = default;

    // Virtual destructor is needed for abstract interfaces
    virtual ~Renderable() = default;

    /// Render the object
    /// \param camera - Camera to use for rendering
    virtual void render(SceneWindow &scene) = 0;

    virtual bool update(float dTime, SceneWindow &scene) = 0;
    virtual std::pair<bool, glm::vec3> get_attributes() = 0;

    glm::vec3 position{0, 0, 0};
    glm::vec3 rotation{0, 0, 0};
    glm::vec3 scale{1, 1, 1};
    glm::mat4 modelMatrix{1};

protected:
    void updateModelMatrix();
};