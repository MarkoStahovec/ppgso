#include "glm/vec3.hpp"

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
// 0-255 ~ 0-21
float get_Y(float x, float z, unsigned char * heightMap){
    int i = (x+100)*19.2/2-1;
    int j = (z+100)*19.2/2-1;

    if(i < 0) i=0;
    if(j < 0) j=0;

    float y = heightMap[3 * (i * 1920 + j)];
    if (y>180) y *= 0.95;

    return y * 0.0809787;
}