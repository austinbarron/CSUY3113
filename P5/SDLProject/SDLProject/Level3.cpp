#include "Level3.h"

#define LEVEL3_WIDTH 14
#define LEVEL3_HEIGHT 8

#define LEVEL3_ENEMY_COUNT 1

int Level3_numEnemiesKilled = 0;

unsigned int level3_data[] =
{
    30, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  0,
    30, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  0,
    30, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  0,
    30, 0, 0, 0, 0, 0, 0, 8, 8, 0, 0, 0, 0,  0,
    30, 0, 0, 0, 0, 8, 0, 0, 0, 0, 0, 0, 30, 0,
    30, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 30, 0,
    30, 1, 1, 1, 1, 1, 1, 0, 8, 8, 0, 2, 30, 0,
    30, 2, 2, 2, 2, 2, 2, 0, 0, 0, 0, 2, 30, 0
};

void Level3::Initialize(int totalLivesLeft) {
    
    
    
    //state.playerLivesLeft = totalLivesLeft;
    
    state.nextScene = -1;
    
    GLuint mapTextureID = Util::LoadTexture("swamp_Tileset.png");
    state.map = new Map(LEVEL3_WIDTH, LEVEL3_HEIGHT, level3_data, mapTextureID, 1.0f, 10, 6);
    
    // lets initialize all of our objects:
    
    // Initialize Player
    state.player = new Entity();
    state.player->entityType = PLAYER;
    
    state.player->livesLeft = totalLivesLeft;
    
    state.player->position = glm::vec3(1,5,0);
    state.player->movement = glm::vec3(0);
    state.player->acceleration = glm::vec3(0,-9.81f,0);
    state.player->speed = 2.0f;
    state.player->textureID = Util::LoadTexture("Female 20-2.png");
    
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
    
    state.player->height = 2.0f;
    state.player->width = 1.0f;
    
    state.player->jumpPower = 8.0f;
    
    // now initializing the enemies
    state.enemies = new Entity[LEVEL3_ENEMY_COUNT];
    GLuint enemyTextureID = Util::LoadTexture("goomba.png");
    
    state.enemies[0].entityType = ENEMY;
    state.enemies[0].textureID = enemyTextureID;
    state.enemies[0].position = glm::vec3(11,-4,0);
    //state.enemies[0].acceleration = glm::vec3(0,-9.81f,0); gonna ignore acceleration for now
    state.enemies[0].speed = 1;
    state.enemies[0].aiType = WAITANDGO;
    state.enemies[0].aiState = IDLE;

}
    
void Level3::Update(float deltaTime) {
    
    state.player->Update(deltaTime, state.player, state.enemies, LEVEL3_ENEMY_COUNT, state.map);
    
    for (int i = 0; i < LEVEL3_ENEMY_COUNT; i++){
        state.enemies[i].Update(deltaTime, state.player, state.enemies, LEVEL3_ENEMY_COUNT, state.map);
    }
    
    if (state.player->position.y <= -15){ //they've fallen off the map;
        state.player->position = glm::vec3(1,5,0);
        state.player->livesLeft--;
    }

    //to see if we've killed all the enemies:
    int countTheDead = 0;
    for (int i = 0; i < LEVEL3_ENEMY_COUNT; i++){
        if (not state.enemies[i].isActive){
            countTheDead++;
        }
    }

    if (countTheDead == LEVEL3_ENEMY_COUNT){
        state.areThereEnemies = false;
    }
    
      
    if (countTheDead > Level3_numEnemiesKilled){
        //Mix_PlayChannel(-1,squish, 0); // this is the sound effect but we dont have that installed yet my bad boys
        Level3_numEnemiesKilled++;
    }

}
    
void Level3::Render(ShaderProgram *program) {
    state.map->Render(program);
    state.player->Render(program);
    
    for (int i = 0; i < LEVEL3_ENEMY_COUNT; i++){
            state.enemies[i].Render(program); // i'm pretty sure this is how we want to render our enemies but i'll figure it out
    }
    
    GLuint fontTextureID = Util::LoadTexture("font2.png");
    
    Util::DrawText(program, fontTextureID, "Get the goomba!", 0.95, -0.5f, glm::vec3(2,-1,0));
    
    if (state.player->livesLeft == 0){ // our main character is dead
        state.player->isActive = false;
        
        Util::DrawText(program, fontTextureID, "you've perished", 1, -0.5f, glm::vec3(1,-1.5,0));
    }
    
    if (not state.areThereEnemies){ // all the enemies have been got
        /* the success music comes tomorrow bad boy
        Mix_HaltMusic();
        Mix_VolumeChunk(success, MIX_MAX_VOLUME / 8);
        Mix_PlayChannel(-1,success, 0);
        */
        Util::DrawText(program, fontTextureID, "BIG SUCCESS!", 1, -0.5f, glm::vec3(10,-1.5,0));
    }
    
}
