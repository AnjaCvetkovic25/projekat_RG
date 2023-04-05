//
// Created by matf-racunarska-grafika on 23.3.23..
//

#ifndef PROJECT_BASE_SCENA_H
#define PROJECT_BASE_SCENA_H

unsigned int loadTerrain()
{
    float vertices[]=
            {
            //coords                         //normals                  //tex coords
            100.0f,0.0f,-100.0f,0.0f,1.0f,0.0f,50.0f,50.0f,//top right
            100.0f,0.0f,100.0f,0.0f,1.0f,0.0f,50.0f,0.0f, //bottom right
            -100.0f,0.0f,-100.0f,0.0f,1.0f,0.0f,0.0f,50.0f, //top left

            100.0f,0.0f,100.0f,0.0f,1.0f,0.0f,50.0f,0.0f, //bottom right
            -100.0f,0.0f,100.0f,0.0f,1.0f,0.0f,0.0f,50.0f,
            -100.0f,0.0f,-100.0f,0.0f,1.0f,0.0f,0.0f,0.0f //bottom left

            };
    float indices[]={
            0,1,3,
            1,2,3
    };
    unsigned int terrainVAO,terrainVBO;
    glGenVertexArrays(1,&terrainVAO);
    glGenBuffers(1,&terrainVBO);
    //glGenBuffers(1,&terrainEBO);

    glBindVertexArray(terrainVAO);


    glBindBuffer(GL_ARRAY_BUFFER,terrainVBO);
    glBufferData(GL_ARRAY_BUFFER,sizeof(vertices),vertices,GL_STATIC_DRAW);

    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,terrainEBO);
    //glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(indices),indices,GL_STATIC_DRAW);

    //coord attrib
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,8*sizeof(float),(void*)0);
    glEnableVertexAttribArray(0);

    //normals attrib
    glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,8*sizeof(float),(void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);

    //tex coord attrib
    glVertexAttribPointer(2,2,GL_FLOAT,GL_FALSE,8*sizeof(float),(void*)(6*sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);

    return terrainVAO;

}


unsigned int loadFloor()
{
    float verticesFloor[]=
            {
                    //coords                         //normals                  //tex coords
                    20.0f,0.0f,-10.0f,0.0f,1.0f,0.0f,10.0f,5.0f,//top right
                    20.0f,0.0f,10.0f,0.0f,1.0f,0.0f,10.0f,0.0f, //bottom right
                    -20.0f,0.0f,-10.0f,0.0f,1.0f,0.0f,0.0f,5.0f, //top left

                    20.0f,0.0f,10.0f,0.0f,1.0f,0.0f,10.0f,0.0f, //bottom right
                    -20.0f,0.0f,10.0f,0.0f,1.0f,0.0f,0.0f,5.0f,
                    -20.0f,0.0f,-10.0f,0.0f,1.0f,0.0f,0.0f,0.0f //bottom left

            };
    float indices[]={
            0,1,3,
            1,2,3
    };
    unsigned int floorVAO,floorVBO;
    glGenVertexArrays(1,&floorVAO);
    glGenBuffers(1,&floorVBO);
    //glGenBuffers(1,&terrainEBO);

    glBindVertexArray(floorVBO);


    glBindBuffer(GL_ARRAY_BUFFER,floorVBO);
    glBufferData(GL_ARRAY_BUFFER,sizeof(verticesFloor),verticesFloor,GL_STATIC_DRAW);

    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,terrainEBO);
    //glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(indices),indices,GL_STATIC_DRAW);

    //coord attrib
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,8*sizeof(float),(void*)0);
    glEnableVertexAttribArray(0);

    //normals attrib
    glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,8*sizeof(float),(void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);

    //tex coord attrib
    glVertexAttribPointer(2,2,GL_FLOAT,GL_FALSE,8*sizeof(float),(void*)(6*sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);

    return floorVAO;

}


#endif //PROJECT_BASE_SCENA_H
