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


#define GLM_ENABLE_EXPERIMENTAL

const unsigned int SIZE = 512;

const unsigned int WINDOW_HEIGHT = 1920;
const unsigned int WINDOW_WIDTH = 1080;

/// Basic particle that will render a sphere
/// TODO: Implement Renderable particle
class Generator final : public Renderable {
    // Static resources shared between all particles
    static std::unique_ptr<ppgso::Mesh> mesh;
    static std::unique_ptr<ppgso::Shader> shader;

    // TODO: add more parameters as needed
    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 scale;
    glm::vec3 speed;
    glm::vec3 color{0.55, 0.55, 0.85};
    glm::mat4 modelMatrix{1.f};
    float alive_timer;
    //ppgso::Shader program{texture_vert_glsl, texture_frag_glsl};


public:
    /// Construct a new Particle
    /// \param p - Initial position
    /// \param s - Initial speed
    /// \param c - Color of particle
    Generator(glm::vec3 p, glm::vec3 r, glm::vec3 s, glm::vec3 c, float t) {
        // First particle will initialize resources
        if (!shader) shader = std::make_unique<ppgso::Shader>(color_vert_glsl, color_frag_glsl);
        if (!mesh) mesh = std::make_unique<ppgso::Mesh>("sphere.obj");

        position = p;
        speed = s;
        scale = c;
        rotation = r;
        alive_timer = t;
    }

    bool update(float dTime, SceneWindow &scene) override {
        // TODO: Animate position using speed and dTime.
        // - Return true to keep the object alive
        // - Returning false removes the object from the scene
        // - hint: you can add more particles to the scene here also

        alive_timer -= dTime;

        if (alive_timer < 0) {
            return false;
        }

        position += speed * dTime;
        modelMatrix = glm::mat4(1.f);
        modelMatrix = rotate(modelMatrix, rotation.x, glm::vec3(1, 0, 0));
        modelMatrix = rotate(modelMatrix, rotation.y, glm::vec3(0, 1, 0));
        modelMatrix = rotate(modelMatrix, rotation.z, glm::vec3(0, 0, 1));
        modelMatrix = glm::scale(modelMatrix, scale);
        modelMatrix = translate(modelMatrix, position);
        /*
        modelMatrix = glm::translate(glm::mat4 {1.0f}, position)
                      * glm::rotate(glm::mat4 {1.0f}, rotation.x, glm::vec3 {1, 0, 0})
                      * glm::rotate(glm::mat4 {1.0f}, rotation.y, glm::vec3 {0, 1, 0})
                      * glm::rotate(glm::mat4 {1.0f}, rotation.z, glm::vec3 {0, 0, 1})
                      * glm::scale(glm::mat4 {1.0f}, scale);*/
        return true;
    }

    void render(SceneWindow& scene) {
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
        //shader->setUniform("ProjectionMatrix", camera.camPosition);
        mesh->render();

    }

    std::pair <bool, glm::vec3> get_attributes() override {
        std::pair <bool, glm::vec3> values;
        values.first = true;
        values.second = position;
        return values;
    }
};

class Mutation final : public Renderable {
    // Static resources shared between all particles
    static std::unique_ptr<ppgso::Mesh> mesh;
    static std::unique_ptr<ppgso::Shader> shader;

    // TODO: add more parameters as needed
    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 scale;
    glm::vec3 speed;
    glm::vec3 color{1, 0.6, 0.9};
    glm::mat4 modelMatrix{1.f};
    float alive_timer;

public:
    /// Construct a new Particle
    /// \param p - Initial position
    /// \param s - Initial speed
    /// \param c - Color of particle
    Mutation(glm::vec3 p, glm::vec3 r, glm::vec3 s, glm::vec3 c, float t) {
        // First particle will initialize resources
        if (!shader) shader = std::make_unique<ppgso::Shader>(color_vert_glsl, color_frag_glsl);
        if (!mesh) mesh = std::make_unique<ppgso::Mesh>("sphere.obj");

        position = p;
        speed = s;
        scale = c;
        rotation = r;
        alive_timer = t;
    }

