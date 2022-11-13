

#include "Renderable.h"
#include "SceneWindow.h"

#include <shaders/color_vert_glsl.h>
#include <shaders/color_frag_glsl.h>

class Rain_Drop final : public Renderable {
    // Static resources shared between all particles
    std::unique_ptr<ppgso::Mesh> mesh;
    std::unique_ptr<ppgso::Shader> shader;


    // TODO: add more parameters as needed
    glm::vec3 pos;
    glm::vec3 rotation;
    glm::vec3 scale = {.1,.1,.1};
    glm::vec3 speed;
    glm::vec3 color{0.55, 0.55, 0.85};

    glm::mat4 modelMatrix{1.f};
    float time;

public:
    /// Construct a new Particle
    /// \param p - Initial position
    /// \param s - Initial speed
    /// \param c - Color of particle

    Rain_Drop(glm::vec3 p, glm::vec3 s, glm::vec3 c, glm::vec3 sc, float t) {
        // First particle will initialize resources
        if (!shader) shader = std::make_unique<ppgso::Shader>(color_vert_glsl, color_frag_glsl);
        if (!mesh) mesh = std::make_unique<ppgso::Mesh>("sphere.obj");

        pos = p;
        speed = s;
        color = c;
        scale = sc;
        time = t;
    }

    bool update(float dTime, SceneWindow &scene) override;

    void render(SceneWindow &scene) override;


};
