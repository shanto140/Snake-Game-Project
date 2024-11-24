#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>

#include <vector>
#include <cmath>
#include <ctime>
#include <cstdlib>
#include <string>

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;
const int RECTANGLE_SIZE = 20;
const int FOOD_SIZE = 20;
int SNAKE_SPEED = 170;

SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;
SDL_Surface *gScreenSurface = NULL;
TTF_Font *font = NULL;

// declaring texture
SDL_Texture *bcgroundTexture = NULL;
SDL_Texture *bonusTexture = NULL;
SDL_Texture *foodTexture = NULL;
SDL_Texture *tailTexture = NULL;
SDL_Texture *bodyTexture = NULL;
SDL_Texture *headTexture = NULL;
SDL_Texture *coverTexture = NULL;
SDL_Texture *playButtonTexture = NULL;
SDL_Texture *exitButtonTexture = NULL;
SDL_Texture *level1ButtonTexture = NULL;
SDL_Texture *level2ButtonTexture = NULL;
SDL_Texture *highScoreButtonTexture = NULL;
SDL_Texture *gameOverBcgroundTexture = NULL;
SDL_Texture *backHomeButtonTexture = NULL;
SDL_Texture *continueButtonTexture = NULL;
SDL_Texture *pauseButtonTexture = NULL;

// positionizing  of  texture
SDL_Rect playButtonRect = {SCREEN_WIDTH / 2 - 115, 290, 170, 35};
SDL_Rect highScoreButtonRect = {SCREEN_WIDTH / 2 - 115, 332, 170, 35};
SDL_Rect level1HighScoreRect = {SCREEN_WIDTH / 2 - 110, 370, 150, 25};
SDL_Rect level2HighScoreRect = {SCREEN_WIDTH / 2 - 110, 395, 150, 25};
SDL_Rect exitButtonRect = {SCREEN_WIDTH / 2 - 115, 380, 170, 35};
SDL_Rect level1ButtonRect = {SCREEN_WIDTH / 2 - 30, 340, 150, 35};
SDL_Rect level2ButtonRect = {SCREEN_WIDTH / 2, 390, 150, 35};
SDL_Rect backHomeButtonRect = {330, 300, 140, 40};
SDL_Rect pauseAndContinueButtonRect = {SCREEN_WIDTH / 2 - 20, SCREEN_HEIGHT - 25, 20, 20};

// obstacle and boundary rectangle for snake
SDL_Rect obstacle1Rect = {SCREEN_WIDTH / 3, SCREEN_HEIGHT / 4, SCREEN_WIDTH / 25, SCREEN_HEIGHT / 2};
SDL_Rect obstacle2Rect = {(SCREEN_WIDTH / 3) * 2, SCREEN_HEIGHT / 4, SCREEN_WIDTH / 25, SCREEN_HEIGHT / 2};
SDL_Rect obstacle3Rect = {SCREEN_WIDTH / 3, SCREEN_HEIGHT / 4 - SCREEN_WIDTH / 25, SCREEN_WIDTH / 6, SCREEN_WIDTH / 25};
SDL_Rect obstacle4Rect = {(SCREEN_WIDTH / 3) * 2, SCREEN_HEIGHT / 4 - SCREEN_WIDTH / 25, SCREEN_WIDTH / 6, SCREEN_WIDTH / 25};
SDL_Rect obstacle5Rect = {SCREEN_WIDTH / 3 - SCREEN_WIDTH / 6 + SCREEN_WIDTH / 25, (SCREEN_HEIGHT / 4) * 3, SCREEN_WIDTH / 6, SCREEN_WIDTH / 25};
SDL_Rect obstacle6Rect = {(SCREEN_WIDTH / 3) * 2 - SCREEN_WIDTH / 6 + SCREEN_WIDTH / 25, (SCREEN_HEIGHT / 4) * 3, SCREEN_WIDTH / 6, SCREEN_WIDTH / 25};

