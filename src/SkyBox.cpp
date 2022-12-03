//
// Created by rszar on 11. 11. 2022.
//
#include "SkyBox.h"
#include <shaders/texture_vert_glsl.h>
#include <shaders/texture_frag_glsl.h>

std::unique_ptr<ppgso::Mesh> SkyBox::mesh;
std::unique_ptr<ppgso::Texture> SkyBox::texture;
std::unique_ptr<ppgso::Shader> SkyBox::shader;
std::unique_ptr<ppgso::Shader> SkyBox::shadow_shader;

SkyBox::SkyBox() {
    scale *= 3000;
    rotation = {0,0,0};
    position.y += 1000;
    if (!shader) shader = std::make_unique<ppgso::Shader>(texture_vert_glsl, texture_frag_glsl);
    if (!texture) texture = std::make_unique<ppgso::Texture>(ppgso::image::loadBMP("sky.bmp"));
    if (!mesh) mesh = std::make_unique<ppgso::Mesh>("sphere.obj");
    if (!shadow_shader) shadow_shader = std::make_unique<ppgso::Shader>(shadow_mapping_depth_vert_glsl, shadow_mapping_depth_frag_glsl);

}

bool SkyBox::update(float dt, SceneWindow &scene) {
    updateModelMatrix();
    return true;
}

void SkyBox::render(SceneWindow &scene) {

    glDepthMask(GL_FALSE);

    shader->use();

    // use camera
    shader->setUniform("ProjectionMatrix", scene.camera->projectionMatrix);
    shader->setUniform("ViewMatrix", scene.camera->viewMatrix);

    // render mesh
    shader->setUniform("ModelMatrix", modelMatrix);
    shader->setUniform("Texture", *texture);
    mesh->render();

    glDepthMask(GL_TRUE);
}

void SkyBox::render_shadow(SceneWindow &scene, glm::mat4 lightSpaceMatrix) {

    shadow_shader->use();
    shadow_shader->setUniform("lightSpaceMatrix", lightSpaceMatrix);
    shadow_shader->setUniform("model", modelMatrix);
    mesh->render();
}