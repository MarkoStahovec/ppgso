#include <memory>

#include <ppgso/ppgso.h>

#include "Renderable.h"
#include "SceneWindow.h"

class Palm final : public Renderable {
private:

    static std::unique_ptr<ppgso::Mesh> mesh;
    static std::unique_ptr<ppgso::Shader> shader;
    static std::unique_ptr<ppgso::Texture> texture;

public:
    Palm(float x, float y, float z, glm::vec3 r, glm::vec3 s);

    bool update(float dt, SceneWindow &scene) override;
    void render(SceneWindow &scene) override;

private:
};
