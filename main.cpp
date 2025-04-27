#define _CRT_SECURE_NO_WARNINGS
#define _USE_MATH_DEFINES
#include<math.h>
#include<stdio.h>
#include<string.h>
#include"./SDL2-2.0.10/include/SDL.h"
#include"./SDL2-2.0.10/include/SDL_main.h"

#define SCREEN_WIDTH 640 
#define SCREEN_HEIGHT 480
#define NUMBER_OF_KEYS 256
#define KEY_SHIFT 1073741869
#define FPS 60
#define GRAVITY_CONSTANT 225
#define NUMBER_OF_LEVELS 1 ////3
#define VERTICAL_PLAYER_VELOCITY 150
#define HORIZONTAL_PLAYER_VELOCITY 150
#define PLAYER_WIDTH 15
#define PLAYER_HEIGHT 20
#define JUMP_PLAYER_VELOCITY 150
#define BLACK_COLOR 0x000000
#define YELLOW_COLOR 0xFFFF00
#define RED_COLOR 0xFF0000
#define GREEN_COLOR 0x00FF00
#define BLUE_COLOR  0x0000FF
#define WHITE_COLOR 0xFFFFFF
#define BROWN_COLOR 0x8B4513
#define PURPLE_COLOR 0x800080
#define CLARET_COLOR 0xD2B48C
#define PINK_COLOR 0xFFC0CB
#define BRIGHTBLUE_COLOR 0xADD8E6
#define MAX_BARREL_NUMBER 16
#define BARREL_SIZE 12
#define BARREL_DESTROY_POSITION 460


struct Display {
	SDL_Surface* screen;
	SDL_Surface* charset;
	SDL_Texture* screenTexture;
	SDL_Window* window;
	SDL_Renderer* renderer;
};

struct GameTime {
	int t1;
	int t2;
	int frames;
	double delta;
	double worldTime;
	double updateTimer;
	double frameTime;
	bool update;
};

struct Controls {
	bool* previouslyPressed;
	bool* currentlyPressed;
};

struct Position {
	double x;
	double y;
};

struct Rectangle {
	Position* position;
	int width;
	int height;
};

struct Velocity {
	double velocityX;
	double velocityY;
};

struct Ladder {
	Rectangle* hitBox;
};

struct EndingArea {
	Rectangle* hitBox;
};

struct BarrelThrower {
	Rectangle* hitBox;
};

struct PlatformPart {
	Rectangle* hitBox;
};

struct Platform {
	PlatformPart** platformParts;
	int numberOfPlatformParts;
	int drop;
};

struct LevelData {
	int barrelSpawnFrequency;
	Velocity* initialBarrelVelocity;
	Position* initialPlayerPosition;
};

struct Trophy {
	Rectangle* hitBox;
};

struct Level {
	int numberOfPlatforms;
	Platform** platforms;
	int numberOfLadders;
	Ladder** ladders;
	EndingArea* endingArea;
	BarrelThrower* barrelThrower;
	int numberOfTrophies;
	Trophy** trophy;
	LevelData* levelData;
	
};

struct Character {
	Rectangle* hitBox;
	Velocity* velocity;
};

struct Barrel {
	Rectangle* hitBox;
	Velocity* velocity;
};

struct Game {
	Level* level;
	Barrel** barrels;
	Character* character;
	int currentLevel;
	double barrelTimer;
	int points;
};

Display* initializeDisplay() {
	Display* display = new Display;
	SDL_CreateWindowAndRenderer(SCREEN_WIDTH, SCREEN_HEIGHT, 0, &display->window, &display->renderer);
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
	SDL_RenderSetLogicalSize(display->renderer, SCREEN_WIDTH, SCREEN_HEIGHT);
	SDL_SetRenderDrawColor(display->renderer, 0, 0, 0, 255);
	SDL_SetWindowTitle(display->window, "Jumping Cube");
	display->screen = SDL_CreateRGBSurface(0, SCREEN_WIDTH, SCREEN_HEIGHT, 32, 0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);
	display->screenTexture = SDL_CreateTexture(display->renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, SCREEN_WIDTH, SCREEN_HEIGHT);
	display->charset = SDL_LoadBMP("./cs8x8.bmp");
	SDL_SetColorKey(display->charset, true, 0x000000);
	return display;
}

GameTime* initializeGameTime() {
	GameTime* gameTime = new GameTime;
	gameTime->t1 = SDL_GetTicks();
	gameTime->worldTime = 0;
	gameTime->updateTimer = 0;
	gameTime->frameTime = 0.0;
	return gameTime;
}

Controls* initializeControls() {
	Controls* controls = new Controls;
	controls->currentlyPressed = new bool[NUMBER_OF_KEYS];
	controls->previouslyPressed = new bool[NUMBER_OF_KEYS];
	for (int i = 0; i < NUMBER_OF_KEYS; i++) {
		controls->currentlyPressed[i] = false;
		controls->previouslyPressed[i] = false;
	}
	return controls;
}

Position* constructPosition(int x, int y) {
	Position* position = new Position;
	position->x = x;
	position->y = y;
	return position;
}

Velocity* constructVelocity(double velocityX, double velocityY) {
	Velocity* velocity = new Velocity;
	velocity->velocityX = velocityX;
	velocity->velocityY = velocityY;
	return velocity;
}

Rectangle* constructRectangle(Position* position, int width, int height) {
	Rectangle* rectangle = new Rectangle;
	rectangle->position = position;
	rectangle->width = width;
	rectangle->height = height;
	return rectangle;
}

Ladder* constructLadder(Rectangle* hitBox) {
	Ladder* ladder = new Ladder;
	ladder->hitBox = hitBox;
	return ladder;
}

EndingArea* constructEndingArea(Rectangle* hitBox) {
	EndingArea* endingArea = new EndingArea;
	endingArea->hitBox = hitBox;
	return endingArea;
}

BarrelThrower* constructBarrelThrower(Rectangle* hitBox) {
	BarrelThrower* barrelThrower = new BarrelThrower;
	barrelThrower->hitBox = hitBox;
	return barrelThrower;
}

