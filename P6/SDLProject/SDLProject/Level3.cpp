#include "Level3.h"

#define LEVEL3_WIDTH 14
#define LEVEL3_HEIGHT 11

#define LEVEL3_ENEMY_COUNT 3

int Level3_numEnemiesKilled = 0;

/* //original
unsigned int level3_data[] =
{   30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30,30,
    30,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,30,
    30,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,30,
    30,  0,  0,  0, 11,  0,  0,  0,  0, 11,  0, 11,  0,30,
    30,  0,  0,  0, 11,  0, 11,  0,  0, 11,  0, 11,  0,30,
    30,  0,  0,  0,  0,  0, 11,  0,  0,  0,  0, 11,  0,30,
    30,  0,  0, 11, 11, 11, 11,  0, 11,  0,  0, 11,  0,30,
    30,  0,  0,  0,  0,  0, 11, 11, 11, 11, 11, 11, 11,30,
    30,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,30,
    30,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,30,
    30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30,30
};
 */

unsigned int level3_data[] =
{   30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30,30,
    30,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,30,
    30,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,30,
    30,  0,  0,  0, 11,  0,  0,  0,  0, 11,  0, 11,  0,30,
    30,  0,  0,  0, 11,  0, 11,  0,  0, 11,  0, 11,  0,30,
    30,  0,  0,  0,  0,  0, 11,  0,  0,  0,  0, 11,  0,30,
    30,  0,  0, 11, 11, 11, 11,  0, 11,  0,  0, 11,  0,30,
    30,  0,  0,  0,  0,  0, 11, 11, 11, 11, 11, 11, 11,30,
    30,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,30,
    30,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,30,
    30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30,30
};

void Level3::Initialize(int totalLivesLeft) {
    
    
    state.clockTime = 61;
    //state.playerLivesLeft = totalLivesLeft;
    
    state.nextScene = -1;
    
    GLuint mapTextureID = Util::LoadTexture("swamp_Tileset.png");
    state.map = new Map(LEVEL3_WIDTH, LEVEL3_HEIGHT, level3_data, mapTextureID, 1.0f, 10, 6);
    
    // lets initialize all of our objects:
    
    // Initialize Player
    state.player = new Entity();
    state.player->entityType = PLAYER;
    
    state.player->livesLeft = totalLivesLeft;
    
    state.player->position = glm::vec3(2,-2,0);
    state.player->movement = glm::vec3(0);
    //state.player->acceleration = glm::vec3(0,-9.81f,0);
    state.player->speed = 4.0f;
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
    GLuint enemyTextureID = Util::LoadTexture("ghost.png");
    
    state.enemies[0].entityType = ENEMY;
    state.enemies[0].textureID = enemyTextureID;
    state.enemies[0].position = glm::vec3(8,-4,0);
    //state.enemies[0].acceleration = glm::vec3(0,-9.81f,0); gonna ignore acceleration for now
    state.enemies[0].speed = 1;
    state.enemies[0].aiType = WAITANDGO;
    state.enemies[0].aiState = IDLE;
    
    state.enemies[1].entityType = ENEMY;
    state.enemies[1].textureID = enemyTextureID;
    state.enemies[1].position = glm::vec3(12,-6,0);
    state.enemies[1].speed = 1;
    state.enemies[1].aiType = WAITANDGO;
    state.enemies[1].aiState = IDLE;
    
    state.enemies[2].entityType = ENEMY;
    state.enemies[2].textureID = enemyTextureID;
    state.enemies[2].position = glm::vec3(8,-8,0);
    state.enemies[2].speed = 1;
    state.enemies[2].aiType = WAITANDGO;
    state.enemies[2].aiState = IDLE;
}
    
void Level3::Update(float deltaTime) {
    
    
    state.clockTime -= deltaTime;
    
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
        state.nextScene = 4;
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
    
    Util::DrawText(program, fontTextureID, "Final Test!", 0.95, -0.5f, glm::vec3(2,-1.5,0));
    
    int clock = floor(state.clockTime);
    
    std::string stringClock = std::to_string(clock);
    
    if (clock > 0){
        Util::DrawText(program, fontTextureID, stringClock, 1, -0.5f, glm::vec3(1.5,-3,0));
    } else { // our main character ran out of time;
        state.player->isActive = false;
        Util::DrawText(program, fontTextureID, "you've perished", 1, -0.5f, glm::vec3(1.5,-3,0));
    }
    
}
