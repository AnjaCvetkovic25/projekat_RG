#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <learnopengl/filesystem.h>
#include <learnopengl/shader.h>
#include <learnopengl/camera.h>
#include <learnopengl/model.h>

#include<rg/scena.h>

#include <iostream>

void framebuffer_size_callback(GLFWwindow *window, int width, int height);

void mouse_callback(GLFWwindow *window, double xpos, double ypos);

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);

void processInput(GLFWwindow *window);

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera

float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

struct PointLight {
    glm::vec3 position;
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;

    float constant;
    float linear;
    float quadratic;
};

struct DirLight {
    glm::vec3 direction;
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
};

struct SpotLight{
    glm::vec3 position;
    glm::vec3 direction;
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;

    float constant;
    float linear;
    float quadratic;

    float cutOff;
    float outerCutOff; //u kosinusima!!!

};
struct ProgramState {
    glm::vec3 clearColor = glm::vec3(0);
    bool ImGuiEnabled = false;
    Camera camera;
    bool CameraMouseMovementUpdateEnabled = true;
    glm::vec3 backpackPosition = glm::vec3(0.0f);
    float backpackScale = 1.0f;
    bool lightOff=false;
    PointLight pointLight;
    ProgramState()
            : camera(glm::vec3(0.0f, 0.0f, 3.0f)) {}

    void SaveToFile(std::string filename);

    void LoadFromFile(std::string filename);
};

void ProgramState::SaveToFile(std::string filename) {
    std::ofstream out(filename);
    out << clearColor.r << '\n'
        << clearColor.g << '\n'
        << clearColor.b << '\n'
        << ImGuiEnabled << '\n'
        << camera.Position.x << '\n'
        << camera.Position.y << '\n'
        << camera.Position.z << '\n'
        << camera.Front.x << '\n'
        << camera.Front.y << '\n'
        << camera.Front.z << '\n';
}

void ProgramState::LoadFromFile(std::string filename) {
    std::ifstream in(filename);
    if (in) {
        in >> clearColor.r
           >> clearColor.g
           >> clearColor.b
           >> ImGuiEnabled
           >> camera.Position.x
           >> camera.Position.y
           >> camera.Position.z
           >> camera.Front.x
           >> camera.Front.y
           >> camera.Front.z;
    }
}

ProgramState *programState;

void DrawImGui(ProgramState *programState);
unsigned int loadSkybox(vector<std::string> faces);
unsigned int loadTexture(char const* path);

