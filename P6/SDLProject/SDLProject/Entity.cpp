#include "Entity.h"

Entity::Entity()
{
    position = glm::vec3(0);
    movement = glm::vec3(0);
    acceleration = glm::vec3(0);
    velocity = glm::vec3(0);
    
    speed = 0;

    modelMatrix = glm::mat4(1.0f);
}

bool Entity::CheckCollision(Entity *other){
    
    if (other == this) return false; // to make sure we can't collide with ourselves
    
    if (isActive == false || other->isActive == false) return false;
    
    float xdist = fabs(position.x - other->position.x) - ((width + other->width) / 2.0f);
    float ydist = fabs(position.y - other->position.y) - ((height + other->height) / 2.0f);
    
    if (xdist < 0 && ydist < 0 ) return true;
    return false;
}


void Entity::CheckCollisionsY(Entity *objects, int objectCount)
{
     for (int i = 0; i < objectCount; i++)
     {
         Entity *object = &objects[i];

         if (CheckCollision(object)) // here you could ask, if i collided with something, what was it? landing pad or death trap?
         {
             float ydist = fabs(position.y - object->position.y);
             float penetrationY = fabs(ydist - (height / 2.0f) - (object->height / 2.0f));
             if (velocity.y > 0) {
                 position.y -= penetrationY;
                 velocity.y = 0;
                 collidedTop = true;
                 if (object->entityType == ENEMY){
                          ////////////////////////////////////////////////////////////////// HERE WE WANT TO LOSE A LIFE INSTEAD
                          //isActive = false; ////////////////////////////////////////////////////////////////////////////////////////////////////
                          object->isActive = false;
                          //position = glm::vec3(1,-2,0);
                          livesLeft--;
                          break;
                 }
             }
             
             else if (velocity.y < 0) {
                 position.y += penetrationY;
                 velocity.y = 0;
                 collidedBottom = true;
                 if (object->entityType == ENEMY){ // IF WE COLLIDED WITH AN ENEMY
                     // HERE WE INACTIVATE THE ENEMY AND STOP DRAWING THEM //////////////////////////
                     object->isActive = false;
                     // maybe we also incorporate a thing to see if its the last enemy here...hmmm...
                     break;
                 }
             }
         }
     }
}


void Entity::CheckCollisionsX(Entity *objects, int objectCount)
{
    
    for (int i = 0; i < objectCount; i++)
     {
         Entity *object = &objects[i];

         if (CheckCollision(object))
         {
             
             if (object->entityType == ENEMY){
                 //isActive = false; //////////////////////////////////// HERE WE WANT TO LOSE A LIFE INSTEAD
                 //livesLeft--; we dont lose a life for touching them in this game
                 object->isActive = false;
                 //position = glm::vec3(1,-2,0);
                 break;
             }
             
             float xdist = fabs(position.x - object->position.x);
             float penetrationX = fabs(xdist - (width / 2.0f) - (object->width / 2.0f));
             if (velocity.x > 0) {
                 position.x -= penetrationX;
                 velocity.x = 0;
                 collidedRight = true;
             }
             else if (velocity.x < 0) {
                 position.x += penetrationX;
                 velocity.x = 0;
                 collidedLeft = true;
             }
         }
     }
}

void Entity::CheckCollisionsY(Map *map)
{
    // Probes for tiles
    glm::vec3 top = glm::vec3(position.x, position.y + (height / 2), position.z);
    glm::vec3 top_left = glm::vec3(position.x - (width / 2), position.y + (height / 2), position.z);
    glm::vec3 top_right = glm::vec3(position.x + (width / 2), position.y + (height / 2), position.z);
    
    glm::vec3 bottom = glm::vec3(position.x, position.y - (height / 2), position.z);
    glm::vec3 bottom_left = glm::vec3(position.x - (width / 2), position.y - (height / 2), position.z);
    glm::vec3 bottom_right = glm::vec3(position.x + (width / 2), position.y - (height / 2), position.z);
    
    float penetration_x = 0;
    float penetration_y = 0;
    
    if (map->IsSolid(top, &penetration_x, &penetration_y) && velocity.y > 0) {
        position.y -= penetration_y;
        velocity.y = 0;
        collidedTop = true;
    }
    else if (map->IsSolid(top_left, &penetration_x, &penetration_y) && velocity.y > 0) {
        position.y -= penetration_y;
        velocity.y = 0;
        collidedTop = true;
    }
    else if (map->IsSolid(top_right, &penetration_x, &penetration_y) && velocity.y > 0) {
        position.y -= penetration_y;
        velocity.y = 0;
        collidedTop = true;
    }
    
    if (map->IsSolid(bottom, &penetration_x, &penetration_y) && velocity.y < 0) {
        position.y += penetration_y;
        velocity.y = 0;
        collidedBottom = true;
    }
    else if (map->IsSolid(bottom_left, &penetration_x, &penetration_y) && velocity.y < 0) {
        position.y += penetration_y;
        velocity.y = 0;
        collidedBottom = true;
    }
    else if (map->IsSolid(bottom_right, &penetration_x, &penetration_y) && velocity.y < 0) {
        position.y += penetration_y;
        velocity.y = 0;
        collidedBottom = true;
    }
}

