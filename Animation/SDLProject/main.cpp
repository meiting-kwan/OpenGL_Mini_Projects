#define GL_SILENCE_DEPRECATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#define GL_GLEXT_PROTOTYPES 1
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

SDL_Window* displayWindow;
bool gameIsRunning = true;

ShaderProgram program;
glm::mat4 viewMatrix, projectionMatrix, sunMatrix, robotMatrix, coneMatrix;

float lastTicks = 0.0f;

float sun_rotate = 0.0f;

float robot_x = -5.0f;
float cone_x = -4.0f;

GLuint sunTextureID;
GLuint robotTextureID;
GLuint coneTextureID;

GLuint LoadTexture(const char* filePath) {
    int w,h,n;
    unsigned char* image = stbi_load(filePath, &w, &h, &n, STBI_rgb_alpha);
    
    if (image == NULL) {
        std::cout << "Unable to load image. Make sure the path is correct\n";
        assert(false);
    }
    
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    
    stbi_image_free(image);
    return textureID;
}

void Initialize() {
    SDL_Init(SDL_INIT_VIDEO);
    displayWindow = SDL_CreateWindow("Animation", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);
    
#ifdef _WINDOWS
    glewInit();
#endif
    
    glViewport(0, 0, 640, 480);
    
    program.Load("shaders/vertex_textured.glsl", "shaders/fragment_textured.glsl");
    
    viewMatrix = glm::mat4(1.0f);
    projectionMatrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);
    sunMatrix = glm::mat4(1.0f);
    robotMatrix = glm::mat4(1.0f);
    coneMatrix = glm::mat4(1.0f);
    
    program.SetProjectionMatrix(projectionMatrix);
    program.SetViewMatrix(viewMatrix);
    
    glUseProgram(program.programID);
    
    glClearColor(0.6f, 0.95f, 1.0f, 1.0f);
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    sunTextureID = LoadTexture("sun.png");
    robotTextureID = LoadTexture("robot.png");
    coneTextureID = LoadTexture("cone.png");

}

void ProcessInput() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
            gameIsRunning = false;
        }
    }
}

void Update() {
    
    float ticks = (float) SDL_GetTicks() / 1000.0f;
    float deltaTime = ticks - lastTicks;
    lastTicks = ticks;
    
    
    sun_rotate += 60.0f * deltaTime;
    
    robot_x += 1.5f * deltaTime;
    cone_x += 1.5f * deltaTime;
    
    
    sunMatrix = glm::mat4(1.0f);
    sunMatrix = glm::translate(sunMatrix, glm::vec3(-3.9f,2.7f,0.0f));
    sunMatrix = glm:: scale(sunMatrix, glm::vec3(2.3f,2.3f,1.0f));
    sunMatrix = glm::rotate(sunMatrix, glm::radians(sun_rotate), glm::vec3(0.0,0.0f,1.0f));
    
    robotMatrix = glm::mat4(1.0f);
    robotMatrix = glm:: scale(robotMatrix, glm::vec3(1.5f,1.3f,1.0f));
    robotMatrix = glm::translate(robotMatrix, glm::vec3(robot_x,-1.0f,0.0f));
    
    coneMatrix = glm::mat4(1.0f);
    coneMatrix = glm:: scale(coneMatrix, glm::vec3(1.5f,1.2f,1.0f));
    coneMatrix = glm::translate(coneMatrix, glm::vec3(cone_x,-1.0f,0.0f));
    
    if (robot_x > 4.8f) {
        
        if (cone_x > 4.8f) {
            cone_x = -3.6f;
        }
        
        robot_x = -4.6f;
    }
    
}

void Render() {
    
    float vertices[] = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };
    float texCoords[] = { 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0 };
    
    
    glClear(GL_COLOR_BUFFER_BIT);
    
    glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program.positionAttribute);
    glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
    glEnableVertexAttribArray(program.texCoordAttribute);
    
    
    program.SetModelMatrix(sunMatrix);
    glBindTexture(GL_TEXTURE_2D, sunTextureID);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    program.SetModelMatrix(robotMatrix);
    glBindTexture(GL_TEXTURE_2D,robotTextureID);
    glDrawArrays(GL_TRIANGLES,0,6);
    
    program.SetModelMatrix(coneMatrix);
    glBindTexture(GL_TEXTURE_2D,coneTextureID);
    glDrawArrays(GL_TRIANGLES,0,6);
    
    glDisableVertexAttribArray(program.positionAttribute);
    glDisableVertexAttribArray(program.texCoordAttribute);
    
    SDL_GL_SwapWindow(displayWindow);

}


void Shutdown() {
    SDL_Quit();
}

int main(int argc, char* argv[]) {
    Initialize();
    
    while (gameIsRunning) {
        ProcessInput();
        Update();
        Render();
    }
    
    Shutdown();
    return 0;
}