int boundaryWidth = 30;
SDL_Rect upperBoundaryRect = {0, 0, SCREEN_WIDTH, boundaryWidth};
SDL_Rect lowerBoundaryRect = {0, SCREEN_HEIGHT - boundaryWidth, SCREEN_WIDTH, boundaryWidth};
SDL_Rect leftBoundaryRect = {0, boundaryWidth, boundaryWidth, SCREEN_HEIGHT - boundaryWidth};
SDL_Rect rightBoundaryRect = {SCREEN_WIDTH - boundaryWidth, boundaryWidth, boundaryWidth, SCREEN_HEIGHT - boundaryWidth};

std::vector<SDL_Rect> obstacle = {upperBoundaryRect, lowerBoundaryRect, leftBoundaryRect, rightBoundaryRect, obstacle1Rect, obstacle2Rect, obstacle3Rect, obstacle4Rect, obstacle5Rect, obstacle6Rect};

// declaring mixer
Mix_Chunk *eatSound = NULL;
Mix_Chunk *bonusSound = NULL;
Mix_Music *menuMusic = NULL;
Mix_Music *gameOverMusic = NULL;
Mix_Music *bcgroundMusic = NULL;
Mix_Music *currentMusic = NULL;

// updating head cordinate by dx,dy
int dx = RECTANGLE_SIZE;
int dy = 0;

// Initialize score
int score = 0;
int highScoreForLevel1 = 0;
int highScoreForLevel2 = 0;
int foodEatenCount = 0;

bool ateFood = false;
bool bonusFoodActive = false;

// co-ordinate of bonus food and regular food
int bonusFoodX = 0, bonusFoodY = 0, foodX = 40, foodY = 40;

Uint32 bonusFoodStartTime = 0;
bool running = false;
bool isClickedHighScore = false;
bool level1 = false, level2 = false;
int Pause = 0;

enum GameState
{
    MENU,
    PLAYING,
    GAME_OVER
};
GameState state = MENU;

struct Snake
{
    int x, y;
    double angle;
};
std::vector<Snake> segment;

bool checkCollision(const Snake &snake, int x, int y)
{
    SDL_Rect snakeRect = {snake.x, snake.y, RECTANGLE_SIZE, RECTANGLE_SIZE};
    SDL_Rect foodRect = {x, y, FOOD_SIZE, FOOD_SIZE};
    return SDL_HasIntersection(&snakeRect, &foodRect);
}

bool checkCollisionWithObstacle(const Snake &snake)
{
    SDL_Rect headRect = {snake.x, snake.y, FOOD_SIZE, FOOD_SIZE};
    int i;
    if (level1)
        i = 5;
    else
        i = 9;
    for (; i >= 0; i--)
    {
        if (SDL_HasIntersection(&headRect, &obstacle[i]))
        {
            return true;
        }
    }
    return false;
}

bool checkSelfBite(const std::vector<Snake> &segment)
{
    for (size_t i = 1; i < segment.size(); ++i)
    {
        if (segment[0].x == segment[i].x && segment[0].y == segment[i].y)
        {
            return true;
        }
    }
    return false;
}

bool checkFoodOnSnake(const std::vector<Snake> &body, int x, int y)
{
    for (const auto &segment : body)
    {
        if (segment.x == x && segment.y == y)
        {
            return true;
        }
    }
    return false;
}

bool checkFoodOnObstacle(int foodX, int foodY)
{
    SDL_Rect foodRect = {foodX, foodY, FOOD_SIZE, FOOD_SIZE};
    int i;
    if (level1)
        i = 5;
    else
        i = 9;
    for (; i >= 0; i--)
    {
        if (SDL_HasIntersection(&foodRect, &obstacle[i]))
        {
            return true;
        }
    }
    return false;
}

void makeFood()
{
    do
    {
        foodX = (std::rand() % (SCREEN_WIDTH / FOOD_SIZE)) * FOOD_SIZE;
        foodY = (std::rand() % (SCREEN_HEIGHT / FOOD_SIZE)) * FOOD_SIZE;

    } while (checkFoodOnSnake(segment, foodX, foodY) || checkFoodOnObstacle(foodX, foodY));
}

void makeBonusFood()
{
    do
    {
        bonusFoodX = (std::rand() % (SCREEN_WIDTH / FOOD_SIZE)) * FOOD_SIZE;
        bonusFoodY = (std::rand() % (SCREEN_HEIGHT / FOOD_SIZE)) * FOOD_SIZE;

    } while (checkFoodOnSnake(segment, bonusFoodX, bonusFoodY) || checkFoodOnObstacle(bonusFoodX, bonusFoodY));
}

