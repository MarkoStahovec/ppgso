#include <memory>

#include <ppgso/ppgso.h>

#include "Renderable.h"
#include "SceneWindow.h"

class Coconut final : public Renderable {
private:

    static std::unique_ptr<ppgso::Mesh> mesh;
    static std::unique_ptr<ppgso::Shader> shader;
    static std::unique_ptr<ppgso::Texture> texture;
    static std::unique_ptr<ppgso::Shader> shadow_shader;


    int n_drop = 5;

    float base_x;
    float base_z;

    bool dropping = 0;
    bool spawning = 1;
    bool despawning = 0;

    float drop_dir_x = ((float) rand() / (float) RAND_MAX) * (12 - 4) + 4;
    float drop_dir_z = ((float) rand() / (float) RAND_MAX) * (12 - 4) + 4;
    float drop_dir_y = 6.66;

    glm::vec3 external_force = {drop_dir_x,
                                drop_dir_y,
                                drop_dir_z};

public:
    Coconut(float x, float y, float z);

    bool update(float dt, SceneWindow &scene) override;
    void drop(SceneWindow &scene);
    void bounce(SceneWindow &scene);
    void drop_chance();
    void handle_coll(SceneWindow &scene);
    glm::vec3 calc_plane_norm(SceneWindow &scene);
    void render(SceneWindow &scene) override;
    void render_shadow(SceneWindow &scene, glm::mat4 lightSpaceMatrix) override;

private:
};
