//
// Created by A on 14/11/2022.
//

#ifndef PPGSO_FIRE_H
#define PPGSO_FIRE_H

#include <memory>

#include <ppgso/ppgso.h>

#include "Renderable.h"
#include "SceneWindow.h"

class Fire final : public Renderable {
private:

    static std::unique_ptr<ppgso::Mesh> mesh;
    static std::unique_ptr<ppgso::Shader> shader;
    static std::unique_ptr<ppgso::Texture> texture;

public:
    /*!
     * Create new asteroid
     */
    Fire();

    /*!
     * Update asteroid
     * @param scene Scene to interact with
     * @param dt Time delta for animation purposes
     * @return
     */
    bool update(float dt, SceneWindow &scene) override;

    void render(SceneWindow &scene) override;

private:
};

#endif //PPGSO_FIRE_H