int main() {
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetKeyCallback(window, key_callback);
    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // tell stb_image.h to flip loaded texture's on the y-axis (before loading model).
    stbi_set_flip_vertically_on_load(true);

    programState = new ProgramState;
    programState->LoadFromFile("resources/program_state.txt");
    if (programState->ImGuiEnabled) {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }

    // Init Imgui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void) io;



    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330 core");

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    //face culling
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    // build and compile shaders
    // -------------------------
    //Shader baseShader("resources/shaders/2.model_lighting.vs", "resources/shaders/2.model_lighting.fs");
    Shader ourShader("resources/shaders/terenSh.vs", "resources/shaders/terenSh.fs"); //shader za teren, nesretno ime
    Shader floorShader("resources/shaders/terenSh.vs", "resources/shaders/terenSh.fs");
    Shader cottageShader("resources/shaders/cottageShader.vs","resources/shaders/cottageShader.fs");
    Shader skyboxShader("resources/shaders/skyboxShader.vs","resources/shaders/skyboxShader.fs");
    Shader grassInstancedShader("resources/shaders/grassInstanced.vs","resources/shaders/grassInstanced.fs");
    Shader treeShader("resources/shaders/treeShader.vs","resources/shaders/treeShader.fs");


//    // load models
//    // -----------
//    Model ourModel("resources/objects/backpack/backpack.obj");
//    ourModel.SetShaderTextureNamePrefix("material.");
//
//    PointLight& pointLight = programState->pointLight;
//    pointLight.position = glm::vec3(4.0f, 4.0, 0.0);
//    pointLight.ambient = glm::vec3(0.1, 0.1, 0.1);
//    pointLight.diffuse = glm::vec3(0.6, 0.6, 0.6);
//    pointLight.specular = glm::vec3(1.0, 1.0, 1.0);
//
//    pointLight.constant = 1.0f;
//    pointLight.linear = 0.09f;
//    pointLight.quadratic = 0.032f;

    DirLight dirLight;
    dirLight.direction=glm::vec3(0.59f,-1.22f,0.0f);
    dirLight.ambient=glm::vec3(0.3,0.3,0.3);
    dirLight.diffuse=glm::vec3(0.5,0.5,0.5);
    dirLight.specular=glm::vec3(0.5,0.5,0.5);

    PointLight pointLight;
    pointLight.position = glm::vec3(-22.0f,21.0f,26.0f);
    pointLight.ambient = glm::vec3(4, 4, 4);
    pointLight.diffuse = glm::vec3(10, 10, 10);
    pointLight.specular = glm::vec3(1.0, 1.0, 1.0);

    pointLight.constant = 1.0f;
    pointLight.linear = 0.7f;
    pointLight.quadratic = 0.032f;

    SpotLight spotLight;
    spotLight.position = glm::vec3(-22.0f,21.0f,26.0f);
    spotLight.direction=glm::vec3(0.0,-1.0,0.0);
    spotLight.ambient = glm::vec3(9.7, 7.2, 1.0);
    spotLight.diffuse = glm::vec3(22.2, 16.1, 5.0);
    spotLight.specular = glm::vec3(1.0, 1.0, 1.0);

    spotLight.constant =1.0f;
    spotLight.linear = 1.0f;
    spotLight.quadratic = 0.0f;

    spotLight.cutOff=glm::cos(glm::radians(10.0f));
    spotLight.outerCutOff=glm::cos(glm::radians(15.0f));



    //load cottage
    stbi_set_flip_vertically_on_load(false);
    Model cottage("resources/objects/cottage/cottage_obj.obj");//nadjen model kolibe koji radi
    cottage.SetShaderTextureNamePrefix("material.");
    stbi_set_flip_vertically_on_load(true);

    //load lamp model
    stbi_set_flip_vertically_on_load(false);
    Model lamp("resources/objects/lamp/StreetLamp.obj");
    lamp.SetShaderTextureNamePrefix("material.");
    stbi_set_flip_vertically_on_load(true);

    //load tree model
    Model tree("resources/objects/tree/SpookyTree_00.obj");
    tree.SetShaderTextureNamePrefix("material.");
    //generating random model matrices for trees
    unsigned int numTree=50;
    glm::mat4* modelTree;
    modelTree=new glm::mat4[numTree];
    srand(12);
    for(unsigned int i=0;i<numTree;i++)
    {
        glm::mat4 model=glm::mat4(1.0f);
        //translation
        float x=(rand()%200)+sin(rand())-100.0f;
        float y=0.0f;
        float z =(rand()%200)+cos(rand())-100.0f;
        if( (x>=-30.0f && x<=30.0f) && (z>=-30 && z<=30))
        {
            x+=(30.0f+rand()%70)*1.0f;
            z+=(13.0f+rand()%70)*1.0f;
        }
        //if x i z unutar kuce
        model=glm::translate(model, glm::vec3(x,y,z));
        model=glm::scale(model,glm::vec3(3.0));
        //3.rotaion ,
        model=glm::rotate(model,(rand()%360)*1.0f,glm::vec3(0.0f,1.0f,0.0f));

        modelTree[i]=model;

    }
    //config instanced array for tree model
    unsigned int treeVBO;
    glGenBuffers(1,&treeVBO);
    glBindBuffer(GL_ARRAY_BUFFER,treeVBO);
    glBufferData(GL_ARRAY_BUFFER,numTree*sizeof(glm::mat4),&modelTree[0],GL_STATIC_DRAW);
    //set vertex attribs
    for(unsigned int i=0;i<tree.meshes.size(); i++)
    {
        unsigned int VAO=tree.meshes[i].VAO;
        glBindVertexArray(VAO);
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3,4,GL_FLOAT,GL_FALSE,sizeof(glm::mat4),(void*)0);
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4,4,GL_FLOAT,GL_FALSE,sizeof(glm::mat4),(void*)(sizeof(glm::vec4)));
        glEnableVertexAttribArray(5);
        glVertexAttribPointer(5,4,GL_FLOAT,GL_FALSE,sizeof(glm::mat4),(void*)(2*sizeof(glm::vec4)));
        glEnableVertexAttribArray(6);
        glVertexAttribPointer(6,4,GL_FLOAT,GL_FALSE,sizeof(glm::mat4),(void*)(3*sizeof(glm::vec4)));

        glVertexAttribDivisor(3,1);
        glVertexAttribDivisor(4,1);
        glVertexAttribDivisor(5,1);
        glVertexAttribDivisor(6,1);

        glBindVertexArray(0);
    }


    //skybox

    float skyboxVertices[]={
            -1.0f,  1.0f, -1.0f,
            -1.0f, -1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,
            1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,

            -1.0f, -1.0f,  1.0f,
            -1.0f, -1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f,  1.0f,
            -1.0f, -1.0f,  1.0f,

            1.0f, -1.0f, -1.0f,
            1.0f, -1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f,  1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,

            -1.0f, -1.0f,  1.0f,
            -1.0f,  1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f, -1.0f,  1.0f,
            -1.0f, -1.0f,  1.0f,

            -1.0f,  1.0f, -1.0f,
            1.0f,  1.0f, -1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            -1.0f,  1.0f,  1.0f,
            -1.0f,  1.0f, -1.0f,

            -1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f,  1.0f,
            1.0f, -1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f,  1.0f,
            1.0f, -1.0f,  1.0f


    };

    //skyboxVAO
    unsigned int skyboxVAO,skyboxVBO;
    glGenVertexArrays(1,&skyboxVAO);
    glGenBuffers(1,&skyboxVBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER,skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER,sizeof(skyboxVertices),&skyboxVertices,GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,3*sizeof(float),(void*)(0));

    //load skybox texture

    vector<std::string> faces{
        FileSystem::getPath("resources/textures/skybox/right.jpg"),
        FileSystem::getPath("resources/textures/skybox/left.jpg"),
        FileSystem::getPath("resources/textures/skybox/top.jpg"),
        FileSystem::getPath("resources/textures/skybox/bottom.jpg"),
        FileSystem::getPath("resources/textures/skybox/front.jpg"),
        FileSystem::getPath("resources/textures/skybox/back.jpg")

    };
    stbi_set_flip_vertically_on_load(false);
    unsigned int skyboxTexture=loadSkybox(faces);
    stbi_set_flip_vertically_on_load(true);


    //load terrain

    unsigned int terrainVAO=loadTerrain();

    unsigned int floorVAO=loadFloor();

    float transparentVertices[]={
            //coords                        //normals                    //tex coords
            0.0f, 0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,1.0f,
            0.0f, -0.5f, 0.0f, 0.0f,0.0f,0.0f,0.0f,0.0f,
            1.0f,-0.5f,0.0f,0.0f,0.0f,0.0f,1.0f,0.0f,

            0.0f,0.5f,0.0f,0.0f,0.0f,0.0f,0.0f,1.0f,
            1.0f,-0.5f,0.0f,0.0f,0.0f,0.0f,1.0f,0.0f,
            1.0f,0.5f,0.0f,0.0f,0.0f,0.0f,1.0f,1.0f
    };

    unsigned int transparentVAO, transparentVBO;
    glGenVertexArrays(1,&transparentVAO);
    glGenBuffers(1,&transparentVBO);
    glBindVertexArray(transparentVAO);
    glBindBuffer(GL_ARRAY_BUFFER,transparentVBO);
    glBufferData(GL_ARRAY_BUFFER,sizeof(transparentVertices),transparentVertices,GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,8*sizeof(float),(void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,8*sizeof(float),(void*)(3*sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2,2,GL_FLOAT,GL_FALSE,8*sizeof(float),(void*)(6*sizeof(float)));





    //load texture

    unsigned int terrainTexture= loadTexture(FileSystem::getPath("resources/textures/dirt/Dirt_01.png").c_str()); //radi seamless textura
    unsigned int floorTexture=loadTexture(FileSystem::getPath("resources/textures/FloorTile_S.jpg").c_str());
    unsigned int grassTexture= loadTexture(FileSystem::getPath("resources/textures/grass_transparent.png").c_str());
    //generating random model matrices
    unsigned int numGrass=100000;
    glm::mat4* modelMatrices;
    modelMatrices=new glm::mat4[numGrass];
    srand(5);
    //float offset=100.0f;
    for(unsigned int i=0;i<numGrass;i++)
    {
        glm::mat4 model=glm::mat4(1.0f);
        //1. translation
        float x=(rand()%200)+sin(rand())-100.0f;
        float y=0.0f;
        float z =(rand()%200)+cos(rand())-100.0f;
        if( (x>=-10.0f && x<=30.0f) && (z>=-7 && z<=13))
        {
            x=(29.0f+rand()%70)*1.0f;
            z=(13.0f+rand()%70)*1.0f; //iscratava travke van terena
        }
        //if x i z unutar kuce
        model=glm::translate(model, glm::vec3(x,y,z));
        //2.rotation , no scaling
        model=glm::rotate(model,(rand()%360)*1.0f,glm::vec3(0.0f,1.0f,0.0f));

        modelMatrices[i]=model;

    }
    //instanced buffer config
    unsigned int instancedGrassVBO;
    glGenBuffers(1,&instancedGrassVBO);
    glBindBuffer(GL_ARRAY_BUFFER,instancedGrassVBO);
    glBufferData(GL_ARRAY_BUFFER,numGrass*sizeof(glm::mat4),&modelMatrices[0],GL_STATIC_DRAW);

    glBindVertexArray(transparentVAO);
    //mat 4x4
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3,4,GL_FLOAT,GL_FALSE,sizeof(glm::mat4),(void*)0);

    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4,4,GL_FLOAT,GL_FALSE,sizeof(glm::mat4),(void*)(sizeof(glm::vec4)));

    glEnableVertexAttribArray(5);
    glVertexAttribPointer(5,4,GL_FLOAT,GL_FALSE,sizeof(glm::mat4),(void*)(2*sizeof(glm::vec4)));

    glEnableVertexAttribArray(6);
    glVertexAttribPointer(6,4,GL_FLOAT,GL_FALSE,sizeof(glm::mat4),(void*)(3*sizeof(glm::vec4)));

    glVertexAttribDivisor(3,1);
    glVertexAttribDivisor(4,1);
    glVertexAttribDivisor(5,1);
    glVertexAttribDivisor(6,1);

    glBindVertexArray(0);


    //shader config
    ourShader.use();
    ourShader.setInt("texture1",0);

    floorShader.use();
    floorShader.setInt("texture1",0);

    skyboxShader.use();
    skyboxShader.setInt("skybox",0);

    // draw in wireframe
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window)) {
        // per-frame time logic
        // --------------------
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        processInput(window);


        // render
        // ------
        glClearColor(programState->clearColor.r, programState->clearColor.g, programState->clearColor.b, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        if(!(programState->lightOff))
        {




            pointLight.position = glm::vec3(-22.0f,21.0f,26.0f);
            pointLight.ambient = glm::vec3(4, 4, 4);
            pointLight.diffuse = glm::vec3(10, 10, 10);
            pointLight.specular = glm::vec3(1.0, 1.0, 1.0);

            pointLight.constant = 1.0f;
            pointLight.linear = 0.7f;
            pointLight.quadratic = 0.032f;


            spotLight.position = glm::vec3(-22.0f,21.0f,26.0f);
            spotLight.direction=glm::vec3(0.0,-1.0,0.0);
            spotLight.ambient = glm::vec3(9.7, 7.2, 1.0);
            spotLight.diffuse = glm::vec3(22.2, 16.1, 5.0);
            spotLight.specular = glm::vec3(1.0, 1.0, 1.0);

            spotLight.constant =1.0f;
            spotLight.linear = 1.0f;
            spotLight.quadratic = 0.0f;

            spotLight.cutOff=glm::cos(glm::radians(10.0f));
            spotLight.outerCutOff=glm::cos(glm::radians(15.0f));
        }
        else
        {



            pointLight.position = glm::vec3(-22.0f,21.0f,26.0f);
            pointLight.ambient = glm::vec3(0, 0, 0);
            pointLight.diffuse = glm::vec3(0, 0, 0);
            pointLight.specular = glm::vec3(0.0);

            pointLight.constant = 1.0f;
            pointLight.linear = 0.7f;
            pointLight.quadratic = 0.032f;


            spotLight.position = glm::vec3(-22.0f,21.0f,26.0f);
            spotLight.direction=glm::vec3(0.0);
            spotLight.ambient = glm::vec3(0.0);
            spotLight.diffuse = glm::vec3(0.0);
            spotLight.specular = glm::vec3(0.0);

            spotLight.constant =1.0f;
            spotLight.linear = 1.0f;
            spotLight.quadratic = 0.0f;

            spotLight.cutOff=glm::cos(glm::radians(10.0f));
            spotLight.outerCutOff=glm::cos(glm::radians(15.0f));
        }

            glDisable(GL_CULL_FACE);

            //render terrain
            ourShader.use();
            glm::mat4 projection=glm::perspective(glm::radians(programState->camera.Zoom),(float)SCR_WIDTH/(float)SCR_HEIGHT,0.1f,100.0f);
            glm::mat4 view=programState->camera.GetViewMatrix();
            ourShader.setMat4("projection",projection);
            ourShader.setMat4("view",view);
            ourShader.setVec3("pointLight.position",pointLight.position);
            ourShader.setVec3("pointLight.ambient",pointLight.ambient);
            ourShader.setVec3("pointLight.diffuse",pointLight.diffuse);
            ourShader.setVec3("pointLight.specular",pointLight.specular);
            ourShader.setFloat("pointLight.constant",pointLight.constant);
            ourShader.setFloat("pointLight.linear",pointLight.linear);
            ourShader.setFloat("pointLight.quadratic",pointLight.quadratic);
            ourShader.setVec3("dirLight.direction",dirLight.direction);
            ourShader.setVec3("dirLight.ambient",dirLight.ambient);
            ourShader.setVec3("dirLight.diffuse",dirLight.diffuse);
            ourShader.setVec3("dirLight.specular",dirLight.specular);
            ourShader.setVec3("spotLight.position",spotLight.position);
            ourShader.setVec3("spotLight.direction",spotLight.direction);
            ourShader.setVec3("spotLight.ambient",spotLight.ambient);
            ourShader.setVec3("spotLight.diffuse",spotLight.diffuse);
            ourShader.setVec3("spotLight.specular",spotLight.specular);
            ourShader.setFloat("spotLight.constant",spotLight.constant);
            ourShader.setFloat("spotLight.linear",spotLight.linear);
            ourShader.setFloat("spotLight.quadratic",spotLight.quadratic);
            ourShader.setFloat("spotLight.cutOff",spotLight.cutOff);
            ourShader.setFloat("spotLight.position",spotLight.outerCutOff);

            ourShader.setVec3("viewPos", programState->camera.Position);

            glm::mat4 model=glm::mat4(1.0f);
            ourShader.setMat4("model",model);

            glBindVertexArray(terrainVAO);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D,terrainTexture);
            glDrawArrays(GL_TRIANGLES,0,6);
            glBindVertexArray(0);


            glEnable(GL_CULL_FACE);
            //floor
        floorShader.use();

        floorShader.setMat4("projection",projection);
        floorShader.setMat4("view",view);
        floorShader.setVec3("pointLight.position",pointLight.position);
        floorShader.setVec3("pointLight.ambient",pointLight.ambient);
        floorShader.setVec3("pointLight.diffuse",pointLight.diffuse);
        floorShader.setVec3("pointLight.specular",pointLight.specular);
        floorShader.setFloat("pointLight.constant",pointLight.constant);
        floorShader.setFloat("pointLight.linear",pointLight.linear);
        floorShader.setFloat("pointLight.quadratic",pointLight.quadratic);
        floorShader.setVec3("dirLight.direction",dirLight.direction);
        floorShader.setVec3("dirLight.ambient",dirLight.ambient);
        floorShader.setVec3("dirLight.diffuse",dirLight.diffuse);
        floorShader.setVec3("dirLight.specular",dirLight.specular);
        floorShader.setVec3("viewPos", programState->camera.Position);

        model=glm::mat4(1.0f);
        model=glm::translate(model, glm::vec3(10.0f,5.2f,3.0f)); //transilramo tamo gde je koliba
        floorShader.setMat4("model",model);

        glBindVertexArray(floorVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D,floorTexture);
        glDrawArrays(GL_TRIANGLES,0,6);
        glBindVertexArray(0);


            //grass
            grassInstancedShader.use();
            grassInstancedShader.setInt("texture_diffuse1",0);
            grassInstancedShader.setMat4("projection",projection);
            grassInstancedShader.setMat4("view",view);

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D,grassTexture);
            glBindVertexArray(transparentVAO);
            glDrawArraysInstanced(GL_TRIANGLES,0,6,numGrass);
            glBindVertexArray(0);

            //cottage
            cottageShader.use();
            cottageShader.setMat4("projection",projection);
            cottageShader.setMat4("view",view);
            cottageShader.setVec3("pointLight.position",pointLight.position);
            cottageShader.setVec3("pointLight.ambient",pointLight.ambient);
            cottageShader.setVec3("pointLight.diffuse",pointLight.diffuse);
            cottageShader.setVec3("pointLight.specular",pointLight.specular);
            cottageShader.setFloat("pointLight.constant",pointLight.constant);
            cottageShader.setFloat("pointLight.linear",pointLight.linear);
            cottageShader.setFloat("pointLight.quadratic",pointLight.quadratic);
            cottageShader.setVec3("dirLight.direction",dirLight.direction);
            cottageShader.setVec3("dirLight.ambient",dirLight.ambient);
            cottageShader.setVec3("dirLight.diffuse",dirLight.diffuse);
            cottageShader.setVec3("dirLight.specular",dirLight.specular);
        cottageShader.setVec3("spotLight.position",spotLight.position);
        cottageShader.setVec3("spotLight.direction",spotLight.direction);
        cottageShader.setVec3("spotLight.ambient",spotLight.ambient);
        cottageShader.setVec3("spotLight.diffuse",spotLight.diffuse);
        cottageShader.setVec3("spotLight.specular",spotLight.specular);
        cottageShader.setFloat("spotLight.constant",spotLight.constant);
        cottageShader.setFloat("spotLight.linear",spotLight.linear);
        cottageShader.setFloat("spotLight.quadratic",spotLight.quadratic);
        cottageShader.setFloat("spotLight.cutOff",spotLight.cutOff);
        cottageShader.setFloat("spotLight.position",spotLight.outerCutOff);
        cottageShader.setFloat("material.shininess",5.0f);
            cottageShader.setVec3("viewPosition", programState->camera.Position);
            model=glm::mat4(1.0f);
            model=glm::translate(model, glm::vec3(10.0f,-0.5f,3.0f));
            model=glm::scale(model,glm::vec3(1.5f));
            cottageShader.setMat4("model",model);
            cottage.Draw(cottageShader);

            //lamp

        model=glm::mat4(1.0f);
        model=glm::translate(model, glm::vec3(-22.0f,3.0f,26.0f));
        model=glm::scale(model,glm::vec3(1.5f));
        cottageShader.setMat4("model",model);
        lamp.Draw(cottageShader);


            //tree

            treeShader.use();
            treeShader.setMat4("projection",projection);
            treeShader.setMat4("view",view);
            treeShader.setInt("material.texture_diffuse1",0);
        treeShader.setVec3("pointLight.position",pointLight.position);
        treeShader.setVec3("pointLight.ambient",pointLight.ambient);
        treeShader.setVec3("pointLight.diffuse",pointLight.diffuse);
        treeShader.setVec3("pointLight.specular",pointLight.specular);
        treeShader.setFloat("pointLight.constant",pointLight.constant);
        treeShader.setFloat("pointLight.linear",pointLight.linear);
        treeShader.setFloat("pointLight.quadratic",pointLight.quadratic);
        treeShader.setVec3("dirLight.direction",dirLight.direction);
        treeShader.setVec3("dirLight.ambient",dirLight.ambient);
        treeShader.setVec3("dirLight.diffuse",dirLight.diffuse);
        treeShader.setVec3("dirLight.specular",dirLight.specular);
        treeShader.setVec3("spotLight.position",spotLight.position);
        treeShader.setVec3("spotLight.direction",spotLight.direction);
        treeShader.setVec3("spotLight.ambient",spotLight.ambient);
        treeShader.setVec3("spotLight.diffuse",spotLight.diffuse);
        treeShader.setVec3("spotLight.specular",spotLight.specular);
        treeShader.setFloat("spotLight.constant",spotLight.constant);
        treeShader.setFloat("spotLight.linear",spotLight.linear);
        treeShader.setFloat("spotLight.quadratic",spotLight.quadratic);
        treeShader.setFloat("spotLight.cutOff",spotLight.cutOff);
        treeShader.setFloat("spotLight.position",spotLight.outerCutOff);
        treeShader.setFloat("material.shininess",5.0f);
        treeShader.setVec3("viewPosition", programState->camera.Position);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D,tree.textures_loaded[0].id);
            for(unsigned int i=0;i<tree.meshes.size();i++)
            {
                glBindVertexArray(tree.meshes[i].VAO);
                glDrawElementsInstanced(GL_TRIANGLES,tree.meshes[i].indices.size(),GL_UNSIGNED_INT,0,numTree);
                glBindVertexArray(0);
            }


            //draw skybox
            glDepthFunc(GL_LEQUAL);
            skyboxShader.use();
            view=glm::mat4(glm::mat3(programState->camera.GetViewMatrix())); //izbacujemo translaciju iz view mat
            skyboxShader.setMat4("view",view);
            skyboxShader.setMat4("projection",projection);

            glBindVertexArray(skyboxVAO);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_CUBE_MAP,skyboxTexture);
            glDrawArrays(GL_TRIANGLES,0,36);
            glBindVertexArray(0);

            glDepthFunc(GL_LESS);



            if (programState->ImGuiEnabled)
            DrawImGui(programState);





        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    programState->SaveToFile("resources/program_state.txt");
    delete programState;
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glDeleteVertexArrays(1,&terrainVAO);
    glDeleteVertexArrays(1,&skyboxVAO);

    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        programState->camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        programState->camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        programState->camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        programState->camera.ProcessKeyboard(RIGHT, deltaTime);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow *window, double xpos, double ypos) {
    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    if (programState->CameraMouseMovementUpdateEnabled)
        programState->camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
    programState->camera.ProcessMouseScroll(yoffset);
}

