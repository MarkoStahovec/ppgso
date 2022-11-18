//
// Created by A on 04/11/2022.
//
// attribution to rawpixel.com, Mark Lyu, textures4photoshop, texturex
//<a href="https://www.freepik.com/free-photo/old-grunge-wood-with-green-leaves_1018915.htm#page=2&query=wood%20leaf&position=9&from_view=keyword">Image by jannoon028</a> on Freepik
// <a href="https://www.freepik.com/free-photo/natural-material-wood-dark-brown_1043710.htm#query=wood%20texture&position=7&from_view=keyword">Image by mrsiraphol</a> on Freepik
#include <iostream>
#include <list>
#include <ppgso/ppgso.h>
#define GLM_ENABLE_EXPERIMENTAL

#include <shaders/color_vert_glsl.h>
#include <shaders/color_frag_glsl.h>
#include <map>
#include "SceneWindow.h"
#include "Island.h"
#include "Surface.h"
#include "Camera.h"
#include "Cloud.h"
#include "SkyBox.h"
#include "Rain_Drop.h"
#include "House.h"
#include "Campfire.h"
#include "Plant_1.h"
#include "Fire.h"
#include "Door.h"
#include "utils.h"
#include "Fish.h"
#include "Bird.h"
#include "Palm.h"
#include "Coconut.h"
#include "Boat.h"
#include "Light.h"

#define GLM_ENABLE_EXPERIMENTAL


const unsigned int SIZE = 512;

class MainScene : public ppgso::Window {
private:
    // Scene of objects
    SceneWindow scene;

    // Store keyboard state
    std::map<int, int> keys;


public:
    MainScene() : Window{"Stahovec robí sám", WINDOW_HEIGHT, WINDOW_WIDTH} {
        // Initialize OpenGL state
        // Enable Z-buffer
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);

        scene.heightMap = readBMP("Height.bmp");

        auto camera = std::make_unique<Camera>();
        scene.camera = move(camera);

        scene.Renderable_objects.push_back(std::make_unique<Island>());
        scene.Renderable_objects.push_back(std::make_unique<Surface>());
        scene.Renderable_objects.push_back(std::make_unique<Cloud>());
        scene.Renderable_objects.push_back(std::make_unique<SkyBox>());
        scene.Renderable_objects.push_back(std::make_unique<House>(-25,scene.get_Y(-25,-40,scene.heightMap),-40));
        scene.Renderable_objects.push_back(std::make_unique<Campfire>());
        scene.Renderable_objects.push_back(std::make_unique<Fire>());
        scene.Renderable_objects.push_back(std::make_unique<Door>());
        scene.Renderable_objects.push_back(std::make_unique<Bird>());
        scene.Renderable_objects.push_back(std::make_unique<Boat>());
        scene.Renderable_objects.push_back(std::make_unique<Fish>());
        scene.Renderable_objects.push_back(std::make_unique<Palm>(30,scene.get_Y(30,10,scene.heightMap), 10,(glm::vec3) {0, 0, 0}, (glm::vec3) {0.15,0.15,0.15}));
        scene.Renderable_objects.push_back(std::make_unique<Palm>(-68,scene.get_Y(-68,78,scene.heightMap),78,(glm::vec3) {-.05,0,.022}, (glm::vec3) {0.15,0.2,0.15}));
        scene.Renderable_objects.push_back(std::make_unique<Palm>(68,scene.get_Y(68,-78,scene.heightMap),-78,(glm::vec3) {.05,0,.022}, (glm::vec3) {0.15,0.15,0.15}));
        scene.Renderable_objects.push_back(std::make_unique<Palm>(30,scene.get_Y(30,73,scene.heightMap),73,(glm::vec3) {.05,0,-.022}, (glm::vec3) {0.2,0.15,0.2}));
        scene.Renderable_objects.push_back(std::make_unique<Palm>(-35,scene.get_Y(-35,-40,scene.heightMap),-40,(glm::vec3) {.01,0,-.01}, (glm::vec3) {0.15,0.12,0.15}));
        scene.Renderable_objects.push_back(std::make_unique<Coconut>(29,scene.get_Y(29,10,scene.heightMap),10));
        scene.Renderable_objects.push_back(std::make_unique<Coconut>(31,scene.get_Y(31,10,scene.heightMap),10));


