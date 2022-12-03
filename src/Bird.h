//
// Created by A on 16/11/2022.
//

#ifndef PPGSO_BIRD_H
#define PPGSO_BIRD_H

#include <memory>

#include <ppgso/ppgso.h>

#include "Renderable.h"
#include "SceneWindow.h"

class Bird final : public Renderable {
private:

    static std::unique_ptr<ppgso::Mesh> mesh;
    static std::unique_ptr<ppgso::Shader> shader;
    static std::unique_ptr<ppgso::Shader> shadow_shader;
    static std::unique_ptr<ppgso::Texture> texture;

public:
    Bird();
    bool update(float dt, SceneWindow &scene) override;

    void render(SceneWindow &scene) override;
    void render_shadow(SceneWindow &scene, glm::mat4 lightSpaceMatrix) override;

private:
};

#endif //PPGSO_BIRD_H
