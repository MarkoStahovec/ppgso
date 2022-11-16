//
// Created by A on 16/11/2022.
//

#ifndef PPGSO_FISH_H
#define PPGSO_FISH_H

#include <memory>

#include <ppgso/ppgso.h>

#include "Renderable.h"
#include "SceneWindow.h"

class Fish final : public Renderable {
private:

    static std::unique_ptr<ppgso::Mesh> mesh;
    static std::unique_ptr<ppgso::Shader> shader;
    static std::unique_ptr<ppgso::Texture> texture;

public:
    Fish();
    bool update(float dt, SceneWindow &scene) override;

    void render(SceneWindow &scene) override;

private:
};

#endif //PPGSO_FISH_H