        /*for(int i=-100; i<100; i++){
            for(int j=-100; j<100;j++){
                float x = i;
                float z = j;
                int y = get_Y(x,z,scene.heightMap);
                std::cout << y<<"\n";
                scene.Renderable_objects.push_back(std::make_unique<Plant_1>(x, y, z));

            }
        }*/

        for(int i=0; i<100; i++){
            int x = random_float(-100,100);
            int z = random_float(-100,100);
            int y = scene.get_Y(x, z, scene.heightMap);
            if (y <= 4 || y >15) {
                i--;
            } else {

                scene.Renderable_objects.push_back(random_plant(x, y, z));
            }
        }

        glm::vec3 position = {11, 0, 29};
        position.y = scene.get_Y(position.x, position.z, scene.heightMap) + 0.25;
        glm::vec3 rotation = {3*ppgso::PI/2,0,0};
        glm::vec3 color = {0.88, 0.1, 0.4};

        scene.lights.position[0] = position;
        scene.lights.direction[0] = {0, 0, 0};

        scene.lights.color[0] = color;
        scene.lights.radius[0] = 15;
        scene.lights.intensity[0] = 6;

        scene.lights.ambient[0] = {0.05f, 0.05f, 0.05f};
        scene.lights.diffuse[0] = {0.85f, 0.85f, 0.85f};
        scene.lights.specular[0] = {1.0f, 1.0f, 1.0f};

        scene.lights.cutOff[0] = glm::cos(glm::radians(12.5f));
        scene.lights.outerCutOff[0] = glm::cos(glm::radians(15.0f));
        scene.lights.isSpot[0] = false;

        auto light1 = std::make_unique<Light>(position, rotation, color);
        scene.Renderable_objects.push_back(move(light1));

        position = {22, 0, 33};
        position.y = scene.get_Y(position.x, position.z, scene.heightMap) + 0.25;
        rotation = {3*ppgso::PI/2,0,0};
        color = {0.1, 0.7, 0.9};

        scene.lights.position[1] = position;
        scene.lights.direction[1] = {0, 0, 0};

        scene.lights.color[1] = color;
        scene.lights.radius[1] = 15;
        scene.lights.intensity[1] = 6;

        scene.lights.ambient[1] = {0.05f, 0.05f, 0.05f};
        scene.lights.diffuse[1] = {0.85f, 0.85f, 0.85f};
        scene.lights.specular[1] = {1.0f, 1.0f, 1.0f};

        scene.lights.cutOff[1] = glm::cos(glm::radians(12.5f));
        scene.lights.outerCutOff[1] = glm::cos(glm::radians(15.0f));
        scene.lights.isSpot[1] = false;

        light1 = std::make_unique<Light>(position, rotation, color);
        scene.Renderable_objects.push_back(move(light1));

        // This is a SpotLight
        position = {-25, 0, 20};
        position.y = scene.get_Y(position.x, position.z, scene.heightMap) + 10.25;
        rotation = {3*ppgso::PI/2,0,0};
        color = {0.4, 0.1, 0.95};

        scene.lights.position[2] = position;
        scene.lights.direction[2] = {-0.25, -1, -0.5};

        scene.lights.color[2] = color;
        scene.lights.radius[2] = 25;
        scene.lights.intensity[2] = 20;

        scene.lights.ambient[2] = {0.0f, 0.0f, 0.0f};
        scene.lights.diffuse[2] = {1.0f, 1.0f, 1.0f};
        scene.lights.specular[2] = {1.0f, 1.0f, 1.0f};

