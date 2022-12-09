#include "SceneWindow.h"

void SceneWindow::update(float dTime) {
    auto i = std::begin(Renderable_objects);
    while (i != std::end(Renderable_objects)) {
        // Update object and remove from list if needed
        auto obj = i->get();
        if (!obj->update(dTime, *this))
            i = Renderable_objects.erase(i);
        else
            ++i;
    }

    camera->update();
    if(isTimeOn) {
        delta += step;

        near_plane = glm::lerp(start_near_plane, end_near_plane, delta / 60.0f);
        far_plane = glm::lerp(start_far_plane, end_far_plane, delta / 60.0f);
        globalLightPosition = glm::lerp(startGlobalLightPosition, endGlobalLightPosition, delta / 60.0f);
        globalLightDirection = glm::lerp(startGlobalLightDirection, endGlobalLightDirection, delta / 60.0f);
        globalLightColor = glm::lerp(startGlobalLightColor, endGlobalLightColor, delta / 60.0f);
        globalLightAmbient = glm::lerp(startGlobalLightAmbient, endGlobalLightAmbient, delta / 60.0f);
        globalLightDiffuse = glm::lerp(startGlobalLightDiffuse, endGlobalLightDiffuse, delta / 60.0f);
        globalLightSpecular = glm::lerp(startGlobalLightSpecular, endGlobalLightSpecular, delta / 60.0f);

        skyboxAmbient = glm::lerp(startSkyboxAmbient, endSkyboxAmbient, delta / 60.0f);
        skyboxDiffuse = glm::lerp(startSkyboxDiffuse, endSkyboxDiffuse, delta / 60.0f);
        skyboxSpecular = glm::lerp(startSkyboxSpecular, endSkyboxSpecular, delta / 60.0f);
        skyboxShininess = glm::lerp(startSkyboxShininess, endSkyboxShininess, delta / 60.0f);
        if(delta > 60) {
            if(dayTime == MORNING) {
                dayTime = AFTERNOON;
            }
            else if(dayTime == AFTERNOON) {
                dayTime = EVENING;
            }
            else if(dayTime == EVENING) {
                dayTime = NIGHT;
            }
            else if(dayTime == NIGHT) {
                dayTime = MORNING;
            }
            if(dayTime == MORNING) {
                start_near_plane = 960.0f;
                end_near_plane = 615.f;
                start_far_plane = 3000.0f;
                end_far_plane = 3000.0f;
                startGlobalLightPosition = {-500.f, 900, -0.f};
                endGlobalLightPosition = {-600.f, 357, -0.f};
                startGlobalLightDirection = {0.25, 0.2, 0.5};
                endGlobalLightDirection = {0.25, 0.2, 0.5};
                startGlobalLightColor = {2.48,2.48,2.476};
                endGlobalLightColor = {4.85,1.78,0.376};
                startGlobalLightAmbient = {0.266,0.266,0.262};
                endGlobalLightAmbient = {0.191, 0.086, 0.071};
                startGlobalLightDiffuse = {0.80, 0.80, 0.796};
                endGlobalLightDiffuse = {0.987, 0.631, 0.555};
                startGlobalLightSpecular = {0.962,0.951,0.940};
                endGlobalLightSpecular = {0.731,0.272,0.151};

                startSkyboxAmbient = {0.42, 0.427, 0.889};
                endSkyboxAmbient = {0.68, 0.605, 0.547};
                startSkyboxDiffuse = {0.25, 0.245, 0.477};
                endSkyboxDiffuse = {0.33, 0.301, 0.265};
                startSkyboxSpecular = {0.325, 0.324, 0.361};
                endSkyboxSpecular = {0.385, 0.352, 0.326};
                startSkyboxShininess = 0.824f;
                endSkyboxShininess = 1.93f;
            }
            else if(dayTime == AFTERNOON) {
                start_near_plane = 615.0f;
                end_near_plane = 960.0f;
                start_far_plane = 3000.0f;
                end_far_plane = 3000.0f;
                startGlobalLightPosition = {-600.f, 357, -0.f};
                endGlobalLightPosition = {-500.f, 900, -0.f};
                startGlobalLightDirection = {0.25, 0.2, 0.5};
                endGlobalLightDirection = {0.25, 0.2, 0.5};
                startGlobalLightColor = {4.85,1.78,0.376};
                endGlobalLightColor = {0.42,0.45,0.86};
                startGlobalLightAmbient = {0.191, 0.086, 0.071};
                endGlobalLightAmbient = {0.041,0.041,0.049};
                startGlobalLightDiffuse = {0.987, 0.631, 0.555};
                endGlobalLightDiffuse = {0.36f, 0.36, 0.5348};
                startGlobalLightSpecular = {0.731,0.272,0.151};
                endGlobalLightSpecular = {0.856,0.856,0.961};

                startSkyboxAmbient = {0.68, 0.605, 0.547};
                endSkyboxAmbient = {0.44, 0.44, 0.88};
                startSkyboxDiffuse = {0.33, 0.301, 0.265};
                endSkyboxDiffuse = {0.22, 0.22, 0.24};
                startSkyboxSpecular = {0.385, 0.352, 0.326};
                endSkyboxSpecular = {0.356, 0.356, 0.485};
                startSkyboxShininess = 1.93f;
                endSkyboxShininess = 9.21f;
            }
            else if(dayTime == EVENING) {
                start_near_plane = 960.0f;
                end_near_plane = 615.0f;
                start_far_plane = 3000.0f;
                end_far_plane = 3000.0f;
                startGlobalLightPosition = {-500.f, 900, -0.f};
                endGlobalLightPosition = {-600.f, 357, -0.f};
                startGlobalLightDirection = {0.25, 0.2, 0.5};
                endGlobalLightDirection = {0.25, 0.2, 0.5};
                startGlobalLightColor = {0.42,0.45,0.86};
                endGlobalLightColor = {2.48,1.58,1.476};
                startGlobalLightAmbient = {0.041,0.041,0.049};
                endGlobalLightAmbient = {0.130,0.113,0.101};
                startGlobalLightDiffuse = {0.36f, 0.36, 0.5348};
                endGlobalLightDiffuse = {0.945, 0.926, 0.841};
                startGlobalLightSpecular = {0.856,0.856,0.961};
                endGlobalLightSpecular = {0.653,0.635,0.607};

                startSkyboxAmbient = {0.44, 0.44, 0.88};
                endSkyboxAmbient = {0.92, 0.88, 0.87};
                startSkyboxDiffuse = {0.22, 0.22, 0.24};
                endSkyboxDiffuse = {0.83, 0.82, 0.79};
                startSkyboxSpecular = {0.356, 0.356, 0.485};
                endSkyboxSpecular = {0.421, 0.405, 0.379};
                startSkyboxShininess = 9.21f;
                endSkyboxShininess = 0.07f;
            }
            else if(dayTime == NIGHT)  {
                start_near_plane = 615.0f;
                end_near_plane = 960.0f;
                start_far_plane = 3000.0f;
                end_far_plane = 3000.0f;
                startGlobalLightPosition = {-600.f, 357, -0.f};
                endGlobalLightPosition = {-500.f, 900, -0.f};
                startGlobalLightDirection = {0.25, 0.2, 0.5};
                endGlobalLightDirection = {0.25, 0.2, 0.5};
                startGlobalLightColor = {2.48,1.58,1.476};
                endGlobalLightColor = {2.48,2.48,2.476};
                startGlobalLightAmbient = {0.130,0.113,0.101};
                endGlobalLightAmbient = {0.266,0.266,0.262};
                startGlobalLightDiffuse = {0.945, 0.926, 0.841};
                endGlobalLightDiffuse = {0.80, 0.80, 0.796};
                startGlobalLightSpecular = {0.653,0.635,0.607};
                endGlobalLightSpecular = {0.962,0.951,0.940};

                startSkyboxAmbient = {0.92, 0.88, 0.87};
                endSkyboxAmbient = {0.42, 0.427, 0.889};
                startSkyboxDiffuse = {0.83, 0.82, 0.79};
                endSkyboxDiffuse = {0.25, 0.245, 0.477};
                startSkyboxSpecular = {0.421, 0.405, 0.379};
                endSkyboxSpecular = {0.325, 0.324, 0.361};
                startSkyboxShininess = 0.07f;
                endSkyboxShininess = 0.824f;
            }
            delta = 0;
        }
    }
}

