//
// Created by rszar on 11. 11. 2022.
//

#include "Plant.h"
#include <shaders/diffuse_vert_glsl.h>
#include <shaders/diffuse_frag_glsl.h>

std::unique_ptr<ppgso::Mesh> Plant::mesh;
std::unique_ptr<ppgso::Texture> Plant::texture;
std::unique_ptr<ppgso::Shader> Plant::shader;

Plant::Plant(int x, int y, int z) {
    scale *= 5;
    rotation = {0,0,0};
    position.y = y;
    position.x = x;
    position.z = z;

    if (!shader) shader = std::make_unique<ppgso::Shader>(diffuse_vert_glsl, diffuse_frag_glsl);
    if (!texture) texture = std::make_unique<ppgso::Texture>(ppgso::image::loadBMP("cloud.bmp"));
    if (!mesh) mesh = std::make_unique<ppgso::Mesh>("plant.obj");
}

bool Plant::update(float dt, SceneWindow &scene) {
    updateModelMatrix();
    return true;
}

void Plant::render(SceneWindow &scene) {
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