Trophy* constructTrophy(Rectangle* hitBox) {
	Trophy* trophy = new Trophy;
	trophy->hitBox = hitBox;
	return trophy;
}

PlatformPart* constructPlatformPart(Rectangle* hitBox) {
	PlatformPart* platformPart = new PlatformPart;
	platformPart->hitBox = hitBox;
	return platformPart;
}

Platform* constructPlatform(int numberOfPlatformParts, PlatformPart** platformParts, int drop) {
	Platform* platform = new Platform;
	platform->platformParts = platformParts;
	platform->numberOfPlatformParts = numberOfPlatformParts;
	platform->drop = drop;
	return platform;
}

LevelData* constructLevelData(int barrelSpawnFrequency, Velocity* initialBarrelVelocity, Position* initialPlayerPosition) {
	LevelData* levelData = new LevelData;
	levelData->barrelSpawnFrequency = barrelSpawnFrequency;
	levelData->initialBarrelVelocity = initialBarrelVelocity;
	levelData->initialPlayerPosition = initialPlayerPosition;
	return levelData;
}

Level* constructLevel(LevelData* levelData, Platform** platforms, int numberOfPlatforms, Ladder** ladders, int numberOfLadders, EndingArea* endingArea, BarrelThrower* barrelThrower, int numberOfTrophies, Trophy** trophy) {
	Level* level = new Level;
	level->levelData = levelData;
	level->platforms = platforms;
	level->numberOfPlatforms = numberOfPlatforms;
	level->ladders = ladders;
	level->numberOfLadders = numberOfLadders;
	level->endingArea = endingArea;
	level->barrelThrower = barrelThrower;
	level->numberOfTrophies = numberOfTrophies;
	level->trophy = trophy;
	return level;
}

Character* constructCharacter(Rectangle* hitBox, Velocity* velocity) {
	Character* character = new Character;
	character->hitBox = hitBox;
	character->velocity = velocity;
	return character;
}

Barrel* constructBarrel(Rectangle* hitBox, Velocity* velocity) {
	Barrel* barrel = new Barrel;
	barrel->hitBox = hitBox;
	barrel->velocity = velocity;
	return barrel;
}

Game* constructGame(Level* level, Barrel** barrels, Character* character, int currentLevel, int points) {
	Game* game = new Game;
	game->level = level;
	game->barrels = barrels;
	game->character = character;
	game->currentLevel = currentLevel;
	game->barrelTimer = 0;
	game->points = points;
	return game;
}

Platform* initializePlatform(int x, int y, int width, int height, int drop, int numberOfPlatformParts) {
	PlatformPart** platformParts = new PlatformPart * [numberOfPlatformParts];
	for (int i = 0; i < numberOfPlatformParts; i++) {
		int partX = x + i * width;
		int partY = y + i * drop;
		Position* partPosition = constructPosition(partX, partY);
		Rectangle* partHitBox = constructRectangle(partPosition, width, height);
		platformParts[i] = constructPlatformPart(partHitBox);
	}
	Platform* platform = constructPlatform(numberOfPlatformParts, platformParts, drop);
	return platform;
}

Ladder* initializeLadder(int x, int y, int width, int height) {
	Position* position = constructPosition(x, y);
	Rectangle* hitBox = constructRectangle(position, width, height);
	Ladder* ladder = constructLadder(hitBox);
	return ladder;
}

EndingArea* initializeEndingArea(int x, int y, int width, int height) {
	Position* position = constructPosition(x, y);
	Rectangle* hitBox = constructRectangle(position, width, height);
	EndingArea* endingArea = constructEndingArea(hitBox);
	return endingArea;
}

BarrelThrower* initializeBarrelThrower(int x, int y, int width, int height) {
	Position* position = constructPosition(x, y);
	Rectangle* hitBox = constructRectangle(position, width, height);
	BarrelThrower* barrelThrower = constructBarrelThrower(hitBox);
	return barrelThrower;
}

Trophy* initializeTrophy(int x, int y, int width, int height) {
	Position* position = constructPosition(x, y);
	Rectangle* hitBox = constructRectangle(position, width, height);
	Trophy* trophy = constructTrophy(hitBox);
	return trophy;
}

LevelData* initializeLevelData(int barrelSpawnFrequency, int barrelVelocityX, int barrelVelocityY, int playerPositionX, int playerPositionY) {
	Velocity* barrelVelocity = constructVelocity(barrelVelocityX, barrelVelocityY);
	Position* playerPosition = constructPosition(playerPositionX, playerPositionY);
	LevelData* levelData = constructLevelData(barrelSpawnFrequency, barrelVelocity, playerPosition);
	return levelData;
}

Character* initializeCharacter(Position* initialPosition) {
	Rectangle* hitBox = constructRectangle(initialPosition, PLAYER_WIDTH, PLAYER_HEIGHT);
	Velocity* velocity = constructVelocity(0, 0);
	Character* character = constructCharacter(hitBox, velocity);
	return character;
}

Barrel* initializeBarrel(BarrelThrower* barrelThrower, Velocity* initialBarrelVelocity) {
	Position* position = constructPosition(barrelThrower->hitBox->position->x + barrelThrower->hitBox->width, barrelThrower->hitBox->position->y);
	Rectangle* hitBox = constructRectangle(position, BARREL_SIZE, BARREL_SIZE);
	Velocity* velocity = constructVelocity(initialBarrelVelocity->velocityX, initialBarrelVelocity->velocityY);
	Barrel* barrel = constructBarrel(hitBox, velocity);
	return barrel;
}

Platform** loadPlatforms(FILE* file, int numberOfPlatforms) {
	Platform** platforms = new Platform * [numberOfPlatforms];
	for (int i = 0; i < numberOfPlatforms; i++) {
		int x, y, width, height, drop, numberOfPlatformParts;
		fscanf(file, "%d%d%d%d%d%d", &x, &y, &width, &height, &drop, &numberOfPlatformParts);
		platforms[i] = initializePlatform(x, y, width, height, drop, numberOfPlatformParts);
	}
	return platforms;
}

