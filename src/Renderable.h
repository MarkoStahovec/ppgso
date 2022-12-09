#pragma once

#include <memory>
#include <list>
#include <map>
#include <glm/glm.hpp>
#include "shader.h"
#include "shaders/shadow_mapping_depth_vert_glsl.h"
#include "shaders/shadow_mapping_depth_frag_glsl.h"


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
    virtual void render_shadow(SceneWindow &scene, glm::mat4 lightSpaceMatrix) = 0;

    virtual bool update(float dTime, SceneWindow &scene) = 0;

    glm::vec3 position{0, 0, 0};
    glm::vec3 rotation{0, 0, 0};
    glm::vec3 scale{1, 1, 1};
    glm::mat4 modelMatrix{1};

    //std::list<std::unique_ptr<Renderable>> children;
    Renderable * parent{nullptr};
    bool inherit = true;


protected:


    void updateModelMatrix();
    void reverseIslandBaseRotation();
    void reverseIslandBasePosition();
    void reverseIslandBaseScale();

};