//
// Created by rszar on 11. 11. 2022.
//

#include "Coconut.h"
#include <shaders/diffuse_vert_glsl.h>
#include <shaders/diffuse_frag_glsl.h>

std::unique_ptr<ppgso::Mesh> Coconut::mesh;
std::unique_ptr<ppgso::Texture> Coconut::texture;
std::unique_ptr<ppgso::Shader> Coconut::shader;

Coconut::Coconut(float x, float y, float z) {
    position.x = x;
    position.y = y-2;
    position.z = z;

    scale *= 0.15;
    rotation = {0,0,0};

    if (!shader) shader = std::make_unique<ppgso::Shader>(diffuse_vert_glsl, diffuse_frag_glsl);
    if (!texture) texture = std::make_unique<ppgso::Texture>(ppgso::image::loadBMP("wood.bmp"));
    if (!mesh) mesh = std::make_unique<ppgso::Mesh>("coconut.obj");
}

bool Coconut::update(float dt, SceneWindow &scene) {
    updateModelMatrix();
    return true;
}

void Coconut::render(SceneWindow &scene) {
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
