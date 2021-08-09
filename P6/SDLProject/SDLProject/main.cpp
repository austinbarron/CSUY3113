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

#include "Entity.h"
#include "Map.h"

#include "Util.h"
#include "Effects.h"
#include "Scene.h"
#include "Menu.h"
#include "Level1.h"
#include "Level2.h"
#include "Level3.h"
#include "Ending.h"


#include <SDL_mixer.h>

SDL_Window* displayWindow;
bool gameIsRunning = true;

ShaderProgram program;
glm::mat4 viewMatrix, modelMatrix, projectionMatrix;

Scene *currentScene;
Scene *sceneList[5];


Effects *effects;

void SwitchToScene(Scene *scene, int totalLivesLeft) {
    currentScene = scene;
    currentScene->Initialize(totalLivesLeft);
}

Mix_Music* music;
Mix_Chunk* bounce;
Mix_Chunk* squish;
Mix_Chunk* failure;
Mix_Chunk* success;
Mix_Chunk* storm;

void Initialize() {
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    displayWindow = SDL_CreateWindow("Textured!", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);
    
#ifdef _WINDOWS
    glewInit();
#endif
    
    glViewport(0, 0, 640, 480);
    
    program.Load("shaders/vertex_lit.glsl", "shaders/fragment_lit.glsl");
    
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);
    
    music = Mix_LoadMUS("thedescent.mp3");
    //Mix_PlayMusic(music, -1);
    
    bounce = Mix_LoadWAV("newbounce.wav");
    squish = Mix_LoadWAV("squish.wav");
    failure = Mix_LoadWAV("failure.wav");
    success = Mix_LoadWAV("success.wav");
    storm = Mix_LoadWAV("thunderstorm.wav");
    
    Mix_VolumeChunk(storm, MIX_MAX_VOLUME * 2);
    Mix_PlayChannel(-1, storm, 0);
    
    viewMatrix = glm::mat4(1.0f);
    modelMatrix = glm::mat4(1.0f);
    projectionMatrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);
    
    program.SetProjectionMatrix(projectionMatrix);
    program.SetViewMatrix(viewMatrix);
    
    glUseProgram(program.programID);
    
    glClearColor(0.0025f, 0.0025f, 0.02f, 1.0f);
    glEnable(GL_BLEND);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
   // Initialize the levels and start at the first one
    sceneList[0] = new Menu();
    sceneList[1] = new Level1();
    sceneList[2] = new Level2();
    sceneList[3] = new Level3();
    sceneList[4] = new Ending();
    
    SwitchToScene(sceneList[0], 3);
    
    effects = new Effects(projectionMatrix,viewMatrix);
    //effects->Start(SHAKE, 1.0f);
    
}

void ProcessInput() {
    
    currentScene->state.player->movement = glm::vec3(0);
    
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
                        if (currentScene->state.player->collidedBottom) {
                            //currentScene->state.player->jump = true;
                            break;
                        }
                        
                    case SDLK_RETURN:
                        if (currentScene == sceneList[0]){
                            SwitchToScene(sceneList[1],3);
                        }
                        
                }
                break; // SDL_KEYDOWN
        }
    }
    
    const Uint8 *keys = SDL_GetKeyboardState(NULL);

    if (keys[SDL_SCANCODE_LEFT]) {
        currentScene->state.player->movement.x = -1.0f;
        currentScene->state.player->animIndices = currentScene->state.player->animLeft;
    }
    else if (keys[SDL_SCANCODE_RIGHT]) {
        currentScene->state.player->movement.x = 1.0f;
        currentScene->state.player->animIndices = currentScene->state.player->animRight;
    }
    
    if (keys[SDL_SCANCODE_DOWN]) {
        currentScene->state.player->movement.y = -1.0f;
        currentScene->state.player->animIndices = currentScene->state.player->animDown;
    }
    
    else if (keys[SDL_SCANCODE_UP]) {
        currentScene->state.player->movement.y = 1.0f;
        currentScene->state.player->animIndices = currentScene->state.player->animUp;
    }
    
    if (glm::length(currentScene->state.player->movement) > 1.0f) {
        currentScene->state.player->movement = glm::normalize(currentScene->state.player->movement);
    }

}

#define FIXED_TIMESTEP 0.0166666f
float lastTicks = 0;
float accumulator = 0.0f;
bool lastCollidedBottom = false;

void Update() {
     float ticks = (float)SDL_GetTicks() / 1000.0f;
     float deltaTime = ticks - lastTicks;
     lastTicks = ticks;

     deltaTime += accumulator;
     if (deltaTime < FIXED_TIMESTEP) {
         accumulator = deltaTime;
         return;
     }

     while (deltaTime >= FIXED_TIMESTEP) {
     // Update. Notice it's FIXED_TIMESTEP. Not deltaTime
         
         currentScene->Update(FIXED_TIMESTEP);
         
         program.SetLightPosition(currentScene->state.player->position);
         
         if (lastCollidedBottom == false && currentScene->state.player->collidedBottom){
             //effects->Start(SHAKE,3.0f);
         }
         
         lastCollidedBottom = currentScene->state.player->collidedBottom;
         
         effects->Update(FIXED_TIMESTEP);
         
         deltaTime -= FIXED_TIMESTEP;
     }

     accumulator = deltaTime;
    
    viewMatrix = glm::mat4(1.0f);
    
        if (currentScene->state.player->position.x > 5) { // if we're passed a certain point then we scroll
            viewMatrix = glm::translate(viewMatrix, glm::vec3(-currentScene->state.player->position.x, 3.75, 0));
        } else { // otherwise leave it alone, so as to not look past that leftern wall
            viewMatrix = glm::translate(viewMatrix, glm::vec3(-5, 3.75, 0));
        }
        
        if (currentScene->state.player->position.y < -6) { // if we're passed a certain point then we scroll
            viewMatrix = glm::translate(viewMatrix, glm::vec3(0,-currentScene->state.player->position.y - 6 , 0));
        } /*else { // otherwise leave it alone, so as to not look past that southern wall
            viewMatrix = glm::translate(viewMatrix, glm::vec3(-5, 3.75, 0));
        }*/

    

    
    viewMatrix = glm::translate(viewMatrix, effects->viewOffset);
}


void Render() {
    glClear(GL_COLOR_BUFFER_BIT);
    
    program.SetViewMatrix(viewMatrix);
    
    glUseProgram(program.programID);
    currentScene->Render(&program);
    
    effects->Render();
    
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
        
   
        if (currentScene->state.nextScene >= 0){ // if they're telling us a scene to go to, go there
            Mix_VolumeChunk(success, MIX_MAX_VOLUME / 8);
            Mix_PlayChannel(-1,success, 0);
            SwitchToScene(sceneList[currentScene->state.nextScene],currentScene->state.player->livesLeft);
        }
        
        Render();
    }
    
    Shutdown();
    return 0;
}
