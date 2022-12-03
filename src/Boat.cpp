//
// Created by A on 17/11/2022.
//

#include "Boat.h"
#include <shaders/diffuse_vert_glsl.h>
#include <shaders/diffuse_frag_glsl.h>

std::unique_ptr<ppgso::Mesh> Boat::mesh;
std::unique_ptr<ppgso::Texture> Boat::texture;
std::unique_ptr<ppgso::Shader> Boat::shader;
std::unique_ptr<ppgso::Shader> Boat::shadow_shader;

Boat::Boat() {
    scale *= 0.25;
    position = {10,10,10};

    if (!shader) shader = std::make_unique<ppgso::Shader>(diffuse_vert_glsl, diffuse_frag_glsl);
    if (!shadow_shader) shadow_shader = std::make_unique<ppgso::Shader>(shadow_mapping_depth_vert_glsl, shadow_mapping_depth_frag_glsl);
    if (!texture) texture = std::make_unique<ppgso::Texture>(ppgso::image::loadBMP("wood.bmp"));
    if (!mesh) mesh = std::make_unique<ppgso::Mesh>("boat.obj");
}

bool Boat::update(float dt, SceneWindow &scene) {
    updateModelMatrix();

    float time = (float) glfwGetTime();
    rotation = {0, 0, -(time/14.6)+ppgso::PI/2};
    position = {300*cos(time/14.6),1.0, 300*sin(time/14.6)};

    return true;
}

void Boat::render(SceneWindow &scene) {
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

void Boat::render_shadow(SceneWindow &scene, glm::mat4 lightSpaceMatrix) {

    shadow_shader->use();
    shadow_shader->setUniform("lightSpaceMatrix", lightSpaceMatrix);
    shadow_shader->setUniform("model", modelMatrix);
    mesh->render();
}