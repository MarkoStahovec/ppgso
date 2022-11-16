//
// Created by rszar on 11. 11. 2022.
//
#include "SkyBox.h"
#include <shaders/texture_vert_glsl.h>
#include <shaders/texture_frag_glsl.h>

std::unique_ptr<ppgso::Mesh> SkyBox::mesh;
std::unique_ptr<ppgso::Texture> SkyBox::texture;
std::unique_ptr<ppgso::Shader> SkyBox::shader;

SkyBox::SkyBox() {
    scale *= 2000;
    rotation = {0,0,0};

    if (!shader) shader = std::make_unique<ppgso::Shader>(texture_vert_glsl, texture_frag_glsl);
    if (!texture) texture = std::make_unique<ppgso::Texture>(ppgso::image::loadBMP("sky.bmp"));
    if (!mesh) mesh = std::make_unique<ppgso::Mesh>("sphere.obj");
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