Ladder** loadLadders(FILE* file, int numberOfLadders) {
	Ladder** ladders = new Ladder * [numberOfLadders];
	for (int i = 0; i < numberOfLadders; i++) {
		int x, y, width, height;
		fscanf(file, "%d%d%d%d", &x, &y, &width, &height);
		ladders[i] = initializeLadder(x, y, width, height);
	}
	return ladders;
}

EndingArea* loadEndingArea(FILE* file) {
	int x, y, width, height;
	fscanf(file, "%d%d%d%d", &x, &y, &width, &height);
	EndingArea* endingArea = initializeEndingArea(x, y, width, height);
	return endingArea;
}

BarrelThrower* loadBarrelThrower(FILE* file) {
	int x, y, width, height;
	fscanf(file, "%d%d%d%d", &x, &y, &width, &height);
	BarrelThrower* barrelThrower = initializeBarrelThrower(x, y, width, height);
	return barrelThrower;
}

Trophy** loadTrophy(FILE* file, int numberOfTrophies) {
	Trophy** trophies = new Trophy * [numberOfTrophies];
	for (int i = 0; i < numberOfTrophies; i++) {
		int x, y, width, height;
		fscanf(file, "%d%d%d%d", &x, &y, &width, &height);
		trophies[i] = initializeTrophy(x, y, width, height);
	}
	return trophies;
}

LevelData* loadLevelData(FILE* file) {
	int barrelSpawnFrequency, barrelVelocityX, barrelVelocityY, playerPositionX, playerPositionY;
	fscanf(file, "%d%d%d%d%d", &barrelSpawnFrequency, &barrelVelocityX, &barrelVelocityY, &playerPositionX, &playerPositionY);
	LevelData* levelData = initializeLevelData(barrelSpawnFrequency, barrelVelocityX, barrelVelocityY, playerPositionX, playerPositionY);
	return levelData;
}

Level* loadLevel(char numberOfLevel) {
	char fileName[6] = "_.txt";
	fileName[0] = numberOfLevel+'0';
	if (fileName[0] == '4') {
		printf("BRAWO WYGRALES");
		exit(0);
	}
	FILE* file = fopen(fileName, "r");

	if (file == NULL) {
		printf("Nie mo¿na otworzyæ pliku.\n");
		exit(0);
	}

	int numberOfPlatforms;
	Platform** platforms;
	int numberOfLadders;
	Ladder** ladders;
	EndingArea* endingArea;
	BarrelThrower* barrelThrower;
	int numberOfTrophies;
	Trophy** trophies;
	LevelData* levelData;

	fscanf(file, "%d", &numberOfPlatforms);
	platforms = loadPlatforms(file, numberOfPlatforms);
	fscanf(file, "%d", &numberOfLadders);
	ladders = loadLadders(file, numberOfLadders);
	endingArea = loadEndingArea(file);
	barrelThrower = loadBarrelThrower(file);
	fscanf(file, "%d", &numberOfTrophies);
	trophies = loadTrophy(file, numberOfTrophies);
	levelData = loadLevelData(file);

	Level* level = constructLevel(levelData, platforms, numberOfPlatforms, ladders, numberOfLadders, endingArea, barrelThrower, numberOfTrophies, trophies);

	fclose(file);
	return level;
}

Game* initializeGame(int currentLevel, int points) {
	Level* level = loadLevel(currentLevel + 1);
	Barrel** barrels = new Barrel * [MAX_BARREL_NUMBER];
	for (int i = 0; i < MAX_BARREL_NUMBER; i++) {
		barrels[i] = nullptr;
	}
	Character* character = initializeCharacter(level->levelData->initialPlayerPosition);
	Game* game = constructGame(level, barrels, character, currentLevel, points);
	return game;
}

void updateDisplay(Display* display) {
	SDL_UpdateTexture(display->screenTexture, NULL, display->screen->pixels, display->screen->pitch);
	SDL_RenderClear(display->renderer);
	SDL_RenderCopy(display->renderer, display->screenTexture, NULL, NULL);
	SDL_RenderPresent(display->renderer);
}

void updateGameTime(GameTime* gameTime) {
	gameTime->t2 = SDL_GetTicks();
	gameTime->delta = (gameTime->t2 - gameTime->t1) * 0.001;
	gameTime->t1 = gameTime->t2;
	gameTime->worldTime += gameTime->delta;
	gameTime->update = false;
	gameTime->updateTimer += gameTime->delta;
	if (gameTime->updateTimer >= 1.0 / (double)FPS)
	{
		gameTime->frameTime = gameTime->updateTimer;
		gameTime->updateTimer -= 1.0 / (double)FPS;
		gameTime->update = true;
	}
}

void handleEvents(bool &quit, Controls* controls) {
	SDL_Event event;

	for (int i = 0; i < NUMBER_OF_KEYS; i++)
	{
		controls->previouslyPressed[i] = controls->currentlyPressed[i];
	}

	while (SDL_PollEvent(&event)) {
		int keyId = event.key.keysym.sym > KEY_SHIFT ? event.key.keysym.sym - KEY_SHIFT : event.key.keysym.sym;
		switch (event.type) {
		case SDL_KEYDOWN:
			controls->currentlyPressed[keyId] = true;
			break;
		case SDL_KEYUP:
			controls->currentlyPressed[keyId] = false;
			break;
		case SDL_QUIT:
			quit = true;
			break;
		}
	}
}

void freeGameTime(GameTime* gameTime) {
	delete gameTime;
	gameTime = nullptr;
}

void freeDisplay(Display* display) {
	SDL_FreeSurface(display->charset);
	SDL_FreeSurface(display->screen);
	SDL_DestroyTexture(display->screenTexture);
	SDL_DestroyRenderer(display->renderer);
	SDL_DestroyWindow(display->window);
	SDL_Quit();
	delete display;
	display = nullptr;
}

void freePosition(Position* position) {
	delete position;
	position = nullptr;
}

void freeVelocity(Velocity* velocity) {
	delete velocity;
	velocity = nullptr;
}

void freeRectangle(Rectangle* rectangle) {
	freePosition(rectangle->position);
	delete rectangle;
	rectangle = nullptr;
}

void freeLadder(Ladder* ladder) {
	freeRectangle(ladder->hitBox);
	delete ladder;
	ladder = nullptr;
}

