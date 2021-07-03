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
glm::mat4 viewMatrix, starMatrix, marioMatrix, luigiMatrix, projectionMatrix;


// Start at 0, 0, 0
glm::vec3 mario_position = glm::vec3(-4.6, 0, 0);
// Don’t go anywhere (yet).
glm::vec3 mario_movement = glm::vec3(0, 0, 0);

float mario_speed = 1.5f;

// Start at 0, 0, 0
glm::vec3 luigi_position = glm::vec3(4.6, 0, 0);
// Don’t go anywhere (yet).
glm::vec3 luigi_movement = glm::vec3(0, 0, 0);

float luigi_speed = 1.5f;

// Start at 0, 0, 0
glm::vec3 star_position = glm::vec3(0, 0, 0);
// Don’t go anywhere (yet).
glm::vec3 star_movement = glm::vec3(0, 0, 0);

float star_speed = 2.0f;

GLuint starTextureID;
GLuint marioTextureID;
GLuint luigiTextureID;

GLuint LoadTexture(const char* filePath) {
 int w, h, n;
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
bool check_collission(float x1, float x2, float y1, float y2, float w1, float w2, float h1, float h2 ){
    float xdist = fabs(x1 - x2) - ((w1 + w2) / 2.0f);
    float ydist = fabs(y1 - y2) - ((h1 + h2) / 2.0f);
    
    if (xdist < 0 && ydist <0){ // then they are colliding
        return true;
    }
    else{
        return false;
    }
}

void Initialize() {
    SDL_Init(SDL_INIT_VIDEO);
    displayWindow = SDL_CreateWindow("Textured!", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);
    
#ifdef _WINDOWS
    glewInit();
#endif
    
    glViewport(0, 0, 640, 480);
    
    program.Load("shaders/vertex_textured.glsl", "shaders/fragment_textured.glsl");
    
    viewMatrix = glm::mat4(1.0f);
    starMatrix = glm::mat4(1.0f);
    marioMatrix = glm::mat4(1.0f);
    luigiMatrix = glm::mat4(1.0f);
    
    projectionMatrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);
    
    program.SetProjectionMatrix(projectionMatrix);
    program.SetViewMatrix(viewMatrix);
    //program.SetColor(1.0f, 0.0f, 1.0f, 1.0f);
    
    glUseProgram(program.programID);
    
    glClearColor(0.0f, 0.5f, 0.5f, 1.0f);
    
    glEnable(GL_BLEND);
    // Good setting for transparency
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    starTextureID = LoadTexture("star.png");
    marioTextureID = LoadTexture("mario.png");
    luigiTextureID = LoadTexture("luigi.png");
}

void ProcessInput() {
    
    //star_movement = glm::vec3(0); //if nothing is pressed, star wont go anywhere
    mario_movement = glm::vec3(0); // if nothing is pressed, mario wont go anywhere
    luigi_movement = glm::vec3(0);
    
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
                         star_movement = glm::vec3(-1,-1.5,0);
                         if (glm::length(star_movement) > 1.0f) {
                             star_movement = glm::normalize(star_movement);
                         }
                         break;
                 }
                 break; // SDL_KEYDOWN
        }
    }
    
    const Uint8 *keys = SDL_GetKeyboardState(NULL);
    
    // for luigi
    if (keys[SDL_SCANCODE_UP]) {
        luigi_movement.y = 1.0f;
    }
    else if (keys[SDL_SCANCODE_DOWN]) {
        luigi_movement.y = -1.0f;
    }
    // for mario
    if (keys[SDL_SCANCODE_W]) {
        mario_movement.y = 1.0f;
    }
    else if (keys[SDL_SCANCODE_S]) {
        mario_movement.y = -1.0f;
    }
    
    // normalizing, so that we stay inside the unit circle ie. no speed boost for holding two keys:
    /*
    not really necessary for pong
    if (glm::length(star_movement) > 1.0f) {
        star_movement = glm::normalize(star_movement);
        mario_movement = glm::normalize(mario_movement);
        luigi_movement = glm::normalize(luigi_movement);
    }
    */

}


