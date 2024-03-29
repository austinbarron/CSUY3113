#include "Level1.h"

#define LEVEL1_WIDTH 14
#define LEVEL1_HEIGHT 8

#define LEVEL1_ENEMY_COUNT 1

int Level1_numEnemiesKilled = 0;

unsigned int level1_data[] =
{
    30,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,
    30,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,
    30,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,
    30,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,
    30,  0,  0,  0,  0,  8,  8,  0,  0,  0,  0,  0, 30, 0,
    30,  0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  1, 30, 0,
    30,  1,  1,  1,  1,  1,  1,  1,  1, 11, 11, 11, 30, 0,
    30, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 30, 0
};

void Level1::Initialize(int totalLivesLeft) {
    
    //state.playerLivesLeft = totalLivesLeft;
    
    state.nextScene = -1;
    
    GLuint mapTextureID = Util::LoadTexture("swamp_Tileset.png");
    state.map = new Map(LEVEL1_WIDTH, LEVEL1_HEIGHT, level1_data, mapTextureID, 1.0f, 10, 6);
    
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
    state.player->width = 0.8f;
    
    state.player->jumpPower = 8.0f;
    
    // now initializing the enemies
    state.enemies = new Entity[LEVEL1_ENEMY_COUNT];
    GLuint enemyTextureID = Util::LoadTexture("goomba.png");
    
    state.enemies[0].entityType = ENEMY;
    state.enemies[0].textureID = enemyTextureID;
    state.enemies[0].position = glm::vec3(6,-3,0);
    //state.enemies[0].acceleration = glm::vec3(0,-9.81f,0);
    state.enemies[0].speed = 1;
    state.enemies[0].aiType = WAITANDGO;
    state.enemies[0].aiState = IDLE;

}
    
void Level1::Update(float deltaTime) {
    
    state.player->Update(deltaTime, state.player, state.enemies, LEVEL1_ENEMY_COUNT, state.map);

    
    for (int i = 0; i < LEVEL1_ENEMY_COUNT; i++){
        state.enemies[i].Update(deltaTime, state.player, state.enemies, LEVEL1_ENEMY_COUNT, state.map);
    }
    
    if (state.player->position.x >= 13 && state.player->position.y <= -8){
        state.nextScene = 2;
    }
    
    
    if (state.player->position.y <= -15){ //they've fallen off the map;
        state.player->position = glm::vec3(1,5,0);
        state.player->livesLeft--;
    }
    
    
    //to see if we've killed all the enemies:
    int countTheDead = 0;
    for (int i = 0; i < LEVEL1_ENEMY_COUNT; i++){
        if (not state.enemies[i].isActive){
            countTheDead++;
        }
    }

    if (countTheDead == LEVEL1_ENEMY_COUNT){
        state.areThereEnemies = false;
    }

    // this is the sound effect but we dont have that installed yet my bad boys
    if (countTheDead > Level1_numEnemiesKilled){
        //Mix_PlayChannel(-1,squish, 0);
        Level1_numEnemiesKilled++;
    }
    
}
    
void Level1::Render(ShaderProgram *program) {
    state.map->Render(program);
    state.player->Render(program);
    
    for (int i = 0; i < LEVEL1_ENEMY_COUNT; i++){
            state.enemies[i].Render(program); // i'm pretty sure this is how we want to render our enemies but i'll figure it out
    }
    
    GLuint fontTextureID = Util::LoadTexture("font2.png");
    Util::DrawText(program, fontTextureID, "jump!", 1, -0.5f, glm::vec3(12,-1.5,0));
    
    
    if (state.player->livesLeft == 0){ // our main character is dead
        state.player->isActive = false;
        Util::DrawText(program, fontTextureID, "you've perished", 1, -0.5f, glm::vec3(1,-1.5,0));
    }
    
}