void snakeSegmentInitialize()
{
    segment.push_back({RECTANGLE_SIZE * 6, SCREEN_HEIGHT / 2, 0.0}); // Head
    segment.push_back({RECTANGLE_SIZE * 5, SCREEN_HEIGHT / 2, 0.0}); // Body
    segment.push_back({RECTANGLE_SIZE * 4, SCREEN_HEIGHT / 2, 0.0}); // Tail
}

void loadHighScore()
{
    FILE *file1 = fopen("level1Highscore.txt", "r");
    FILE *file2 = fopen("level2Highscore.txt", "r");
    if (file1)
    {
        fscanf(file1, "%d", &highScoreForLevel1);
        fclose(file1);
    }
    if (file2)
    {
        fscanf(file2, "%d", &highScoreForLevel2);
        fclose(file2);
    }
}

void updateHighScore()
{
    if (level1)
    {
        if (score > highScoreForLevel1)
        {
            highScoreForLevel1 = score;
            FILE *file = fopen("level1Highscore.txt", "w");
            if (file)
            {
                fprintf(file, "%d", highScoreForLevel1);
                fclose(file);
            }
        }
    }
    else if (level2)
    {
        if (score > highScoreForLevel2)
        {
            highScoreForLevel2 = score;
            FILE *file = fopen("level2Highscore.txt", "w");
            if (file)
            {
                fprintf(file, "%d", highScoreForLevel2);
                fclose(file);
            }
        }
    }
}

bool initializing()
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        std::cerr << "SDL_Init Error: " << SDL_GetError() << std::endl;
        return 0;
    }

    window = SDL_CreateWindow("Snake Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (window == nullptr)
    {
        std::cerr << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
        return 0;
    }

    else
    {
        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
        if (renderer == nullptr)
        {
            SDL_DestroyWindow(window);
            SDL_Quit();
            std::cerr << "SDL_CreateRenderer Error: " << SDL_GetError() << std::endl;
            return 0;
        }
    }

    if (TTF_Init() != 0)
    {
        std::cerr << "TTF_Init Error: " << TTF_GetError() << std::endl;

        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 0;
    }
    else
    {
        font = TTF_OpenFont("./textf/NexaRustHandmade-Trial-Extended.otf", 28); // Make sure to replace with the path to your font
        if (font == nullptr)
        {
            TTF_Quit();
            SDL_DestroyRenderer(renderer);
            SDL_DestroyWindow(window);
            SDL_Quit();
            std::cerr << "TTF_OpenFont Error: " << TTF_GetError() << std::endl;
            return 0;
        }
    }

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) == -1)
    {
        std::cerr << "Mix_OpenAudio Error: " << Mix_GetError() << std::endl;
        TTF_Quit();
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 0;
    }

    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG))
    {
        std::cerr << "IMG_Init Error: " << IMG_GetError() << std::endl;
        TTF_Quit();
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return false;
    }
    else
    {
        gScreenSurface = SDL_GetWindowSurface(window);
    }

    return 1;
}

void RenderText(SDL_Renderer *renderer, TTF_Font *font, const std::string &message, SDL_Rect &rect, SDL_Color color)
{
    SDL_Surface *surface = TTF_RenderText_Solid(font, message.c_str(), color);
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_RenderCopy(renderer, texture, nullptr, &rect);
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}

