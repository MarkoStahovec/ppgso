//
// Created by A on 04/11/2022.
//

#ifndef PPGSO_SCENEWINDOW_H
#define PPGSO_SCENEWINDOW_H

#include <map>
#include <list>
#include "Renderable.h"
#include "Camera.h"

class SceneWindow {
public:
    //std::unique_ptr<Camera> camera;
    std::list<std::unique_ptr<Renderable>> Renderable_objects;

    typedef enum
    {
        AFTERNOON = 1,
        EVENING = 2,
        NIGHT = 3,
        MORNING = 4
    } DayTime;

    DayTime dayTime = NIGHT;
    float delta;
    float step = 1.;
    float start_near_plane, end_near_plane, start_far_plane, end_far_plane;
    glm::vec3 startGlobalLightPosition, endGlobalLightPosition, startGlobalLightDirection, endGlobalLightDirection, startGlobalLightColor, endGlobalLightColor, startGlobalLightAmbient,
    endGlobalLightAmbient, startGlobalLightDiffuse, endGlobalLightDiffuse, startGlobalLightSpecular, endGlobalLightSpecular;
    glm::vec3 startSkyboxAmbient, endSkyboxAmbient, startSkyboxDiffuse, endSkyboxDiffuse, startSkyboxSpecular, endSkyboxSpecular;
    float startSkyboxShininess, endSkyboxShininess;
    glm::vec3 skyboxAmbient = {0.44, 0.44, 0.88};
    glm::vec3 skyboxDiffuse = {0.22, 0.22, 0.24};
    glm::vec3 skyboxSpecular = {0.356, 0.356, 0.485};
    float skyboxShininess = 9.21f;

    bool isTimeOn = false;

    std::unique_ptr<Camera> camera;
    Renderable * root;
    Renderable * cloud;
    Renderable * palm;

    // Keyboard state
    std::map< int, int > keyboard;

    float current_frame_time;
    float last_frame_time;



    float near_plane = 960.0f, far_plane = 3000.f;

    glm::vec3 gravity = {0,10,0};
    glm::vec3 wind = {0,0,0};

    glm::vec3 globalLightPosition = {-500.f, 900, -0.f};

    glm::mat4 lightSpaceMatrix;

    unsigned int depthMap;

    glm::vec3 globalLightDirection = {0.25, 0.2, 0.5};
    glm::vec3 globalLightColor = {0.42,0.45,0.86};
    glm::vec3 globalLightAmbient = {0.041,0.041,0.049};
    glm::vec3 globalLightDiffuse = {0.36f, 0.36, 0.5348};
    glm::vec3 globalLightSpecular = {0.856,0.856,0.961};

    struct Lights {
        glm::vec3 position[100];
        glm::vec3 direction[100];

        float cutOff[100];
        float outerCutOff[100];
        bool isSpot[100];

        glm::vec3 color[100];
        float intensity[100];
        float radius[100];

        glm::vec3 ambient[100];
        glm::vec3 diffuse[100];
        glm::vec3 specular[100];
    };

    Lights lights;

    unsigned char * heightMap;

    void update(float time);
    void render();
    void render_shadow();

    // 0-255 ~ 0-21
    float get_Y(float x, float z, unsigned char * heightMap);

    void addLight(glm::vec3 position,
                  glm::vec3 color,
                  glm::vec3 direction,
                  float radius,
                  float intensity,
                  glm::vec3 ambient,
                  glm::vec3 diffuse,
                  glm::vec3 specular,
                  float cutOff,
                  float outerCutOff,
                  bool isSpot,
                  int index
                  );


};

#endif //PPGSO_SCENEWINDOW_H
