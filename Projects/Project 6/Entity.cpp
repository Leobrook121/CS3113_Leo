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

bool Entity::CheckCollision(Entity *other) {
    if (other == this) return false;

    if (isActive == false || other->isActive == false) return false;

    float xdist = fabs(position.x - other->position.x) - ((width + other->width) / 2.0f);
    float ydist = fabs(position.y - other->position.y) - ((height + other->height) / 2.0f);

    if (xdist < 0 && ydist < 0) return true;

    return false;
}

void Entity::CheckCollisionsY(Entity *objects, int objectCount)
{
    for (int i = 0; i < objectCount; i++)
    {
        Entity* object = &objects[i];
        if (CheckCollision(object))
        {

            float xdist = fabs(position.x - object->position.x);
            if ((entityType == PLAYER) &&(object->entityType == ENEMY) && (xdist < 0.4)) { // Collide with an enemy vertically, the enemy is eliminated
                object->isActive = false;
                win = false;
            }

            float ydist = fabs(position.y - object->position.y);
            float penetrationY = fabs(ydist - (height / 2.0f) - (object->height / 2.0f));
            if (velocity.y > 0) {
                position.y -= penetrationY;
                velocity.y = 0;
                collidedTop = true;
            }
            else if (velocity.y < 0) {
                position.y += penetrationY;
                velocity.y = 0;
                collidedBottom = true;
            }
        }
    }
}

void Entity::CheckCollisionsX(Entity *objects, int objectCount)
{
    for (int i = 0; i < objectCount; i++)
    {
        Entity* object = &objects[i];
        if (CheckCollision(object))
        {

            if (object->entityType == ENEMY) { // Collide with an enemy horizontally, lose a live
                collide = true;
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

void Entity::AIIdle() {}

void Entity::AIWaitAndGo(Entity *player) {
    switch (aiState) {
        case WAITING:
            if (glm::distance(position, player->position) < 3.0f) {
                aiState = WALKING;
            }
            break;
        case WALKING:
            if (player->position.x < position.x) {
                movement = glm::vec3(-1, 0, 0);
            }
            else {
                movement = glm::vec3(1, 0, 0);
            }
            break;
        case ATTACKING:
            break;
        case LEVEL1:
            break;
        case LEVEL2:
            break;
        case LEVEL3:
            break;
    }
}

void Entity::AIFollow() {
    switch (aiState) {
        case LEVEL1:
            movement = glm::vec3(0.5, 0, 0);
            if (collidedBottom && (position.x >= 10) && (position.x <= 15)) {
                jump = true;
            }
            break;
        case LEVEL2:
            if ((position.y >= -6.2) && (position.y <= -6)) {
                movement.x = 0.5;

                if (collidedBottom) {
                    if ( ((position.x >= 1.6) && (position.x <= 2)) ||
                        ((position.x >= 3.6) && (position.x <= 4)) ||
                        ((position.x >= 5.6) && (position.x <= 6)) ||
                        ((position.x >= 7.6) && (position.x <= 8)) ||
                        ((position.x >= 9) && (position.x <= 9.4)) )
                    jump = true;
                }
            }
            else if ((position.y >= -5.2) && (position.y <= -4.8) && (position.x >= 10) && (position.x <= 10.5)) {
                movement.x = -0.7;

            }
            else if ((movement.x < 0) && (position.y >= -5.2) && (position.y <= -4.8) && (position.x >= 9.6) && (position.x <= 9.8)) {
                if (collidedBottom)
                    jump = true;
            }
            else if ((position.y >= -3.2) && (position.y <= -3)) {
                if (collidedBottom && (position.x >= 1.8) && (position.x <= 2.15)) {
                    jump = true;
                }
            }
            else if ((position.y >= -1.2) && (position.y <= -1) && (position.x >= 0.5) && (position.x <= 1)) {
                movement.x = 0.5;
            }
            else if ((movement.x > 0) && (position.y >= -1.2) && (position.y <= -0.8) && (position.x >= 1.6) && (position.x <= 1.8)) {
                if(collidedBottom)
                    jump = true;
            }
            else if (position.x >= 12) {
                movement.x = 0;
            }
                
                
            break;
    }
}


void Entity::AI(Entity *player) {
    switch (aiType) {
        case IDLE:
            AIIdle();
            break;
        case WAITANDGO:
            AIWaitAndGo(player);
            break;
        case FOLLOW:
            AIFollow();
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

    /* Check for collisions
    for (int i = 0; i < platformCount; i++) {
        if (CheckCollision(&platforms[i])) return;
    }
    */

    if (jump) {
        jump = false;

        velocity.y += jumpPower;
    }
    
    velocity.x = movement.x * speed;
    velocity += acceleration * deltaTime;
    position += velocity * deltaTime;

    position.y += velocity.y * deltaTime; // Move on Y
    CheckCollisionsY(map);
    CheckCollisionsY(objects, objectCount); // Fix if needed

    position.x += velocity.x * deltaTime; // Move on X
    CheckCollisionsX(map);
    CheckCollisionsX(objects, objectCount); // Fix if needed

    if (entityType == ENEMY) {
        AI(player);
    }

    // For players, check position and collisions with enemies
    if (entityType == PLAYER) {

        CheckCollisionsY(objects, objectCount);
        CheckCollisionsX(objects, objectCount);

    }


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
