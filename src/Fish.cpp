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
    shader->setUniform("projection", scene.camera->projectionMatrix);
    shader->setUniform("view", scene.camera->viewMatrix);
    shader->setUniform("model", modelMatrix);

    shader->setUniform("light.position", normalize(glm::vec3{1.2f, 1.0f, 2.0f}));
    shader->setUniform("viewPos", scene.camera->cameraPos);

    shader->setUniform("light.ambient", {0.2f,0.2f,0.2f});
    shader->setUniform("light.diffuse", {0.5f,0.5f,0.5f});
    shader->setUniform("light.specular", {1.0f,1.0f,1.0f});
    shader->setUniform("light.constant", 1.0f);
    shader->setUniform("light.linear", 0.01f);
    shader->setUniform("light.quadratic", 0.0f);

    shader->setUniform("material.diffuse", 0);
    shader->setUniform("material.specular", 1);
    shader->setUniform("material.shininess", 32.0f);


    // render mesh
    shader->setUniform("Texture", *texture);
    mesh->render();
}