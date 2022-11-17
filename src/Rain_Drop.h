

#include "Renderable.h"
#include "SceneWindow.h"

#include <shaders/color_vert_glsl.h>
#include <shaders/color_frag_glsl.h>

class Rain_Drop final : public Renderable {
    // Static resources shared between all particles
    std::unique_ptr<ppgso::Mesh> mesh;
    std::unique_ptr<ppgso::Shader> shader;


    // TODO: add more parameters as needed
    glm::vec3 speed;
    glm::vec3 gravity={0,-10,0};
    glm::vec3 color{0.55, 0.55, 0.85};
    bool droplet;

public:
    /// Construct a new Particle
    /// \param p - Initial position
    /// \param s - Initial speed
    /// \param c - Color of particle

    Rain_Drop(glm::vec3 p, glm::vec3 s, glm::vec3 c, glm::vec3 sc, bool is_droplet) {
        // First particle will initialize resources
        if (!shader) shader = std::make_unique<ppgso::Shader>(color_vert_glsl, color_frag_glsl);
        if (!mesh) mesh = std::make_unique<ppgso::Mesh>("sphere.obj");

        position = p;
        speed = s;
        color = c;
        scale = sc;
        droplet = is_droplet;
    }

    bool update(float dTime, SceneWindow &scene) override;

    void render(SceneWindow &scene) override;

};
