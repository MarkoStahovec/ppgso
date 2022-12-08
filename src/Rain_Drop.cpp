#include "Rain_Drop.h"


bool Rain_Drop::update(float dTime, SceneWindow &scene) {
    // TODO: Animate position using speed and dTime.
    // - Return true to keep the object alive
    // - Returning false removes the object from the scene
    // - hint: you can add more particles to the scene here also

    if (position.y+5 <= scene.get_Y(position.x, position.z, scene.heightMap) && droplet == 0){
        if(rand() % 100 <= 50) {
            std::vector<glm::vec3> speed_list{{7,  150, 7},
                                              {-7, 150, 7},
                                              {7,  150, -7},
                                              {-7, 150, -7}};

            for (glm::vec3 speed_iteration: speed_list) {
                glm::vec3 pos = {position.x, position.y, position.z};
                pos[1] +=9;
                glm::vec3 rotation = {0, 0, 0};
                glm::vec3 speed = speed_iteration;
                glm::vec3 scale = {0.2, 0.2, 0.2};
                glm::vec3 color = {0, ((float) rand() / (float) RAND_MAX) * (.8 - .2) + .3, 1};

                auto raindrop_h = std::make_unique<Rain_Drop>(pos, speed, color, scale, 1);
                raindrop_h->parent = scene.root;

                scene.Renderable_objects.push_back(std::move(raindrop_h));
            }

        }
        return false;
    } else if (position.y <= scene.get_Y(position.x, position.z, scene.heightMap) && droplet == 1){
        return false;
    }

    speed *= (float)0.99;

    position += speed * (float).01 - scene.gravity * (float)0.1 + scene.wind;

    updateModelMatrix();
    return true;
}

void Rain_Drop::render(SceneWindow &scene) {
    // TODO: Render the object
    // - Use the shader
    // - Setup all needed shader inputs
    // - hint: use OverallColor in the color_vert_glsl shader for color
    // - Render the mesh
    shader->use();
    shader->setUniform("OverallColor", color);
    shader->setUniform("ModelMatrix", modelMatrix);
    shader->setUniform("ProjectionMatrix", scene.camera->projectionMatrix);
    shader->setUniform("ViewMatrix", scene.camera->viewMatrix);
    mesh->render();
}

void Rain_Drop::render_shadow(SceneWindow &scene, glm::mat4 lightSpaceMatrix) {

    shadow_shader->use();
    shadow_shader->setUniform("lightSpaceMatrix", lightSpaceMatrix);
    shadow_shader->setUniform("model", modelMatrix);
    mesh->render();
}
