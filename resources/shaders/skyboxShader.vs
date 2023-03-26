#version 330 core
layout (location=0) in vec3 aPos;

out vec3 TexCoords;

uniform mat4 projection;
uniform mat4 view;
//ne definisemo model matricu jer smo sky box namestili tako da kada se prebacimo u clip space cela scena bude zarobljena u sky boxu
void main()
{
    TexCoords=aPos;
    vec4 pos=projection*view*vec4(aPos,1.0);
    gl_Position=pos.xyww; //zbog perspektivnog deljenja da se svi frag nalaze na far ravni
}