float lastTicks = 0.0f;
void Update() {
    
    float ticks = (float)SDL_GetTicks() / 1000.0f;
    float deltaTime = ticks - lastTicks;
    lastTicks = ticks;
    
        
    // Add (direction * units per second * elapsed time)
    luigi_position += luigi_movement * luigi_speed * deltaTime;
    luigiMatrix = glm::mat4(1.0f);
    luigiMatrix = glm::translate(luigiMatrix, luigi_position);
    
    mario_position += mario_movement * mario_speed * deltaTime;
    marioMatrix = glm::mat4(1.0f);
    marioMatrix = glm::translate(marioMatrix, mario_position);
    
    star_position += star_movement * star_speed * deltaTime;
    starMatrix = glm::mat4(1.0f);
    starMatrix = glm::translate(starMatrix, star_position);
    
    bool luigi_star_collided = check_collission(luigi_position.x, star_position.x, luigi_position.y, star_position.y, 0.6f, 0.6f, 0.65f, 0.65f);
    
    bool mario_star_collided = check_collission(mario_position.x, star_position.x, mario_position.y, star_position.y, 0.6f, 0.6f, 0.65f, 0.65f);
    
    if (luigi_star_collided | mario_star_collided){ //if one of our players hits the star,
        star_movement.x = star_movement.x * -1; // we make the star switch directions
    }
    
    if (star_position.y > 3.5 | star_position.y < -3.5){ //if the star hits the roof or the floor
        star_movement.y = star_movement.y * -1;// switch its direction
    }
    
    if (star_position.x > 4.3 | star_position.x < -4.3){ //if the star hits the side
        star_position = glm::vec3(0,0,0); // gonna reset it to the center
        star_movement = glm::vec3(0,0,0); // gonna end its current motion
        mario_position = glm::vec3(-4.6,0,0); // gonna reset it to the center
        mario_movement = glm::vec3(0,0,0); // gonna end its current motion
        luigi_position = glm::vec3(4.6,0,0); // gonna reset it to the center
        luigi_movement = glm::vec3(0,0,0); // gonna end its current motion
    }
    

    if (luigi_position.y >= 3.3){ //if mario hits the roof or the floor
        luigi_movement.y = 0; // make it stop moving
        luigi_position.y = 3.3;
    }
    else if (luigi_position.y <= -3.3){ //if mario hits the roof or the floor
        luigi_movement.y = 0; // make it stop moving
        luigi_position.y = -3.3;
    }
    if (mario_position.y >= 3.3){ //if mario hits the roof or the floor
        mario_movement.y = 0; // make it stop moving
        mario_position.y = 3.3;
    }
    else if (mario_position.y <= -3.3){ //if mario hits the roof or the floor
        mario_movement.y = 0; // make it stop moving
        mario_position.y = -3.3;
    }
}

void Render() {
 glClear(GL_COLOR_BUFFER_BIT);

 float vertices[] = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };
 float texCoords[] = { 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0 };
    
 glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices);
 glEnableVertexAttribArray(program.positionAttribute);
    
 glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
 glEnableVertexAttribArray(program.texCoordAttribute);
    
 program.SetModelMatrix(starMatrix);
 glBindTexture(GL_TEXTURE_2D, starTextureID);
 glDrawArrays(GL_TRIANGLES, 0, 6);
    
 program.SetModelMatrix(marioMatrix);
 glBindTexture(GL_TEXTURE_2D, marioTextureID);
 glDrawArrays(GL_TRIANGLES, 0, 6);
    
 program.SetModelMatrix(luigiMatrix);
 glBindTexture(GL_TEXTURE_2D, luigiTextureID);
 glDrawArrays(GL_TRIANGLES, 0, 6);
    
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
