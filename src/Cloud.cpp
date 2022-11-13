//
// Created by rszar on 11. 11. 2022.
//

#include "Cloud.h"
#include <shaders/diffuse_vert_glsl.h>
#include <shaders/diffuse_frag_glsl.h>

std::unique_ptr<ppgso::Mesh> Cloud::mesh;
std::unique_ptr<ppgso::Texture> Cloud::texture;
std::unique_ptr<ppgso::Shader> Cloud::shader;

Cloud::Cloud() {
    scale *= 90;
    rotation = {0,0,0};
    position.y = 1;

    if (!shader) shader = std::make_unique<ppgso::Shader>(diffuse_vert_glsl, diffuse_frag_glsl);
    if (!texture) texture = std::make_unique<ppgso::Texture>(ppgso::image::loadBMP("cloud.bmp"));
    if (!mesh) mesh = std::make_unique<ppgso::Mesh>("cloud.obj");
}

bool Cloud::update(float dt, SceneWindow &scene) {
    auto t = (float) glfwGetTime();
    position = {90*cos(t/10),100, 90*sin(t/10)};
    updateModelMatrix();
    return true;
}

void Cloud::render(SceneWindow &scene) {

    shader->use();

    // use camera
    shader->setUniform("ProjectionMatrix", scene.camera->projectionMatrix);
    shader->setUniform("ViewMatrix", scene.camera->viewMatrix);
    shader->setUniform("LightDirection", normalize(glm::vec3{1.0f, 10.0f, 1.0f}));

    // render mesh
    shader->setUniform("ModelMatrix", modelMatrix);
    shader->setUniform("Texture", *texture);
    mesh->render();
}
