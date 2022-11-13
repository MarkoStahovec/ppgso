//
// Created by A on 04/11/2022.
//
// attribution to rawpixel.com
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
#include "utils.h"
#define GLM_ENABLE_EXPERIMENTAL


const unsigned int SIZE = 512;

class MainScene : public ppgso::Window {
private:
    // Scene of objects
    SceneWindow scene;

    // Store keyboard state
    std::map<int, int> keys;


public:
    MainScene() : Window{"Stahovec robi sam", WINDOW_WIDTH, WINDOW_WIDTH} {
        // Initialize OpenGL state
        // Enable Z-buffer
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);

        auto camera = std::make_unique<Camera>();
        scene.camera = move(camera);
        // TODO here will be initialization of all objects (call to some function)
        scene.Renderable_objects.push_back(std::make_unique<Island>());
        scene.Renderable_objects.push_back(std::make_unique<Surface>());
        scene.Renderable_objects.push_back(std::make_unique<Cloud>());
        scene.Renderable_objects.push_back(std::make_unique<SkyBox>());
    }

    void onKey(int key, int scanCode, int action, int mods) override {
        // Collect key state in a map
        scene.keyboard[key] = action;

        // https://learnopengl.com/Getting-started/Camera
        static auto time = (float) glfwGetTime();
        float dTime = (float) glfwGetTime() - time;
        float cameraSpeed = 0.05f * dTime;

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
            scene.camera->startPos = {0, 3, 8};
            scene.camera->endPos = {4, 3, 2};
            scene.camera->startPosLookAt = {0.0f, -0.15f, -1.0f};
            scene.camera->endPosLookAt = {0.0f, -0.15f, -0.8f};
            scene.camera->isAnimating = true;
            scene.keyboard[GLFW_KEY_1] = GLFW_RELEASE;
        }
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
            for(int i=0; i<10;i++) {
                // TODO: Add renderable object to the scene
                glm::vec3 position =  glm::sphericalRand(50.0);
                position[1] = 50;
                glm::vec3 rotation = generate_random_vec3(-ppgso::PI, ppgso::PI);
                glm::vec3 speed = speed_rain_vec3(-2, -4);
                glm::vec3 scale = generate_equal_vec3(0.01, 0.02);
                glm::vec3 color = generate_random_vec3(0, 1);

                float timer = generate_timer(20, 30);
                auto generator = std::make_unique<Rain_Drop>(position, speed, color, scale, timer);
                scene.Renderable_objects.push_back(std::make_unique<Rain_Drop>(position, speed, color, scale, timer));
            }
        }
    }

    void onIdle() override {
        // Track time
        static auto time = (float) glfwGetTime();
        // Compute time delta
        float dTime = (float) glfwGetTime() - time;
        dTime *= 0.001;
        //time = (float) glfwGetTime();

        for(int i=0; i<1;i++) {
            // TODO: Add renderable object to the scene
            glm::vec3 position =  glm::sphericalRand(150.0);
            position[1] = 100;
            glm::vec3 rotation = generate_random_vec3(-ppgso::PI, ppgso::PI);
            glm::vec3 speed = speed_rain_vec3(-80, -100);
            glm::vec3 scale = generate_equal_vec3(0.5, 1);
            glm::vec3 color = {0, ((float) rand() / (float) RAND_MAX) * (.8 - .2) + .3,1};

            float timer = generate_timer(5, 5);
            auto generator = std::make_unique<Rain_Drop>(position, speed, color, scale, timer);
            scene.Renderable_objects.push_back(std::make_unique<Rain_Drop>(position, speed, color, scale, timer));
        }

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
    }
};

int main() {
    // Create new window
    auto window = MainScene{};

    // Main execution loop
    while (window.pollEvents()) {}

    return EXIT_SUCCESS;
}