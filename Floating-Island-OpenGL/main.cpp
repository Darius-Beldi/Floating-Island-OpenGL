#include <vector>
#include <stdio.h>
#include <string>
#include <cstring>
#include <iostream>
#include <stdlib.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include "loadShaders.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "objloader.hpp"

// OpenGL object identifiers
GLuint VaoId, VboId, ProgramId, nrVertLocation, myMatrixLocation, viewPosLocation, viewLocation, projLocation;
GLuint light1PosLocation, light1ColorLocation;
GLuint light2PosLocation, light2ColorLocation;
GLuint light3PosLocation, light3ColorLocation;

// Pi constant for mathematical calculations
float PI = 3.141592;

// Variable to store the number of vertices
int nrVertices;

// Vectors for vertices, texture coordinates, normals and colors
std::vector<glm::vec3> vertices;
std::vector<glm::vec2> uvs;
std::vector<glm::vec3> normals;
std::vector<glm::vec3> colors;

// Transformation matrices
glm::mat4 myMatrix;
glm::mat4 view;
glm::mat4 projection;

// View matrix elements
float refX = 0.0f, refY = 0.0f, refZ = 0.0f, obsX, obsY, obsZ, vX = 0.0f, vY = 0.0f, vZ = 1.0f;

// Spherical movement parameters
float alpha = 0.0f, beta = 0.0f, dist = 6.0f, incrAlpha1 = 0.01, incrAlpha2 = 0.01;

// Projection matrix parameters
float width = 800, height = 600, dNear = 4.f, fov = 60.f * PI / 180;

// Light 1 warm dawn illumination from the left lamp post
glm::vec3 light1Pos = glm::vec3(-0.5f, 1.0f, 2.98429f);
float light1Intensity = 0.8f;
glm::vec3 light1Color = glm::vec3(1.0f, 0.8f, 0.6f) * light1Intensity;  

// Light 2  warm light from the right lamp post
glm::vec3 light2Pos = glm::vec3(1.4f, 0.462519f, 2.9411f);
float light2Intensity = 0.8f;
glm::vec3 light2Color = glm::vec3(1.0f, 0.8f, 0.6f) * light2Intensity;  

// Light 3 soft pink-ish  light in the center front of the house
glm::vec3 light3Pos = glm::vec3(0.35f, 0.0f, 3.23623f);
float light3Intensity = 0.4f;  // Dimmer than the other two lights
glm::vec3 light3Color = glm::vec3(0.9f, 0.7f, 0.8f) * light3Intensity;  // Soft pink glow

// Handles regular keyboard input
void processNormalKeys(unsigned char key, int x, int y)
{
    switch (key)
    {
    case '+':
        dist -= 0.25; // Move camera closer to the scene
        break;
    case '-':
        dist += 0.25; // Move camera farther from the scene
        break;
    }
    if (key == 27) // ESC key exits the program
        exit(0);
}

// Handles special keyboard keys (arrow keys)
void processSpecialKeys(int key, int xx, int yy)
{
    switch (key)
    {
    case GLUT_KEY_LEFT:
        beta -= 0.01; // Rotate camera left around the scene
        break;
    case GLUT_KEY_RIGHT:
        beta += 0.01; // Rotate camera right around the scene
        break;
    case GLUT_KEY_UP:
        alpha += incrAlpha1; // Move camera upward along the sphere
        if (abs(alpha - PI / 2) < 0.05)
        {
            incrAlpha1 = 0.f; // Stop at the top pole to prevent gimbal lock
        }
        else
        {
            incrAlpha1 = 0.01f;
        }
        break;
    case GLUT_KEY_DOWN:
        alpha -= incrAlpha2; // Move camera downward along the sphere
        if (abs(alpha + PI / 2) < 0.05)
        {
            incrAlpha2 = 0.f; // Stop at the bottom pole
        }
        else
        {
            incrAlpha2 = 0.01f;
        }
        break;
    }
}

// Initializes Vertex Buffer Object for transferring data to the GPU
void CreateVBO(void)
{
    // Generate and bind Vertex Array Object
    glGenVertexArrays(1, &VaoId);
    glBindVertexArray(VaoId);

    // Generate and bind Vertex Buffer Object
    glGenBuffers(1, &VboId);
    glBindBuffer(GL_ARRAY_BUFFER, VboId);

    // Allocate space for positions, normals and colors in a single buffer
    glBufferData(GL_ARRAY_BUFFER,
        vertices.size() * sizeof(glm::vec3) +
        normals.size() * sizeof(glm::vec3) +
        colors.size() * sizeof(glm::vec3),
        NULL, GL_STATIC_DRAW);

    // Copy data to buffer in separate sections
    glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(glm::vec3), &vertices[0]);
    glBufferSubData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3),
        normals.size() * sizeof(glm::vec3), &normals[0]);
    glBufferSubData(GL_ARRAY_BUFFER,
        vertices.size() * sizeof(glm::vec3) + normals.size() * sizeof(glm::vec3),
        colors.size() * sizeof(glm::vec3), &colors[0]);

    // Set up vertex attributes
    glEnableVertexAttribArray(0); // Attribute 0 = position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);

    glEnableVertexAttribArray(1); // Attribute 1 = normals
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0,
        (GLvoid*)(vertices.size() * sizeof(glm::vec3)));

    glEnableVertexAttribArray(2); // Attribute 2 = color
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0,
        (GLvoid*)(vertices.size() * sizeof(glm::vec3) +
            normals.size() * sizeof(glm::vec3)));
}

