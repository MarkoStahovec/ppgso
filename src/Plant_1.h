#include <memory>

#include <ppgso/ppgso.h>

#include "Renderable.h"
#include "SceneWindow.h"

class Plant_1 final : public Renderable {
private:

    static std::unique_ptr<ppgso::Mesh> mesh;
    static std::unique_ptr<ppgso::Shader> shader;
    static std::unique_ptr<ppgso::Texture> texture;

public:
    Plant_1(int x, int y, int z, std::string obj_name, std::string texture_name, float scale_factor);

    bool update(float dt, SceneWindow &scene) override;

    void render(SceneWindow &scene) override;

private:
};
