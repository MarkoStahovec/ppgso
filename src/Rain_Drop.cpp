#include "Rain_Drop.h"


bool Rain_Drop::update(float dTime, SceneWindow &scene) {
    // TODO: Animate position using speed and dTime.
    // - Return true to keep the object alive
    // - Returning false removes the object from the scene
    // - hint: you can add more particles to the scene here also

    if (pos.y <= scene.get_Y(pos.x, pos.z, scene.heightMap) && droplet == 0){
        for(int i = 0; i < 4; i++){
            glm::vec3 position = glm::sphericalRand(3.0);
            position[0] += pos.x;
            position[1] += pos.y+10;
            position[2] += pos.z;
            glm::vec3 rotation = {0,0,0};
            glm::vec3 speed = {0,5,0};
            glm::vec3 scale = {0.2,0.2,0.2};
            glm::vec3 color = {0, ((float) rand() / (float) RAND_MAX) * (.8 - .2) + .3,1};
            scene.Renderable_objects.push_back(std::make_unique<Rain_Drop>(position, speed, color, scale, 1));

        }
        return false;
    } else if (pos.y <= scene.get_Y(pos.x, pos.z, scene.heightMap) && droplet == 1){
        return false;
    }

    pos += speed * (float).01 - scene.gravity * (float)0.1;
    modelMatrix = glm::mat4(1.f);
    modelMatrix = rotate(modelMatrix, rotation.x, glm::vec3(1, 0, 0));
    modelMatrix = rotate(modelMatrix, rotation.y, glm::vec3(0, 1, 0));
    modelMatrix = rotate(modelMatrix, rotation.z, glm::vec3(0, 0, 1));
    modelMatrix = glm::scale(modelMatrix, scale);
    modelMatrix = translate(modelMatrix, pos);
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
