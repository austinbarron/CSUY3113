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
#include <vector>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "Entity.h"
#include <SDL_mixer.h>

#define PLATFORM_COUNT 15
#define ENEMY_COUNT 3

int numEnemiesKilled = 0;

struct GameState {
    Entity *player;
    Entity *platforms;
    Entity *enemies;
    int numEnemies;
    bool areThereEnemies = true;
};

GameState state;


SDL_Window* displayWindow;
bool gameIsRunning = true;

ShaderProgram program;
glm::mat4 viewMatrix, modelMatrix, projectionMatrix;

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

void DrawText(ShaderProgram *program, GLuint fontTextureID, std::string text, float size, float spacing, glm::vec3 position){
    float width = 1.0f / 16.0f; float height = 1.0f / 16.0f;
    std::vector<float> vertices;
    std::vector<float> texCoords;
    
    for(int i = 0; i < text.size(); i++) {
        int index = (int)text[i];
        float offset = (size + spacing) * i;
        float u = (float)(index % 16) / 16.0f; float v = (float)(index / 16) / 16.0f;
        
        vertices.insert(vertices.end(), {
        offset + (-0.5f * size), 0.5f * size, offset + (-0.5f * size), -0.5f * size, offset + (0.5f * size), 0.5f * size, offset + (0.5f * size), -0.5f * size, offset + (0.5f * size), 0.5f * size, offset + (-0.5f * size), -0.5f * size,
        });
        texCoords.insert(texCoords.end(), { u, v,
                u, v + height,
                u + width, v,
                u + width, v + height,
                u + width, v,
                u, v + height,
            });
        } // end of for loop
    
    glm::mat4 modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, position);
    program->SetModelMatrix(modelMatrix);
    
    glUseProgram(program->programID);
    
    glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices.data());
    glEnableVertexAttribArray(program->positionAttribute);
    
    glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords.data());
    glEnableVertexAttribArray(program->texCoordAttribute);
    
    glBindTexture(GL_TEXTURE_2D, fontTextureID);
    glDrawArrays(GL_TRIANGLES, 0, (int)(text.size() * 6));
    
    glDisableVertexAttribArray(program->positionAttribute);
    glDisableVertexAttribArray(program->texCoordAttribute);
    
}

Mix_Music* music;
Mix_Chunk* bounce;
Mix_Chunk* squish;
Mix_Chunk* failure;
Mix_Chunk* success;

void Initialize() {
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    displayWindow = SDL_CreateWindow("Textured!", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);
    
#ifdef _WINDOWS
    glewInit();
#endif
    
    glViewport(0, 0, 640, 480);
    
    program.Load("shaders/vertex_textured.glsl", "shaders/fragment_textured.glsl");
    
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);
    
    music = Mix_LoadMUS("thedescent.mp3");
    Mix_PlayMusic(music, -1);
    
    bounce = Mix_LoadWAV("newbounce.wav");
    squish = Mix_LoadWAV("squish.wav");
    failure = Mix_LoadWAV("failure.wav");
    success = Mix_LoadWAV("success.wav");
    
    viewMatrix = glm::mat4(1.0f);
    modelMatrix = glm::mat4(1.0f);
    projectionMatrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);
    
    program.SetProjectionMatrix(projectionMatrix);
    program.SetViewMatrix(viewMatrix);
    
    glUseProgram(program.programID);
    
    glClearColor(0.2f, 0.4f, 0.6f, 1.0f);
    glEnable(GL_BLEND);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
   
    // Initialize Game Objects
    
    state.numEnemies = 3; // there are three enemies that must be dealt with
    
    // Initialize Player
    state.player = new Entity();
    state.player->entityType = PLAYER;
    state.player->position = glm::vec3(0,-1,0);
    state.player->movement = glm::vec3(0);
    state.player->acceleration = glm::vec3(0,-9.81f,0);
    state.player->speed = 2.0f;
    state.player->textureID = LoadTexture("Female 20-2.png");
    //state.player->textureID = LoadTexture("mario.png"); -> needs to be a texture atlas
    
    state.player->animRight = new int[3] {6, 7, 8};
    state.player->animLeft = new int[3] {3, 4, 5};
    state.player->animUp = new int[3] {9, 10, 11};
    state.player->animDown = new int[3] {0, 1, 2};
    
    state.player->animIndices = state.player->animRight;
    state.player->animFrames = 3;
    state.player->animIndex = 0;
    state.player->animTime = 0;
    state.player->animCols = 3;
    state.player->animRows = 4;
    
    state.player->height = 1.0f;
    state.player->width = 1.0f;
    
    state.player->jumpPower = 7.0f;
    
    state.platforms = new Entity[PLATFORM_COUNT];
    GLuint platformTextureID = LoadTexture("tile_grass.png");

    int j = -5;
    for (int i = 0; i < 11; i++){
        state.platforms[i].textureID = platformTextureID;
        state.platforms[i].position = glm::vec3(j,-3.75f, 0);
        state.platforms[i].entityType = PLATFORM;
        j++;
    }
    
    state.platforms[11].textureID = platformTextureID;
    state.platforms[11].position = glm::vec3(2, 0,0);
    state.platforms[11].entityType = PLATFORM;
    
    state.platforms[12].textureID = platformTextureID;
    state.platforms[12].position = glm::vec3(3, 0,0);
    state.platforms[12].entityType = PLATFORM;

    state.platforms[13].textureID = platformTextureID;
    state.platforms[13].position = glm::vec3(0, 0.75,0);
    state.platforms[13].entityType = PLATFORM;

    state.platforms[14].textureID = platformTextureID;
    state.platforms[14].position = glm::vec3(-2, -1.5,0);
    state.platforms[14].entityType = PLATFORM;
    
    
    for (int i = 0; i < PLATFORM_COUNT; i++){
        state.platforms[i].Update(0, NULL, NULL, 0, NULL, 0);
    }
    
    // now initializing the enemies
    state.enemies = new Entity[ENEMY_COUNT];
    GLuint enemyTextureID = LoadTexture("luigi.png");
    
    for (int i = 0; i < ENEMY_COUNT; i++){
        state.enemies[i].entityType = ENEMY;
        state.enemies[i].textureID = enemyTextureID;
        state.enemies[i].speed = 1;
        state.enemies[i].width = 0.75;
    }
    
    //the wait and go enemy
    state.enemies[0].position = glm::vec3(4,-2.75f,0);
    state.enemies[0].aiType = WAITANDGO;
    state.enemies[0].aiState = IDLE;
    
    //the enemy that immediately approaches
    state.enemies[1].position = glm::vec3(-4,-2.75f,0);
    state.enemies[1].aiType = WALKER;
    state.enemies[1].aiState = IDLE;
    
    //the patroling enemy
    state.enemies[2].position = glm::vec3(2.5f,1,0);
    state.enemies[2].movement = glm::vec3(0.5,0,0);
    state.enemies[2].aiType = PATROL;
    
}

