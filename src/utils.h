#include "glm/vec3.hpp"
#include "Plant_2.h"
#include "Plant_3.h"

glm::vec3 generate_random_vec3(float min, float max) {
    return {((float) rand() / (float) RAND_MAX) * (max - min) + min, ((float) rand() / (float) RAND_MAX) * (max - min) + min, ((float) rand() / RAND_MAX) * (max - min) + min};
};

glm::vec3 generate_rain_vec3(float min, float max) {
    return {((float) rand() / (float) RAND_MAX) * (max - min) + min, 100, ((float) rand() / RAND_MAX) * (max - min) + min};
};

float random_float(int min, int max) {
    return ((float) rand() / (float) RAND_MAX) * (max - min) + min;
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

//https://stackoverflow.com/questions/9296059/read-pixel-value-in-bmp-file?fbclid=IwAR1SFqAY5Fo_WQ2cD-QhOnq0anhD1PwSUzGZuob9nb548ow3G_7hYQKxyhE
unsigned char* readBMP(char* filename)
{
    int i;
    FILE* f = fopen(filename, "rb");
    unsigned char info[54];

    // read the 54-byte header
    fread(info, sizeof(unsigned char), 54, f);

    // extract image height and width from header
    int width = *(int*)&info[18];
    int height = *(int*)&info[22];

    // allocate 3 bytes per pixel
    int size = 3 * width * height;
    unsigned char* data = new unsigned char[size];

    // read the rest of the data at once
    fread(data, sizeof(unsigned char), size, f);
    fclose(f);

    for(i = 0; i < size; i += 3)
    {
        // flip the order of every 3 bytes
        unsigned char tmp = data[i];
        data[i] = data[i+2];
        data[i+2] = tmp;
    }

    return data;
}

void raise_wind(SceneWindow &scene){
    if(scene.wind[0] < 1){
        if(scene.wind[0] < 0.2){
            scene.wind[0] += 0.01;
        }
        scene.wind *= 1.035;
    }
}

void calm_wind(SceneWindow &scene, GLFWwindow *window){
    if (scene.wind[0] > 0.0001 && glfwGetKey(window, GLFW_KEY_SPACE) != GLFW_PRESS) {
        scene.wind *= 0.98;
    }
}

std::unique_ptr<Renderable> random_plant(float x, float y, float z){
    int decide = rand() % 3;
    std::cout << decide << "\n";
    if (decide == 0) {
        return std::make_unique<Plant_1>(x, y, z, "plant_1.obj", "plant_1.bmp", 10);
    } else if (decide == 1) {
        return std::make_unique<Plant_2>(x, y, z, "plant_2.obj", "plant_2.bmp", 0.3);
    } else {
        return std::make_unique<Plant_3>(x, y, z, "plant_3.obj", "plant_3.bmp", 0.3);
    }
}