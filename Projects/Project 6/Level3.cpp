#include "Level3.h"

#define LEVEL3_WIDTH 25
#define LEVEL3_HEIGHT 8
#define LEVEL3_ENEMY_COUNT 1

unsigned int level3_data[] =
{
    93, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 105, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
    93, 0, 0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 0, 0, 4, 4, 4, 4, 4, 4, 4, 4, 4, 0, 0,
    93, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 2, 0, 4, 4, 4, 4, 4, 4, 4, 4, 0, 0, 0,
    93, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 4, 0, 4, 4, 4, 4, 4, 4, 4, 4, 27, 0, 0,
    93, 0, 2, 2, 2, 2, 2, 2, 2, 0, 0, 4, 4, 0, 4, 4, 4, 4, 4, 4, 4, 4, 0, 0, 0,
    93, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2,
    93, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 4, 0, 0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 4, 4,
    93, 2, 0, 2, 0, 2, 0, 2, 0, 2, 4, 4, 2, 0, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4
};

void Level3::Initialize() {

    state.nextScene = -1;

    GLuint mapTextureID = Util::LoadTexture("assets\\platform.png");
    state.map = new Map(LEVEL3_WIDTH, LEVEL3_HEIGHT, level3_data, mapTextureID, 1.0f, 14, 8);

    // Initialize Game Objects

    // Initialize Player
    state.player = new Entity();
    state.player->entityType = PLAYER;
    state.player->position = glm::vec3(3, -5, 0);
    state.player->movement = glm::vec3(0);
    state.player->acceleration = glm::vec3(0, -3.0f, 0);
    state.player->speed = 0.5f;
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
    state.player->width = 0.7f;
    state.player->jumpPower = 2.4f;



    // Initialize Enemies
    state.enemies = new Entity[LEVEL3_ENEMY_COUNT];
    GLuint enemyTextureID = Util::LoadTexture("assets\\zombie2.png");

    state.enemies[0].entityType = ENEMY;
    state.enemies[0].textureID = enemyTextureID;
    state.enemies[0].position = glm::vec3(1, -6, 0);
    state.enemies[0].movement = glm::vec3(0, 0, 0);
    state.enemies[0].speed = 0.6f;
    state.enemies[0].jumpPower = 2.4f;
    state.enemies[0].acceleration = glm::vec3(0, -3.0f, 0);
    state.enemies[0].aiType = FOLLOW;
    state.enemies[0].aiState = LEVEL2;
    state.enemies[0].width = 0.8f;
}

void Level3::Update(float deltaTime) {
    if (state.player->lose || state.player->win) return;

    state.player->Update(deltaTime, state.player, state.enemies, LEVEL3_ENEMY_COUNT, state.map);

    for (int i = 0; i < LEVEL3_ENEMY_COUNT; i++) {
        state.enemies[i].Update(deltaTime, state.player, state.enemies, LEVEL3_ENEMY_COUNT, state.map);
    }

    if ((state.player->position.x >= 21.5) && (state.player->position.x <= 22) &&
        (state.player->position.y >= -2.5) && (state.player->position.y <= -2)) {
        state.player->win = true;
    }
}

void Level3::Render(ShaderProgram *program) {
    state.map->Render(program);
    state.player->Render(program);
    for (int i = 0; i < LEVEL3_ENEMY_COUNT; i++) {
        state.enemies[i].Render(program);
    }
}