void ProcessInput() {
    
    state.player->movement = glm::vec3(0);
    
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
                        if (state.player->collidedBottom) {
                            state.player->jump = true;
                            Mix_VolumeChunk(bounce, MIX_MAX_VOLUME / 4);
                            Mix_PlayChannel(-1,bounce, 0);
                            break;
                        }
                }
                break; // SDL_KEYDOWN
        }
    }
    
    const Uint8 *keys = SDL_GetKeyboardState(NULL);

    if (keys[SDL_SCANCODE_LEFT]) {
        state.player->movement.x = -1.0f;
        state.player->animIndices = state.player->animLeft;
    }
    
    else if (keys[SDL_SCANCODE_RIGHT]) {
        state.player->movement.x = 1.0f;
        state.player->animIndices = state.player->animRight;
    }
    
    if (glm::length(state.player->movement) > 1.0f) {
        state.player->movement = glm::normalize(state.player->movement);
    }

}

#define FIXED_TIMESTEP 0.0166666f
float lastTicks = 0;
float accumulator = 0.0f;

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
         state.player->Update(FIXED_TIMESTEP, state.player, state.platforms, PLATFORM_COUNT, state.enemies, ENEMY_COUNT);
         
         for (int i = 0; i < ENEMY_COUNT; i++){
             state.enemies[i].Update(FIXED_TIMESTEP, state.player, state.platforms, PLATFORM_COUNT, state.enemies, ENEMY_COUNT);
         }
         
         deltaTime -= FIXED_TIMESTEP;
     }

    accumulator = deltaTime;
    
    //to see if we've killed all the enemies:
    int countTheDead = 0;
    for (int i = 0; i < ENEMY_COUNT; i++){
        if (not state.enemies[i].isActive){
            countTheDead++;
        }
    }

    if (countTheDead == ENEMY_COUNT){
        state.areThereEnemies = false;
    }
    
    if (countTheDead > numEnemiesKilled){
        Mix_PlayChannel(-1,squish, 0);
        numEnemiesKilled++;
    }
    
    
}


void Render() {
    glClear(GL_COLOR_BUFFER_BIT);

    for (int i = 0; i < PLATFORM_COUNT; i++){
        state.platforms[i].Render(&program);
    }
    
    for (int i = 0; i < ENEMY_COUNT; i++){
        state.enemies[i].Render(&program);
    }
    
    state.player->Render(&program);
    
    if (not state.areThereEnemies){ // all the enemies have been got
        Mix_HaltMusic();
        Mix_VolumeChunk(success, MIX_MAX_VOLUME / 8);
        Mix_PlayChannel(-1,success, 0);
        GLuint fontTextureID = LoadTexture("font2.png");
        DrawText(&program, fontTextureID, "YES! WOO!", 1, -0.5f, glm::vec3(-2,2.5,0));
    }
    
    if (not state.player->isActive){  //we lost :( dang it
        Mix_HaltMusic();
        Mix_VolumeChunk(failure, MIX_MAX_VOLUME / 4);
        Mix_PlayChannel(-1,failure, 0);
        GLuint fontTextureID = LoadTexture("font2.png");
        DrawText(&program, fontTextureID, "no, no good", 1, -0.5f, glm::vec3(-2.5,2.5,0));
    }

    SDL_GL_SwapWindow(displayWindow);
}

void Shutdown() {
    Mix_FreeChunk(bounce);
    Mix_FreeChunk(squish);
    Mix_FreeChunk(success);
    Mix_FreeChunk(failure);
    Mix_FreeMusic(music);

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