void SceneWindow::render() {
    // Simply render all objects
    for (auto &object: Renderable_objects) {
        object->render(*this);
    }
}

void SceneWindow::render_shadow() {

    for (auto &object: Renderable_objects) {
        object->render_shadow(*this,lightSpaceMatrix);
    }
}

float SceneWindow::get_Y(float x, float z, unsigned char * heightMap){
    int i = (x+100)*19.2/2-1;
    int j = (z+100)*19.2/2-1;

    if(i < 0) i=0;
    if(j < 0) j=0;
    if(i >= 1920) i=0;
    if(j >= 1920) j=0;

    float y = heightMap[3 * (i * 1920 + j)];
    if (y>180) y *= 0.95;

    return y * 0.0809787;
}

void SceneWindow::addLight(glm::vec3 position, glm::vec3 color, glm::vec3 direction, float radius, float intensity,
                           glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, float cutOff,
                           float outerCutOff, bool isSpot, int index) {

    lights.position[index] = position;
    lights.direction[index] = direction;

    lights.color[index] = color;
    lights.radius[index] = radius;
    lights.intensity[index] = intensity;

    lights.ambient[index] = ambient;
    lights.diffuse[index] = diffuse;
    lights.specular[index] = specular;

    lights.cutOff[index] = cutOff;
    lights.outerCutOff[index] = outerCutOff;
    lights.isSpot[index] = isSpot;
}