// Cleanup function to destroy VBO objects
void DestroyVBO(void)
{
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glDeleteVertexArrays(1, &VaoId);
}

// Creates and compiles shader programs
void CreateShaders(void)
{
    ProgramId = LoadShaders("Shader.vert", "Shader.frag");
    glUseProgram(ProgramId);
}

// Cleanup function to destroy shader programs
void DestroyShaders(void)
{
    glDeleteProgram(ProgramId);
}

// Main cleanup function called when program exits
void Cleanup(void)
{
    DestroyShaders();
    DestroyVBO();
};

// Initialize rendering parameters and load 3D model
void Initialize(void)
{
    // Set background color to match dawn/dusk atmosphere
    glClearColor(0.15f, 0.10f, 0.12f, 1.0f);  // Dark purple-orange sky

    // Load the 3D model from OBJ file format
    bool model = loadOBJ("Assets/Island.obj", vertices, uvs, normals, colors);
    nrVertices = vertices.size();

    // Create VBO and shader programs
    CreateVBO();
    CreateShaders();

    // Get uniform locations from shader program for later use
    nrVertLocation = glGetUniformLocation(ProgramId, "nrVertices");
    myMatrixLocation = glGetUniformLocation(ProgramId, "myMatrix");
    viewPosLocation = glGetUniformLocation(ProgramId, "viewPos");
    viewLocation = glGetUniformLocation(ProgramId, "view");
    projLocation = glGetUniformLocation(ProgramId, "projection");

    // Get uniform locations for all three light sources
    light1PosLocation = glGetUniformLocation(ProgramId, "light1Pos");
    light1ColorLocation = glGetUniformLocation(ProgramId, "light1Color");
    light2PosLocation = glGetUniformLocation(ProgramId, "light2Pos");
    light2ColorLocation = glGetUniformLocation(ProgramId, "light2Color");
    light3PosLocation = glGetUniformLocation(ProgramId, "light3Pos");
    light3ColorLocation = glGetUniformLocation(ProgramId, "light3Color");

    // Pass initial values to shaders
    glUniform1i(ProgramId, nrVertices);
}

// Main rendering function called every frame
void RenderFunction(void)
{
    // Clear color and depth buffers to prepare for new frame
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST); // Enable depth testing for proper 3D rendering

    // Set up model transformation matrix (rotations to orient the model correctly)
    myMatrix = glm::rotate(glm::mat4(1.0f), PI / 2, glm::vec3(0.0, 1.0, 0.0)) *
        glm::rotate(glm::mat4(1.0f), PI / 2, glm::vec3(0.0, 0.0, 1.0));
    glUniformMatrix4fv(myMatrixLocation, 1, GL_FALSE, &myMatrix[0][0]);

    // Calculate observer position using spherical coordinates
    obsX = refX + dist * cos(alpha) * cos(beta);
    obsY = refY + dist * cos(alpha) * sin(beta);
    obsZ = refZ + dist * sin(alpha);

    // Define view matrix vectors
    glm::vec3 obs = glm::vec3(obsX, obsY, obsZ);        // Observer position
    glm::vec3 pctRef = glm::vec3(refX, refY, refZ);     // Point to look at (origin)
    glm::vec3 vert = glm::vec3(vX, vY, vZ);             // Up vector

    // Send observer position to shader for lighting calculations
    glUniform3f(viewPosLocation, obsX, obsY, obsZ);

    // Create and send view matrix to shader
    view = glm::lookAt(obs, pctRef, vert);
    glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &view[0][0]);

    // Set up perspective projection with infinite far plane
    projection = glm::infinitePerspective(GLfloat(fov), GLfloat(width) / GLfloat(height), dNear);
    glUniformMatrix4fv(projLocation, 1, GL_FALSE, &projection[0][0]);

    // Send light parameters to shaders for all three light sources
    glUniform3f(light1PosLocation, light1Pos.x, light1Pos.y, light1Pos.z);
    glUniform3f(light1ColorLocation, light1Color.r, light1Color.g, light1Color.b);

    glUniform3f(light2PosLocation, light2Pos.x, light2Pos.y, light2Pos.z);
    glUniform3f(light2ColorLocation, light2Color.r, light2Color.g, light2Color.b);

    glUniform3f(light3PosLocation, light3Pos.x, light3Pos.y, light3Pos.z);
    glUniform3f(light3ColorLocation, light3Color.r, light3Color.g, light3Color.b);

    // Bind VAO and draw the 3D model
    glBindVertexArray(VaoId);
    glEnableVertexAttribArray(0); // Enable position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
    glDrawArrays(GL_TRIANGLES, 0, vertices.size()); // Render all triangles

    // Swap buffers for double buffering
    glutSwapBuffers();
    glFlush();
}

int main(int argc, char* argv[])
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(1200, 900);
    glutCreateWindow("Haunted house");

    glewInit();
    Initialize();

    //register callback functions
    glutIdleFunc(RenderFunction);      
    glutDisplayFunc(RenderFunction);   
    glutKeyboardFunc(processNormalKeys);    
    glutSpecialFunc(processSpecialKeys);    
    glutCloseFunc(Cleanup);            

    glutMainLoop();
}