//
// Created by A on 04/11/2022.
//
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>
#include "Renderable.h"
#include "ppgso.h"

/*position.y = -0.1;
    position.x -= 2;
    position.z +=3;*/

void Renderable::updateModelMatrix() {

    if(parent!= nullptr and !parent->inherit) {
        glm::vec3 vector = parent->position;
        vector.x += 2;
        vector.y += 0.1;
        vector.z -= 3;
        modelMatrix =
                glm::translate(glm::mat4(1.0f), vector)
                * glm::translate(glm::mat4(1.0f), position)
                * glm::orientate4(rotation)
                * glm::scale(glm::mat4(1.0f), scale);
    } else if(parent!= nullptr){
        modelMatrix =
                parent->modelMatrix *
                glm::translate(glm::mat4(1.0f), position)
                * glm::orientate4(rotation)
                * glm::scale(glm::mat4(1.0f), scale);
    } else {
        modelMatrix =
                glm::translate(glm::mat4(1.0f), position)
                * glm::orientate4(rotation)
                * glm::scale(glm::mat4(1.0f), scale);
    }
}

void Renderable::reverseIslandBaseRotation() {
    rotation -= glm::vec3 {0,0,3*ppgso::PI/2};
}

void Renderable::reverseIslandBasePosition() {
    position.x +=2;
    position.z -=3;
}

void Renderable::reverseIslandBaseScale() {
    scale *= glm::vec3 {1/(0.803,1/(0.803*.8),1/(0.803))};
}