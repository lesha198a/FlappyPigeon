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

        /*треба перемістити створення ректів у АппІтерейт
        for (int i = 0; i < 3; i++) {
            rect.x = 220;
            rect.y = 80 + 80 * i;
            rect.w = 200;
            rect.h = 40;
            SDL_RenderFillRect(renderer, &rect);
        }
        SDL_RenderPresent(renderer);*/
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

SDL_AppResult SDL_AppIterate(void *appstate)
{
    SDL_FRect rect;
    SDL_FRect rect1;
    SDL_FRect rect2;
    SDL_FRect rect3;
    SDL_FRect rect4;
    const Uint64 now = SDL_GetTicks();
    int i;

    const float direction = ((now % 2000) >= 1000) ? 1.0f : -1.0f;
    const float scale = ((float)(((int)(now % 1000)) - 500) / 500.0f) * direction;

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer);

    const float elapsed = ((float)(now - last_time)) / 1000.0f;

    rect.x = 0;
    rect.y = 0;
    rect.w = WINDOW_WIDTH;
    rect.h = WINDOW_HEIGHT;
    SDL_RenderTexture(renderer, background_texture, NULL, &rect);

    // ці 3 рядка роблять рухи на екрані => без них там "пауза"

    if (!pause)
    {
        column -= 0.18 * elapsed;
        bird += 0.1 * elapsed;
        birdLimit();
    }

    rect1.x = column * WINDOW_WIDTH;
    rect1.y = 0;
    rect1.w = 60;
    rect1.h = rect_hole * WINDOW_HEIGHT;
    SDL_RenderTexture(renderer, column_texture, NULL, &rect1);

    rect2.x = column * WINDOW_WIDTH;
    rect2.y = rect_hole * WINDOW_HEIGHT + 120;
    rect2.w = 60;
    rect2.h = WINDOW_HEIGHT - rect_hole * WINDOW_HEIGHT;
    SDL_RenderTexture(renderer, column_texture, NULL, &rect2);
    if (rect2.x <= -60)
    {
        column = 1;
        rect_hole = SDL_randf();
    }

    rect3.x = BIRD_LEFT_MARGIN;
    rect3.y = bird * WINDOW_HEIGHT;
    rect3.w = BIRD_WIDTH; 
    rect3.h = 30; // todo: change 30 to a constant like WINDOW_HEIGHT

    if (rect3.y < 0)
    {
        rect3.y = 0;
    }
    if (rect3.x + BIRD_WIDTH <= rect2.x + 60 && rect3.x + BIRD_WIDTH >= rect2.x || rect3.x <= rect2.x + 60 && rect3.x >= rect2.x)
    {
        if (rect3.y + 20 < rect_hole * WINDOW_HEIGHT + 110 && rect3.y > rect_hole * WINDOW_HEIGHT)
        {
            game_over = 0;
        }
        else if (rect3.x + BIRD_WIDTH <= rect2.x + 60 && rect3.x + BIRD_WIDTH >= rect2.x || rect3.x <= rect2.x + 60 && rect3.x >= rect2.x)
        {
            if (rect3.y + 20 >= rect2.y + 60 || rect3.y <= rect2.y)
            {
                game_over = 1;
            }
        }
    } 

    SDL_RenderTexture(renderer, bird_texture, NULL, &rect3);

    if (pause)
{
    SDL_SetRenderDrawColor(renderer, 255, 130, 0, SDL_ALPHA_OPAQUE);
    SDL_FRect rect;
    for (int i = 0; i < 3; i++) {
        rect.x = 160;
        rect.y = 90 + 90 * i;
        rect.w = 320;
        rect.h = 60;
        SDL_RenderFillRect(renderer, &rect);
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
