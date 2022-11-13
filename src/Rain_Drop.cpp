

#include "Rain_Drop.h"

bool Rain_Drop::update(float dTime, SceneWindow &scene) {
    // TODO: Animate position using speed and dTime.
    // - Return true to keep the object alive
    // - Returning false removes the object from the scene
    // - hint: you can add more particles to the scene here also


    time -= dTime;
    if (time < 0) {
        return false;
    }
    pos += speed * (float).01;
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
