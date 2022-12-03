//
// Created by A on 04/11/2022.
//
// attribution to rawpixel.com, Mark Lyu, textures4photoshop, texturex
//<a href="https://www.freepik.com/free-photo/old-grunge-wood-with-green-leaves_1018915.htm#page=2&query=wood%20leaf&position=9&from_view=keyword">Image by jannoon028</a> on Freepik
// <a href="https://www.freepik.com/free-photo/natural-material-wood-dark-brown_1043710.htm#query=wood%20texture&position=7&from_view=keyword">Image by mrsiraphol</a> on Freepik
#include <iostream>
#include <list>
#include <ppgso/ppgso.h>

#include <shaders/color_vert_glsl.h>
#include <shaders/color_frag_glsl.h>
#include <shaders/bloom_vert_glsl.h>
#include <shaders/bloom_frag_glsl.h>
#include <shaders/blur_vert_glsl.h>
#include <shaders/blur_frag_glsl.h>
#include <shaders/shadow_mapping_depth_vert_glsl.h>
#include <shaders/shadow_mapping_depth_frag_glsl.h>
#include <shaders/debug_quad_frag_glsl.h>
#include <shaders/debug_quad_vert_glsl.h>
#include <map>
#include "SceneWindow.h"
#include "Island.h"
#include "Surface.h"
#include "Camera.h"
#include "Cloud.h"
#include "SkyBox.h"
#include "Rain_Drop.h"
#include "House.h"

#include "Plant_1.h"
#include "Door.h"
#include "utils.h"
#include "Fish.h"
#include "Bird.h"
#include "Palm.h"
#include "Coconut.h"
#include "Boat.h"
#include "Light.h"
#include "Firefly.h"


extern "C"
{
  __declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}


const unsigned int SIZE = 512;
const unsigned int SHADOW_WIDTH = 4096*3, SHADOW_HEIGHT = 4096*3;

std::unique_ptr<ppgso::Shader> shaderBlur;
std::unique_ptr<ppgso::Shader> shaderBloomFinal;
unsigned int hdrFBO;
unsigned int colorBuffers[2];
unsigned int pingpongFBO[2];
unsigned int pingpongColorbuffers[2];
unsigned int quadVAO, quadVBO;
unsigned int rboDepth;

std::unique_ptr<ppgso::Shader> shadowMapping;
std::unique_ptr<ppgso::Shader> simpleDepthShader;
std::unique_ptr<ppgso::Shader> debugQuad;

unsigned int depthMapFBO;

class MainScene : public ppgso::Window {
private:
    // Scene of objects
    SceneWindow scene;

    // Store keyboard state
    std::map<int, int> keys;

