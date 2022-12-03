//
// Created by rszar on 11. 11. 2022.
//

#include "Coconut.h"
#include <shaders/phong_frag_glsl.h>
#include <shaders/phong_vert_glsl.h>

std::unique_ptr<ppgso::Mesh> Coconut::mesh;
std::unique_ptr<ppgso::Texture> Coconut::texture;
std::unique_ptr<ppgso::Shader> Coconut::shader;
std::unique_ptr<ppgso::Shader> Coconut::shadow_shader;


Coconut::Coconut(float x, float y, float z) {
    position.x = x;
    position.y = y+23;
    position.z = z;

    base_x = x;
    base_z = z;

    scale *= 0.01;
    rotation = {0,0,0};

    if (!shader) shader = std::make_unique<ppgso::Shader>(phong_vert_glsl, phong_frag_glsl);
    if (!texture) texture = std::make_unique<ppgso::Texture>(ppgso::image::loadBMP("coconut.bmp"));
    if (!shadow_shader) shadow_shader = std::make_unique<ppgso::Shader>(shadow_mapping_depth_vert_glsl, shadow_mapping_depth_frag_glsl);
    if (!mesh) mesh = std::make_unique<ppgso::Mesh>("sphere.obj");
}

bool Coconut::update(float dt, SceneWindow &scene) {
    if(spawning == 1){
        scale *= 1.05;
        if(scale[0] >= 2.5){
            spawning = 0;
        }
    }

    if (rand() % 1000 < 2 && dropping == 0 && spawning == 0 && despawning == 0){
        dropping = 1;
    }

    if(dropping==1){
        position += external_force * (float) 0.01 - scene.gravity * (float) 0.01;
        external_force *= .98;
        external_force += scene.wind/(float)3;
        rotation[0] -= drop_dir_x/200 * n_drop/5;
        rotation[2] -= drop_dir_z/200 * n_drop/5;
    }

    if(scene.get_Y(position.x, position.z, scene.heightMap)==0){
        dropping = 0;
        despawning = 1;
    } else if (scene.get_Y(position.x, position.z, scene.heightMap)+1 >= position.y && n_drop>0){
        external_force = {drop_dir_x * n_drop/5, drop_dir_y*3, drop_dir_z * n_drop/5};
        external_force += scene.wind/(float)3;
        n_drop--;
    } else if (scene.get_Y(position.x, position.z, scene.heightMap)+1 >= position.y && n_drop == 0){
        dropping = 0;
        despawning = 1;
    }

    if(despawning == 1){
        scale *= 0.98;
        if(scale[0] < 0.0001){
            scene.Renderable_objects.push_back(std::make_unique<Coconut>(base_x,scene.get_Y(base_x,base_z,scene.heightMap),base_z));
            return false;
        }
    }


    updateModelMatrix();
    return true;
}

void Coconut::render(SceneWindow &scene) {
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
    glUniform1i(glGetUniformLocation(shader->getProgram(), "shadowMap"), 1);

    // render mesh
    shader->setUniform("model", modelMatrix);
    shader->setUniform("Texture", *texture);
    shader->setUniform("shadowMap",1);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, scene.depthMap);
    mesh->render();
}


void Coconut::render_shadow(SceneWindow &scene, glm::mat4 lightSpaceMatrix) {

    shadow_shader->use();
    shadow_shader->setUniform("lightSpaceMatrix", lightSpaceMatrix);
    shadow_shader->setUniform("model", modelMatrix);
    mesh->render();
}
