//
// Created by rszar on 22. 11. 2022.
//

#include "Firefly.h"
#include <glm/gtc/random.hpp>

#include <shaders/color_vert_glsl.h>
#include <shaders/color_frag_glsl.h>
#include <shaders/phong_frag_glsl.h>
#include <shaders/phong_vert_glsl.h>

// Static resources
std::unique_ptr<ppgso::Mesh> Firefly::mesh;
std::unique_ptr<ppgso::Shader> Firefly::shader;
std::unique_ptr<ppgso::Shader> Firefly::shadow_shader;


Firefly::Firefly(glm::vec3 pos, glm::vec3 col, int light_index){
    position = pos;
    color = col;
    scale = {.5f, .5f, .5f};
    speed = {gen_speed(GLFW_TRUE),
             0,
             gen_speed(GLFW_TRUE)};
    i=light_index;

    // Initialize static resources if needed
    if (!shader) shader = std::make_unique<ppgso::Shader>(color_vert_glsl, color_frag_glsl);
    if (!mesh) mesh = std::make_unique<ppgso::Mesh>("sphere.obj");
    if (!shadow_shader) shadow_shader = std::make_unique<ppgso::Shader>(shadow_mapping_depth_vert_glsl, shadow_mapping_depth_frag_glsl);

}

bool Firefly::update(float dt, SceneWindow &scene) {
    // Generate modelMatrix from position, rotation and scale
    position += speed;

    position += glm::vec3 {(float) (rand()%10) /100 - (float) (rand()%10) /100,
                           (float) (rand()%10) /100 - (float) (rand()%10) /100,
                           (float) (rand()%10) /100 - (float) (rand()%10) /100};

    position.y = scene.get_Y(position.x, position.z, scene.heightMap) +4;

    scene.lights.position[i] = position;

    check_speed();

    updateModelMatrix();
    return true;
}

void Firefly::render(SceneWindow &scene) {
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

void Firefly::render_shadow(SceneWindow &scene, glm::mat4 lightSpaceMatrix) {

    shadow_shader->use();
    shadow_shader->setUniform("lightSpaceMatrix", lightSpaceMatrix);
    shadow_shader->setUniform("model", modelMatrix);
    mesh->render();
}

void Firefly::check_speed() {
    if (-100 > position.x){
        speed[0] = gen_speed(GLFW_FALSE);

    } else if (100 < position.x){
        speed[0] = -gen_speed(GLFW_FALSE);

    } else if (-100 > position.z){
        speed[2] = gen_speed(GLFW_FALSE);

    } else if (position.z > 100){
        speed[2] = -gen_speed(GLFW_FALSE);
    }
}

float Firefly::gen_speed(bool randomize) {
    if (randomize){
        if (rand() % 2 == 0) return -(float) (rand()%2) /75 - (float) (rand()%9) /750 - (float)0.01;
        return (float) (rand()%2) /75 + (float) (rand()%9) /750 + (float)0.01;
    } else {
        return (float) (rand()%2) /75 + (float) (rand()%9) /750 + (float)0.01;
    }
}