void DrawImGui(ProgramState *programState) {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();


    {
        static float f = 0.0f;
        ImGui::Begin("Hello stranger");
        ImGui::Checkbox("Turn off the light", &programState->lightOff);

        ImGui::End();
    }

    {
        ImGui::Begin("Camera info");
        const Camera& c = programState->camera;
        ImGui::Text("Camera position: (%f, %f, %f)", c.Position.x, c.Position.y, c.Position.z);
        ImGui::Text("(Yaw, Pitch): (%f, %f)", c.Yaw, c.Pitch);
        ImGui::Text("Camera front: (%f, %f, %f)", c.Front.x, c.Front.y, c.Front.z);
        ImGui::Checkbox("Camera mouse update", &programState->CameraMouseMovementUpdateEnabled);
        ImGui::End();
    }

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_F1 && action == GLFW_PRESS) {
        programState->ImGuiEnabled = !programState->ImGuiEnabled;
        if (programState->ImGuiEnabled) {
            programState->CameraMouseMovementUpdateEnabled = false;
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        } else {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        }
    }
}

unsigned int loadSkybox(vector<std::string> faces)
{
    unsigned int textureID;
    glGenTextures(1,&textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP,textureID);

    int width,height,nrChannels;
    for(unsigned int i =0; i<faces.size();i++)
    {
        unsigned char *data=stbi_load(faces[i].c_str(),&width,&height,&nrChannels,0);
        if(data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X+i,0,GL_RGB,width,height,0,GL_RGB,GL_UNSIGNED_BYTE,data);
            stbi_image_free(data);
        }
        else
        {
            std::cout<<"Skybox texture failed to load at path: "<<faces[i]<<std::endl;
            stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP,GL_TEXTURE_WRAP_R,GL_CLAMP_TO_EDGE);

    return textureID;
}
unsigned int loadTexture(char const* path)
{
    unsigned int textureID;
    glGenTextures(1,&textureID);

    int width,height, nrComponents;
    unsigned char *data=stbi_load(path,&width,&height,&nrComponents,0);
    if(data)
    {
        GLenum format;
        if(nrComponents==1)
            format=GL_RED;
        else if(nrComponents==3)
            format=GL_RGB;
        else if(nrComponents==4)
            format=GL_RGBA;

        glBindTexture(GL_TEXTURE_2D,textureID);
        glTexImage2D(GL_TEXTURE_2D,0,format, width, height,0,format,GL_UNSIGNED_BYTE,data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);

        stbi_image_free(data);

    }
    else
    {
        std::cout<<"TExture failed to load at path: "<<path<<std::endl;
        stbi_image_free(data);
    }

    return textureID;
}