#pragma once
#include <memory>

#include <ppgso/ppgso.h>

#include "Renderable.h"
#include "SceneWindow.h"

class Cloud final : public Renderable {
private:

    static std::unique_ptr<ppgso::Mesh> mesh;
    static std::unique_ptr<ppgso::Shader> shader;
    static std::unique_ptr<ppgso::Texture> texture;
    static std::unique_ptr<ppgso::Shader> shadow_shader;

public:
    /*!
     * Create new asteroid
     */
    Cloud();

    /*!
     * Update asteroid
     * @param scene Scene to interact with
     * @param dt Time delta for animation purposes
     * @return
     */
    bool update(float dt, SceneWindow &scene) override;

    void render(SceneWindow &scene) override;

    void render_shadow(SceneWindow &scene, glm::mat4 lightSpaceMatrix) override;


private:
};