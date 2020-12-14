#include "Level1.h"

#define LEVEL1_WIDTH 30
#define LEVEL1_HEIGHT 8
#define LEVEL1_ENEMY_COUNT 2

unsigned int level1_data[] =
{
	93, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	93, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	93, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	93, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	93, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 4, 4, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0,
	93, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 4, 4, 4, 0, 0, 0, 0, 2, 2, 2, 2, 4, 2, 0, 2, 2, 27,
	93, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 4, 4, 4, 4, 2, 2, 2, 2, 4, 4, 4, 4, 4, 4, 0, 4, 4, 4,
	93, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 0, 4, 4, 4
};

void Level1::Initialize() {

    state.nextScene = -1;

	GLuint mapTextureID = Util::LoadTexture("assets\\platform.png");
	state.map = new Map(LEVEL1_WIDTH, LEVEL1_HEIGHT, level1_data, mapTextureID, 1.0f, 14, 8);
    
    // Initialize Game Objects

    // Initialize Player
    state.player = new Entity();
    state.player->entityType = PLAYER;
    state.player->position = glm::vec3(4, 1, 0);
    state.player->movement = glm::vec3(0);
    state.player->acceleration = glm::vec3(0, -3.0f, 0);
    state.player->speed = 0.4f;
    state.player->textureID = Util::LoadTexture("assets\\player.png");

    state.player->animRight = new int[4]{ 0, 10, 9 };
    state.player->animLeft = new int[4]{ 35, 43, 44 };
    state.player->animUp = new int[4]{ 0, 1, 1 };
    state.player->animDown = new int[4]{ 1, 2, 2 };

    state.player->animIndices = state.player->animRight;
    state.player->animFrames = 3;
    state.player->animIndex = 0;
    state.player->animTime = 0;
    state.player->animCols = 9;
    state.player->animRows = 6;

    state.player->height = 1.0f;
    state.player->width = 0.8f;
    state.player->jumpPower = 1.6f;



    // Initialize Enemies
    state.enemies = new Entity[LEVEL1_ENEMY_COUNT];     
    GLuint enemyTextureID = Util::LoadTexture("assets\\zombie.png");
    GLuint enemyTextureID2 = Util::LoadTexture("assets\\zombie2.png");

    state.enemies[0].entityType = ENEMY;
    state.enemies[0].textureID = enemyTextureID;
    state.enemies[0].position = glm::vec3(16, -5, 0);
    state.enemies[0].speed = 0.5;
    state.enemies[0].aiType = WAITANDGO;
    state.enemies[0].aiState = WAITING;

    state.enemies[1].entityType = ENEMY;
    state.enemies[1].textureID = enemyTextureID2;
    state.enemies[1].position = glm::vec3(1, -6, 0);
    state.enemies[1].acceleration = glm::vec3(0, -3.0f, 0);
    state.enemies[1].speed = 0.6;
    state.enemies[1].jumpPower = 1.6f;
    state.enemies[1].aiType = FOLLOW;
    state.enemies[1].aiState = LEVEL1;
}

void Level1::Update(float deltaTime) {
    if (state.player->lose || state.player->win) return;

	state.player->Update(deltaTime, state.player, state.enemies, LEVEL1_ENEMY_COUNT, state.map);
    
    for (int i = 0; i < LEVEL1_ENEMY_COUNT; i++) {
        state.enemies[i].Update(deltaTime, state.player, state.enemies, LEVEL1_ENEMY_COUNT, state.map);
    }
    
    if ((state.player->position.x >= 29) && (state.player->position.x <= 29.5) &&
        (state.player->position.y >= -4.5) && (state.player->position.y <= -4)) {
        state.nextScene = 2;
    }
}

void Level1::Render(ShaderProgram *program) {
	state.map->Render(program);
	state.player->Render(program);
    for (int i = 0; i < LEVEL1_ENEMY_COUNT; i++) {
        state.enemies[i].Render(program);
    }
}