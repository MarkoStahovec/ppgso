//
// Created by A on 16/11/2022.
//

#include "Fish.h"
#include <shaders/diffuse_vert_glsl.h>
#include <shaders/diffuse_frag_glsl.h>

std::unique_ptr<ppgso::Mesh> Fish::mesh;
std::unique_ptr<ppgso::Texture> Fish::texture;
std::unique_ptr<ppgso::Shader> Fish::shader;

Fish::Fish() {
    scale *= 4.5;
    position = {10,10,10};

    if (!shader) shader = std::make_unique<ppgso::Shader>(diffuse_vert_glsl, diffuse_frag_glsl);
    if (!texture) texture = std::make_unique<ppgso::Texture>(ppgso::image::loadBMP("fish.bmp"));
    if (!mesh) mesh = std::make_unique<ppgso::Mesh>("fish.obj");
}

bool Fish::update(float dt, SceneWindow &scene) {
    updateModelMatrix();

    float time = (float) glfwGetTime();
    rotation = {-sin(time/2), 0, -(time/5)+ppgso::PI};
    position = {200*cos(time/5),100*cos(time/2)-70, 200*sin(time/5)};

    return true;
}

void Fish::render(SceneWindow &scene) {
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