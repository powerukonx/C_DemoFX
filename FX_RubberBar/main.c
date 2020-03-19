/* =============================================================================
                  ██╗   ██╗██╗  ██╗ ██████╗ ███╗   ██╗██╗  ██╗
                  ██║   ██║██║ ██╔╝██╔═══██╗████╗  ██║╚██╗██╔╝
                  ██║   ██║█████╔╝ ██║   ██║██╔██╗ ██║ ╚███╔╝
                  ██║   ██║██╔═██╗ ██║   ██║██║╚██╗██║ ██╔██╗
                  ╚██████╔╝██║  ██╗╚██████╔╝██║ ╚████║██╔╝ ██╗
                   ╚═════╝ ╚═╝  ╚═╝ ╚═════╝ ╚═╝  ╚═══╝╚═╝  ╚═╝
  File name:    main.c
  Date:         23 07 2017
  Author:       Power.
  Description:  Oldschool demoeffect - FX_RubberBar.
============================================================================= */

/* =============================================================================
                                 DEBUG Section
============================================================================= */


/* =============================================================================
                                 Include Files
============================================================================= */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
/* -User specific- */
#include<math.h>
/* -User specific- */

/* =============================================================================
                          Private defines and typedefs
============================================================================= */
/* FX specific. */
#define SDL_SUBSYSTEM_TO_INIT   SDL_INIT_VIDEO
#define FX_NAME                 "FX_RubberBar by UKONX"
#define SCREEN_WIDTH            ((uint16_t)800)
#define SCREEN_HIGH             ((uint16_t)600)
#define SCREEN_BPP              ((uint8_t)32)
#define YMAX                    ((uint16_t)(SCREEN_HIGH - (SCREEN_HIGH / 3)))
#define ALPHA_MASK              ((uint32_t)0xFF000000)
#define RED_MASK                ((uint32_t)0x00FF0000)
#define GREEN_MASK              ((uint32_t)0x0000FF00)
#define BLUE_MASK               ((uint32_t)0x000000FF)

// #define TRUE_FULLSCREEN
// #define DESKTOP_FULLSCREEN
#if defined(TRUE_FULLSCREEN)
  #define SDL_FLAG SDL_(WINDOW_FULLSCREEN | SDL_WINDOW_RESIZABLE)
#elif defined(DESKTOP_FULLSCREEN)
  #define SDL_FLAG (SDL_WINDOW_FULLSCREEN_DESKTOP | SDL_WINDOW_RESIZABLE)
#else
  #define SDL_FLAG (SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE)
#endif
/* FX specific. */

/* -User specific- */
#define TEXTURE_WIDTH           ((uint16_t)256)
#define TEXTURE_HEIGHT          ((uint16_t)256)
/* -User specific- */


/* =============================================================================
                        Private constants and variables
============================================================================= */
static SDL_Window   *g_pSDL_Window                              = NULL;
static SDL_Renderer *g_pSDL_Renderer                            = NULL;
static SDL_Surface  *g_pSDL_Screen                              = NULL;
static SDL_Texture  *g_pSDL_Texture                             = NULL;

/* -User specific- */
static SDL_Surface  *g_pSDL_FxTexture                           = NULL;
static const char   g_cachFxTexture[]                           = {"./texture/rubber_bar.png"};
/* -User specific- */


/* =============================================================================
                        Public constants and variables
============================================================================= */


/* =============================================================================
                        Private function declarations
============================================================================= */
static bool bSetup    (void);
static bool bLoop     (void);
static void vQuit     (void);
static bool bFxSetup  (void);
static void vFxQuit   (void);
static bool bFxLoop   (void);

/* -User specific- */
/* -User specific- */

/* =============================================================================
                               Public functions
============================================================================= */


/*==============================================================================
Function    : main
Describe    : Program entry point.
Parameters  : Don't care.
Returns     : -1 on error else 0.
==============================================================================*/
int main (int argc, char* argv[])
{
  /* Locals variables declaration. */
  bool l_bReturn = false;

  /* Program initialization. */
  l_bReturn = bSetup ();
  if (true == l_bReturn)
  {
    /* Program loop. */
    while (true == bLoop () );
  }

  /* Program de-initialization. */
  vQuit ();

  return (l_bReturn == true ? 0 : -1);
}


