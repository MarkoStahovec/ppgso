//
// Created by A on 04/11/2022.
// Special thanks to learnopengl tutorials
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

#include "utils.h"
#include "Ufo.h"
#include "Palm.h"
#include "BeachBall.h"
#include "Boat.h"
#include "Firefly.h"
#include "Asteroid.h"
#include "Emerald.h"
#include "Shark.h"
#include "Hat.h"



const unsigned int SIZE = 512;
const unsigned int SHADOW_WIDTH = 4096*4, SHADOW_HEIGHT = 4096*4;

bool firstMouse = true;
float yaw   = -90.0f;	// yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right so we initially rotate a bit to the left.
float pitch =  0.0f;
float lastX =  1920.0f;
float lastY =  1080.0f;

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
SceneWindow scene;

extern "C"
{
__declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}

static void cursor_position_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top
    lastX = xpos;
    lastY = ypos;

    float sensitivity = 0.091f; // change this value to your liking
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    // make sure that when pitch is out of bounds, screen doesn't get flipped
    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;

    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    scene.camera->cameraFront = glm::normalize(front);
}

class MainScene : public ppgso::Window {
private:
    // Scene of objects

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
        glfwMakeContextCurrent(window);
        //glfwSetCursorPosCallback(window, reinterpret_cast<GLFWcursorposfun>(delegate_mouse_callback));
        glfwSetCursorPosCallback(window, cursor_position_callback);
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);

        initializeCustomFramebuffer();
        initShadowFramebuffer();

        scene.heightMap = readBMP("Height.bmp");

        auto camera = std::make_unique<Camera>();
        scene.camera = std::move(camera);

        auto island_h = std::make_unique<Island>();
        scene.root = island_h.get();
        island_h->inherit = false;

        auto surface_h= std::make_unique<Surface>();
        surface_h->parent = island_h.get();

        auto cloud_h = std::make_unique<Cloud>();
        scene.cloud = cloud_h.get();
        cloud_h->parent = island_h.get();

        auto skybox_h = std::make_unique<SkyBox>();
        skybox_h->parent = island_h.get();

        auto house_h = std::make_unique<House>(-25, scene.get_Y(-25, -40, scene.heightMap), -40);
        house_h->parent = island_h.get();

        auto ufo_h = std::make_unique<Ufo>();
        ufo_h->parent = island_h.get();

        auto asteroid_h1 = std::make_unique<Asteroid>(1);
        asteroid_h1->parent = ufo_h.get();

        auto asteroid_h2 = std::make_unique<Asteroid>(0);
        asteroid_h2->parent = ufo_h.get();

        auto emerald_h1 = std::make_unique<Emerald>(1);
        emerald_h1->parent = asteroid_h1.get();

        auto emerald_h2 = std::make_unique<Emerald>(0);
        emerald_h2->parent = asteroid_h1.get();

        auto emerald_h3 = std::make_unique<Emerald>(1);
        emerald_h3->parent = asteroid_h2.get();

        auto emerald_h4 = std::make_unique<Emerald>(0);
        emerald_h4->parent = asteroid_h2.get();


        auto boat_h = std::make_unique<Boat>();
        boat_h->parent = island_h.get();

        auto shark_h = std::make_unique<Shark>();
        shark_h->parent = boat_h.get();

        auto hat_h = std::make_unique<Hat>();
        hat_h->parent = shark_h.get();

        auto palm_h1 = std::make_unique<Palm>(30,scene.get_Y(30,10,scene.heightMap), 10,(glm::vec3) {0, 0, 0}, (glm::vec3) {0.15,0.15,0.15});
        palm_h1->parent = island_h.get();

        auto coconut_h1 = std::make_unique<BeachBall>(29, scene.get_Y(29, 10, scene.heightMap), 10);
        coconut_h1->parent = island_h.get();

        auto coconut_h2 = std::make_unique<BeachBall>(31, scene.get_Y(31, 10, scene.heightMap), 10);
        coconut_h2->parent = island_h.get();

        scene.palm = palm_h1.get();

        auto palm_h2 = std::make_unique<Palm>(-68,scene.get_Y(-68,78,scene.heightMap),78,(glm::vec3) {-.05,0,.022}, (glm::vec3) {0.15,0.2,0.15});
        palm_h2->parent = island_h.get();

        auto palm_h3 = std::make_unique<Palm>(30,scene.get_Y(30,73,scene.heightMap),73,(glm::vec3) {.05,0,-.022}, (glm::vec3) {0.2,0.15,0.2});
        palm_h3->parent = island_h.get();

        auto palm_h4 = std::make_unique<Palm>(-35,scene.get_Y(-35,-40,scene.heightMap),-40,(glm::vec3) {.01,0,-.01}, (glm::vec3) {0.15,0.12,0.15});
        palm_h4->parent = island_h.get();

        auto palm_h5 = std::make_unique<Palm>(68,scene.get_Y(68,-78,scene.heightMap),-78,(glm::vec3) {.05,0,.022}, (glm::vec3) {0.15,0.15,0.15});
        palm_h5->parent = island_h.get();

        auto firefly_h1 = std::make_unique<Firefly>((glm::vec3){11, scene.get_Y(11,50,scene.heightMap)+4, 50}, (glm::vec3){5, 13, 0.3}, 0);
        firefly_h1->parent = island_h.get();

        auto firefly_h2 = std::make_unique<Firefly>((glm::vec3){-75, scene.get_Y(-75,30,scene.heightMap)+4, 30}, (glm::vec3){16, 2, 30}, 1);
        firefly_h2->parent = island_h.get();

        auto firefly_h3 = std::make_unique<Firefly>((glm::vec3){50, scene.get_Y(50,0,scene.heightMap)+4, 0}, (glm::vec3){0.3, 6, 13}, 2);
        firefly_h3->parent = island_h.get();

        scene.Renderable_objects.push_back(std::move(island_h));
        scene.Renderable_objects.push_back(std::move(surface_h));
        scene.Renderable_objects.push_back(std::move(cloud_h));
        scene.Renderable_objects.push_back(std::move(skybox_h));
        scene.Renderable_objects.push_back(std::move(house_h));
        scene.Renderable_objects.push_back(std::move(ufo_h));
        scene.Renderable_objects.push_back(std::move(asteroid_h1));
        scene.Renderable_objects.push_back(std::move(asteroid_h2));
        scene.Renderable_objects.push_back(std::move(emerald_h1));
        scene.Renderable_objects.push_back(std::move(emerald_h2));
        scene.Renderable_objects.push_back(std::move(emerald_h3));
        scene.Renderable_objects.push_back(std::move(emerald_h4));
        scene.Renderable_objects.push_back(std::move(boat_h));
        scene.Renderable_objects.push_back(std::move(shark_h));
        scene.Renderable_objects.push_back(std::move(hat_h));
        scene.Renderable_objects.push_back(std::move(palm_h1));
        scene.Renderable_objects.push_back(std::move(palm_h2));
        scene.Renderable_objects.push_back(std::move(palm_h3));
        scene.Renderable_objects.push_back(std::move(palm_h4));
        scene.Renderable_objects.push_back(std::move(palm_h5));
        scene.Renderable_objects.push_back(std::move(coconut_h1));
        scene.Renderable_objects.push_back(std::move(coconut_h2));

        //glowing objects
        scene.Renderable_objects.push_back(std::move(firefly_h1));
        scene.Renderable_objects.push_back(std::move(firefly_h2));
        scene.Renderable_objects.push_back(std::move(firefly_h3));

        //sun
        scene.Renderable_objects.push_back(std::make_unique<Light>(scene.globalLightPosition-glm::vec3{0,-1,0}, glm::vec3 {3*ppgso::PI/2,0,0}, glm::vec3 {13, 13, 13},2, false, -1));



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
        generate_torches(scene);

        for(int i=0; i<50; i++){
            int x = random_float(-100,100);
            int z = random_float(-100,100);
            int y = scene.get_Y(x, z, scene.heightMap);
            if (y <= 4 || y >15 || (x < -15 && x > -35 && z < -30 && z >-50)) {
                i--;
            } else {

                scene.Renderable_objects.push_back(random_plant(x, y, z, scene));
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

        //this is spotlight green
        scene.addLight(glm::vec3 {-25,scene.get_Y(-25, 20, scene.heightMap) + 10.25,20},
                       glm::vec3 {0.4, 15.1, 0.95},
                       glm::vec3 {0, -1, 0},
                       15.f,
                       15.f,
                       glm::vec3 {0.0f, 0.0f, 0.0f},
                       glm::vec3 {1.0f, 1.0f, 1.0f},
                       glm::vec3 {1.0f, 1.0f, 1.0f},
                       glm::cos(glm::radians(12.5f)),
                       glm::cos(glm::radians(15.0f)),
                       true,
                       3
        );

        glm::mat4 lightProjection, lightView;
        glm::mat4 lightSpaceMatrix;

        scene.lightSpaceMatrix = glm::perspective(glm::radians(90.0f), 16.f / 9.f, scene.near_plane, scene.far_plane) * glm::lookAt(scene.globalLightPosition, scene.globalLightDirection, glm::vec3(0.0, 1.0, 0.0));

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
            scene.camera->increment = 0.02;
            scene.camera->startPos = {145.643326, 97.8423615, -57.1512032};
            scene.camera->endPos = {-87.7087097, 107.765419, 206.029327};
            scene.camera->startPosLookAt = {-0.812781215f, -0.534425974, 0.231895626f};
            scene.camera->endPosLookAt = {0.466904998, -0.391100347f, -0.793120742f};
            scene.camera->isAnimating = true;
            scene.keyboard[GLFW_KEY_1] = GLFW_RELEASE;
        }

        if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) {
            scene.camera->t = 0;
            scene.camera->increment = 0.02;
            scene.camera->startPos = {36.7917938, 87.1458511, -438.971863};
            scene.camera->endPos = {306.790771, 8.1699028, -11.1807976};
            scene.camera->startPosLookAt = {-0.696514428f, -0.158123508f, 0.699903309f};
            scene.camera->endPosLookAt = {-0.932051837f, 0.231391191f, -0.278814524f};
            scene.camera->isAnimating = true;
            scene.keyboard[GLFW_KEY_2] = GLFW_RELEASE;
        }

        if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS) {
            scene.camera->t = 0;
            scene.camera->increment = 0.08;
            scene.camera->startPos = {-26.1184998, 11.3824825, -36.625618};
            scene.camera->endPos = {-32.62, 20.233, 3.4220603};
            scene.camera->startPosLookAt = {0.179955497, -0.259071678, -0.948576655f};
            scene.camera->endPosLookAt = {0.179955497, -0.259071678, -0.948576655f};
            scene.camera->isAnimating = true;
            scene.keyboard[GLFW_KEY_3] = GLFW_RELEASE;
        }

        if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS) {
            scene.camera->t = 0;
            scene.camera->increment = 0.15;
            scene.camera->startPos = {35.08734,33.138629,40.3482895};
            scene.camera->endPos = {27.4320087,8.92064857,27.257185};
            scene.camera->startPosLookAt = {-0.181552365,0.030137497,-0.982919455};
            scene.camera->endPosLookAt = {0.201891154,-0.00794147141,-0.97937578};
            scene.camera->isAnimating = true;
            scene.keyboard[GLFW_KEY_4] = GLFW_RELEASE;
        }

        if (glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS) {
            scene.camera->t = 0;
            scene.camera->increment = 0.022;
            scene.camera->startPos = {-146.0928224,133.611469,-111.249485};
            scene.camera->endPos = {168.674663,131.175262,-158.81233};
            scene.camera->startPosLookAt = {0.770972711,-0.215894718,0.599211866};
            scene.camera->endPosLookAt = {-0.735852261,-0.162858855,0.657227303};
            scene.camera->isAnimating = true;
            scene.keyboard[GLFW_KEY_5] = GLFW_RELEASE;
        }

        if (glfwGetKey(window, GLFW_KEY_6) == GLFW_PRESS) {
            scene.camera->t = 0;
            scene.camera->increment = 0.05;
            scene.camera->startPos = {306.790771, 8.1699028, -11.1807976};
            scene.camera->endPos = {36.7917938, 87.1458511, -438.971863};
            scene.camera->startPosLookAt = {-0.932051837f, 0.231391191f, -0.278814524f};
            scene.camera->endPosLookAt = {-0.696514428f, -0.158123508f, 0.699903309f};
            scene.camera->isAnimating = true;
            scene.keyboard[GLFW_KEY_6] = GLFW_RELEASE;
        }

        if (scene.keyboard[GLFW_KEY_SPACE]){
            raise_wind(scene);
        }

        if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
            if(scene.dayTime == scene.MORNING) {
                scene.near_plane = 600.0f;
                scene.far_plane = 1200.f;
                scene.globalLightPosition = {-500.f, 900, -0.f};
                scene.globalLightDirection = {0.0, 0.0, 0.0};
                scene.globalLightColor = {2.48,2.48,2.476};
                scene.globalLightAmbient = {0.266,0.266,0.262};
                scene.globalLightDiffuse = {0.80, 0.80, 0.796};
                scene.globalLightSpecular = {0.962,0.951,0.940};

                scene.skyboxAmbient = {0.42, 0.427, 0.889};
                scene.skyboxDiffuse = {0.25, 0.245, 0.477};
                scene.skyboxSpecular = {0.325, 0.324, 0.361};
                scene.skyboxShininess = 0.824f;

                scene.dayTime = scene.AFTERNOON;
            }
            else if(scene.dayTime == scene.AFTERNOON) {
                generate_torches(scene);
                scene.near_plane = 450.0f;
                scene.far_plane = 1020.f;
                scene.globalLightPosition = {-600.f, 357, -0.f};
                scene.globalLightDirection = {0.0, 0.0, 0.0};
                scene.globalLightColor = {4.85,1.78,0.376};
                scene.globalLightAmbient = {0.191, 0.086, 0.071};
                scene.globalLightDiffuse = {0.987, 0.631, 0.555};
                scene.globalLightSpecular = {0.731,0.272,0.151};

                scene.skyboxAmbient = {0.68, 0.605, 0.547};
                scene.skyboxDiffuse = {0.33, 0.301, 0.265};
                scene.skyboxSpecular = {0.385, 0.352, 0.326};
                scene.skyboxShininess = 1.93f;

                scene.dayTime = scene.EVENING;
            }
            else if(scene.dayTime == scene.EVENING) {

                scene.near_plane = 600.0f;
                scene.far_plane = 1200.f;
                scene.globalLightPosition = {-500.f, 900, -0.f};
                scene.globalLightDirection = {0.0, 0.0, 0.0};
                scene.globalLightColor = {0.42,0.45,0.86};
                scene.globalLightAmbient = {0.041,0.041,0.049};
                scene.globalLightDiffuse = {0.36f, 0.36, 0.5348};
                scene.globalLightSpecular = {0.856,0.856,0.961};

                scene.skyboxAmbient = {0.44, 0.44, 0.88};
                scene.skyboxDiffuse = {0.22, 0.22, 0.24};
                scene.skyboxSpecular = {0.356, 0.356, 0.485};
                scene.skyboxShininess = 9.21f;

                scene.dayTime = scene.NIGHT;
            }
            else {
                scene.near_plane = 450.0f;
                scene.far_plane = 1020.f;
                scene.globalLightPosition = {-600.f, 357, -0.f};
                scene.globalLightDirection = {0.0, 0.0, 0.0};
                scene.globalLightColor = {2.48,1.58,1.476};
                scene.globalLightAmbient = {0.130,0.113,0.101};
                scene.globalLightDiffuse = {0.945, 0.926, 0.841};
                scene.globalLightSpecular = {0.653,0.635,0.607};

                scene.skyboxAmbient = {0.92, 0.88, 0.87};
                scene.skyboxDiffuse = {0.83, 0.82, 0.79};
                scene.skyboxSpecular = {0.421, 0.405, 0.379};
                scene.skyboxShininess = 0.07f;

                scene.dayTime = scene.MORNING;
            }
            scene.keyboard[GLFW_KEY_Q] = GLFW_RELEASE;
        }

        if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
            if(scene.isTimeOn) {
                scene.isTimeOn = false;
            }
            else {
                if(scene.dayTime == scene.MORNING) {
                    scene.start_near_plane = 450.0f;
                    scene.end_near_plane = 600.0f;
                    scene.start_far_plane = 1020.0f;
                    scene.end_far_plane = 1200.0f;
                    scene.startGlobalLightPosition = {-600.f, 357, -0.f};
                    scene.endGlobalLightPosition = {-500.f, 900, -0.f};
                    scene.startGlobalLightDirection = {0.0, 0.0, 0.0};
                    scene.endGlobalLightDirection = {0.0, 0.0, 0.0};
                    scene.startGlobalLightColor = {2.48,1.58,1.476};
                    scene.endGlobalLightColor = {2.48,2.48,2.476};
                    scene.startGlobalLightAmbient = {0.130,0.113,0.101};
                    scene.endGlobalLightAmbient = {0.266,0.266,0.262};
                    scene.startGlobalLightDiffuse = {0.945, 0.926, 0.841};
                    scene.endGlobalLightDiffuse = {0.80, 0.80, 0.796};
                    scene.startGlobalLightSpecular = {0.653,0.635,0.607};
                    scene.endGlobalLightSpecular = {0.962,0.951,0.940};

                    scene.startSkyboxAmbient = {0.92, 0.88, 0.87};
                    scene.endSkyboxAmbient = {0.42, 0.427, 0.889};
                    scene.startSkyboxDiffuse = {0.83, 0.82, 0.79};
                    scene.endSkyboxDiffuse = {0.25, 0.245, 0.477};
                    scene.startSkyboxSpecular = {0.421, 0.405, 0.379};
                    scene.endSkyboxSpecular = {0.325, 0.324, 0.361};
                    scene.startSkyboxShininess = 0.07f;
                    scene.endSkyboxShininess = 0.824f;
                }
                else if(scene.dayTime == scene.AFTERNOON) {
                    scene.start_near_plane = 600.0f;
                    scene.end_near_plane = 450.f;
                    scene.start_far_plane = 1200.0f;
                    scene.end_far_plane = 1020.0f;
                    scene.startGlobalLightPosition = {-500.f, 900, -0.f};
                    scene.endGlobalLightPosition = {-600.f, 357, -0.f};
                    scene.startGlobalLightDirection = {0.0, 0.0, 0.0};
                    scene.endGlobalLightDirection = {0.0, 0.0, 0.0};
                    scene.startGlobalLightColor = {2.48,2.48,2.476};
                    scene.endGlobalLightColor = {4.85,1.78,0.376};
                    scene.startGlobalLightAmbient = {0.266,0.266,0.262};
                    scene.endGlobalLightAmbient = {0.191, 0.086, 0.071};
                    scene.startGlobalLightDiffuse = {0.80, 0.80, 0.796};
                    scene.endGlobalLightDiffuse = {0.987, 0.631, 0.555};
                    scene.startGlobalLightSpecular = {0.962,0.951,0.940};
                    scene.endGlobalLightSpecular = {0.731,0.272,0.151};

                    scene.startSkyboxAmbient = {0.42, 0.427, 0.889};
                    scene.endSkyboxAmbient = {0.68, 0.605, 0.547};
                    scene.startSkyboxDiffuse = {0.25, 0.245, 0.477};
                    scene.endSkyboxDiffuse = {0.33, 0.301, 0.265};
                    scene.startSkyboxSpecular = {0.325, 0.324, 0.361};
                    scene.endSkyboxSpecular = {0.385, 0.352, 0.326};
                    scene.startSkyboxShininess = 0.824f;
                    scene.endSkyboxShininess = 1.93f;
                }
                else if(scene.dayTime == scene.EVENING) {
                    scene.start_near_plane = 450.0f;
                    scene.end_near_plane = 600.0f;
                    scene.start_far_plane = 1020.0f;
                    scene.end_far_plane = 1200.0f;
                    scene.startGlobalLightPosition = {-600.f, 357, -0.f};
                    scene.endGlobalLightPosition = {-500.f, 900, -0.f};
                    scene.startGlobalLightDirection = {0.0, 0.0, 0.0};
                    scene.endGlobalLightDirection = {0.0, 0.0, 0.0};
                    scene.startGlobalLightColor = {4.85,1.78,0.376};
                    scene.endGlobalLightColor = {0.42,0.45,0.86};
                    scene.startGlobalLightAmbient = {0.191, 0.086, 0.071};
                    scene.endGlobalLightAmbient = {0.041,0.041,0.049};
                    scene.startGlobalLightDiffuse = {0.987, 0.631, 0.555};
                    scene.endGlobalLightDiffuse = {0.36f, 0.36, 0.5348};
                    scene.startGlobalLightSpecular = {0.731,0.272,0.151};
                    scene.endGlobalLightSpecular = {0.856,0.856,0.961};

                    scene.startSkyboxAmbient = {0.68, 0.605, 0.547};
                    scene.endSkyboxAmbient = {0.44, 0.44, 0.88};
                    scene.startSkyboxDiffuse = {0.33, 0.301, 0.265};
                    scene.endSkyboxDiffuse = {0.22, 0.22, 0.24};
                    scene.startSkyboxSpecular = {0.385, 0.352, 0.326};
                    scene.endSkyboxSpecular = {0.356, 0.356, 0.485};
                    scene.startSkyboxShininess = 1.93f;
                    scene.endSkyboxShininess = 9.21f;
                }
                else if(scene.dayTime == scene.NIGHT)  {
                    scene.start_near_plane = 600.0f;
                    scene.end_near_plane = 450.0f;
                    scene.start_far_plane = 1200.0f;
                    scene.end_far_plane = 1020.0f;
                    scene.startGlobalLightPosition = {-500.f, 900, -0.f};
                    scene.endGlobalLightPosition = {-600.f, 357, -0.f};
                    scene.startGlobalLightDirection = {0.0, 0.0, 0.0};
                    scene.endGlobalLightDirection = {0.0, 0.0, 0.0};
                    scene.startGlobalLightColor = {0.42,0.45,0.86};
                    scene.endGlobalLightColor = {2.48,1.58,1.476};
                    scene.startGlobalLightAmbient = {0.041,0.041,0.049};
                    scene.endGlobalLightAmbient = {0.130,0.113,0.101};
                    scene.startGlobalLightDiffuse = {0.36f, 0.36, 0.5348};
                    scene.endGlobalLightDiffuse = {0.945, 0.926, 0.841};
                    scene.startGlobalLightSpecular = {0.856,0.856,0.961};
                    scene.endGlobalLightSpecular = {0.653,0.635,0.607};

                    scene.startSkyboxAmbient = {0.44, 0.44, 0.88};
                    scene.endSkyboxAmbient = {0.92, 0.88, 0.87};
                    scene.startSkyboxDiffuse = {0.22, 0.22, 0.24};
                    scene.endSkyboxDiffuse = {0.83, 0.82, 0.79};
                    scene.startSkyboxSpecular = {0.356, 0.356, 0.485};
                    scene.endSkyboxSpecular = {0.421, 0.405, 0.379};
                    scene.startSkyboxShininess = 9.21f;
                    scene.endSkyboxShininess = 0.07f;
                }
                scene.isTimeOn = true;
            }
            scene.keyboard[GLFW_KEY_E] = GLFW_RELEASE;
        }

        if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
            scene.step *= 2;
            scene.keyboard[GLFW_KEY_R] = GLFW_RELEASE;
        }

        if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS) {
            scene.step /= 2;
            scene.keyboard[GLFW_KEY_F] = GLFW_RELEASE;
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
            glm::vec3 position = glm::sphericalRand(50.0);
            position+=scene.cloud->position;
            position[1] = scene.cloud->position.y;
            glm::vec3 rotation = generate_random_vec3(-ppgso::PI, ppgso::PI);
            glm::vec3 speed = {0, 0, 0};
            glm::vec3 scale = {1, 1, 1};
            glm::vec3 color = {0, ((float) rand() / (float) RAND_MAX) * (.8 - .2) + .3, 1};

            auto raindrop_h = std::make_unique<Rain_Drop>(position, speed, color, scale, 0);
            raindrop_h->parent = scene.root;

            scene.Renderable_objects.push_back(std::move(raindrop_h));
        }

        calm_wind(scene, window);

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);




        // Update all objects in scene
        // Because we need to delete while iterating this is implemented using c++ iterators
        // In most languages mutating the container during iteration is undefined behaviour
        scene.update(dTime);

        // Render every object in scene
        scene.lightSpaceMatrix = glm::perspective(glm::radians(90.0f), 16.f / 9.f, scene.near_plane, scene.far_plane) * glm::lookAt(scene.globalLightPosition, scene.globalLightDirection, glm::vec3(0.0, 1.0, 0.0));
        glCullFace(GL_FRONT);
        renderFromLight();
        glCullFace(GL_BACK); // don't forget to reset original culling face
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
        std::cout << scene.dayTime << "\n";
    }
};

int main() {
    // Create new window
    auto window = MainScene{};

    // Main execution loop
    while (window.pollEvents()) {}

    return EXIT_SUCCESS;
}