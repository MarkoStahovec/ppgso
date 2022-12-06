//
// Created by A on 16/11/2022.
//


#include "Ufo.h"
#include <shaders/phong_frag_glsl.h>
#include <shaders/phong_vert_glsl.h>
#include "KeyPicture.h"

std::unique_ptr<ppgso::Mesh> Ufo::mesh;
std::unique_ptr<ppgso::Texture> Ufo::texture;
std::unique_ptr<ppgso::Shader> Ufo::shader;
std::unique_ptr<ppgso::Shader> Ufo::shadow_shader;

Ufo::Ufo() {
    scale *= 1;
    position = {1000,1000,1000};

    if (!shader) shader = std::make_unique<ppgso::Shader>(phong_vert_glsl, phong_frag_glsl);
    if (!shadow_shader) shadow_shader = std::make_unique<ppgso::Shader>(shadow_mapping_depth_vert_glsl, shadow_mapping_depth_frag_glsl);
    if (!texture) texture = std::make_unique<ppgso::Texture>(ppgso::image::loadBMP("corsair.bmp"));
    if (!mesh) mesh = std::make_unique<ppgso::Mesh>("ufo.obj");
}

bool Ufo::update(float dt, SceneWindow &scene) {
    updateModelMatrix();

    float time = (float) glfwGetTime() - base_time;

    if (i == keys.size()-1){
        scale = {0,0,0};
        if(time > period){
            i=0;
            base_time = (float) glfwGetTime();
            scale = {1,1,1};
        }
        return true;
    }
    if(time>keys[i+1].time){
        i+=1;
    }

    float t = (time-keys[i].time)/(keys[i+1].time-keys[i].time);
    position = keys[i].position * (1-t) + keys[i+1].position * t;
    rotation = keys[i].rotation * (1-t) + keys[i+1].rotation * t;

    scene.lights.position[6] = position;
    //std::cout << position[0] << "  "<< time << "  " << i << "\n";

    return true;
}

void Ufo::render(SceneWindow &scene) {
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

    shader->setUniform("material.ambient", {0.29f, 0.29f, 0.29f});
    shader->setUniform("material.diffuse", {0.74f, 0.74, 0.75f});
    shader->setUniform("material.specular", {0.8232f, 0.8232f, 0.8232f});
    shader->setUniform("material.shininess", 5.0f);

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
}
void Ufo::render_shadow(SceneWindow &scene, glm::mat4 lightSpaceMatrix) {

    shadow_shader->use();
    shadow_shader->setUniform("lightSpaceMatrix", lightSpaceMatrix);
    shadow_shader->setUniform("model", modelMatrix);
    mesh->render();
}
