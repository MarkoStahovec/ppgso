#include "glm/vec3.hpp"

glm::vec3 generate_random_vec3(float min, float max) {
    return {((float) rand() / (float) RAND_MAX) * (max - min) + min, ((float) rand() / (float) RAND_MAX) * (max - min) + min, ((float) rand() / RAND_MAX) * (max - min) + min};
};

glm::vec3 generate_rain_vec3(float min, float max) {
    return {((float) rand() / (float) RAND_MAX) * (max - min) + min, 100, ((float) rand() / RAND_MAX) * (max - min) + min};
};

glm::vec3 speed_rain_vec3(float min, float max) {
    return {0, ((float) rand() / (float) RAND_MAX) * (max - min) + min, 0};
};

float generate_timer(float min, float max) {
    return (float)(((float) rand() / RAND_MAX) * (max - min) + min);
};

glm::vec3 generate_equal_vec3(float min, float max) {
    float coef = ((float) rand() / (float) RAND_MAX) * (max - min) + min;
    return {coef, coef, coef};
};