void freeEndingArea(EndingArea* endingArea) {
	freeRectangle(endingArea->hitBox);
	delete endingArea;
	endingArea = nullptr;
}

void freeBarrelThrower(BarrelThrower* barrelThrower) {
	freeRectangle(barrelThrower->hitBox);
	delete barrelThrower;
	barrelThrower = nullptr;
}

void freePlatformPart(PlatformPart* platformPart){
	freeRectangle(platformPart->hitBox);
	delete platformPart;
	platformPart = nullptr;
} 

void freeLevelData(LevelData* levelData) {
	freeVelocity(levelData->initialBarrelVelocity);
	freePosition(levelData->initialPlayerPosition);
	delete levelData;
	levelData = nullptr;
}

void freeTrophy(Trophy* trophy) {
	freeRectangle(trophy->hitBox);
	delete trophy;
	trophy = nullptr;
}

void freeLevel(Level* level) {
	freeLevelData(level->levelData);
	for (int i = 0; i < level->numberOfPlatforms; i++)
	{
		delete level->platforms[i];
	}
	delete[] level->platforms;
	for (int i = 0; i < level->numberOfLadders; i++)
	{
		freeLadder(level->ladders[i]);
	}
	delete[] level->ladders;
	freeEndingArea(level->endingArea);
	freeBarrelThrower(level->barrelThrower);
	for (int i = 0; i < level->numberOfTrophies; i++) {
		freeTrophy(level->trophy[i]);
	}
	delete[] level->trophy;
	delete level;
	level = nullptr;
}

void freeCharacter(Character* character) {
	freeRectangle(character->hitBox);
	freeVelocity(character->velocity);
	delete character;
	character = nullptr;
}

void freeBarrel(Barrel* barrel) {
	freeRectangle(barrel->hitBox);
	freeVelocity(barrel->velocity);
	delete barrel;
	barrel = nullptr;
}


void freeGame(Game* game) {
	delete game->level;
	for (int i = 0; i < MAX_BARREL_NUMBER; i++) {
		if (game->barrels[i] != nullptr) {
			freeBarrel(game->barrels[i]);
		}
	}
	delete[] game->barrels;
	freeCharacter(game->character);
	delete game;
	game = nullptr;
}

void freeControls(Controls* controls) {
	delete[] controls->currentlyPressed;
	delete[] controls->previouslyPressed;
	delete controls;
	controls = nullptr;
}

void destroyBarrel(Game* game, Barrel* barrel) {
	for (int i = 0; i < MAX_BARREL_NUMBER; i++) {
		if (game->barrels[i] == barrel) {
			freeBarrel(game->barrels[i]);
			game->barrels[i] = nullptr;
		}
	}
}

bool isKeyDown(Controls* controls, int keyId) {
	keyId = keyId > KEY_SHIFT ? keyId - KEY_SHIFT : keyId;
	return controls->currentlyPressed[keyId] && !controls->previouslyPressed[keyId];
}

bool isKeyUp(Controls* controls, int keyId) {
	keyId = keyId > KEY_SHIFT ? keyId - KEY_SHIFT : keyId;
	return !controls->currentlyPressed[keyId] && controls->previouslyPressed[keyId];
}

bool isKeyHeld(Controls* controls, int keyId) {
	keyId = keyId > KEY_SHIFT ? keyId - KEY_SHIFT : keyId;
	return controls->currentlyPressed[keyId];
}

void drawString(Display* display, int x, int y, const char* text)
{
	int px, py, c;
	SDL_Rect s, d;
	s.w = 8;
	s.h = 8;
	d.w = 8;
	d.h = 8;
	while (*text) {
		c = *text & 255;
		px = (c % 16) * 8;
		py = (c / 16) * 8;
		s.x = px;
		s.y = py;
		d.x = x;
		d.y = y;
		SDL_BlitSurface(display->charset, &s, display->screen, &d);
		x += 8;
		text++;
	}
}

void drawSurface(Display* display, SDL_Surface *sprite, int x, int y) {
	SDL_Rect dest;
	dest.x = x - sprite->w / 2;
	dest.y = y - sprite->h / 2;
	dest.w = sprite->w;
	dest.h = sprite->h;
	SDL_BlitSurface(sprite, NULL, display->screen, &dest);
}

void drawPixel(Display* display, int x, int y, int color) {
	int bpp = display->screen->format->BytesPerPixel;
	Uint8 *p = (Uint8 *)display->screen->pixels + y * display->screen->pitch + x * bpp;
	*(Uint32 *)p = color;
}

void DrawLine(Display* display, int x, int y, int l, int dx, int dy, int color) {
	for(int i = 0; i < l; i++) {
		drawPixel(display, x, y, color);
		x += dx;
		y += dy;
	}
}

void DrawRectangle(Display* display, int x, int y, int width, int height, int outlineColor, int fillColor) {
	DrawLine(display, x, y, height, 0, 1, outlineColor);
	DrawLine(display, x + width - 1, y, height, 0, 1, outlineColor);
	DrawLine(display, x, y, width, 1, 0, outlineColor);
	DrawLine(display, x, y + height - 1, width, 1, 0, outlineColor);
	for (int i = y + 1; i < y + height - 1; i++) {
		DrawLine(display, x + 1, i, width - 2, 1, 0, fillColor);
	}
}

void applyGravity(Velocity* velocity, double timeDelta) {
	velocity->velocityY += GRAVITY_CONSTANT * timeDelta;
}

void updateGravity(GameTime* gameTime, Game* game) {
	for (int i = 0; i < MAX_BARREL_NUMBER; i++) {
		if (game->barrels[i] != nullptr) {
			applyGravity(game->barrels[i]->velocity, gameTime->frameTime);
		}
	}
	applyGravity(game->character->velocity, gameTime->frameTime);
}

void applyVelocity(Rectangle* rectangle, Velocity* velocity, double timeDelta) {
	double newPositionX = rectangle->position->x + velocity->velocityX * timeDelta;
	double newPositionY = rectangle->position->y + velocity->velocityY * timeDelta;
	if (0 <= newPositionX && newPositionX + rectangle->width < SCREEN_WIDTH) {
		rectangle->position->x = newPositionX;
	}
	if (0 <= newPositionY && newPositionY + rectangle->height < SCREEN_HEIGHT) {
		rectangle->position->y = newPositionY;
	}
}

