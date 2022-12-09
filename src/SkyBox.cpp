//
// Created by rszar on 11. 11. 2022.
//
#include "SkyBox.h"
#include <shaders/texture_vert_glsl.h>
#include <shaders/texture_frag_glsl.h>
#include <shaders/phong_vert_glsl.h>
#include <shaders/phong_frag_glsl.h>
#include <shaders/texture_vert_glsl.h>
#include <shaders/texture_frag_glsl.h>

std::unique_ptr<ppgso::Mesh> SkyBox::mesh;
std::unique_ptr<ppgso::Texture> SkyBox::texture;
std::unique_ptr<ppgso::Shader> SkyBox::shader;
std::unique_ptr<ppgso::Shader> SkyBox::texture_shader;
std::unique_ptr<ppgso::Shader> SkyBox::shadow_shader;

SkyBox::SkyBox() {
    scale *= 10;
    rotation = {0,0,0};
    position.y += 800;
    if (!shader) shader = std::make_unique<ppgso::Shader>(phong_vert_glsl, phong_frag_glsl);
    if (!texture_shader) texture_shader = std::make_unique<ppgso::Shader>(texture_vert_glsl, texture_frag_glsl);
    if (!texture) texture = std::make_unique<ppgso::Texture>(ppgso::image::loadBMP("sky.bmp"));
    if (!mesh) mesh = std::make_unique<ppgso::Mesh>("ball.obj");
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
        shader->setUniform("globalLightDirection", scene.globalLightDirection);
        shader->setUniform("globalLightColor", scene.globalLightColor);
        shader->setUniform("globalLightAmbient", scene.globalLightAmbient);
        shader->setUniform("globalLightDiffuse", scene.globalLightDiffuse);
        shader->setUniform("globalLightSpecular", scene.globalLightSpecular);
        shader->setUniform("globalLightPosition", scene.globalLightPosition);

        // use camera
        shader->setUniform("projection", scene.camera->projectionMatrix);
        shader->setUniform("view", scene.camera->viewMatrix);
        shader->setUniform("viewPos", scene.camera->cameraPos);
        shader->setUniform("globalLight", true);
        shader->setUniform("lightSpaceMatrix",scene.lightSpaceMatrix);

        shader->setUniform("material.ambient", scene.skyboxAmbient);
        shader->setUniform("material.diffuse", scene.skyboxDiffuse);
        shader->setUniform("material.specular", scene.skyboxSpecular);
        shader->setUniform("material.shininess", scene.skyboxShininess);

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
        glUniform1i(glGetUniformLocation(shader->getProgram(), "shadowMap"), 1);

        // render mesh
        shader->setUniform("model", modelMatrix);
        shader->setUniform("Texture", *texture);
        shader->setUniform("shadowMap",1);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, scene.depthMap);
        mesh->render();


    glDepthMask(GL_TRUE);
}

void SkyBox::render_shadow(SceneWindow &scene, glm::mat4 lightSpaceMatrix) {

    shadow_shader->use();
    shadow_shader->setUniform("lightSpaceMatrix", lightSpaceMatrix);
    shadow_shader->setUniform("model", modelMatrix);
    mesh->render();
}