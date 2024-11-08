#ifndef PLAYGROUND_H
#define PLAYGROUND_H

// Include GLEW
#include <GL/glew.h>

#include <glm/glm.hpp>

// some global variables for handling the vertex buffer
GLuint vertexbuffer;
GLuint elementbuffer;
GLuint vertexArrayID;
GLuint vertexbuffer_size;

// program ID of the shaders, required for handling the shaders with OpenGL
GLuint shaderProgram;

//GLfloat* g_vertex_buffer_data;
//GLint* g_vertex_buffer_indices;

// Texture
GLuint helicopterTextureID;
GLuint spriteLoc;
glm::vec2 spriteOffset = glm::vec2(0.0f, 0.0f);

// Transformation 
GLuint transformLoc;
glm::mat4 transformMat = glm::mat4(1.0f);

int main(void);                //<<< main function, called at startup
void updateAnimationLoop();    //<<< updates the animation loop
bool initializeWindow();       //<<< initializes the window using GLFW and GLEW
bool initializeVertexbuffer(); //<<< initializes the vertex buffer array and binds it OpenGL
bool initialzeTexture();		// <<< initializes the textures
bool cleanupVertexbuffer(); //<<< frees all recources from the vertex buffer
bool closeWindow();         //<<< Closes the OpenGL window and terminates GLFW

void handleInputs(float&, float&, float&, float&, float&, float&);
void handleGravity(float&, float&, float&);
void updateTransform(float&, float&, float&, float&, float&, float&);
void updateSprite();


#endif