void updateVelocity(GameTime* gameTime, Game* game) {
	for (int i = 0; i < MAX_BARREL_NUMBER; i++) {
		if (game->barrels[i] != nullptr) {
			applyVelocity(game->barrels[i]->hitBox, game->barrels[i]->velocity, gameTime->frameTime);
		}
	}
	applyVelocity(game->character->hitBox, game->character->velocity, gameTime->frameTime);
}

bool isPointInRectangle(double x, double y, Rectangle* rectangle) {
	return rectangle->position->x < x &&
		x < rectangle->position->x + rectangle->width &&
		rectangle->position->y < y &&
		y < rectangle->position->y + rectangle->height;
}

bool areRectanglesColliding(Rectangle* rectangle1, Rectangle* rectangle2) {
	return isPointInRectangle(rectangle1->position->x, rectangle1->position->y, rectangle2) ||
		isPointInRectangle(rectangle1->position->x + rectangle1->width, rectangle1->position->y, rectangle2) ||
		isPointInRectangle(rectangle1->position->x, rectangle1->position->y + rectangle1->height, rectangle2) ||
		isPointInRectangle(rectangle1->position->x + rectangle1->width, rectangle1->position->y + rectangle1->height, rectangle2) ||
		isPointInRectangle(rectangle2->position->x, rectangle2->position->y, rectangle1) ||
		isPointInRectangle(rectangle2->position->x + rectangle2->width, rectangle2->position->y, rectangle1) ||
		isPointInRectangle(rectangle2->position->x, rectangle2->position->y + rectangle2->height, rectangle1) ||
		isPointInRectangle(rectangle2->position->x + rectangle2->width, rectangle2->position->y + rectangle2->height, rectangle1);
} 

void alignAfterCollision(Rectangle* rectangleToAlign, Velocity* velocityToAlign, Rectangle* staticRectangle) {
	if (rectangleToAlign->position->y < staticRectangle->position->y && staticRectangle->position->y < rectangleToAlign->position->y + rectangleToAlign->height) {
		rectangleToAlign->position->y = staticRectangle->position->y - rectangleToAlign->height;
		if (velocityToAlign->velocityY > 0) {
			velocityToAlign->velocityY = 0;
		}
	}
	else if (rectangleToAlign->position->y > staticRectangle->position->y && rectangleToAlign->position->y + rectangleToAlign->height > staticRectangle->position->y + staticRectangle->height) {
		rectangleToAlign->position->y = staticRectangle->position->y + staticRectangle->height;
		if (velocityToAlign->velocityY < 0) {
			velocityToAlign->velocityY = 0;
		}
	}
	else if (rectangleToAlign->position->x < staticRectangle->position->x && staticRectangle->position->x < rectangleToAlign->position->x + rectangleToAlign->width) {
		rectangleToAlign->position->x = staticRectangle->position->x - rectangleToAlign->width;
	}
	else if (rectangleToAlign->position->x > staticRectangle->position->x && rectangleToAlign->position->x + rectangleToAlign->width > staticRectangle->position->x + staticRectangle->width) {
		rectangleToAlign->position->x = staticRectangle->position->x + staticRectangle->width;
	}
}

void updatePlayerMovement(Character* character, Controls* controls) {
	if (isKeyHeld(controls, SDLK_LEFT)) {
		character->velocity->velocityX = -HORIZONTAL_PLAYER_VELOCITY;
	}
	if (isKeyHeld(controls, SDLK_RIGHT)) {
		character->velocity->velocityX = HORIZONTAL_PLAYER_VELOCITY;
	}
	if (isKeyUp(controls, SDLK_LEFT)) {
		character->velocity->velocityX = 0;
	}
	if (isKeyUp(controls, SDLK_RIGHT)) {
		character->velocity->velocityX = 0;
	}
}

void playerMovementOnLadder(Character* character, Controls* controls) {
	if (isKeyHeld(controls, SDLK_UP)) {
		character->velocity->velocityY = -VERTICAL_PLAYER_VELOCITY;
	}
	if (isKeyHeld(controls, SDLK_DOWN)) {
		character->velocity->velocityY = VERTICAL_PLAYER_VELOCITY;
	}
	if (isKeyUp(controls, SDLK_UP)) {
		character->velocity->velocityY = 0;	
	}
	if (isKeyUp(controls, SDLK_DOWN)) {
		character->velocity->velocityY = 0;
	}
}

void updatePlayerJump(Character* character, Controls* controls) {
	if (isKeyDown(controls, SDLK_SPACE) && character->velocity->velocityY == 0) {
		character->velocity->velocityY = -JUMP_PLAYER_VELOCITY;
	}
}

void updatePlayerMovementOnLadder(Character* character, Ladder** ladders, int numberOfLadders, Controls* controls) {
	for (int i = 0; i < numberOfLadders; i++) {	
		if (areRectanglesColliding(character->hitBox, ladders[i]->hitBox)) {
			character->velocity->velocityY = 0;
			playerMovementOnLadder(character, controls);
		}
	}
}

void updatePlayerCollisionsWithPlatforms(Character* character, Platform** platforms, int numberOfPlatforms) {
	for (int i = 0; i < numberOfPlatforms; i++) {
		for (int j = 0; j < platforms[i]->numberOfPlatformParts; j++) {
			if (areRectanglesColliding(character->hitBox, platforms[i]->platformParts[j]->hitBox)) {
				alignAfterCollision(character->hitBox, character->velocity, platforms[i]->platformParts[j]->hitBox);
			}
		}
	}
}

