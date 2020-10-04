
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
glm::mat4 viewMatrix, projectionMatrix, ballMatrix, padLeftMatrix, padRightMatrix;



void Initialize() {
    SDL_Init(SDL_INIT_VIDEO);
    displayWindow = SDL_CreateWindow("League of Legends!", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);

#ifdef _WINDOWS
    glewInit();
#endif

    glViewport(0, 0, 640, 480);

    // If we need texture
    //program.Load("shaders/vertex_textured.glsl", "shaders/fragment_textured.glsl");

    program.Load("shaders/vertex.glsl", "shaders/fragment.glsl");

    viewMatrix = glm::mat4(1.0f);
    projectionMatrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);
    ballMatrix = glm::mat4(1.0f);
    padLeftMatrix = glm::mat4(1.0f);
    padRightMatrix = glm::mat4(1.0f);

    program.SetProjectionMatrix(projectionMatrix);
    program.SetViewMatrix(viewMatrix);
    program.SetColor(1.0f, 1.0f, 0.0f, 1.0f);

    glUseProgram(program.programID);

    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
}



float pad_speed = 25.0f;
float ball_speed = 15.0f;
glm::vec3 padLeft_position = glm::vec3(0, 0, 0);
glm::vec3 padLeft_movement = glm::vec3(0, 0, 0);

glm::vec3 padRight_position = glm::vec3(0, 0, 0);
glm::vec3 padRight_movement = glm::vec3(0, 0, 0);

glm::vec3 ball_position = glm::vec3(0, 0, 0);
glm::vec3 ball_movement = glm::vec3(1.0f, 1.0f, 0);

void ProcessInput() {

    padLeft_movement = glm::vec3(0);
    padRight_movement = glm::vec3(0);

    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
        case SDL_QUIT:
        case SDL_WINDOWEVENT_CLOSE:
            gameIsRunning = false;
            break;

        case SDL_KEYDOWN:
            switch (event.key.keysym.sym) {
            case SDLK_LEFT:
                // Move the player left
                break;
            case SDLK_RIGHT:
                // Move the player right
                break;
            case SDLK_SPACE:
                // Some sort of action
                break;
            }
            break; // SDL_KEYDOWN
        }
    }


    const Uint8* keys = SDL_GetKeyboardState(NULL);
    if (keys[SDL_SCANCODE_UP]) {
        padRight_movement.y = 1.0f;
    }
    else if (keys[SDL_SCANCODE_DOWN]) {
        padRight_movement.y = -1.0f;
    }
    if (keys[SDL_SCANCODE_W]) {
        padLeft_movement.y = 1.0f;
    }
    else if (keys[SDL_SCANCODE_S]) {
        padLeft_movement.y = -1.0f;
    }


}


float lastTicks = 0.0f;

void Update() {
    float ticks = (float)SDL_GetTicks() / 10000.0f;
    float deltaTime = ticks - lastTicks;
    lastTicks = ticks;


    ball_position += ball_movement * ball_speed * deltaTime;
    bool collide_l = ball_position.x - 0.2 <= -4.4 && abs(ball_position.y - padLeft_position.y) <= 0.95;
    bool collide_r = ball_position.x + 0.2 >= 4.4 && abs(ball_position.y - padRight_position.y) <= 0.95;

    if (ball_position.x + 0.2 >= 5 || ball_position.x - 0.2 <= -5) { // end of game, STOP
        ball_movement.x = 0;
        ball_movement.y = 0;
        padLeft_movement.y = 0;
        padRight_movement.y = 0;

    }
    else if (ball_position.y + 0.2 >= 3.75 || ball_position.y - 0.2 <= -3.75) // collide with wall, change y direction
        ball_movement.y *= -1;
    else if (collide_l || collide_r) // collide with paddle, change x direction
        ball_movement.x *= -1;
    


    // Add (direction * units per second * elapsed time)
    padLeft_position += padLeft_movement * pad_speed * deltaTime;
    padRight_position += padRight_movement * pad_speed * deltaTime;

    if (padLeft_position.y + 0.75 >= 3.75)
        padLeft_position.y = 3;
    else if (padLeft_position.y - 0.75 <= -3.75)
        padLeft_position.y = -3;

    if (padRight_position.y + 0.75 >= 3.75)
        padRight_position.y = 3;
    else if (padRight_position.y - 0.75 <= -3.75)
        padRight_position.y = -3;






    padLeftMatrix = glm::mat4{ 1.0f };
    padLeftMatrix = glm::translate(padLeftMatrix, padLeft_position);
    padRightMatrix = glm::mat4{ 1.0f };
    padRightMatrix = glm::translate(padRightMatrix, padRight_position);

    ballMatrix = glm::mat4(1.0f);
    ballMatrix = glm::translate(ballMatrix, ball_position);
}



void Render() {
   
    glClear(GL_COLOR_BUFFER_BIT);

    
    // Left paddle
    float vertices_padLeft[] = { -5, -0.75, -4.4, -0.75, -4.4, 0.75,    -5, -0.75, -4.4, 0.75, -5, 0.75 };
    program.SetModelMatrix(padLeftMatrix);
    glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices_padLeft);
    glEnableVertexAttribArray(program.positionAttribute);

    glDrawArrays(GL_TRIANGLES, 0, 6);

    glDisableVertexAttribArray(program.positionAttribute);



    // Right paddle
    float vertices_padRight[] = { 5, -0.75, 4.4, -0.75, 4.4, 0.75,    5, -0.75, 4.4, 0.75, 5, 0.75 };
    program.SetModelMatrix(padRightMatrix);
    glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices_padRight);
    glEnableVertexAttribArray(program.positionAttribute);

    glDrawArrays(GL_TRIANGLES, 0, 6);

    glDisableVertexAttribArray(program.positionAttribute);



    // Ball
    float vertices_ball[] = { -0.2, -0.2, 0.2, -0.2, 0.2, 0.2,    -0.2, -0.2, 0.2, 0.2, -0.2, 0.2 };
    program.SetModelMatrix(ballMatrix);
    glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices_ball);
    glEnableVertexAttribArray(program.positionAttribute);

    glDrawArrays(GL_TRIANGLES, 0, 6);

    glDisableVertexAttribArray(program.positionAttribute);


    
    // End
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
