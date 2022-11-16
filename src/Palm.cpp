//
// Created by rszar on 11. 11. 2022.
//

#include "Palm.h"
#include <shaders/diffuse_vert_glsl.h>
#include <shaders/diffuse_frag_glsl.h>

std::unique_ptr<ppgso::Mesh> Palm::mesh;
std::unique_ptr<ppgso::Texture> Palm::texture;
std::unique_ptr<ppgso::Shader> Palm::shader;

Palm::Palm(float x, float y, float z, glm::vec3 r) {
    position.x = x;
    position.y = y-2;
    position.z = z;

    scale *= 0.15;
    rotation = r;

    if (!shader) shader = std::make_unique<ppgso::Shader>(diffuse_vert_glsl, diffuse_frag_glsl);
    if (!texture) texture = std::make_unique<ppgso::Texture>(ppgso::image::loadBMP("wood.bmp"));
    if (!mesh) mesh = std::make_unique<ppgso::Mesh>("palm.obj");
}

bool Palm::update(float dt, SceneWindow &scene) {
    updateModelMatrix();
    return true;
}

void Palm::render(SceneWindow &scene) {
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
