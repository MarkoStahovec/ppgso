//
// Created by A on 17/11/2022.
//

#ifndef PPGSO_LIGHT_H
#define PPGSO_LIGHT_H

#include <memory>

#include <ppgso/ppgso.h>

#include "Renderable.h"
#include "SceneWindow.h"

class Light final : public Renderable {
private:
    glm::vec3 color;
    static std::unique_ptr<ppgso::Mesh> mesh;
    static std::unique_ptr<ppgso::Shader> shader;
    static std::unique_ptr<ppgso::Texture> texture;
    static std::unique_ptr<ppgso::Shader> shadow_shader;

    bool only_night = false;
    int light_index;


public:
    Light(glm::vec3 pos, glm::vec3 rot, glm::vec3 col, float sc, bool night, int index);
    bool update(float dt, SceneWindow &scene) override;

    void render(SceneWindow &scene) override;
    void render_shadow(SceneWindow &scene, glm::mat4 lightSpaceMatrix) override;

private:
};

#endif //PPGSO_LIGHT_H