void updateBarrelsCollisionsWithPlatforms(Barrel** barrels, Platform** platforms, int numberOfPlatforms) {
	for (int k = 0; k < MAX_BARREL_NUMBER; k++) {
		if (barrels[k] != nullptr) {
			for (int i = 0; i < numberOfPlatforms; i++) {
				for (int j = 0; j < platforms[i]->numberOfPlatformParts; j++) {
					if (areRectanglesColliding(barrels[k]->hitBox, platforms[i]->platformParts[j]->hitBox)) {
						alignAfterCollision(barrels[k]->hitBox, barrels[k]->velocity, platforms[i]->platformParts[j]->hitBox);
						if (platforms[i]->drop > 0 && barrels[k]->velocity->velocityX < 0) {
							barrels[k]->velocity->velocityX *= -1;
						}
						if (platforms[i]->drop < 0 && barrels[k]->velocity->velocityX > 0) {
							barrels[k]->velocity->velocityX *= -1;
						}
					}
				}
			}
		}
	}
}

void updateBarrelsCollisionsWithCharacter(Barrel** barrels, Character* character) {
	for (int i = 0; i < MAX_BARREL_NUMBER; i++) {
		if (barrels[i] != nullptr && areRectanglesColliding(character->hitBox, barrels[i]->hitBox)) {
			printf("PRZEGRALES");
			exit(0);
		}
	}
}

bool isPlayerCollidingWithTrophy(Trophy* trophy, Character* character, int numberOfTrophies) {
		if (areRectanglesColliding(trophy->hitBox, character->hitBox)) {
			printf("kolizja");
			trophy->hitBox->position->x = 0;
			trophy->hitBox->position->y = 0;
			trophy->hitBox->width = 0;
			trophy->hitBox->height = 0;
			return true;
		}
		else {
			return false;
		}
}

void spawnBarrel(Game* game) {
	Barrel* barrel = initializeBarrel(game->level->barrelThrower, game->level->levelData->initialBarrelVelocity);
	for (int i = 0; i < MAX_BARREL_NUMBER; i++) {
		if (game->barrels[i] == nullptr) {
			game->barrels[i] = barrel;
			break;
		}
	}
}

void updateBarrelTimer(Game* game, double timeDelta) {
	game->barrelTimer += timeDelta;
	if (game->barrelTimer >= game->level->levelData->barrelSpawnFrequency) {
		game->barrelTimer = 0;
		spawnBarrel(game);
	}
}

void updateBarrels(Game* game) {
	for (int i = 0; i < MAX_BARREL_NUMBER; i++) {
		if (game->barrels[i] != nullptr && game->barrels[i]->hitBox->position->y + game->barrels[i]->hitBox->height > BARREL_DESTROY_POSITION) {
			destroyBarrel(game, game->barrels[i]);
		}
	}
}

bool isPlayerNotInAir(Game* game);

void updatePointsFromBarrelsAndTrophies(Game*& game) {
	for (int i = 0; i < MAX_BARREL_NUMBER; i++) { 
		if (game->barrels[i] != nullptr) {
			Position* position = constructPosition(game->character->hitBox->position->x, game->character->hitBox->position->y + PLAYER_HEIGHT);
			Rectangle* rectangle = constructRectangle(position, PLAYER_WIDTH, 40);
			if (!isPlayerNotInAir(game)) {
				if (areRectanglesColliding(rectangle, game->barrels[i]->hitBox)) {
					game->points = game->points + 1;
					break;
				}
			}
		}
	}
	for (int i = 0; i < game->level->numberOfTrophies; i++) {
		if (areRectanglesColliding(game->character->hitBox, game->level->trophy[i]->hitBox)) {
			game->points = game->points + 40;
		}
	}
}

void exitGame(Controls* controls, bool& quit) {
	if (isKeyUp(controls, SDLK_ESCAPE)) {
		quit = true;
	}
}

void restartGame(Controls* controls, Game*& game, GameTime* gameTime) {
	if (isKeyUp(controls, SDLK_n)) {
		int currentPoints = game->points;
		currentPoints = 0;
		int currentLevel = game->currentLevel;
		freeGame(game);
		game = initializeGame(currentLevel, currentPoints);
		gameTime->worldTime = 0;
	}
}

void changeLevels(Game*& game, Controls* controls, Display* display, GameTime* gameTime) {
	if (areRectanglesColliding(game->character->hitBox, game->level->endingArea->hitBox)) {
		int currentPoints = game->points + 100;
		int currentLevel = game->currentLevel;
		freeGame(game);
		game = initializeGame(currentLevel + 1, currentPoints);
		gameTime->worldTime = 0;
	}
	if (isKeyUp(controls, SDLK_1)) {
		int currentPoints = game->points;
		freeGame(game);
		game = initializeGame(0, currentPoints);
		gameTime->worldTime = 0;
	}
	if (isKeyUp(controls, SDLK_2)) {
		int currentPoints = game->points;
		freeGame(game);
		game = initializeGame(1, currentPoints);
		gameTime->worldTime = 0;
	}
	if (isKeyUp(controls, SDLK_3)) {
		int currentPoints = game->points;
		freeGame(game);
		game = initializeGame(2, currentPoints);
		gameTime->worldTime = 0;
	}
}


void update(GameTime* gameTime, Game* game, Controls* controls, Display* display, bool& quit) {
	updateGravity(gameTime, game);
	updatePlayerMovementOnLadder(game->character, game->level->ladders, game->level->numberOfLadders, controls);
	updateVelocity(gameTime, game);
	updatePlayerMovement(game->character, controls);
	updatePlayerCollisionsWithPlatforms(game->character, game->level->platforms, game->level->numberOfPlatforms);
	updatePlayerJump(game->character, controls);
	updateBarrelTimer(game, gameTime->frameTime);
	updateBarrels(game);
	updateBarrelsCollisionsWithPlatforms(game->barrels, game->level->platforms, game->level->numberOfPlatforms);
	updateBarrelsCollisionsWithCharacter(game->barrels, game->character);
	updatePointsFromBarrelsAndTrophies(game);
	
}

void renderPlatforms(Display* display, Game* game) {
	for (int i = 0; i < game->level->numberOfPlatforms; i++) {
		for (int j = 0; j < game->level->platforms[i]->numberOfPlatformParts; j++) {
			PlatformPart* platformPart = game->level->platforms[i]->platformParts[j];
			DrawRectangle(display, platformPart->hitBox->position->x, platformPart->hitBox->position->y, platformPart->hitBox->width, platformPart->hitBox->height, YELLOW_COLOR, RED_COLOR);
		}
	}
}