/* =============================================================================
                               Private functions
============================================================================= */

/*==============================================================================
Function    :   bSetup
Describe    :   Program setup.
Parameters  :   None.
Returns     :   false on error.
==============================================================================*/
static bool bSetup (void)
{
  /* Locals variables declaration. */
  bool l_bReturn = false;

  /* Initialize the SDL library. */
  if (0 == SDL_Init (SDL_SUBSYSTEM_TO_INIT) )
  {
    /* Initialize PNG loader/parser. */
    if (IMG_INIT_PNG == (IMG_INIT_PNG & IMG_Init (IMG_INIT_PNG) ) )
    {
      /* Create window. */
      g_pSDL_Window = SDL_CreateWindow (FX_NAME, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HIGH, SDL_FLAG);
      if (NULL != g_pSDL_Window)
      {
        /* Create renderer. */
        g_pSDL_Renderer = SDL_CreateRenderer (g_pSDL_Window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
        if (NULL != g_pSDL_Renderer)
        {
          /* Allocate a new RGB surface. */
          g_pSDL_Screen = SDL_CreateRGBSurface (0, SCREEN_WIDTH, SCREEN_HIGH, SCREEN_BPP, RED_MASK, GREEN_MASK, BLUE_MASK, ALPHA_MASK);
          if (NULL != g_pSDL_Screen)
          {
            /* Create a texture for a rendering context. */
            g_pSDL_Texture = SDL_CreateTexture (g_pSDL_Renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, SCREEN_WIDTH, SCREEN_HIGH);
            if (NULL != g_pSDL_Texture)
            {
              /* Fx setup. */
              l_bReturn = bFxSetup ();
            }
            else
            {
              printf ("SDL_CreateTexture failed: %s\n", SDL_GetError () );
            }
          }
          else
          {
            printf ("SDL_CreateRGBSurface failed: %s\n", SDL_GetError () );
          }
        }
        else
        {
          printf ("SDL_CreateWindow failed: %s\n", SDL_GetError () );
        }
      }
      else
      {
        printf ("SDL_CreateWindow failed: %s\n", SDL_GetError () );
      }
    }
    else
    {
      printf ("IMG_Init failed: %s\n", IMG_GetError () );
    }
  }
  else
  {
    printf ("SDL_Init failed: %s\n", SDL_GetError () );
  }

  return (l_bReturn);
}


/*==============================================================================
Function    :   bLoop
Describe    :   Program loop.
Parameters  :   None.
Returns     :   false to quit main loop or on error.
==============================================================================*/
static bool bLoop (void)
{
  /* Locals variables declaration. */
  SDL_Event l_sEvent;
  bool l_bReturn = true;

  /* SDL Event management */
  while(0 != SDL_PollEvent (&l_sEvent) )
  {
    switch (l_sEvent.type)
    {
      case SDL_KEYDOWN :
      {
        l_bReturn = false;
        break;
      }
      default:
      {
        break;
      }
    }
  }

  /* Fx still running ? */
  if (true == l_bReturn)
  {
    /* Fx loop. */
    l_bReturn = bFxLoop ();
    if (true == l_bReturn)
    {
      /* Update SDL Texture */
      if (0 == SDL_UpdateTexture (g_pSDL_Texture, NULL, g_pSDL_Screen->pixels, g_pSDL_Screen->w * sizeof (uint32_t) ) )
      {
        /* Render new texture*/
        if (0 == SDL_RenderCopy (g_pSDL_Renderer, g_pSDL_Texture, NULL, NULL) )
        {
          /* Select new render */
          SDL_RenderPresent (g_pSDL_Renderer);
        }
        else
        {
          l_bReturn = false;
          printf ("SDL_RenderCopy failed: %s\n", SDL_GetError () );
        }
      }
      else
      {
        l_bReturn = false;
        printf ("SDL_UpdateTexture failed: %s\n", SDL_GetError () );
      }
    }
  }

  return (l_bReturn);
}


/*==============================================================================
Function    :   vQuit
Describe    :   Program quit.
Parameters  :   None.
Returns     :   None.
==============================================================================*/
static void vQuit (void)
{
  /* Fx Quit. */
  vFxQuit ();

  /* Free RGB surfaces.*/
  if (NULL != g_pSDL_Screen)
  {
    SDL_FreeSurface (g_pSDL_Screen);
  }


  /* Destroy the rendering context and free associated textures. */
  if (NULL != g_pSDL_Renderer)
  {
    SDL_DestroyRenderer (g_pSDL_Renderer);
  }

  /* Destroy window.*/
  if (NULL != g_pSDL_Window)
  {
    SDL_DestroyWindow (g_pSDL_Window);
  }

  /* Destroy the texture. */
  if (NULL != g_pSDL_Texture)
  {
    SDL_DestroyTexture (g_pSDL_Texture);
  }

  /* Cleans up all dynamically loaded library handles, freeing memory. */
  IMG_Quit ();

  /* Clean up all initialized subsystems. */
  if (0UL != SDL_WasInit (SDL_INIT_EVERYTHING))
    SDL_Quit ();
}


/*==============================================================================
Function    :   bFxSetup
Describe    :   Fx setup.
Parameters  :   None.
Returns     :   false on error.
==============================================================================*/
static bool bFxSetup (void)
{
  /* Locals variables declaration. */
  bool l_bReturn = false;

  /* -User specific- */

  /* Load a surface from Image file. */
  g_pSDL_FxTexture  = IMG_Load (g_cachFxTexture);
  if (NULL != g_pSDL_FxTexture)
  {
    l_bReturn = true;
  }
  else
  {
    printf("IMG_Load failed: %s\n", IMG_GetError());
  }
  /* -User specific- */

  return (l_bReturn);
}


/*==============================================================================
Function    :   bFxQuit
Describe    :   Fx Quit.
Parameters  :   None.
Returns     :   None.
==============================================================================*/
static void vFxQuit (void)
{
  /* -User specific- */
  if (NULL != g_pSDL_FxTexture)
  {
    SDL_FreeSurface (g_pSDL_FxTexture);
  }
  /* -User specific- */
}


/*==============================================================================
Function    :   bFxLoop
Describe    :   Fx loop.
Parameters  :   None.
Returns     :   false on error.
==============================================================================*/
static bool bFxLoop (void)
{
  /* Locals variables declaration. */
  bool l_bReturn = false;

  /* -User specific- */
  static float l_fAngle1 = 0.0f;
  static float l_fAngle2 = 0.0f;

  uint16_t l_u16RubberXPos = (SCREEN_WIDTH>>1u) - (g_pSDL_FxTexture->w>>1u);

  /* Compute FX. */
  for (uint16_t l_u16PosY = 0; l_u16PosY < YMAX; l_u16PosY++)
  {
    for (uint16_t l_u16PosX = 0; l_u16PosX < g_pSDL_FxTexture->w; l_u16PosX++)
    {
      Uint8 *p_ecran = (Uint8 *)g_pSDL_Screen->pixels
                      + (l_u16PosY + SCREEN_HIGH/6) * g_pSDL_Screen->pitch
                      + (l_u16PosX + l_u16RubberXPos) * g_pSDL_Screen->format->BytesPerPixel;

      uint16_t l_u16OffY = (g_pSDL_FxTexture->h>>1u) + (g_pSDL_FxTexture->h>>1u) * sin(l_fAngle1 + l_u16PosY /600.0f)*cos(l_fAngle2);

      Uint8 *p_texture = (Uint8 *)g_pSDL_FxTexture->pixels
                        + (unsigned int)((l_u16OffY)  % g_pSDL_FxTexture->h) * g_pSDL_FxTexture->pitch
                        + ((unsigned int)(l_u16PosX) % g_pSDL_FxTexture->w) * g_pSDL_FxTexture->format->BytesPerPixel;

      if (255 != *(p_texture + 1) )
      {
        *p_ecran        = *(p_texture + 2);
        *(p_ecran + 1)  = *(p_texture + 1);
        *(p_ecran + 2)  = *(p_texture + 0);
      }
    }
  }

  l_fAngle1 += 0.02f;
  l_fAngle2 += 0.03f;

  l_bReturn = true;
  /* -User specific- */

  return (l_bReturn);
}


/* -User specific- */
/*==============================================================================
Function    :   todo
Describe    :   todo.
Parameters  :   todo.
Returns     :   todo.
==============================================================================*/
/* -User specific- */


