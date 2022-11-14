//
// Created by A on 14/11/2022.
//

#include "Fire.h"
#include <shaders/diffuse_vert_glsl.h>
#include <shaders/diffuse_frag_glsl.h>

std::unique_ptr<ppgso::Mesh> Fire::mesh;
std::unique_ptr<ppgso::Texture> Fire::texture;
std::unique_ptr<ppgso::Shader> Fire::shader;

Fire::Fire() {
    scale *= 1.15;
    rotation = {0,10,0};
    position.y = 4;
    position.x = 30;
    position.z = 95;

    if (!shader) shader = std::make_unique<ppgso::Shader>(diffuse_vert_glsl, diffuse_frag_glsl);
    if (!texture) texture = std::make_unique<ppgso::Texture>(ppgso::image::loadBMP("fire.bmp"));
    if (!mesh) mesh = std::make_unique<ppgso::Mesh>("fire.obj");
}

bool Fire::update(float dt, SceneWindow &scene) {
    updateModelMatrix();
    return true;
}

void Fire::render(SceneWindow &scene) {
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