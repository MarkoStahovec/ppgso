//
// Created by A on 14/11/2022.
//

#ifndef PPGSO_DOOR_H
#define PPGSO_DOOR_H

#include <memory>

#include <ppgso/ppgso.h>

#include "Renderable.h"
#include "SceneWindow.h"

class Door final : public Renderable {
private:

    static std::unique_ptr<ppgso::Mesh> mesh;
    static std::unique_ptr<ppgso::Shader> shader;
    static std::unique_ptr<ppgso::Texture> texture;

public:
    Door();

    bool update(float dt, SceneWindow &scene) override;
    void render(SceneWindow &scene) override;

private:
};

#endif //PPGSO_DOOR_H
