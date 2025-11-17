#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_surface.h>

static SDL_Window *window = NULL;
static SDL_Renderer *renderer = NULL;
static float rect_hole = 0.5;
static float bird = 0.5;
static float column = 1;
static float pause = 0;
static int game_over = 0;
static char background_path[] = "background.bmp";
static char bird_path[] = "Bird.bmp";
static char column_path[] = "column.bmp";
static SDL_Texture *background_texture = NULL;
static SDL_Texture *bird_texture = NULL;
static SDL_Texture *column_texture = NULL;

#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480
#define PIXELS_PER_SECOND 60
#define BIRD_LEFT_MARGIN 120
#define BIRD_WIDTH 80
#define BIRD_HEIGHT 30
#define COLUMN_WIDTH 60
#define HOLE_HEIGHT 60
#define MENURECT_LEFT_MARGIN 160
#define MENURECT_WIDTH 320
#define MENURECT_HEIGHT 60

static Uint64 last_time = 0;
static float rect_speed = 120;

SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[])
{
    SDL_SetAppMetadata("Example Renderer Rectangles", "1.0", "com.example.renderer-rectangles");

    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    if (!SDL_CreateWindowAndRenderer("examples/renderer/rectangles", WINDOW_WIDTH, WINDOW_HEIGHT, 0, &window, &renderer))
    {
        SDL_Log("Couldn't create window/renderer: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }
    SDL_Surface *surface = NULL;

    surface = SDL_LoadBMP(background_path);
    if (!surface)
    {
        SDL_Log("Couldn't load bitmap: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    background_texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (!background_texture)
    {
        SDL_Log("Couldn't create static texture: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    SDL_DestroySurface(surface);

    surface = SDL_LoadBMP(bird_path);
    if (!surface)
    {
        SDL_Log("Couldn't load bitmap: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    bird_texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (!bird_texture)
    {
        SDL_Log("Couldn't create static texture: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    SDL_DestroySurface(surface);

    surface = SDL_LoadBMP(column_path);
    if (!surface)
    {
        SDL_Log("Couldn't load bitmap: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    column_texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (!column_texture)
    {
        SDL_Log("Couldn't create static texture: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    SDL_DestroySurface(surface);

    return SDL_APP_CONTINUE;
}

void birdLimit()
{
    if (bird > 1)
    {
        game_over = 1;
    }
    else if (bird < 0)
    {
        bird = 0;
    }
}

static SDL_AppResult handle_key_event_(SDL_Scancode key_code)
{
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, SDL_ALPHA_OPAQUE);

    switch (key_code)
    {
    case SDL_SCANCODE_ESCAPE:
        pause = !pause;
        break;
    case SDL_SCANCODE_Q:
        return SDL_APP_SUCCESS;
    case SDL_SCANCODE_UP:
        bird -= 0.1;
        birdLimit();
        break;
    case SDL_SCANCODE_DOWN:
        bird += 0.1;
        birdLimit();
        break;
    default:
        break;
    }
    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event)
{
    if (event->type == SDL_EVENT_QUIT)
    {
        return SDL_APP_SUCCESS;
    }
    else if (event->type == SDL_EVENT_KEY_DOWN)
    {
        return handle_key_event_(event->key.scancode);
    }
    return SDL_APP_CONTINUE;
}

void drawBackground()
{
    SDL_FRect background;
    background.x = 0;
    background.y = 0;
    background.w = WINDOW_WIDTH;
    background.h = WINDOW_HEIGHT;
    SDL_RenderTexture(renderer, background_texture, NULL, &background);
}

float getColumnX() {
    return column * WINDOW_WIDTH;
}


float getHoleTopY() {
    return rect_hole * WINDOW_HEIGHT;
}

float getHoleBottomY() {
    return getHoleTopY() + HOLE_HEIGHT * 2;
}

void updateColumn(const float elapsed) {
    if (!pause)
    {
        column -= 0.18 * elapsed;
    }
    if (getColumnX() <= -60)
    {
        column = 1;
        rect_hole = SDL_randf();
    }
}

void drawColumn() {

    SDL_FRect column1;
    column1.x = getColumnX();
    column1.y = 0;
    column1.w = COLUMN_WIDTH;
    column1.h = getHoleTopY();
    SDL_RenderTexture(renderer, column_texture, NULL, &column1);

    SDL_FRect column2;
    column2.x = getColumnX();
    column2.y = getHoleBottomY();
    column2.w = COLUMN_WIDTH;
    column2.h = WINDOW_HEIGHT - getHoleBottomY();
    SDL_RenderTexture(renderer, column_texture, NULL, &column2);
}

SDL_AppResult SDL_AppIterate(void *appstate)
{
    
    SDL_FRect birdRect;
    const Uint64 now = SDL_GetTicks();

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer);

    const float elapsed = ((float)(now - last_time)) / 1000.0f;

    drawBackground();

    updateColumn(elapsed);
    drawColumn();

    //todo extract bird draw to separate function
    if (!pause)
    {
        bird += 0.1 * elapsed;
        birdLimit();
    }
    birdRect.x = BIRD_LEFT_MARGIN;
    birdRect.y = bird * WINDOW_HEIGHT;
    birdRect.w = BIRD_WIDTH;
    birdRect.h = BIRD_HEIGHT;

    if (birdRect.y < 0)
    {
        birdRect.y = 0;
    }
    float columnX = getColumnX();
    float holeBottomY = getHoleBottomY();
    if (birdRect.x + BIRD_WIDTH <= columnX + COLUMN_WIDTH && birdRect.x + BIRD_WIDTH >= columnX || birdRect.x <= columnX + COLUMN_WIDTH && birdRect.x >= columnX)
    {
        if (birdRect.y + BIRD_HEIGHT < rect_hole * WINDOW_HEIGHT + 120 && birdRect.y > rect_hole * WINDOW_HEIGHT)
        {
            game_over = 0;
        }
        else if (birdRect.x + BIRD_WIDTH <= columnX + COLUMN_WIDTH && birdRect.x + BIRD_WIDTH >= columnX || birdRect.x <= columnX + COLUMN_WIDTH && birdRect.x >= columnX)
        {
            if (birdRect.y + BIRD_HEIGHT >= holeBottomY + HOLE_HEIGHT || birdRect.y <= holeBottomY)
            {
                game_over = 1;
            }
        }
    }

    SDL_RenderTexture(renderer, bird_texture, NULL, &birdRect);

    //todo extract menu draw to separate function
    if (pause)
    {
        SDL_SetRenderDrawColor(renderer, 255, 130, 0, SDL_ALPHA_OPAQUE);
        SDL_FRect menuRect;
        for (int i = 0; i < 3; i++)
        {
            menuRect.x = MENURECT_LEFT_MARGIN;
            menuRect.y = 90 + 90 * i;
            menuRect.w = MENURECT_WIDTH;
            menuRect.h = MENURECT_HEIGHT;
            SDL_RenderFillRect(renderer, &menuRect);
        }
    }

    last_time = now;

    SDL_RenderPresent(renderer);
    if (game_over == 0)
    {
        return SDL_APP_CONTINUE;
    }
    else
    {
        return SDL_APP_SUCCESS;
    }
}

void SDL_AppQuit(void *appstate, SDL_AppResult result)
{
}
