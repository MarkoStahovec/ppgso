//
// Created by A on 14/11/2022.
//

#include "Door.h"
#include <shaders/diffuse_vert_glsl.h>
#include <shaders/diffuse_frag_glsl.h>

std::unique_ptr<ppgso::Mesh> Door::mesh;
std::unique_ptr<ppgso::Texture> Door::texture;
std::unique_ptr<ppgso::Shader> Door::shader;

Door::Door() {
    scale *= 0.0123;
    rotation = {0,-0.2,-1};
    position.y = 4.95;
    position.x = 19.35;
    position.z = 91.9;

    if (!shader) shader = std::make_unique<ppgso::Shader>(diffuse_vert_glsl, diffuse_frag_glsl);
    if (!texture) texture = std::make_unique<ppgso::Texture>(ppgso::image::loadBMP("wood.bmp"));
    if (!mesh) mesh = std::make_unique<ppgso::Mesh>("door.obj");
}

bool Door::update(float dt, SceneWindow &scene) {
    updateModelMatrix();
    return true;
}

void Door::render(SceneWindow &scene) {
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