        scene.lights.cutOff[2] = glm::cos(glm::radians(12.5f));
        scene.lights.outerCutOff[2] = glm::cos(glm::radians(15.0f));
        scene.lights.isSpot[2] = true;

        light1 = std::make_unique<Light>(position, rotation, color);
        scene.Renderable_objects.push_back(move(light1));


        scene.last_frame_time = -1;
        scene.current_frame_time = (float) glfwGetTime();
    }

    void onKey(int key, int scanCode, int action, int mods) override {
        // Collect key state in a map
        scene.keyboard[key] = action;

        // https://learnopengl.com/Getting-started/Camera
        static auto time = (float) glfwGetTime();
        float dTime = (float) glfwGetTime() - time;
        float cameraSpeed = 200 * (scene.current_frame_time - scene.last_frame_time);

        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
            scene.camera->isAnimating = false;
            scene.camera->cameraPos += cameraSpeed * scene.camera->cameraFront;
        }
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS){
            scene.camera->isAnimating = false;
            scene.camera->cameraPos -= cameraSpeed * scene.camera->cameraFront;
        }
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
            scene.camera->isAnimating = false;
            scene.camera->cameraPos -= glm::normalize(glm::cross(scene.camera->cameraFront, scene.camera->cameraUp)) * cameraSpeed;
        }
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
            scene.camera->isAnimating = false;
            scene.camera->cameraPos += glm::normalize(glm::cross(scene.camera->cameraFront, scene.camera->cameraUp)) * cameraSpeed;
        }

        if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) {
            scene.camera->t = 0;
            scene.camera->startPos = {50, 40, 50};
            scene.camera->endPos = {80, 65, 25};
            scene.camera->startPosLookAt = {0.0f, -0.15f, -1.0f};
            scene.camera->endPosLookAt = {0.0f, -0.15f, -0.8f};
            scene.camera->isAnimating = true;
            scene.keyboard[GLFW_KEY_1] = GLFW_RELEASE;
        }

        if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) {
            scene.camera->t = 0;
            scene.camera->startPos = {20, 6, 90};
            scene.camera->endPos = {20, 6, 90};
            scene.camera->startPosLookAt = {10.0f, 6.15f, 100.0f};
            scene.camera->endPosLookAt = {10.0f, 6.15f, 100.0f};
            scene.camera->isAnimating = true;
            scene.keyboard[GLFW_KEY_2] = GLFW_RELEASE;
        }

        if (scene.keyboard[GLFW_KEY_SPACE]){
            raise_wind(scene);
        }

    }

    void onIdle() override {
        // Track time
        scene.last_frame_time = scene.current_frame_time;
        static auto time = (float) glfwGetTime();
        // Compute time delta
        float dTime = (float) glfwGetTime() - time;
        dTime *= 0.001;
        //time = (float) glfwGetTime();

        // TODO: Add renderable object to the scene
        if(rand() % 100 <= 5) {
            glm::vec3 position = glm::sphericalRand(90.0);
            position[1] = 100;
            glm::vec3 rotation = generate_random_vec3(-ppgso::PI, ppgso::PI);
            glm::vec3 speed = {0, 0, 0};
            glm::vec3 scale = {1, 1, 1};
            glm::vec3 color = {0, ((float) rand() / (float) RAND_MAX) * (.8 - .2) + .3, 1};

            scene.Renderable_objects.push_back(std::make_unique<Rain_Drop>(position, speed, color, scale, 0));
        }

        calm_wind(scene, window);

        // Set gray background
        glClearColor(.1f, .1f, .1f, 1.0f);

        // Clear depth and color buffers
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Update all objects in scene
        // Because we need to delete while iterating this is implemented using c++ iterators
        // In most languages mutating the container during iteration is undefined behaviour
        scene.update(dTime);

        // Render every object in scene
        scene.render();

        scene.current_frame_time = (float) glfwGetTime();
    }
};

int main() {
    // Create new window
    auto window = MainScene{};

    // Main execution loop
    while (window.pollEvents()) {}

    return EXIT_SUCCESS;
}