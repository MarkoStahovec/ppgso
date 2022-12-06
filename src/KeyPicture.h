//
// Created by rszar on 6. 12. 2022.
//

#ifndef PPGSO_KEYPICTURE_H
#define PPGSO_KEYPICTURE_H


#include "glm/vec3.hpp"

class KeyPicture {

public:
    KeyPicture(glm::vec3 pos, glm::vec3 rot, float t);

    glm::vec3 position;
    glm::vec3 rotation;
    float time;
};


#endif //PPGSO_KEYPICTURE_H
