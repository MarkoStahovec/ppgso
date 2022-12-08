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
    if (!texture) texture = std::make_unique<ppgso::Texture>(ppgso::image::loadBMP("beachball.bmp"));
    if (!shadow_shader) shadow_shader = std::make_unique<ppgso::Shader>(shadow_mapping_depth_vert_glsl, shadow_mapping_depth_frag_glsl);
    if (!mesh) mesh = std::make_unique<ppgso::Mesh>("ball.obj");
}

void Coconut::drop(SceneWindow &scene) {
    position += external_force * (float) 0.01 - scene.gravity * (float) 0.01;
    external_force *= .98;
    external_force += scene.wind/(float)3;
    rotation[0] -= drop_dir_x/200 * n_drop/5;
    rotation[2] -= drop_dir_z/200 * n_drop/5;
}



void Coconut::drop_chance() {
    if (rand() % 1000 < 2 && dropping == 0 && spawning == 0 && despawning == 0){
        dropping = 1;
    }
}

void Coconut::handle_coll(SceneWindow &scene) {
    if(scene.get_Y(position.x, position.z, scene.heightMap)==0){
        dropping = 0;
        despawning = 1;
    } else if (scene.get_Y(position.x, position.z, scene.heightMap)+1 >= position.y && n_drop>0){
        bounce(scene);
    } else if (scene.get_Y(position.x, position.z, scene.heightMap)+1 >= position.y && n_drop == 0){
        dropping = 0;
        despawning = 1;
    }
}

void Coconut::bounce(SceneWindow &scene) {
    //glm::vec3 norm = calc_plane_norm(scene);
    external_force = glm::vec3{2*drop_dir_x * n_drop/5, drop_dir_y*5 * n_drop/5, 2*drop_dir_z * n_drop/5} * calc_plane_norm(scene);
    external_force += scene.wind/(float)3;
    n_drop--;
}

glm::vec3 Coconut::calc_plane_norm(SceneWindow &scene) {
    glm::vec3 A = {position.x-1, scene.get_Y(position.x-1,position.z-1,scene.heightMap), position.z-1};
    glm::vec3 B = {position.x+1, scene.get_Y(position.x+1,position.z+1,scene.heightMap), position.z+1};
    glm::vec3 C = {position.x+1, scene.get_Y(position.x+1,position.z-1,scene.heightMap), position.z-1};

    glm::vec3 norm = glm::cross(A-B, A-C);

    return glm::normalize(norm);
}

bool Coconut::update(float dt, SceneWindow &scene) {
    if(spawning == 1){
        scale *= 1.05;
        if(scale[0] >= 0.005){
            spawning = 0;
        }

    }

    drop_chance();

    if(dropping==1){
        drop(scene);
    }

    handle_coll(scene);

    if(despawning == 1){
        scale *= 0.98;
        if(scale[0] < 0.0001){
            auto coconut_h = std::make_unique<Coconut>(base_x,scene.get_Y(base_x,base_z,scene.heightMap),base_z);
            coconut_h->parent = scene.root;
            scene.Renderable_objects.push_back(std::move(coconut_h));
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

    shader->setUniform("material.ambient", {0.32f, 0.25, 0.2f});
    shader->setUniform("material.diffuse", {0.89f, 0.85, 0.82f});
    shader->setUniform("material.specular", {0.84f, 0.75f, 0.70f});
    shader->setUniform("material.shininess", 6.9f);

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
