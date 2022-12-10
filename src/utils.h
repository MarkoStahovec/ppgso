#include "glm/vec3.hpp"
#include "Plant_1.h"
#include "Plant_2.h"
#include "Plant_3.h"
#include "Light.h"
#include "Pillar.h"

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

std::unique_ptr<Renderable> random_plant(float x, float y, float z, SceneWindow &scene){
    int decide = rand() % 3;
    if (decide == 0) {
        auto plant =  std::make_unique<Plant_1>(x, y, z, "plant_1.obj", "plant_1.bmp", 10);
        plant->parent = scene.root;
        return std::move(plant);
    } else if (decide == 1) {
        auto plant = std::make_unique<Plant_2>(x, y, z, "plant_2.obj", "plant_2.bmp", 0.3);
        plant->parent = scene.root;
        return std::move(plant);
    } else {
        auto plant = std::make_unique<Plant_3>(x, y, z, "plant_3.obj", "plant_3.bmp", 0.3);
        plant->parent = scene.root;
        return std::move(plant);
    }
}

glm::vec3 generate_random_color() {
    int num_of_dominant_colors = rand() % 2 + 1;
    std::vector<float> possibilities(3);
    for (int j = 0; j < 3; j++)
    {
        if (rand() % 2 == 0 && num_of_dominant_colors != 0) {
            possibilities[j] = random_float(2.25, 10);
            num_of_dominant_colors--;
            if(j == 1) {
                possibilities[j] /= 2.38;
            }
            else if(j == 2) {
                possibilities[j] *= 3.88;
            }
        }
        else {
            possibilities[j] = random_float(0.01, 2.25);
        }
    }
    if (num_of_dominant_colors >= 1) {
        possibilities[rand() % 3] = random_float(1.25, 2.5);
    }
    return {possibilities[0], possibilities[1], possibilities[2]};
}

void generate_torches(SceneWindow &scene){
    for(int i=0; i<7; i++){
        int x = random_float(-100,100);
        int z = random_float(-100,100);
        int y = scene.get_Y(x, z, scene.heightMap);
        if (y <= 4 || y >15 || (x < -15 && x > -35 && z < -30 && z >-50)) {
            i--;
        } else {
            glm::vec3 color = generate_random_color();
            auto night_light = std::make_unique<Light>(glm::vec3{x,y+7,z}, glm::vec3 {3*ppgso::PI/2,0,0}, color,2, true,4+i);
            night_light->parent = scene.root;

            auto pillar_h = std::make_unique<Pillar>();
            pillar_h->parent = night_light.get();

            scene.Renderable_objects.push_back(std::move(night_light));
            scene.Renderable_objects.push_back(std::move(pillar_h));
            scene.addLight(glm::vec3 {x,y+7,z},
                           color,
                           glm::vec3 {0, 0, 0},
                           15.f,
                           15.f,
                           glm::vec3 {0.05f, 0.05f, 0.05f},
                           glm::vec3 {0.85f, 0.85f, 0.85f},
                           glm::vec3 {1.0f, 1.0f, 1.0f},
                           glm::cos(glm::radians(12.5f)),
                           glm::cos(glm::radians(15.0f)),
                           false,
                           4+i
            );
        }
    }
}