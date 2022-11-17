//
// Created by A on 16/11/2022.
//


#include "Bird.h"
#include <shaders/diffuse_vert_glsl.h>
#include <shaders/diffuse_frag_glsl.h>

std::unique_ptr<ppgso::Mesh> Bird::mesh;
std::unique_ptr<ppgso::Texture> Bird::texture;
std::unique_ptr<ppgso::Shader> Bird::shader;

Bird::Bird() {
    scale *= 0.01;
    position = {10,10,10};

    if (!shader) shader = std::make_unique<ppgso::Shader>(diffuse_vert_glsl, diffuse_frag_glsl);
    if (!texture) texture = std::make_unique<ppgso::Texture>(ppgso::image::loadBMP("bird.bmp"));
    if (!mesh) mesh = std::make_unique<ppgso::Mesh>("bird.obj");
}

bool Bird::update(float dt, SceneWindow &scene) {
    updateModelMatrix();

    float time = (float) glfwGetTime();
    rotation = {0, -0.5, -(time/4.6)+ppgso::PI};
    position = {200*cos(time/4.6),75, 200*sin(time/4.6)};

    return true;
}

void Bird::render(SceneWindow &scene) {
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