    bool update(float dTime, SceneWindow &scene) override {
        // TODO: Animate position using speed and dTime.
        // - Return true to keep the object alive
        // - Returning false removes the object from the scene
        // - hint: you can add more particles to the scene here also

        alive_timer -= dTime;

        if (alive_timer < 0) {
            return false;
        }

        position += speed * dTime;
        modelMatrix = glm::mat4(1.f);
        modelMatrix = glm::scale(modelMatrix, scale);
        modelMatrix = translate(modelMatrix, position);
        modelMatrix = rotate(modelMatrix, rotation.x, glm::vec3(1, 0, 0));
        modelMatrix = rotate(modelMatrix, rotation.y, glm::vec3(0, 1, 0));
        modelMatrix = rotate(modelMatrix, rotation.z, glm::vec3(0, 0, 1));

        return true;
    }

    void render(SceneWindow& scene) {
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
        //shader->setUniform("ProjectionMatrix", camera.camPosition);
        mesh->render();

    }

    std::pair <bool, glm::vec3> get_attributes() override {
        std::pair <bool, glm::vec3> values;
        values.first = false;
        values.second = position;
        return values;
    }
};

// Static resources need to be instantiated outside of the class as they are globals
std::unique_ptr<ppgso::Mesh> Generator::mesh;
std::unique_ptr<ppgso::Shader> Generator::shader;
std::unique_ptr<ppgso::Mesh> Mutation::mesh;
std::unique_ptr<ppgso::Shader> Mutation::shader;

class MainScene : public ppgso::Window {
private:
    // Scene of objects
    SceneWindow scene;

    // Create camera
    //Camera camera = {120.0f, (float) width / (float) height, 1.0f, 400.0f};

    // Store keyboard state
    std::map<int, int> keys;
public:
    MainScene() : Window{"Stahovec robi sam", SIZE, SIZE} {
        // Initialize OpenGL state
        // Enable Z-buffer
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);
        auto camera = std::make_unique<Camera>();
        scene.camera = move(camera);


        // TODO here will be initialization of all objects (call to some function)
    }

    void onKey(int key, int scanCode, int action, int mods) override {
        // Collect key state in a map
        keys[key] = action;
        if (keys[GLFW_KEY_SPACE]) {
            // TODO: Add renderable object to the scene
            glm::vec3 position = generate_random_vec3(2, 5);
            glm::vec3 rotation = generate_random_vec3(-ppgso::PI, ppgso::PI);
            glm::vec3 speed = generate_random_vec3(-3, 3);
            glm::vec3 scale = generate_equal_vec3(0.25, 0.5 );
            glm::vec3 color = generate_random_vec3(0, 1);

            float timer = generate_timer(10, 15);
            auto generator = std::make_unique<Generator>(position, rotation, speed, scale, timer);
            scene.Renderable_objects.push_back(std::make_unique<Generator>(position, rotation, speed, scale, timer));
        }

        if (keys[GLFW_KEY_B]) {
            for (auto& i : scene.Renderable_objects) {
                auto obj = i.get();
                auto attr = obj->get_attributes();

                if (attr.first) {
                    for (int j = 0; j < (int)generate_timer(10, 20); j++) {
                        glm::vec3 position = attr.second;
                        glm::vec3 rotation = generate_random_vec3(-ppgso::PI, ppgso::PI);
                        glm::vec3 speed = generate_random_vec3(-3, 3);
                        glm::vec3 scale = {0.33, 0.33, 0.33};

                        float timer = generate_timer(10, 15);
                        scene.Renderable_objects.push_back(std::make_unique<Mutation>(position, rotation, speed, scale, timer));
                    }
                }
            }
        }
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