void renderLadders(Display* display, Game* game) {
	for (int i = 0; i < game->level->numberOfLadders; i++) {
		Ladder* ladder = game->level->ladders[i];
		DrawRectangle(display, ladder->hitBox->position->x, ladder->hitBox->position->y, ladder->hitBox->width, ladder->hitBox->height, GREEN_COLOR, BLUE_COLOR);
	}
}

void renderTrophies(Display* display, Game* game) {
		for (int i = 0; i < game->level->numberOfTrophies; i++) {
			Trophy* trophy = game->level->trophy[i];
				if (!isPlayerCollidingWithTrophy(game->level->trophy[i], game->character, game->level->numberOfTrophies)) {
					DrawRectangle(display, trophy->hitBox->position->x, trophy->hitBox->position->y, trophy->hitBox->width, trophy->hitBox->height, BLUE_COLOR, PINK_COLOR);
				}
		}
}

void renderEndingArea(Display* display, Game* game) {
	EndingArea* endingArea = game->level->endingArea;
	DrawRectangle(display, endingArea->hitBox->position->x, endingArea->hitBox->position->y, endingArea->hitBox->width, endingArea->hitBox->height, BLACK_COLOR, BLACK_COLOR);
}

bool barrelThrowerAnimation(Game*& game, GameTime* gameTime) {
	
	if (game->barrelTimer >= game->level->levelData->barrelSpawnFrequency - 1.5 && (game->barrelTimer <= game->level->levelData->barrelSpawnFrequency - 1.2)) {
		return true;
	}
	if (game->barrelTimer >= game->level->levelData->barrelSpawnFrequency - 1 && (game->barrelTimer <= game->level->levelData->barrelSpawnFrequency - 0.7)) {
		return true;
	}
	if(game->barrelTimer >= game->level->levelData->barrelSpawnFrequency - 0.5 && (game->barrelTimer <= game->level->levelData->barrelSpawnFrequency - 0.2)) {
		return true;
	}
	else return false;

}

void renderBarrelThrower(Display* display, Game* game, GameTime* gameTime) {
	BarrelThrower* barrelThrower = game->level->barrelThrower;
	if (barrelThrowerAnimation(game, gameTime)) {
		DrawRectangle(display, barrelThrower->hitBox->position->x, barrelThrower->hitBox->position->y, barrelThrower->hitBox->width, barrelThrower->hitBox->height, WHITE_COLOR, RED_COLOR);
	}
	else {
		DrawRectangle(display, barrelThrower->hitBox->position->x, barrelThrower->hitBox->position->y, barrelThrower->hitBox->width, barrelThrower->hitBox->height, WHITE_COLOR, YELLOW_COLOR);
	}
}

bool isPlayerOnLadder(Game* game) {
	for (int i = 0; i < game->level->numberOfLadders; i++) {
		Ladder* ladder = game->level->ladders[i];
		if (areRectanglesColliding(game->character->hitBox, ladder->hitBox)) {
			return true;
		}
	}
	return false;
}

bool isPlayerNotInAir(Game* game) {
	if (!isPlayerOnLadder(game)) {
		for (int i = 0; i < game->level->numberOfPlatforms; i++) {
			for (int j = 0; j < game->level->platforms[i]->numberOfPlatformParts; j++) {
				for (int k = 0; k < 5; k++) {
					if (isPointInRectangle(game->character->hitBox->position->x, game->character->hitBox->position->y + game->character->hitBox->height + k, game->level->platforms[i]->platformParts[j]->hitBox) ||
						isPointInRectangle(game->character->hitBox->position->x + PLAYER_WIDTH, game->character->hitBox->position->y + game->character->hitBox->height + k, game->level->platforms[i]->platformParts[j]->hitBox)) {
						return true;
					}
				}
			}
		}
	}
	return false;
}

void renderPlayerStartingPosition(Display* display, Game* game, GameTime* gameTime) {
	static double timer;
	timer += gameTime->frameTime * 2;
	if (timer >= 1) {
		timer = 0.0;
	}

		if (isPlayerOnLadder(game)) {
			if (timer < 0.5) {
				DrawRectangle(display, game->character->hitBox->position->x, game->character->hitBox->position->y, game->character->hitBox->width, game->character->hitBox->height, YELLOW_COLOR, CLARET_COLOR);
				DrawRectangle(display, game->character->hitBox->position->x, game->character->hitBox->position->y, game->character->hitBox->width, 8, YELLOW_COLOR, PURPLE_COLOR);
			}
			else {
				DrawRectangle(display, game->character->hitBox->position->x, game->character->hitBox->position->y, game->character->hitBox->width, game->character->hitBox->height, YELLOW_COLOR, CLARET_COLOR);
				DrawRectangle(display, game->character->hitBox->position->x, game->character->hitBox->position->y, game->character->hitBox->width, 8, YELLOW_COLOR, PINK_COLOR);
			}
		}
		else if (game->character->velocity->velocityX != 0 && isPlayerNotInAir(game)) {
				if (timer < 0.5) {
					DrawRectangle(display, game->character->hitBox->position->x, game->character->hitBox->position->y, game->character->hitBox->width, game->character->hitBox->height, YELLOW_COLOR, BLUE_COLOR);
					DrawRectangle(display, game->character->hitBox->position->x, game->character->hitBox->position->y + PLAYER_HEIGHT - 8, game->character->hitBox->width, 8, YELLOW_COLOR, GREEN_COLOR);
				}
				else
				{
					DrawRectangle(display, game->character->hitBox->position->x, game->character->hitBox->position->y, game->character->hitBox->width, game->character->hitBox->height, YELLOW_COLOR, BLUE_COLOR);
					DrawRectangle(display, game->character->hitBox->position->x, game->character->hitBox->position->y + PLAYER_HEIGHT - 8, game->character->hitBox->width, 8, YELLOW_COLOR, YELLOW_COLOR);
				}
		}
		else if (isPlayerNotInAir(game)) {
			DrawRectangle(display, game->character->hitBox->position->x, game->character->hitBox->position->y, game->character->hitBox->width, game->character->hitBox->height, YELLOW_COLOR, BROWN_COLOR);
		}
		else {
			DrawRectangle(display, game->character->hitBox->position->x, game->character->hitBox->position->y, game->character->hitBox->width, game->character->hitBox->height, YELLOW_COLOR, WHITE_COLOR);
		}
}

