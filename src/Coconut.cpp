//
// Created by rszar on 11. 11. 2022.
//

#include "Coconut.h"
#include <shaders/diffuse_vert_glsl.h>
#include <shaders/diffuse_frag_glsl.h>

std::unique_ptr<ppgso::Mesh> Coconut::mesh;
std::unique_ptr<ppgso::Texture> Coconut::texture;
std::unique_ptr<ppgso::Shader> Coconut::shader;

Coconut::Coconut(float x, float y, float z) {
    position.x = x;
    position.y = y+23;
    position.z = z;

    base_x = x;
    base_z = z;

    scale *= 0.01;
    rotation = {0,0,0};

    if (!shader) shader = std::make_unique<ppgso::Shader>(diffuse_vert_glsl, diffuse_frag_glsl);
    if (!texture) texture = std::make_unique<ppgso::Texture>(ppgso::image::loadBMP("coconut.bmp"));
    if (!mesh) mesh = std::make_unique<ppgso::Mesh>("sphere.obj");
}

bool Coconut::update(float dt, SceneWindow &scene) {
    if(spawning == 1){
        scale *= 1.05;
        if(scale[0] >= 2.5){
            spawning = 0;
        }
    }

    if (rand() % 1000 < 3 && dropping == 0 && spawning == 0 && despawning == 0){
        dropping = 1;
    }

    if(dropping==1){
        position += external_force * (float) 0.01 - scene.gravity * (float) 0.01;
        external_force *= .98;
        rotation[0] -= drop_dir_x/200 * n_drop/5;
        rotation[2] -= drop_dir_z/200 * n_drop/5;
    }

    if(scene.get_Y(position.x, position.z, scene.heightMap)==0){
        dropping = 0;
        despawning = 1;
    } else if (scene.get_Y(position.x, position.z, scene.heightMap)+1 >= position.y && n_drop>0){
        external_force = {drop_dir_x * n_drop/5, drop_dir_y*2, drop_dir_z * n_drop/5};
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
    shader->setUniform("ProjectionMatrix", scene.camera->projectionMatrix);
    shader->setUniform("ViewMatrix", scene.camera->viewMatrix);
    shader->setUniform("LightDirection", normalize(glm::vec3{1.0f, 1.0f, 1.0f}));

    // render mesh
    shader->setUniform("ModelMatrix", modelMatrix);
    shader->setUniform("Texture", *texture);
    mesh->render();
}
