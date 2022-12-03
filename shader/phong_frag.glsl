// https://learnopengl.com/Advanced-Lighting/Advanced-Lighting
#version 330 core
layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

struct Lights {
    vec3 position[100];
    vec3 direction[100];
    float cutOff[100];
    float outerCutOff[100];

    bool isSpot[100];

    vec3 color[100];
    float intensity[100];
    float radius[100];

    vec3 ambient[100];
    vec3 diffuse[100];
    vec3 specular[100];
};

in vec3 FragPos;
in vec4 FragPosLightSpace;
in vec3 Normal;
in vec2 TexCoords;

uniform sampler2D shadowMap;
uniform sampler2D Texture;
uniform vec2 textureOffset;
uniform vec3 viewPos;
uniform Material material;
uniform Lights lights;
uniform float Transparency;
uniform int numberOfLights;
uniform bool globalLight;
uniform vec3 globalLightColor;
uniform vec3 globalLightDirection;
uniform vec3 globalLightPosition;
uniform vec3 globalLightAmbient;
uniform vec3 globalLightDiffuse;
uniform vec3 globalLightSpecular;

float ShadowCalculation(vec4 fragPosLightSpace)
{
    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(shadowMap, projCoords.xy).r;
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    // calculate bias (based on depth map resolution and slope)
    vec3 normal = normalize(Normal);
    vec3 lightDir = normalize(globalLightPosition - FragPos);
    float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);
    // check whether current frag pos is in shadow
    // float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;
    // PCF
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
            shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;
        }
    }
    shadow /= 9.0;

    // keep the shadow at 0.0 when outside the far_plane region of the light's frustum.
    if(projCoords.z > 1.0)
    shadow = 0.0;

    return shadow;
}

void main()
{

    vec3 normal = normalize(Normal);
    vec3 viewDirection = normalize(viewPos - FragPos);
    vec4 result = vec4(0, 0, 0, 0);
    vec3 lightDirection = vec3(0, 0, 0);

    float decayConstant = 1.0;
    float decayLinear = 0.08;
    float decayQuadratic = 0.027;

    const float offset = 1.0 / 300.0;



    float shadow = ShadowCalculation(FragPosLightSpace);

    if(globalLight) {
        lightDirection = normalize(globalLightDirection);
        float diff = max(dot(normal, lightDirection), 0.0);

        vec3 reflectDirection = reflect(-lightDirection, normal);
        float spec = pow(max(dot(viewDirection, reflectDirection), 0.0), material.shininess);

        vec3 ambient = globalLightAmbient * material.ambient * globalLightColor;
        vec3 diffuse = globalLightDiffuse * diff * material.diffuse * globalLightColor;
        vec3 specular = globalLightSpecular * spec * material.specular * globalLightColor;

        result = result + vec4(ambient + (diffuse + specular) * (1.0f-shadow), 1.0);
    }
    else {
        vec3 ambient = globalLightAmbient * material.ambient * globalLightColor;
        result = result + vec4(ambient, 1.0);
    }


    for (int i = 0; i < 10; i++) {
        float distance = length(lights.position[i] - FragPos);
        //if(lights.radius[i] > distance) {
            lightDirection = normalize(lights.position[i] - FragPos);
            float attenuation = 1.0 / (decayConstant + decayLinear * distance + decayQuadratic * (distance * distance));

            // diffuse
            float diff = max(dot(normal, lightDirection), 0.0);
            // specular
            vec3 reflectDirection = reflect(-lightDirection, normal);
            float spec = pow(max(dot(viewDirection, reflectDirection), 0.0), material.shininess);
            // combine
            vec3 diffuse = diff * lights.color[i] * lights.diffuse[i] * material.diffuse;
            vec3 specular = spec * lights.color[i] * lights.specular[i] * material.specular;

            if(lights.isSpot[i]) {
                float theta = dot(lightDirection, normalize(-lights.direction[i]));
                float epsilon = lights.cutOff[i] - lights.outerCutOff[i];
                float spotlightIntensity = clamp((theta - lights.outerCutOff[i]) / epsilon, 0.0, 1.0);

                result += (vec4(diffuse + specular, 1)) * attenuation * lights.intensity[i] * spotlightIntensity;
            }
            else {
                result += (vec4(diffuse + specular, 1)) * attenuation * lights.intensity[i];
            }
       // }
    }

    vec4 out_color = texture(Texture, vec2(TexCoords.x, 1.0 - TexCoords.y) + textureOffset) * result;

    FragColor = out_color;
    FragColor.a = Transparency;

    float brightness = dot(FragColor.rgb, vec3(0.2126, 0.7152, 0.0722));
    if(brightness > 1.0)
    BrightColor = vec4(FragColor.rgb, 1.0);
    else
    BrightColor = vec4(0.0, 0.0, 0.0, 1.0);
}