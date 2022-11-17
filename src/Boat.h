//
// Created by A on 17/11/2022.
//

#ifndef PPGSO_BOAT_H
#define PPGSO_BOAT_H

#include <memory>

#include <ppgso/ppgso.h>

#include "Renderable.h"
#include "SceneWindow.h"

class Boat final : public Renderable {
private:

    static std::unique_ptr<ppgso::Mesh> mesh;
    static std::unique_ptr<ppgso::Shader> shader;
    static std::unique_ptr<ppgso::Texture> texture;

public:
    Boat();
    bool update(float dt, SceneWindow &scene) override;

    void render(SceneWindow &scene) override;

private:
};

#endif //PPGSO_BOAT_H
