#version 330 core
out vec4 FragColor;
in vec2 TexCoords;
in vec3 Normal;
in vec3 FragPos;

struct DirLight{
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight{
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;

};

struct Material {
    sampler2D texture_diffuse1;
    sampler2D texture_specular1;

    float shininess;
};


uniform vec3 viewPosition;
uniform DirLight dirLight;
uniform PointLight pointLight;
uniform Material material;

vec3 CalcDirLight(DirLight light, vec3 normal,vec3 viewDir);
vec3 CalcPointLight(PointLight light, vec3 normal,vec3 fragPos,vec3 viewDir);


void main()
{
    vec3 normal = normalize(Normal);
    vec3 viewDir = normalize(viewPosition - FragPos);
    vec3 result = CalcPointLight(pointLight, normal, FragPos, viewDir);
    result+=CalcDirLight(dirLight,normal,viewDir);

    FragColor = vec4(result, 1.0);
}

vec3 CalcDirLight(DirLight light, vec3 normal,vec3 viewDir)
{
    vec3 lightDir=normalize(-light.direction);

    float diff = max(dot(normal,lightDir),0.0);
    //vec3 reflectDir=reflect(-light.direction,normal);

    vec3 ambient=light.ambient*vec3(texture(material.texture_diffuse1,TexCoords));
    vec3 diffuse=light.diffuse*diff*vec3(texture(material.texture_diffuse1,TexCoords));
    return (ambient+diffuse);
}
vec3 CalcPointLight(PointLight light, vec3 normal,vec3 fragPos,vec3 viewDir)
{
    vec3 lightDir=normalize(light.position-fragPos);

    float diff=max(dot(normal, lightDir),0.0);
    //attenuation
    float distance=length(light.position-fragPos);
    float attenuation=1.0/(light.constant+light.linear*distance+light.quadratic*(distance*distance));

    vec3 ambient=light.ambient*vec3(texture(material.texture_diffuse1,TexCoords));
    vec3 diffuse=light.diffuse*diff*vec3(texture(material.texture_diffuse1,TexCoords));

    ambient*=attenuation;
    diffuse*=attenuation;

    return (ambient+diffuse);
}