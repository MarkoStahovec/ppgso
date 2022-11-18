//
// Created by A on 17/11/2022.
//

#include "Light.h"
#include <glm/gtc/random.hpp>

#include <shaders/color_vert_glsl.h>
#include <shaders/color_frag_glsl.h>
#include <shaders/phong_frag_glsl.h>
#include <shaders/phong_vert_glsl.h>

// Static resources
std::unique_ptr<ppgso::Mesh> Light::mesh;
std::unique_ptr<ppgso::Shader> Light::shader;

Light::Light(glm::vec3 pos, glm::vec3 rot, glm::vec3 col){
    position = pos;
    rotation = rot;
    color = col;
    scale = {0.5f, 0.5f, 0.5f};
    // Initialize static resources if needed
    if (!shader) shader = std::make_unique<ppgso::Shader>(color_vert_glsl, color_frag_glsl);
    if (!mesh) mesh = std::make_unique<ppgso::Mesh>("sphere.obj");
}

bool Light::update(float dt, SceneWindow &scene) {
    // Generate modelMatrix from position, rotation and scale
    updateModelMatrix();

    return true;
}

void Light::render(SceneWindow &scene) {
    shader->use();

    // Set up light
    shader->setUniform("LightDirection", {0.25, 1, 0.5});

    // use camera
    shader->setUniform("ProjectionMatrix", scene.camera->projectionMatrix);
    shader->setUniform("ViewMatrix", scene.camera->viewMatrix);

    // render mesh
    shader->setUniform("ModelMatrix", modelMatrix);
    shader->setUniform("OverallColor", color);
    mesh->render();
}