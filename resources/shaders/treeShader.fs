#version 330 core
out vec4 FragColor;

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


in vec2 TexCoords;
in vec3 Normal;
in vec3 FragPos;

struct Material {
    sampler2D texture_diffuse1;
    sampler2D texture_specular1;

   float shininess;
};

struct SpotLight{
    vec3 position;
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;

    float cutOff;
    float outerCutOff;

};

uniform Material material;
uniform vec3 viewPos;
uniform DirLight dirLight;
uniform PointLight pointLight;
uniform SpotLight spotLight;

vec3 CalcDirLight(DirLight light, vec3 normal,vec3 viewDir);
vec3 CalcPointLight(PointLight light, vec3 normal,vec3 fragPos,vec3 viewDir);
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos,vec3 viewDir);

void main()
{


    vec3 norm=normalize(Normal);
    vec3 viewDir=normalize(viewPos-FragPos);
        //directional dirLight
        vec3 result=CalcDirLight(dirLight,norm,viewDir);

        //point light
        result+=CalcPointLight(pointLight,norm,FragPos,viewDir);

        //vec4 texColor=texture(texture1,TexCoords);
        result+=CalcSpotLight(spotLight,norm,FragPos,viewDir);

        FragColor=vec4(result,1.0);
}

vec3 CalcDirLight(DirLight light, vec3 normal,vec3 viewDir)
{
    vec3 lightDir=normalize(-light.direction);

    float diff = max(dot(normal,lightDir),0.0);
    //vec3 reflectDir=reflect(-light.direction,normal); //nemamo spek komponentu

    vec3 ambient=light.ambient*vec3(texture(material.texture_diffuse1,TexCoords)*vec4(0.51, 0.416, 0.282,0.0));
    vec3 diffuse=light.diffuse*diff*vec3(texture(material.texture_diffuse1,TexCoords)*vec4(0.51, 0.416, 0.282,0.0));
    return (ambient+diffuse);
}
vec3 CalcPointLight(PointLight light, vec3 normal,vec3 fragPos,vec3 viewDir)
{
    vec3 lightDir=normalize(light.position-fragPos);

    float diff=max(dot(normal, lightDir),0.0);
    //attenuation
    float distance=length(light.position-fragPos);
    float attenuation=1.0/(light.constant+light.linear*distance+light.quadratic*(distance*distance));

    vec3 ambient=light.ambient*vec3(texture(material.texture_diffuse1,TexCoords)*vec4(0.51, 0.416, 0.282,0.0));
    vec3 diffuse=light.diffuse*diff*vec3(texture(material.texture_diffuse1,TexCoords)*vec4(0.51, 0.416, 0.282,0.0));

    ambient*=attenuation;
    diffuse*=attenuation;

    return (ambient+diffuse);
}

vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos,vec3 viewDir)
{
    vec3 lightDir=normalize(light.position-fragPos);
    float diff=max(dot(normal,lightDir),0.0);
    vec3 reflectDir=reflect(-lightDir,normal);
    vec3 halfwayDir=normalize(lightDir+viewDir);
    float spec=pow(max(dot(normal,halfwayDir),0.0),material.shininess);


   float distance = length(light.position-fragPos);
   float attenuation=1.0/(light.constant+light.linear*distance+light.quadratic*distance*distance);

   //spotlight intensity
   float theta=dot(lightDir,normalize(-light.direction));
   float epsilon=light.cutOff-light.outerCutOff;
   float intensity=clamp((theta-light.outerCutOff)/epsilon,0.0,1.0);
   vec3 ambient=light.ambient*vec3(texture(material.texture_diffuse1,TexCoords));
   vec3 diffuse=light.diffuse*diff*vec3(texture(material.texture_diffuse1,TexCoords));
   vec3 specular=light.specular*spec*vec3(texture(material.texture_specular1,TexCoords));

       ambient*=attenuation*intensity;
       diffuse*=attenuation*intensity;
       specular*=attenuation*intensity;

       return (ambient+diffuse+specular);



}