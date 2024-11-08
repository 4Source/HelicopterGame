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
#include <glm/gtc/type_ptr.hpp>
using namespace glm;

#include <common/shader.hpp>

#include "stb_image.h"

#include<filesystem>
namespace fs = std::filesystem;

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
        
        handleInputs(deltaX, deltaY, deltaRoll, currentX, currentY, currentRoll);

        handleGravity(deltaX, deltaY, deltaRoll);

        //// Update transformation
        updateTransform(deltaX, deltaY, deltaRoll, currentX, currentY, currentRoll);

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
    // Specify the color of the background
    glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
    // Clear the back buffer 
    glClear(GL_COLOR_BUFFER_BIT);

    // render container
    glUseProgram(shaderProgram);
    
    // Bind textures on corresponding texture units
    //glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, helicopterTextureID);

    // Set the uniform location and set 
    glUseProgram(shaderProgram);
    transformLoc = glGetUniformLocation(shaderProgram, "transform");
    glUniformMatrix4fv(transformLoc, 1, GL_FALSE, value_ptr(transformMat));

    // Bind the vertex array object
    glBindVertexArray(vertexArrayID);

    // Draw the triangles
    //glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0); //glDrawElements(GL_TRIANGLES, 8, GL_UNSIGNED_INT, g_vertex_buffer_indices);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    // Swap buffers
    glfwSwapBuffers(window);
    glfwPollEvents();

    // Unbind vertex array
    glBindVertexArray(0);

    // Unbind the OpenGL texture object
    glBindTexture(GL_TEXTURE_2D, 0);
}

bool initializeWindow()
{
    int width = 1536, height = 1536;
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
    window = glfwCreateWindow(width, height, "Helicopter Game", NULL, NULL);
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

    glViewport(0, 0, width, height);

    return true;
}

bool initializeVertexbuffer()
{
    vertexbuffer_size = 4;
    vec3 btmLeftV = vec3(-0.15f, -0.05f, 0.0f);   vec2 btmLeftTC = vec2(0.13f, 0.0f);
    vec3 topLeftV = vec3(-0.15f, 0.05f, 0.0f);    vec2 topLeftTC = vec2(0.13f, 1.0f);
    vec3 topRightV = vec3(0.15f, 0.05f, 0.0f);    vec2 topRightTC = vec2(0.95f, 1.0f);
    vec3 btmRightV = vec3(0.15f, -0.05f, 0.0f);   vec2 btmRightTC = vec2(0.95f, 0.0f);

    // g_vertex_buffer_data = new GLfloat[vertexbuffer_size * (3 + 3 + 2)]{    // vertexbuffer_size * (positions + colors + texure coords)
    GLfloat g_vertex_buffer_data[] = {
        // position                                // colors            // texture coords
        // first triangle
        btmRightV.x, btmRightV.y, btmRightV.z,     1.0f, 1.0f, 0.0f,    btmRightTC.x, btmRightTC.y,     // bottom right
        btmLeftV.x,  btmLeftV.y,  btmLeftV.z,      1.0f, 1.0f, 0.0f,    btmLeftTC.x,  btmLeftTC.y,      // bottom left
        topLeftV.x,  topLeftV.y,  topLeftV.z,      1.0f, 1.0f, 0.0f,    topLeftTC.x,  topLeftTC.y,      // top left

        // second triangle
        topLeftV.x,  topLeftV.y,  topLeftV.z,      1.0f, 1.0f, 0.0f,    topLeftTC.x,  topLeftTC.y,      // top left
        topRightV.x, topRightV.y, topRightV.z,     1.0f, 1.0f, 0.0f,    topRightTC.x, topRightTC.y,     // top right
        btmRightV.x, btmRightV.y, btmRightV.z,     1.0f, 1.0f, 0.0f,    btmRightTC.x, btmRightTC.y,     // bottom right

    };

    //// g_vertex_buffer_indices = new GLint[6]{    // vertexbuffer_size * (positions + colors + texure coords)
    //GLint g_vertex_buffer_indices[] = {
    //    0, 1, 3,    // first triangle
    //    1, 2, 3     // second triangle
    //};

    // Generate the vertex array object and bind it
    glGenVertexArrays(1, &vertexArrayID); // VAO
    glBindVertexArray(vertexArrayID);

    // Generate the vertex buffer object and link it to vertices
    glGenBuffers(1, &vertexbuffer); // VBO
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

    // Generate element buffer object and link it ot indices
    //glGenBuffers(1, &elementbuffer); // EBO
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
    //glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(g_vertex_buffer_indices), g_vertex_buffer_indices, GL_STATIC_DRAW);

    // Link vertex buffer object attributes 
    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0); // index, size, type, normalized, size of arraybuffer, offset
    glEnableVertexAttribArray(0);
    
    // color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float))); // index, size, type, normalized, size of arraybuffer, offset
    glEnableVertexAttribArray(1);

    // texture coordinates attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float))); // index, size, type, normalized, size of arraybuffer, offset
    glEnableVertexAttribArray(2);

    //glPolygonMode(GL_FRONT, GL_FILL);
    //glPolygonMode(GL_BACK, GL_FILL);

    // Unbind all to prevent accidentally modifying them
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    return true;
}

