#include "playground.h"
#include <time.h>
#include <Windows.h>

#include <iostream>

// Include standard headers
#include <stdio.h>
#include <stdlib.h>

// Include GLFW
#include <glfw3.h>
GLFWwindow *window;

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

#include <common/shader.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

int main(void)
{
    // Initialize window
    if (!initializeWindow())
        return -1;

    // Create and compile our GLSL program from the shaders
    //shaderProgram = LoadShaders("SimpleVertexShader.vertexshader", "SimpleFragmentShader.fragmentshader");
    shaderProgram = LoadShaders("TextureVertexShader.vertexshader", "TextureFragmentShader.fragmentshader");

    // Initialize vertex buffer
    if (!initializeVertexbuffer())
        return -1;

    if (!initialzeTexture())
        return -1;

    float deltaX{ 0 };
    float deltaY{ 0 };
    float deltaRoll{ 0 };
    float currentX{ 0 };
    float currentY{ 0 };
    float currentRoll{ 0 };

    // start animation loop until escape key is pressed
    do
    {
        clock_t beginTime = clock();
        
        //handleInputs(deltaX, deltaY, deltaRoll, currentX, currentY, currentRoll);

        //handleGravity(deltaX, deltaY, deltaRoll);

        //// Update vertex buffer
        //if (!updateVertexbuffer(deltaX, deltaY, deltaRoll, currentX, currentY, currentRoll))
        //    return -1;

        updateAnimationLoop();

        // min time for one loop 10mss
        Sleep(max(10 - float(clock() - beginTime), 0.f));
    } // Check if the ESC key was pressed or the window was closed
    while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
           glfwWindowShouldClose(window) == 0);

    // Cleanup and close window
    cleanupVertexbuffer();
    glDeleteProgram(shaderProgram);
    closeWindow();

    return 0;
}

void updateAnimationLoop()
{
    // Clear the screen
    // Dark blue background
    // TODO: Change Background to texture
    glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glBindTexture(GL_TEXTURE_2D, helicopterTextureID);

    // Use our shader
    glUseProgram(shaderProgram);

    glBindVertexArray(vertexArrayID);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);

    // Draw the triangles
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0); 

    //glDisableVertexAttribArray(0);

    // Swap buffers
    glfwSwapBuffers(window);
    glfwPollEvents();
}

bool initializeWindow()
{
    // Initialise GLFW
    if (!glfwInit())
    {
        fprintf(stderr, "Failed to initialize GLFW\n");
        return false;
    }

    glfwWindowHint(GLFW_SAMPLES, 4);    // Anti Aliasing
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Open a window and create its OpenGL context
    window = glfwCreateWindow(1024, 1024, "Helicopter Game", NULL, NULL);
    if (window == NULL)
    {
        fprintf(stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n");
        glfwTerminate();
        return false;
    }
    glfwMakeContextCurrent(window);

    // Initialize GLEW
    glewExperimental = true; // Needed for core profile
    if (glewInit() != GLEW_OK)
    {
        fprintf(stderr, "Failed to initialize GLEW\n");
        glfwTerminate();
        return false;
    }

    // Ensure we can capture the escape key being pressed below
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

    return true;
}

bool initializeVertexbuffer()
{
    vertexbuffer_size = 4;
    // g_vertex_buffer_data = new GLfloat[vertexbuffer_size * (3 + 3 + 2)]{    // vertexbuffer_size * (positions + colors + texure coords)
    GLfloat g_vertex_buffer_data[] = {
        // position             // colors           // texture coords
         0.5f,  0.5f,  0.0f,     1.0f, 0.0f, 0.0f,   1.0f, 1.0f,     // top right
         0.5f, -0.5f,  0.0f,     0.0f, 1.0f, 0.0f,   1.0f, 0.0f,     // bottom right
        -0.5f, -0.5f,  0.0f,     0.0f, 0.0f, 1.0f,   0.0f, 0.0f,     // bottom left
        -0.5f,  0.5f,  0.0f,     1.0f, 1.0f, 0.0f,   0.0f, 1.0f      // top left
    };    
    
    // g_vertex_buffer_indices = new GLint[6]{    // vertexbuffer_size * (positions + colors + texure coords)
    GLfloat g_vertex_buffer_indices[] = {    // vertexbuffer_size * (positions + colors + texure coords)
        0, 1, 3,    // first triangle
        1, 2, 3     // second triangle
    };

    glGenVertexArrays(1, &vertexArrayID); // VAO
    glGenBuffers(1, &vertexbuffer); // VBO
    glGenBuffers(1, &elementbuffer); // EBO

    glBindVertexArray(vertexArrayID);
    
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(g_vertex_buffer_indices), g_vertex_buffer_indices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // position attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // position attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    return true;
}

bool initialzeTexture()
{
    // Helicopter asset
    // https://opengameart.org/content/helicopter-2
    char* helicopterPath = "./../assets/separated_frames/helicopter_1.png";
    int helicopterWidth, helicopterHeight, helicopterNrChannels;
    unsigned char* helicopterData = stbi_load(helicopterPath, &helicopterWidth, &helicopterHeight, &helicopterNrChannels, 0);

    if (!helicopterData) {
        fprintf(stderr, "Failed to load image %s\n", helicopterPath);
        return false;
    }

    glGenTextures(1, &helicopterTextureID);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, helicopterTextureID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); // GL_NEAREST
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, helicopterWidth, helicopterHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, helicopterData);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(helicopterData);


    // Background asset 
    // <a href="https://www.vectorstock.com/royalty-free-vector/turquoise-cave-of-stalagmites-and-stalactites-vector-50064667">Vector image by VectorStock / Heorhii</a>
    // TODO: Load background image
    //glActiveTexture(GL_TEXTURE2);
    //glBindTexture(GL_TEXTURE_2D, helicopterTextureID);

    //glUseProgram(shaderProgram);
    //glUniform1i(glGetUniformLocation(shaderProgram, "helicopterTextureID"), 0);
    //glUniform1i(glGetUniformLocation(shaderProgram, "helicopterTextureID"), 1);

    return true;
}

