//
// Created by rszar on 11. 11. 2022.
//

#include "Plant_3.h"
#include <shaders/diffuse_vert_glsl.h>
#include <shaders/diffuse_frag_glsl.h>

std::unique_ptr<ppgso::Mesh> Plant_3::mesh;
std::unique_ptr<ppgso::Texture> Plant_3::texture;
std::unique_ptr<ppgso::Shader> Plant_3::shader;
std::unique_ptr<ppgso::Shader> Plant_3::shadow_shader;


Plant_3::Plant_3(int x, int y, int z, std::string obj_name, std::string texture_name, float scale_factor) {
    scale *= scale_factor;
    rotation = {0,0,0};
    position.y = y;
    position.x = x;
    position.z = z;

    if (!shader) shader = std::make_unique<ppgso::Shader>(diffuse_vert_glsl, diffuse_frag_glsl);
    if (!texture) texture = std::make_unique<ppgso::Texture>(ppgso::image::loadBMP(texture_name));
    if (!mesh) mesh = std::make_unique<ppgso::Mesh>(obj_name);
    if (!shadow_shader) shadow_shader = std::make_unique<ppgso::Shader>(shadow_mapping_depth_vert_glsl, shadow_mapping_depth_frag_glsl);

}

bool Plant_3::update(float dt, SceneWindow &scene) {
    updateModelMatrix();
    return true;
}

void Plant_3::render(SceneWindow &scene) {
    shader->use();

    // use camera
    shader->setUniform("ProjectionMatrix", scene.camera->projectionMatrix);
    shader->setUniform("ViewMatrix", scene.camera->viewMatrix);
    shader->setUniform("LightDirection", normalize(glm::vec3{1.0f, 1.0f, 1.0f}));

    // render mesh
    shader->setUniform("ModelMatrix", modelMatrix);
    shader->setUniform("Texture", *texture);
    mesh->render();
}

void Plant_3::render_shadow(SceneWindow &scene, glm::mat4 lightSpaceMatrix) {

    shadow_shader->use();
    shadow_shader->setUniform("lightSpaceMatrix", lightSpaceMatrix);
    shadow_shader->setUniform("model", modelMatrix);
    mesh->render();
}
