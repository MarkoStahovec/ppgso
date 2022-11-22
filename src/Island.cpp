//
// Created by rszar on 11. 11. 2022.
//

#include "Island.h"
#include <shaders/diffuse_vert_glsl.h>
#include <shaders/diffuse_frag_glsl.h>
#include <shaders/phong_vert_glsl.h>
#include <shaders/phong_frag_glsl.h>

std::unique_ptr<ppgso::Mesh> Island::mesh;
std::unique_ptr<ppgso::Texture> Island::texture;
std::unique_ptr<ppgso::Shader> Island::shader;

Island::Island() {
    scale *= 0.803;
    scale.z *= 1;
    scale.y *= 0.8;
    rotation = {0,0,3*ppgso::PI/2};
    position.y = -0.1;
    position.x -= 2;
    position.z +=3;


    if (!shader) shader = std::make_unique<ppgso::Shader>(phong_vert_glsl, phong_frag_glsl);
    if (!texture) texture = std::make_unique<ppgso::Texture>(ppgso::image::loadBMP("isld1.bmp"));
    if (!mesh) mesh = std::make_unique<ppgso::Mesh>("ostrov.obj");
}

bool Island::update(float dt, SceneWindow &scene) {
    updateModelMatrix();
    return true;
}

void Island::render(SceneWindow &scene) {
    shader->use();

    // use camera
    shader->setUniform("globalLightDirection", scene.globalLightDirection);
    shader->setUniform("globalLightColor", scene.globalLightColor);
    shader->setUniform("globalLightAmbient", scene.globalLightAmbient);
    shader->setUniform("globalLightDiffuse", scene.globalLightDiffuse);
    shader->setUniform("globalLightSpecular", scene.globalLightSpecular);

    // use camera
    shader->setUniform("projection", scene.camera->projectionMatrix);
    shader->setUniform("view", scene.camera->viewMatrix);
    shader->setUniform("viewPos", scene.camera->cameraPos);
    shader->setUniform("globalLight", true);

    shader->setUniform("material.ambient", {0.25f, 0.25f, 0.25f});
    shader->setUniform("material.diffuse", {0.8f, 0.8, 0.8f});
    shader->setUniform("material.specular", {0.774597f, 0.774597f, 0.774597f});
    shader->setUniform("material.shininess", 6.8f);

    for (int i = 0; i < sizeof(scene.lights.position) / sizeof(scene.lights.position[0]); i++) {
        shader->setUniform("lights.position[" + std::to_string(i) + "]", scene.lights.position[i]);
        shader->setUniform("lights.direction[" + std::to_string(i) + "]", scene.lights.direction[i]);

        shader->setUniform("lights.color[" + std::to_string(i) + "]", scene.lights.color[i]);
        shader->setUniform("lights.intensity[" + std::to_string(i) + "]", scene.lights.intensity[i]);
        shader->setUniform("lights.radius[" + std::to_string(i) + "]", scene.lights.radius[i]);

        shader->setUniform("lights.ambient[" + std::to_string(i) + "]", scene.lights.ambient[i]);
        shader->setUniform("lights.diffuse[" + std::to_string(i) + "]", scene.lights.diffuse[i]);
        shader->setUniform("lights.specular[" + std::to_string(i) + "]", scene.lights.specular[i]);

        shader->setUniform("lights.cutOff[" + std::to_string(i) + "]", scene.lights.cutOff[i]);
        shader->setUniform("lights.outerCutOff[" + std::to_string(i) + "]", scene.lights.outerCutOff[i]);
        shader->setUniform("lights.isSpot[" + std::to_string(i) + "]", scene.lights.isSpot[i]);
    }

    // render mesh
    shader->setUniform("model", modelMatrix);
    shader->setUniform("Texture", *texture);
    mesh->render();
}