bool updateVertexbuffer(float& deltaX, float& deltaY, float& deltaRoll, float& currentX, float& currentY, float& currentRoll)
{
    // Limit Roll
    if (currentRoll < -20 && deltaRoll < 0)
    {
        deltaRoll = 0;
    }
    else if (currentRoll > 20 && deltaRoll > 0)
    {
        deltaRoll = 0;
    }
    else
    {
        currentRoll += deltaRoll;
    }

    // Window bounding
    for (int i = 0; i < vertexbuffer_size * 3; i += 3)
    {
        if (deltaX == 0 && deltaY == 0) 
        {
            break;
        }
        vec4 v(g_vertex_buffer_data[i], g_vertex_buffer_data[i + 1], g_vertex_buffer_data[i + 2], 1);
        // Hit x bounding
        if (v.x >= 1.0 && deltaX > 0) {
            deltaX = 0;
        }
        else if (v.x <= -1.0 && deltaX < 0) {
            deltaX = 0;
        }
        // Hit y bounding
        if (v.y >= 1.0 && deltaY > 0) {
            deltaY = 0;
        }
        else if (v.y <= -1.0 && deltaY < 0) {
            deltaY = 0;
        }
        
    }

    // Move to origin -> Rotate around origin -> Move Back to position -> Move about delta
    glm::mat4 transform = glm::translate(glm::mat4(1.0f), glm::vec3(deltaX, deltaY, 0.0))
                        * glm::translate(glm::mat4(1.0f), glm::vec3(currentX, currentY, 0.0))
                        * glm::rotate(glm::mat4(1.0f), glm::radians(deltaRoll), glm::vec3(0, 0, 1))
                        * glm::translate(glm::mat4(1.0f), glm::vec3(-currentX, -currentY, 0.0));

    currentX += deltaX;
    currentY += deltaY;

    for (int i = 0; i < vertexbuffer_size * 3; i += 3)
    {
        vec4 v(g_vertex_buffer_data[i], g_vertex_buffer_data[i + 1], g_vertex_buffer_data[i + 2], 1);
        v = transform * v;
        g_vertex_buffer_data[i] = v.x;
        g_vertex_buffer_data[i + 1] = v.y;
        g_vertex_buffer_data[i + 2] = v.z;
    }

    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

    return true;
}

bool cleanupVertexbuffer()
{
    // Cleanup VBO
    glDeleteBuffers(1, &vertexbuffer);
    glDeleteVertexArrays(1, &vertexArrayID);
    return true;
}

bool closeWindow()
{
    glfwTerminate();
    return true;
}

void handleInputs(float& deltaX, float& deltaY, float& deltaRoll, float& currentX, float& currentY, float& currentRoll)
{
    if (glfwGetKey(window, GLFW_KEY_W))
    {
        deltaY += 0.002f;
    }
    else if (glfwGetKey(window, GLFW_KEY_S))
    {
        deltaY -= 0.002f;
    }
    else {
        deltaY *= 0.85f;
        if (abs(deltaY) < 0.00001f)
        {
            deltaY = 0;
        }
    }
    // normalize y
    deltaY = max(-0.02f, min(deltaY, 0.02f));

    if (glfwGetKey(window, GLFW_KEY_A))
    {
        deltaRoll += 0.1f;
        if (currentRoll + deltaRoll > 0) 
        {
            deltaX -= 0.002f;
        }       
    }
    else if (glfwGetKey(window, GLFW_KEY_D))
    {
        deltaRoll -= 0.1f;
        if (currentRoll + deltaRoll < 0)
        {
            deltaX += 0.002f;
        }
    }
    else {
        deltaX *= 0.85f;
        if (abs(deltaX) < 0.0001f)
        {
            deltaX = 0;
        }
        deltaRoll = currentRoll * -0.08f;
        if (abs(deltaRoll) < 0.0001f)
        {
            deltaRoll = 0.f;
        }
    }
    // normalize x
    deltaX = max(-0.02f, min(deltaX, 0.02f));

    // normalize speed
    vec2 dir(deltaX, deltaY);
    if (length(dir) < -0.02f || length(dir) > 0.02f)
    {
        dir = normalize(dir) * 0.02f;
        deltaX = dir.x;
        deltaY = dir.y;
    }

    // normalize angle roll
    deltaRoll = max(-3.f, min(deltaRoll, 3.f));
}

void handleGravity(float& deltaX, float& deltaY, float& deltaRoll) 
{    
    deltaY -= 0.001f;
    // normalize y
    deltaY = max(-0.04f, min(deltaY, 0.02f));
}

