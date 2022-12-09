//
// Created by A on 16/11/2022.
//

#ifndef PPGSO_UFO_H
#define PPGSO_UFO_H

#include <memory>

#include <ppgso/ppgso.h>

#include "Renderable.h"
#include "SceneWindow.h"
#include "KeyPicture.h"

class Ufo final : public Renderable {
private:

    static std::unique_ptr<ppgso::Mesh> mesh;
    static std::unique_ptr<ppgso::Shader> shader;
    static std::unique_ptr<ppgso::Shader> shadow_shader;
    static std::unique_ptr<ppgso::Texture> texture;

    int i = 0;
    float base_time = 0;
    int period = 120;

    std::vector<KeyPicture> keys {
            KeyPicture(glm::vec3 {1000,88,1000}, glm::vec3{0,0,10}, 10),
            KeyPicture(glm::vec3 {0,88,0}, glm::vec3{0,0,0}, 20),
            KeyPicture(glm::vec3 {-20,88,-20}, glm::vec3{0,0,1}, 25),
            KeyPicture(glm::vec3 {40,88,40}, glm::vec3{0,0,-1}, 30),
            KeyPicture(glm::vec3 {-70,88,-70}, glm::vec3{0,0,0}, 35),
            KeyPicture(glm::vec3 {80,88,-80}, glm::vec3{0,0,-1}, 40),
            KeyPicture(glm::vec3 {-80,88,80}, glm::vec3{0,0,2}, 45),
            KeyPicture(glm::vec3 {-1000,88,-1000}, glm::vec3{0,0,10}, 55),
            };

public:
    Ufo();
    bool update(float dt, SceneWindow &scene) override;

    void render(SceneWindow &scene) override;
    void render_shadow(SceneWindow &scene, glm::mat4 lightSpaceMatrix) override;

private:
};

#endif //PPGSO_UFO_H
