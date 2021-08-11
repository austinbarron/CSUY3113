#include "Ending.h"

#define ENDING_WIDTH 11
#define ENDING_HEIGHT 8

#define ENDING_ENEMY_COUNT 1

unsigned int ENDING_data[] =
{
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

void Ending::Initialize(int totalLivesLeft) {
    
    state.playerLivesLeft = totalLivesLeft;
    
    state.nextScene = -1;
    
    GLuint mapTextureID = Util::LoadTexture("tileset.png");
    state.map = new Map(ENDING_WIDTH, ENDING_HEIGHT, ENDING_data, mapTextureID, 1.0f, 4, 1);
    
    // lets initialize all of our objects:
    
    // Initialize Player
    state.player = new Entity();
    state.player->entityType = PLAYER;
    state.player->position = glm::vec3(4,-6,0);
    state.player->movement = glm::vec3(0);
    state.player->acceleration = glm::vec3(0,25.0f,0);
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
    
    state.player->jumpPower = 6.0f;
    
    // now initializing the enemies
    state.enemies = new Entity[ENDING_ENEMY_COUNT];
    GLuint enemyTextureID = Util::LoadTexture("ctg.png");
    
    state.enemies[0].entityType = ENEMY;
    state.enemies[0].textureID = enemyTextureID;
    state.enemies[0].position = glm::vec3(4,-2.75f,0);
    state.enemies[0].speed = 1;
    state.enemies[0].aiType = WAITANDGO;
    state.enemies[0].aiState = IDLE;
    // FOR NOW!!:
    state.enemies[0].isActive = false;
    
    
}

void Ending::Update(float deltaTime) {
    
    state.player->Update(deltaTime, state.player, state.enemies, ENDING_ENEMY_COUNT, state.map);
    
    if (state.player->position.x >= 12){
        state.nextScene = 1;
    }
}
    
void Ending::Render(ShaderProgram *program) {
    state.map->Render(program); state.player->Render(program);
    
    GLuint fontTextureID = Util::LoadTexture("font2.png");
    Util::DrawText(program, fontTextureID, "FREE AT LAST", 1, -0.5f, glm::vec3(2.5,-3.5,0));
}
