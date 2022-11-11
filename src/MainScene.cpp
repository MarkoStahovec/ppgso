//
// Created by A on 04/11/2022.
//

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


#define GLM_ENABLE_EXPERIMENTAL

const unsigned int SIZE = 512;

const unsigned int WINDOW_HEIGHT = 1920;
const unsigned int WINDOW_WIDTH = 1080;



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
        //scene.Renderable_objects.push_back(std::make_unique<Surface>());
        scene.Renderable_objects.push_back(std::make_unique<Island>());
    }

    void onKey(int key, int scanCode, int action, int mods) override {
        // Collect key state in a map
        keys[key] = action;
    }

    glm::vec3 generate_random_vec3(float min, float max) {
        return {((float) rand() / (float) RAND_MAX) * (max - min) + min, ((float) rand() / (float) RAND_MAX) * (max - min) + min, ((float) rand() / RAND_MAX) * (max - min) + min};
    };

    float generate_timer(float min, float max) {
        return (float)(((float) rand() / RAND_MAX) * (max - min) + min);
    };

    glm::vec3 generate_equal_vec3(float min, float max) {
        float coef = ((float) rand() / (float) RAND_MAX) * (max - min) + min;
        return {coef, coef, coef};
    };

    void onIdle() override {
        // Track time
        static auto time = (float) glfwGetTime();
        // Compute time delta
        float dTime = (float) glfwGetTime() - time;
        time = (float) glfwGetTime();

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