#version 330 core
layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;
// A texture is expected as program attribute
uniform sampler2D Texture;

// Direction of light
uniform vec3 LightDirection;

// (optional) Transparency
uniform float Transparency;

// (optional) Texture offset
uniform vec2 TextureOffset;

// The vertex shader will feed this input
in vec2 texCoord;

// Wordspace normal passed from vertex shader
in vec4 normal;


void main() {
  // Compute diffuse lighting
  float diffuse = max(dot(normal, vec4(normalize(LightDirection), 1.0f)), 0.2f);

  // Lookup the color in Texture on coordinates given by texCoord
  // NOTE: Texture coordinate is inverted vertically for compatibility with OBJ
  FragColor = texture(Texture, vec2(texCoord.x, 1.0 - texCoord.y) + TextureOffset) * diffuse;
  FragColor.a = Transparency;

  float brightness = dot(FragColor.rgb, vec3(0.2126, 0.7152, 0.0722));
  if(brightness > 1.0)
  BrightColor = vec4(FragColor.rgb, 1.0);
  else
  BrightColor = vec4(0.0, 0.0, 0.0, 1.0);
}
