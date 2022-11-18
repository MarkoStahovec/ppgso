//
// Created by A on 16/11/2022.
//

#include "Fish.h"
#include <shaders/diffuse_vert_glsl.h>
#include <shaders/diffuse_frag_glsl.h>
#include <shaders/phong_vert_glsl.h>
#include <shaders/phong_frag_glsl.h>

std::unique_ptr<ppgso::Mesh> Fish::mesh;
std::unique_ptr<ppgso::Texture> Fish::texture;
std::unique_ptr<ppgso::Shader> Fish::shader;

Fish::Fish() {
    scale *= 4.5;
    position = {10,10,10};

    if (!shader) shader = std::make_unique<ppgso::Shader>(phong_vert_glsl, phong_frag_glsl);
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
    shader->setUniform("globalLightDirection", {0.25,1,0.5});
    shader->setUniform("globalLightColor", {1,1,1});

    // use camera
    shader->setUniform("projection", scene.camera->projectionMatrix);
    shader->setUniform("view", scene.camera->viewMatrix);
    shader->setUniform("viewPos", scene.camera->cameraPos);
    shader->setUniform("globalLight", true);

    shader->setUniform("material.ambient", {0.25f, 0.25f, 0.25f});
    shader->setUniform("material.diffuse", {0.4f, 0.4f, 0.4f});
    shader->setUniform("material.specular", {0.774597f, 0.774597f, 0.774597f});
    shader->setUniform("material.shininess", 6.8f);

    for (int i = 0; i < sizeof(scene.lights.position) / sizeof(scene.lights.position[0]); i++) {
        shader->setUniform("lights.position[" + std::to_string(i) + "]", scene.lights.position[i]);
        shader->setUniform("lights.color[" + std::to_string(i) + "]", scene.lights.color[i]);
        shader->setUniform("lights.intensity[" + std::to_string(i) + "]", scene.lights.intensity[i]);
        shader->setUniform("lights.radius[" + std::to_string(i) + "]", scene.lights.radius[i]);
    }

    // render mesh
    shader->setUniform("model", modelMatrix);
    shader->setUniform("Texture", *texture);
    mesh->render();
}