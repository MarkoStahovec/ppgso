#include "Emerald.h"
#include <shaders/color_vert_glsl.h>
#include <shaders/color_frag_glsl.h>

std::unique_ptr<ppgso::Mesh> Emerald::mesh;
std::unique_ptr<ppgso::Texture> Emerald::texture;
std::unique_ptr<ppgso::Shader> Emerald::shader;
std::unique_ptr<ppgso::Shader> Emerald::shadow_shader;

Emerald::Emerald(int rot_type) {
    scale *= 0.1;
    position = {0,0,0};

    type = rot_type;
    rot_speed = rand() % 5+5;
    color = {1,10,0.5};

    if (!shader) shader = std::make_unique<ppgso::Shader>(color_vert_glsl, color_frag_glsl);
    if (!mesh) mesh = std::make_unique<ppgso::Mesh>("sphere.obj");
    if (!shadow_shader) shadow_shader = std::make_unique<ppgso::Shader>(shadow_mapping_depth_vert_glsl, shadow_mapping_depth_frag_glsl);
}

bool Emerald::update(float dt, SceneWindow &scene) {
    updateModelMatrix();

    float time = (float) glfwGetTime();
    rotation = {0, 0, (float)rot_speed*time};
    if(type){
        position.x = cos(time+0.6)*2;
        position.y = sin(time+0.6*2);
    } else {
        position.z = cos(time)*3;
        position.y = sin(time)*3;
    }

    return true;
}

void Emerald::render(SceneWindow &scene) {
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

void Emerald::render_shadow(SceneWindow &scene, glm::mat4 lightSpaceMatrix) {

    shadow_shader->use();
    shadow_shader->setUniform("lightSpaceMatrix", lightSpaceMatrix);
    shadow_shader->setUniform("model", modelMatrix);
    mesh->render();
}