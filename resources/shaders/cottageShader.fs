#version 330 core
out vec4 FragColor;
in vec2 TexCoords;
//in vec3 Normal;
in vec3 FragPos;

struct Material {
    sampler2D texture_diffuse1;
   // sampler2D texture_specular1;

   // float shininess;
};

uniform Material material;

void main()
{
    //vec3 normal = normalize(Normal);
    //vec3 viewDir = normalize(viewPosition - FragPos);
    //vec3 result = CalcPointLight(pointLight, normal, FragPos, viewDir);

    FragColor = vec4(vec3(texture(material.texture_diffuse1,TexCoords)), 1.0);
}