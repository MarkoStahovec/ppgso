//
// Created by rszar on 11. 11. 2022.
//

#include "Surface.h"
#include <shaders/diffuse_vert_glsl.h>
#include <shaders/diffuse_frag_glsl.h>

std::unique_ptr<ppgso::Mesh> Surface::mesh;
std::unique_ptr<ppgso::Texture> Surface::texture;
std::unique_ptr<ppgso::Shader> Surface::shader;

Surface::Surface() {
    scale *= 1;
    rotation = {0,0,0};
    position.y = 0;

    if (!shader) shader = std::make_unique<ppgso::Shader>(diffuse_vert_glsl, diffuse_frag_glsl);
    if (!texture) texture = std::make_unique<ppgso::Texture>(ppgso::image::loadBMP("water.bmp"));
    if (!mesh) mesh = std::make_unique<ppgso::Mesh>("surface.obj");
}

bool Surface::update(float dt, SceneWindow &scene) {
    updateModelMatrix();
    auto time = (float) glfwGetTime();
    //scale={2,0.1*sin(time)+1,2};
    return true;
}

void Surface::render(SceneWindow &scene) {
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