    void initializeCustomFramebuffer() {
        // screen quad VAO
        glEnable(GL_DEPTH_TEST);

        if (!shaderBlur) shaderBlur = std::make_unique<ppgso::Shader>(blur_vert_glsl, blur_frag_glsl);
        if (!shaderBloomFinal) shaderBloomFinal = std::make_unique<ppgso::Shader>(bloom_vert_glsl, bloom_frag_glsl);


        // framebuffer configuration
        // -------------------------

        glGenFramebuffers(1, &hdrFBO);
        glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
        // create a color attachment texture

        glGenTextures(2, colorBuffers);
        for (unsigned int i = 0; i < 2; i++)
        {
            glBindTexture(GL_TEXTURE_2D, colorBuffers[i]);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, WINDOW_HEIGHT, WINDOW_WIDTH, 0, GL_RGBA, GL_FLOAT, nullptr);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);  // we clamp to the edge as the blur filter would otherwise sample repeated texture values!
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            // attach texture to framebuffer
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, colorBuffers[i], 0);
        }
        // create a renderbuffer object for depth and stencil attachment (we won't be sampling these)

        glGenRenderbuffers(1, &rboDepth);
        glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, WINDOW_HEIGHT, WINDOW_WIDTH);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
        // now that we actually created the framebuffer and added all attachments we want to check if it is actually complete now
        unsigned int attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
        glDrawBuffers(2, attachments);
        // finally check if framebuffer is complete
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            std::cout << "Framebuffer not complete!" << std::endl;
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        glGenFramebuffers(2, pingpongFBO);
        glGenTextures(2, pingpongColorbuffers);
        for (unsigned int i = 0; i < 2; i++)
        {
            glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[i]);
            glBindTexture(GL_TEXTURE_2D, pingpongColorbuffers[i]);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, WINDOW_HEIGHT, WINDOW_WIDTH, 0, GL_RGBA, GL_FLOAT, nullptr);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // we clamp to the edge as the blur filter would otherwise sample repeated texture values!
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pingpongColorbuffers[i], 0);
            // also check if framebuffers are complete (no need for depth buffer)
            if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
                std::cout << "Framebuffer not complete!" << std::endl;
        }

        shaderBlur->use();
        shaderBlur->setUniform("image", 0);
        shaderBloomFinal->use();

        glUniform1i(glGetUniformLocation(shaderBloomFinal->getProgram(), "screenTexture"), 0);
        glUniform1i(glGetUniformLocation(shaderBloomFinal->getProgram(), "bloomBlur"), 1);
    }

    void bindToCustomFramebuffer() {
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void rebindToOriginalFramebuffer() {

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        bool horizontal = true, first_iteration = true;
        unsigned int amount = 2;
        shaderBlur->use();
        for (unsigned int i = 0; i < amount; i++)
        {
            glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[horizontal]);
            shaderBlur->setUniform("horizontal", horizontal);
            glBindTexture(GL_TEXTURE_2D, first_iteration ? colorBuffers[1] : pingpongColorbuffers[!horizontal]);  // bind texture of other framebuffer (or scene if first iteration)
            renderQuad();
            horizontal = !horizontal;
            if (first_iteration)
                first_iteration = false;
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // 3. now render floating point color buffer to 2D quad and tonemap HDR colors to default framebuffer's (clamped) color range
        // --------------------------------------------------------------------------------------------------------------------------
        shaderBloomFinal->use();
        shaderBloomFinal->setUniform("bloom", true);
        shaderBloomFinal->setUniform("exposure", 0.91f);
        shaderBloomFinal->setUniform("screenTexture", 0);
        shaderBloomFinal->setUniform("bloomBlur", 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, pingpongColorbuffers[horizontal]);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, colorBuffers[0]);
        renderQuad();
    }

    void renderQuad()
    {
        if (quadVAO == 0)
        {
            float quadVertices[] = {
                    // positions        // texture Coords
                    -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
                    -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
                    1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
                    1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
            };
            // setup plane VAO
            glGenVertexArrays(1, &quadVAO);
            glGenBuffers(1, &quadVBO);
            glBindVertexArray(quadVAO);
            glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
            glEnableVertexAttribArray(1);
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
        }
        glBindVertexArray(quadVAO);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glBindVertexArray(0);
    }

    void initShadowFramebuffer(){

        if (!simpleDepthShader) simpleDepthShader = std::make_unique<ppgso::Shader>(shadow_mapping_depth_vert_glsl, shadow_mapping_depth_frag_glsl);
        if (!debugQuad) debugQuad = std::make_unique<ppgso::Shader>(debug_quad_vert_glsl, debug_quad_frag_glsl);

        glGenFramebuffers(1, &depthMapFBO);
        // create depth texture

        glGenTextures(1, &scene.depthMap);
        glBindTexture(GL_TEXTURE_2D, scene.depthMap);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        float borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
        // attach depth texture as FBO's depth buffer
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, scene.depthMap, 0);
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        debugQuad->use();
        debugQuad->setUniform("depthMap", 0);
    }

    void renderFromLight(){
        glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
        glClear(GL_DEPTH_BUFFER_BIT);
        scene.render_shadow();
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // reset viewport
        glViewport(0, 0, WINDOW_HEIGHT, WINDOW_WIDTH);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

public:
    MainScene() : Window{"Stahovec robí sám", WINDOW_HEIGHT, WINDOW_WIDTH} {
        // Initialize OpenGL state
        // Enable Z-buffer
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);

        initializeCustomFramebuffer();
        initShadowFramebuffer();

        scene.heightMap = readBMP("Height.bmp");

        auto camera = std::make_unique<Camera>();
        scene.camera = move(camera);

        scene.Renderable_objects.push_back(std::make_unique<Island>());
        scene.Renderable_objects.push_back(std::make_unique<Surface>());
        scene.Renderable_objects.push_back(std::make_unique<Cloud>());
        scene.Renderable_objects.push_back(std::make_unique<SkyBox>());
        scene.Renderable_objects.push_back(std::make_unique<House>(-25,scene.get_Y(-25,-40,scene.heightMap),-40));
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

        //glowing objects
        scene.Renderable_objects.push_back(std::make_unique<Firefly>((glm::vec3){11, scene.get_Y(11,50,scene.heightMap)+4, 50}, (glm::vec3){5, 13, 0.3}, 0));
        scene.Renderable_objects.push_back(std::make_unique<Firefly>((glm::vec3){-75, scene.get_Y(-75,30,scene.heightMap)+4, 30}, (glm::vec3){16, 2, 30}, 1));
        scene.Renderable_objects.push_back(std::make_unique<Firefly>((glm::vec3){50, scene.get_Y(50,0,scene.heightMap)+4, 0}, (glm::vec3){0.3, 6, 13}, 2));

        scene.Renderable_objects.push_back(std::make_unique<Light>(glm::vec3 {-25, scene.get_Y(-25, 20, scene.heightMap) + 10.25, 20}, glm::vec3 {3*ppgso::PI/2,0,0}, glm::vec3 {0.4, 12.1, 0.95}));
        scene.Renderable_objects.push_back(std::make_unique<Light>(glm::vec3 {22, scene.get_Y(22, 33, scene.heightMap) + 5.25, 33}, glm::vec3 {3*ppgso::PI/2,0,0}, glm::vec3 {0.1, 0.7, 12.9}));
        scene.Renderable_objects.push_back(std::make_unique<Light>(glm::vec3 {11, scene.get_Y(11, 22, scene.heightMap) + 0.25, 22}, glm::vec3 {3*ppgso::PI/2,0,0}, glm::vec3 {13, 0.5, 0.3}));

        //sun
        scene.Renderable_objects.push_back(std::make_unique<Light>(scene.globalLightPosition-glm::vec3{0,-1,0}, glm::vec3 {3*ppgso::PI/2,0,0}, glm::vec3 {13, 13, 13}));



        /*for(int i=-100; i<100; i++){
            for(int j=-100; j<100;j++){
                float x = i;
                float z = j;
                int y = get_Y(x,z,scene.heightMap);
                std::cout << y<<"\n";
                scene.Renderable_objects.push_back(std::make_unique<Plant_1>(x, y, z));

            }
        }*/

        //plants
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

        scene.addLight(glm::vec3 {11,scene.get_Y(11, 50, scene.heightMap),50},
                       glm::vec3 {6, 13, 0.3},
                       glm::vec3 {0, 0, 0},
                       15.f,
                       .25f,
                       glm::vec3 {0.05f, 0.05f, 0.05f},
                       glm::vec3 {0.85f, 0.85f, 0.85f},
                       glm::vec3 {1.0f, 1.0f, 1.0f},
                       glm::cos(glm::radians(12.5f)),
                       glm::cos(glm::radians(15.0f)),
                       false,
                       0
        );

        scene.addLight(glm::vec3 {-75,scene.get_Y(-75, 30, scene.heightMap),30},
                       glm::vec3 {16, 2, 30},
                       glm::vec3 {0, 0, 0},
                       15.f,
                       .25f,
                       glm::vec3 {0.05f, 0.05f, 0.05f},
                       glm::vec3 {0.85f, 0.85f, 0.85f},
                       glm::vec3 {1.0f, 1.0f, 1.0f},
                       glm::cos(glm::radians(12.5f)),
                       glm::cos(glm::radians(15.0f)),
                       false,
                       1
        );

        scene.addLight(glm::vec3 {50,scene.get_Y(50, 0, scene.heightMap),0},
                       glm::vec3 {0.3, 11, 27},
                       glm::vec3 {0, 0, 0},
                       15.f,
                       .25f,
                       glm::vec3 {0.05f, 0.05f, 0.05f},
                       glm::vec3 {0.85f, 0.85f, 0.85f},
                       glm::vec3 {1.0f, 1.0f, 1.0f},
                       glm::cos(glm::radians(12.5f)),
                       glm::cos(glm::radians(15.0f)),
                       false,
                       2
        );

        //red
        scene.addLight(glm::vec3 {11,scene.get_Y(11, 22, scene.heightMap) + 0.25,22},
                       glm::vec3 {13, 0.5, 0.3},
                       glm::vec3 {0, 0, 0},
                       15.f,
                       6.f,
                       glm::vec3 {0.05f, 0.05f, 0.05f},
                       glm::vec3 {0.85f, 0.85f, 0.85f},
                       glm::vec3 {1.0f, 1.0f, 1.0f},
                       glm::cos(glm::radians(12.5f)),
                       glm::cos(glm::radians(15.0f)),
                       false,
                       3
                );

        //blue
        scene.addLight(glm::vec3 {22,scene.get_Y(22, 33, scene.heightMap) + 5.25,33},
                       glm::vec3 {0.1, 0.7, 12.9},
                       glm::vec3 {0, 0, 0},
                       15.f,
                       5.f,
                       glm::vec3 {0.05f, 0.05f, 0.05f},
                       glm::vec3 {0.85f, 0.85f, 0.85f},
                       glm::vec3 {1.0f, 1.0f, 1.0f},
                       glm::cos(glm::radians(12.5f)),
                       glm::cos(glm::radians(15.0f)),
                       false,
                       4
        );

        //this is spotlight green
        scene.addLight(glm::vec3 {-25,scene.get_Y(-25, 20, scene.heightMap) + 10.25,20},
                       glm::vec3 {0.4, 12.1, 0.95},
                       glm::vec3 {-0.25, -1, -0.5},
                       15.f,
                       12.f,
                       glm::vec3 {0.0f, 0.0f, 0.0f},
                       glm::vec3 {1.0f, 1.0f, 1.0f},
                       glm::vec3 {1.0f, 1.0f, 1.0f},
                       glm::cos(glm::radians(12.5f)),
                       glm::cos(glm::radians(15.0f)),
                       true,
                       5
        );

        glm::mat4 lightProjection, lightView;
        glm::mat4 lightSpaceMatrix;

        lightProjection = glm::perspective(glm::radians(120.0f), 16.f / 9.f, scene.near_plane, scene.far_plane); // note that if you use a perspective projection matrix you'll have to change the light position as the current light position isn't enough to reflect the whole scene
        //lightProjection = glm::ortho(-0.0f, 100.0f, -.0f, 100.0f, scene.near_plane, scene.far_plane);
        lightView = glm::lookAt(scene.globalLightPosition, glm::vec3(5.f), glm::vec3(0.0, 1.0, 0.0));
        scene.lightSpaceMatrix = lightProjection * lightView;

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
        //std::cout << 1/(scene.current_frame_time - scene.last_frame_time) << "\n";

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

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);




        // Update all objects in scene
        // Because we need to delete while iterating this is implemented using c++ iterators
        // In most languages mutating the container during iteration is undefined behaviour
        scene.update(dTime);

        // Render every object in scene

        renderFromLight();

        bindToCustomFramebuffer();
        scene.render();

        debugQuad->use();
        debugQuad->setUniform("near_plane", scene.near_plane);
        debugQuad->setUniform("far_plane", scene.far_plane);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, scene.depthMap);
        //renderQuad();

        rebindToOriginalFramebuffer();

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