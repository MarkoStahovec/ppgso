//
// Created by rszar on 22. 11. 2022.
//

#include <memory>

#include <ppgso/ppgso.h>

#include "Renderable.h"
#include "SceneWindow.h"

class Firefly final : public Renderable {
private:
    glm::vec3 color;
    glm::vec3 speed;
    int i;
    static std::unique_ptr<ppgso::Mesh> mesh;
    static std::unique_ptr<ppgso::Shader> shader;
    static std::unique_ptr<ppgso::Texture> texture;


public:
    Firefly(glm::vec3 pos, glm::vec3 col, int light_index);
    bool update(float dt, SceneWindow &scene) override;

    void render(SceneWindow &scene) override;

    void check_speed();

    float gen_speed(bool randomize);

private:
};