void barrelUp(Display* display, Game* game, Barrel* barrel) {
	DrawRectangle(display, barrel->hitBox->position->x, barrel->hitBox->position->y, barrel->hitBox->width, barrel->hitBox->height, YELLOW_COLOR, BROWN_COLOR);
	DrawRectangle(display, barrel->hitBox->position->x, barrel->hitBox->position->y, barrel->hitBox->width, 4, YELLOW_COLOR, YELLOW_COLOR);
}

void barrelRight(Display* display, Game* game, Barrel* barrel) {
	DrawRectangle(display, barrel->hitBox->position->x, barrel->hitBox->position->y, barrel->hitBox->width, barrel->hitBox->height, YELLOW_COLOR, BROWN_COLOR);
	DrawRectangle(display, barrel->hitBox->position->x + barrel->hitBox->width - 4, barrel->hitBox->position->y , 4, barrel->hitBox->height, YELLOW_COLOR, YELLOW_COLOR);
}

void barrelDown(Display* display, Game* game, Barrel* barrel) {
	DrawRectangle(display, barrel->hitBox->position->x, barrel->hitBox->position->y, barrel->hitBox->width, barrel->hitBox->height, YELLOW_COLOR, BROWN_COLOR);
	DrawRectangle(display, barrel->hitBox->position->x, barrel->hitBox->position->y + barrel->hitBox->height -4 , barrel->hitBox->width, 4, YELLOW_COLOR, YELLOW_COLOR);
}

void barrelLeft(Display* display, Game* game, Barrel* barrel) {
	DrawRectangle(display, barrel->hitBox->position->x, barrel->hitBox->position->y, barrel->hitBox->width, barrel->hitBox->height, YELLOW_COLOR, BROWN_COLOR);
	DrawRectangle(display, barrel->hitBox->position->x , barrel->hitBox->position->y, 4, barrel->hitBox->height, YELLOW_COLOR, YELLOW_COLOR);
}

void renderBarrels(Display* display, Game* game, GameTime* gameTime) {
	for (int i = 0; i < MAX_BARREL_NUMBER; i++) {
		if (game->barrels[i] != nullptr) {
			Barrel* barrel = game->barrels[i];	
			if (barrel->velocity->velocityX > 0) {
				if ((int)gameTime->worldTime % 4 >= 1 && (int)gameTime->worldTime % 4 < 2) {
					barrelUp(display, game, barrel);
					printf("%d\n", (int)game->barrelTimer );
				}
				else if ((int)gameTime->worldTime % 4 >= 2 && (int)gameTime->worldTime % 4 < 3) {
					barrelRight(display, game, barrel);
					printf("%d\n", (int)game->barrelTimer );
				}
				else if ((int)gameTime->worldTime % 4 >= 3 && (int)gameTime->worldTime % 4 < 4) {
					barrelDown(display, game, barrel);
					printf("%d\n", (int)game->barrelTimer );
				}
				else if ((int)gameTime->worldTime % 4 >= 0 && (int)gameTime->worldTime % 4 < 1) {
					printf("%d\n", (int)game->barrelTimer );
					barrelLeft(display, game, barrel);
				}
			}
			else {
				if ((int)gameTime->worldTime % 4 >= 1 && (int)gameTime->worldTime % 4 < 2) {
					barrelUp(display, game, barrel);
					printf("%d\n", (int)game->barrelTimer );
				}
				else if ((int)gameTime->worldTime % 4 >= 2 && (int)gameTime->worldTime % 4 < 3) {
					printf("%d\n", (int)game->barrelTimer );
					barrelLeft(display, game, barrel);
				}
				else if ((int)gameTime->worldTime % 4 >= 3 && (int)gameTime->worldTime % 4 < 4) {
					barrelDown(display, game, barrel);
					printf("%d\n", (int)game->barrelTimer );
				}
				else if ((int)gameTime->worldTime % 4 >= 0 && (int)gameTime->worldTime % 4 < 1) {
					barrelRight(display, game, barrel);
					printf("%d\n", (int)game->barrelTimer );
				}
			}
		}
	}
}

void render(Display* display, Game* game, GameTime* gameTime, char* text, char* points, double timeDelta) {
	DrawRectangle(display, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, BLACK_COLOR, BLACK_COLOR);
	renderPlatforms(display, game);
	renderLadders(display, game);
	renderEndingArea(display, game);
	renderBarrelThrower(display, game, gameTime);
	renderPlayerStartingPosition(display, game, gameTime);
	renderBarrels(display, game, gameTime);
	renderTrophies(display, game);
	sprintf(text, "WORLD TIME :%.1lf", gameTime->worldTime);
	drawString(display, SCREEN_WIDTH - 165, 20, text);
	drawString(display, SCREEN_WIDTH - 200, 30, "PODSTAWA + A,B,C,F,H,I");
	sprintf(points, "PUNKTY:%d", game->points);
	drawString(display, SCREEN_WIDTH / 2 - 10, 50, points);
}

int main(int argc, char **argv) {
	Display* display = initializeDisplay();
	GameTime* gameTime = initializeGameTime();
	int startingPoints = 0;
	Game* game = initializeGame(0, startingPoints);
	Controls* controls = initializeControls();
	bool quit = false;
	char text[128];
	char points[1000];
	
	
	while (!quit) {
		updateGameTime(gameTime);
		if (gameTime->update) {
			handleEvents(quit, controls);
			update(gameTime, game, controls, display, quit);
			render(display, game, gameTime, text, points, gameTime->delta);
			updateDisplay(display);
			changeLevels(game, controls, display, gameTime);
			restartGame(controls, game,gameTime);
			exitGame(controls, quit);
		}
	}

	
	freeGameTime(gameTime);
	freeDisplay(display);
	freeGame(game);
	freeControls(controls);
	return 0;
}