SDL_Surface *loadSurface(const std::string &path)
{
    SDL_Surface *optimizedSurface = NULL;
    SDL_Surface *surface = IMG_Load(path.c_str());
    if (surface == NULL)
    {
        printf("Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError());
    }

    else
    {
        // Convert surface to screen format

        optimizedSurface = SDL_ConvertSurface(surface, gScreenSurface->format, 0);
        if (optimizedSurface == NULL)
        {
            printf("Unable to optimize image %s! SDL Error: %s\n", path.c_str(), SDL_GetError());
        }

        // Get rid of old loaded surface
        SDL_FreeSurface(surface);
    }

    return optimizedSurface;
}

bool loadMedia()
{
    SDL_Surface *surface = loadSurface("./images/cover.png");
    coverTexture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    if (coverTexture == nullptr)
    {
        printf("Failed to load PNG image!\n");
        return false;
    }

    surface = loadSurface("./images/bground.png");
    bcgroundTexture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    if (bcgroundTexture == nullptr)
    {
        printf("Failed to load PNG image!\n");
        return false;
    }

    surface = loadSurface("./images/bonus_food.png");
    bonusTexture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    if (bonusTexture == nullptr)
    {
        printf("Failed to load PNG image!\n");
        return false;
    }

    surface = loadSurface("./images/tail.png");
    tailTexture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    if (tailTexture == nullptr)
    {
        printf("Failed to load PNG image!\n");
        return false;
    }

    surface = loadSurface("./images/body.png");
    bodyTexture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    if (bodyTexture == nullptr)
    {
        printf("Failed to load PNG image!\n");
        return false;
    }

    surface = loadSurface("./images/head.png");
    headTexture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    if (headTexture == nullptr)
    {
        printf("Failed to load PNG image!\n");
        return false;
    }

    surface = loadSurface("./images/food.png");
    foodTexture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    if (foodTexture == nullptr)
    {
        printf("Failed to load PNG image!\n");
        return false;
    }

    surface = loadSurface("./images/play.png");
    playButtonTexture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    if (playButtonTexture == nullptr)
    {
        printf("Failed to load PNG image!\n");
        return false;
    }

    surface = loadSurface("./images/exit.png");
    exitButtonTexture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    if (exitButtonTexture == nullptr)
    {
        printf("Failed to load PNG image!\n");
        return false;
    }

    surface = loadSurface("./images/level 1.png");
    level1ButtonTexture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    if (level1ButtonTexture == nullptr)
    {
        printf("Failed to load PNG image!\n");
        return false;
    }

    surface = loadSurface("./images/level 2.png");
    level2ButtonTexture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    if (level2ButtonTexture == nullptr)
    {
        printf("Failed to load PNG image!\n");
        return false;
    }

    surface = loadSurface("./images/highScore.png");
    highScoreButtonTexture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    if (highScoreButtonTexture == nullptr)
    {
        printf("Failed to load PNG image!\n");
        return false;
    }

    surface = loadSurface("./images/gameOverBc.png");
    gameOverBcgroundTexture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    if (gameOverBcgroundTexture == nullptr)
    {
        printf("Failed to load PNG image!\n");
        return false;
    }

    surface = loadSurface("./images/home.png");
    backHomeButtonTexture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    if (backHomeButtonTexture == nullptr)
    {
        printf("Failed to load PNG image!\n");
        return false;
    }

    surface = loadSurface("./images/pause.png");
    pauseButtonTexture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    if (pauseButtonTexture == nullptr)
    {
        printf("Failed to load PNG image!\n");
        return false;
    }

    surface = loadSurface("./images/continue.png");
    continueButtonTexture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    if (continueButtonTexture == nullptr)
    {
        printf("Failed to load PNG image!\n");
        return false;
    }

    return true;
}

bool loadMixer()
{

    bonusSound = Mix_LoadWAV("./music/bonus.wav");
    if (bonusSound == NULL)
    {
        printf("Failed to load bonus sound! SDL_mixer Error: %s\n", Mix_GetError());
        return false;
    }

    eatSound = Mix_LoadWAV("./music/eat.wav");
    if (eatSound == NULL)
    {
        printf("Failed to load eat sound! SDL_mixer Error: %s\n", Mix_GetError());
        return false;
    }

    menuMusic = Mix_LoadMUS("./music/menu.mp3");
    if (menuMusic == NULL)
    {
        printf("Failed to load start music! SDL_mixer Error: %s\n", Mix_GetError());
        return false;
    }

    gameOverMusic = Mix_LoadMUS("./music/over.mp3");
    if (gameOverMusic == NULL)
    {
        printf("Failed to load game over music! SDL_mixer Error: %s\n", Mix_GetError());
        return false;
    }

    bcgroundMusic = Mix_LoadMUS("./music/bcground.mp3");
    if (bcgroundMusic == NULL)
    {
        printf("Failed to load background music! SDL_mixer Error: %s\n", Mix_GetError());
        return false;
    }
    return true;
}

void close()
{
    SDL_DestroyTexture(bcgroundTexture);
    SDL_DestroyTexture(bonusTexture);
    SDL_DestroyTexture(foodTexture);
    SDL_DestroyTexture(tailTexture);
    SDL_DestroyTexture(bodyTexture);
    SDL_DestroyTexture(headTexture);
    SDL_DestroyTexture(coverTexture);

    Mix_FreeChunk(eatSound);
    Mix_FreeChunk(bonusSound);

    Mix_FreeMusic(menuMusic);
    Mix_FreeMusic(gameOverMusic);
    Mix_FreeMusic(bcgroundMusic);

    TTF_CloseFont(font);

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    TTF_Quit();
    IMG_Quit();
    Mix_Quit();
    SDL_Quit();
}

void eventLoop()
{

    SDL_Event e;

    while (SDL_PollEvent(&e))
    {
        if (e.type == SDL_QUIT)
        {
            running = false;
        }

        else if (e.type == SDL_MOUSEBUTTONDOWN)
        {
            int x, y;
            SDL_GetMouseState(&x, &y);
            if (state == MENU)
            {
                if (x >= playButtonRect.x && x <= playButtonRect.x + playButtonRect.w && y >= playButtonRect.y && y <= playButtonRect.y + playButtonRect.h)
                {
                    state = PLAYING;
                }
                else if (x >= exitButtonRect.x && x <= exitButtonRect.x + exitButtonRect.w && y >= exitButtonRect.y && y <= exitButtonRect.y + exitButtonRect.h)
                {
                    running = false;
                }
                else if (x >= highScoreButtonRect.x && x <= highScoreButtonRect.x + highScoreButtonRect.w && y >= highScoreButtonRect.y && y <= highScoreButtonRect.y + highScoreButtonRect.h)
                {
                    if (isClickedHighScore)
                        isClickedHighScore = false;
                    else
                        isClickedHighScore = true;
                }
            }
            else if (state == GAME_OVER)
            {
                if (x >= backHomeButtonRect.x && x <= backHomeButtonRect.x + backHomeButtonRect.w && y >= backHomeButtonRect.y && y <= backHomeButtonRect.y + backHomeButtonRect.h)
                {
                    state = MENU;
                    score = 0;
                }
            }
            else if (state == PLAYING)
            {

                if (!level1 && !level2)
                {
                    if (x >= level1ButtonRect.x && x <= level1ButtonRect.x + level1ButtonRect.w && y >= level1ButtonRect.y && y <= level1ButtonRect.y + level1ButtonRect.h)
                    {
                        level1 = true;
                    }
                    else if (x >= level2ButtonRect.x && x <= level2ButtonRect.x + level2ButtonRect.w && y >= level2ButtonRect.y && y <= level2ButtonRect.y + level2ButtonRect.h)
                    {
                        level2 = true;
                    }
                }
                else if (x >= pauseAndContinueButtonRect.x && x <= pauseAndContinueButtonRect.x + pauseAndContinueButtonRect.w && y >= pauseAndContinueButtonRect.y && y <= pauseAndContinueButtonRect.y + pauseAndContinueButtonRect.h)
                {
                    Pause ^= 1;
                }
            }
        }
        else if (e.type == SDL_KEYDOWN)
        {
            if (state == MENU)
            {
                if (e.key.keysym.sym == SDLK_RETURN)
                {
                    state = PLAYING;
                }
                else if (e.key.keysym.sym == SDLK_ESCAPE)
                {
                    running = false;
                }
            }

            else if (state == PLAYING)
            {

                switch (e.key.keysym.sym)
                {
                case SDLK_UP:
                    if (dy != RECTANGLE_SIZE)
                    {
                        dx = 0;
                        dy = -RECTANGLE_SIZE;
                    }
                    break;
                case SDLK_DOWN:
                    if (dy != -RECTANGLE_SIZE)
                    {
                        dx = 0;
                        dy = RECTANGLE_SIZE;
                    }
                    break;
                case SDLK_LEFT:
                    if (dx != RECTANGLE_SIZE)
                    {
                        dx = -RECTANGLE_SIZE;
                        dy = 0;
                    }
                    break;
                case SDLK_RIGHT:
                    if (dx != -RECTANGLE_SIZE)
                    {
                        dx = RECTANGLE_SIZE;
                        dy = 0;
                    }
                    break;
                }
            }
        }
    }
}

void render()
{
    if (state == MENU)
    {
        if (currentMusic != menuMusic)
        {
            currentMusic = menuMusic;
            Mix_PlayMusic(currentMusic, -1);
        }
        SDL_RenderCopy(renderer, coverTexture, nullptr, nullptr);

        SDL_RenderCopy(renderer, playButtonTexture, nullptr, &playButtonRect);
        if (!isClickedHighScore)
        {

            SDL_RenderCopy(renderer, highScoreButtonTexture, nullptr, &highScoreButtonRect);
            SDL_RenderCopy(renderer, exitButtonTexture, nullptr, &exitButtonRect);
        }
        else
        {

            SDL_RenderCopy(renderer, highScoreButtonTexture, nullptr, &highScoreButtonRect);

            SDL_Color blue = {0, 0, 255, 255};
            std::string highscore1 = "Level-1 : " + std::to_string(highScoreForLevel1);
            RenderText(renderer, font, highscore1, level1HighScoreRect, blue);

            std::string highscore2 = "Level-2 : " + std::to_string(highScoreForLevel2);
            RenderText(renderer, font, highscore2, level2HighScoreRect, blue);

            SDL_Rect rect = {SCREEN_WIDTH / 2 - 115, 420, 170, 35};
            SDL_RenderCopy(renderer, exitButtonTexture, nullptr, &rect);
        }
    }

    if (state == PLAYING)
    {
        SDL_RenderCopy(renderer, bcgroundTexture, nullptr, nullptr);

        if (!level1 && !level2)
        {

            if (Mix_PlayingMusic())
            {
                Mix_PauseMusic();
            }
            SDL_RenderCopy(renderer, playButtonTexture, nullptr, &playButtonRect);
            SDL_RenderCopy(renderer, level1ButtonTexture, nullptr, &level1ButtonRect);
            SDL_RenderCopy(renderer, level2ButtonTexture, nullptr, &level2ButtonRect);

            SDL_RenderPresent(renderer);
        }

        else if (level1 == true || level2 == true)
        {

            if (currentMusic != bcgroundMusic || Mix_PausedMusic() != 0)
            {
                currentMusic = bcgroundMusic;
                Mix_PlayMusic(currentMusic, -1);
            }

            SDL_SetRenderDrawColor(renderer, 100, 100, 225, 0);

            // Render the abstacle and boundary rectangle
            if (level1)
            {
                for (int i = 0; i < 6; i++)
                {
                    SDL_RenderFillRect(renderer, &obstacle[i]);
                }
            }

            else if (level2)
            {
                for (int i = 0; i < 10; i++)
                {
                    SDL_RenderFillRect(renderer, &obstacle[i]);
                }
            }
            SDL_RenderPresent(renderer);

            if (!Pause)
            {
                SDL_RenderCopy(renderer, continueButtonTexture, nullptr, &pauseAndContinueButtonRect);

                int nextX = segment[0].x + dx;
                int nextY = segment[0].y + dy;

                // Check if snake eats regular food
                if (checkCollision(segment[0], foodX, foodY))
                {
                    Mix_PlayChannel(-1, eatSound, 0); // Play eat sound
                    ateFood = true;
                    makeFood();

                    score += 10;
                    foodEatenCount++;

                    // Activate bonus food after eating 10 regular food
                    if (foodEatenCount % 10 == 0)
                    {
                        bonusFoodActive = true;
                        Mix_PlayChannel(1, bonusSound, 0);
                        makeBonusFood();
                        bonusFoodStartTime = SDL_GetTicks();
                    }
                }

                // Check if snake eats bonus food
                if (bonusFoodActive && checkCollision(segment[0], bonusFoodX, bonusFoodY))
                {
                    Mix_PlayChannel(-1, eatSound, 0);
                    score += 50;
                    bonusFoodActive = false;
                }

                // Deactivate bonus food after 3 seconds
                if (bonusFoodActive && SDL_GetTicks() - bonusFoodStartTime > 5000)
                {
                    bonusFoodActive = false;
                }

                // Update snake body
                if (!ateFood)
                {
                    segment.pop_back();
                }
                else
                {
                    ateFood = false; // Add a new segment to the snake's tail
                }
                segment.insert(segment.begin(), {nextX, nextY, atan2(dy, dx) * 180 / M_PI});
            }

            if (Pause)
            {
                if (Mix_PlayingMusic())
                {
                    Mix_PauseMusic();
                }
                SDL_RenderCopy(renderer, pauseButtonTexture, nullptr, &pauseAndContinueButtonRect);
            }

            // Draw food using the food texture
            SDL_Rect foodRect = {foodX, foodY, FOOD_SIZE, FOOD_SIZE};
            SDL_RenderCopy(renderer, foodTexture, NULL, &foodRect);

            // Draw bonus food using the bonus food texture
            if (bonusFoodActive)
            {
                SDL_Rect bonusFoodRect = {bonusFoodX, bonusFoodY, FOOD_SIZE, FOOD_SIZE};
                SDL_RenderCopy(renderer, bonusTexture, NULL, &bonusFoodRect);
            }

            // Draw snake body,tail and head segments
            for (size_t i = 0; i < segment.size(); ++i)
            {
                SDL_Rect Rect = {segment[i].x, segment[i].y, RECTANGLE_SIZE, RECTANGLE_SIZE};
                if (i == 0)
                    SDL_RenderCopyEx(renderer, headTexture, NULL, &Rect, segment[i].angle, NULL, SDL_FLIP_NONE);
                else if (i == segment.size() - 1)
                    SDL_RenderCopyEx(renderer, tailTexture, NULL, &Rect, segment[i].angle, NULL, SDL_FLIP_NONE);
                else
                    SDL_RenderCopyEx(renderer, bodyTexture, NULL, &Rect, segment[i].angle, NULL, SDL_FLIP_NONE);
            }

            // Check if snake bites itself or collision Obstacle
            if (checkSelfBite(segment) || checkCollisionWithObstacle(segment[0]))
            {
                state = GAME_OVER;
                segment.clear();
                snakeSegmentInitialize();
                SNAKE_SPEED = 170;
                foodEatenCount = 0;
                level1 = false;
                level2 = false;
            }

            // Render the score
            SDL_Color Color = {150, 220, 102, 255};
            SDL_Rect Rect = {360, 2, 80, 30};
            std::string scoreText = "SCORE: " + std::to_string(score);
            RenderText(renderer, font, scoreText, Rect, Color);

            updateHighScore();
        }
    }

    if (state == GAME_OVER)
    {

        if (currentMusic != gameOverMusic)
        {
            currentMusic = gameOverMusic;
            Mix_PlayMusic(currentMusic, -1);
        }

        SDL_RenderCopy(renderer, gameOverBcgroundTexture, nullptr, nullptr);

        // text rendering in game over window
        SDL_Color textColor = {255, 0, 0, 255}; // Red color for text

        SDL_Rect Rect = {320, 250, 150, 40};
        std::string finalScoreText = "FINAL SCORE : " + std::to_string(score);
        RenderText(renderer, font, finalScoreText, Rect, textColor);

        SDL_RenderCopy(renderer, backHomeButtonTexture, nullptr, &backHomeButtonRect);
    }

    SDL_RenderPresent(renderer);
    SDL_Delay(SNAKE_SPEED);
}

int main(int argc, char *argv[])
{

    if (initializing() && loadMedia() && loadMixer())
    {
        running = true;
    }
    else
        return 0;
    loadHighScore();
    Mix_PlayMusic(currentMusic, -1);
    snakeSegmentInitialize();
    std::srand(std::time(nullptr));

    while (running)
    {
        eventLoop();

        if (SNAKE_SPEED > 60 && score % 50 == 0 && score != 0)
            SNAKE_SPEED -= 1;

        render();
        SDL_Delay(10);
    }
    close();
    return 0;
}