void Entity::CheckCollisionsX(Map *map)
{
    // Probes for tiles
    glm::vec3 left = glm::vec3(position.x - (width / 2), position.y, position.z);
    glm::vec3 right = glm::vec3(position.x + (width / 2), position.y, position.z);
    
    float penetration_x = 0;
    float penetration_y = 0;
    
    if (map->IsSolid(left, &penetration_x, &penetration_y) && velocity.x < 0) {
        position.x += penetration_x;
        velocity.x = 0;
        collidedLeft = true;
    }
    
    if (map->IsSolid(right, &penetration_x, &penetration_y) && velocity.x > 0) {
        position.x -= penetration_x;
        velocity.x = 0;
        collidedRight = true;
    }
}

void Entity::AIWalker(){
    movement = glm::vec3(-1,0,0);
}

void Entity::AIWaitAndGo(Entity* player){ //gonna need a pointer to the player to get their position
    switch (aiState){
        case IDLE: // if we're idle, and the player is within three units, switch our state to WALKING
            if (glm::distance(position, player->position) < 3.0f){
                aiState = WALKING;
            }
            break;
            
        case WALKING: // if they're in a walking state, get them to go in this direction
            if (player->position.x < position.x){
                movement = glm::vec3(-1,0,0);
            }else{
                movement = glm::vec3(1,0,0);
            }
            break;
            
        case ATTACKING:
            break;
    }
}

void Entity::AI(Entity* player){ // SORT OF LIKE THE PROCESS INPUT FOR OUR ENEMY
    switch(aiType){
        case WALKER:
            AIWalker();
            break;
            
        case WAITANDGO:
            AIWaitAndGo(player);
            break;
    }
}


void Entity::Update(float deltaTime, Entity* player, Entity *objects, int objectCount, Map *map)
{
    if (isActive == false) return;
    
    collidedTop = false;
    collidedBottom = false;
    collidedLeft = false;
    collidedRight = false;
    
    if (entityType == ENEMY){ // here, should also ask if its a player, and if it is, probably want to check if the player
        AI(player); //              has hit any of our ai.
    }
    
    if (animIndices != NULL) {
        if (glm::length(movement) != 0) {
            animTime += deltaTime;

            if (animTime >= 0.25f)
            {
                animTime = 0.0f;
                animIndex++;
                if (animIndex >= animFrames)
                {
                    animIndex = 0;
                }
            }
        } else {
            animIndex = 0;
        }
    }
    
    if (jump){
        jump = false;
        velocity.y += jumpPower;
    }
    
    velocity.x = movement.x * speed;
    velocity.y = movement.y * speed;
    velocity += acceleration * deltaTime;

    position.y += velocity.y * deltaTime; // Move on Y
    CheckCollisionsY(map);
    CheckCollisionsY(objects, objectCount);

    position.x += velocity.x * deltaTime; // Move on X
    CheckCollisionsX(map);
    CheckCollisionsX(objects, objectCount);
        
    modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, position);
}

void Entity::DrawSpriteFromTextureAtlas(ShaderProgram *program, GLuint textureID, int index)
{
    float u = (float)(index % animCols) / (float)animCols;
    float v = (float)(index / animCols) / (float)animRows;
    
    float width = 1.0f / (float)animCols;
    float height = 1.0f / (float)animRows;
    
    float texCoords[] = { u, v + height, u + width, v + height, u + width, v,
        u, v + height, u + width, v, u, v};
    
    float vertices[]  = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };
    
    glBindTexture(GL_TEXTURE_2D, textureID);
    
    glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program->positionAttribute);
    
    glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
    glEnableVertexAttribArray(program->texCoordAttribute);
    
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    glDisableVertexAttribArray(program->positionAttribute);
    glDisableVertexAttribArray(program->texCoordAttribute);
}

void Entity::Render(ShaderProgram *program) {
    
    if (isActive == false) return;
    
    program->SetModelMatrix(modelMatrix);
    
    if (animIndices != NULL) {
        DrawSpriteFromTextureAtlas(program, textureID, animIndices[animIndex]);
        return;
    }
    
    float vertices[]  = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };
    float texCoords[] = { 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0 };
    
    glBindTexture(GL_TEXTURE_2D, textureID);
    
    glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program->positionAttribute);
    
    glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
    glEnableVertexAttribArray(program->texCoordAttribute);
    
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    glDisableVertexAttribArray(program->positionAttribute);
    glDisableVertexAttribArray(program->texCoordAttribute);
}