bool initialzeTexture()
{
    // Enabel texturing
    glEnable(GL_TEXTURE_2D);

    // Import the texture
    // https://opengameart.org/content/helicopter-2
    std::string parentDir = (fs::current_path().fs::path::parent_path()).string();
    std::string helicopterPath = parentDir + "\\assets\\separated_frames\\helicopter_1.png";
    int helicopterWidth, helicopterHeight, helicopterNrChannels;
    // Flip image
    stbi_set_flip_vertically_on_load(true);
    // Load image
    unsigned char* helicopterData = stbi_load(helicopterPath.c_str(), &helicopterWidth, &helicopterHeight, &helicopterNrChannels, 0);

    if (!helicopterData) {
        fprintf(stderr, "Failed to load image %s\n", helicopterPath.c_str());
        return false;
    }

    // Generate OpenGL texture object
    glGenTextures(1, &helicopterTextureID);

    // Assign the texture to a texture unit
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, helicopterTextureID);

    // Configure the type of algorithm that is used to make the image smaller or bigger
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Configure the way the texture repeats
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // Assign the image to the OpenGL texture object
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, helicopterWidth, helicopterHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, helicopterData);
    // Generate Mipmap
    glGenerateMipmap(GL_TEXTURE_2D);

    // Delete the image data
    stbi_image_free(helicopterData);

    // Unbind the OpenGL texture object
    glBindTexture(GL_TEXTURE_2D, 0);

    // Background asset 
    // <a href="https://www.vectorstock.com/royalty-free-vector/turquoise-cave-of-stalagmites-and-stalactites-vector-50064667">Vector image by VectorStock / Heorhii</a>
    // TODO: Load background image
    //glActiveTexture(GL_TEXTURE2);
    //glBindTexture(GL_TEXTURE_2D, helicopterTextureID);

    // Activate the Shader program
    glUseProgram(shaderProgram);
    // Set the value of the uniform 
    glUniform1i(glGetUniformLocation(shaderProgram, "tex0"), 0);
    //glUniform1i(glGetUniformLocation(shaderProgram, "helicopterTextureID"), 1);

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
}

void handleGravity(float& deltaX, float& deltaY, float& deltaRoll)
{    
    deltaY -= 0.001f;
    // normalize y
    deltaY = max(-0.04f, min(deltaY, 0.02f));    
}

void updateTransform(float& deltaX, float& deltaY, float& deltaRoll, float& currentX, float& currentY, float& currentRoll)
{
    // Window bounding
    // Hit x bounding
    if (currentX >= 1.0 && deltaX > 0) {
        deltaX = 0;
    }
    else if (currentX <= -1.0 && deltaX < 0) {
        deltaX = 0;
    }
    // Hit y bounding
    if (currentY >= 1.0 && deltaY > 0) {
        deltaY = 0;
    }
    else if (currentY <= -1.0 && deltaY < 0) {
        deltaY = 0;
    }
        
     
    // Move to origin -> Rotate around origin -> Move Back to position -> Move about delta
    transformMat = glm::translate(glm::mat4(1.0f), glm::vec3(deltaX, deltaY, 0.0))
        * glm::translate(glm::mat4(1.0f), glm::vec3(currentX, currentY, 0.0))
        * glm::rotate(glm::mat4(1.0f), glm::radians(deltaRoll), glm::vec3(0, 0, 1))
        * glm::translate(glm::mat4(1.0f), glm::vec3(-currentX, -currentY, 0.0))
        * transformMat;

    currentX += deltaX;
    currentY += deltaY;
}

