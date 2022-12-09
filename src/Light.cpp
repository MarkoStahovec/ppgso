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
std::unique_ptr<ppgso::Shader> Light::shadow_shader;


Light::Light(glm::vec3 pos, glm::vec3 rot, glm::vec3 col, float sc, bool night, int index){
    position = pos;
    rotation = rot;
    color = col;
    scale *= sc;
    only_night = night;
    light_index = index;
    // Initialize static resources if needed
    if (!shader) shader = std::make_unique<ppgso::Shader>(color_vert_glsl, color_frag_glsl);
    if (!mesh) mesh = std::make_unique<ppgso::Mesh>("cube.obj");
    if (!shadow_shader) shadow_shader = std::make_unique<ppgso::Shader>(shadow_mapping_depth_vert_glsl, shadow_mapping_depth_frag_glsl);

}

bool Light::update(float dt, SceneWindow &scene) {
    // Generate modelMatrix from position, rotation and scale
    updateModelMatrix();
    if (only_night and scene.dayTime==scene.MORNING){
        scene.lights.intensity[light_index] = 0;
        return false;
    }

    return true;
}

void Light::render(SceneWindow &scene) {
    shader->use();

    // Set up light
    shader->setUniform("LightDirection", scene.globalLightDirection);

    // use camera
    shader->setUniform("ProjectionMatrix", scene.camera->projectionMatrix);
    shader->setUniform("ViewMatrix", scene.camera->viewMatrix);

    // render mesh
    shader->setUniform("ModelMatrix", modelMatrix);
    shader->setUniform("OverallColor", color);
    mesh->render();
}

void Light::render_shadow(SceneWindow &scene, glm::mat4 lightSpaceMatrix) {

    shadow_shader->use();
    shadow_shader->setUniform("lightSpaceMatrix", lightSpaceMatrix);
    shadow_shader->setUniform("model", modelMatrix);
    mesh->render();
}