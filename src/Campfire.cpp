//
// Created by rszar on 11. 11. 2022.
//

#include "Campfire.h"
#include <shaders/diffuse_vert_glsl.h>
#include <shaders/diffuse_frag_glsl.h>
#include <shaders/color_vert_glsl.h>
#include <shaders/color_frag_glsl.h>

std::unique_ptr<ppgso::Mesh> Campfire::mesh;
std::unique_ptr<ppgso::Texture> Campfire::texture;
std::unique_ptr<ppgso::Shader> Campfire::shader;

Campfire::Campfire() {
    scale *=0.25;
    rotation = {0,-0.2,-1};
    position.y = 4;
    position.x = 20;
    position.z = 100;

    if (!shader) shader = std::make_unique<ppgso::Shader>(diffuse_vert_glsl, diffuse_frag_glsl);
    if (!texture) texture = std::make_unique<ppgso::Texture>(ppgso::image::loadBMP("ashes.bmp"));
    if (!mesh) mesh = std::make_unique<ppgso::Mesh>("campfire_3.obj");
}

bool Campfire::update(float dt, SceneWindow &scene) {
    updateModelMatrix();
    return true;
}

void Campfire::render(SceneWindow &scene) {
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
