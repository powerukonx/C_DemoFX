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
  Description:  Oldschool demoeffect - FX Metaball.
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
#include <string.h>
#include <math.h>
#include <time.h>
/* -User specific- */

/* =============================================================================
                          Private defines and typedefs
============================================================================= */
/* FX specific. */
#define SDL_SUBSYSTEM_TO_INIT   SDL_INIT_VIDEO
#define FX_NAME                 "FX_Plasma by UKONX"
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
#define UINT8_MAX_VALUE   ((uint8_t)255)
#define MAKE_RGB(r,g,b)   ( ( ((uint8_t)(r))<<16u) | (((uint8_t)(g))<<8) | ((uint8_t)(b) ) )
#define GET_RED(rgb)      ((uint8_t)(rgb>>16u))
#define GET_GREEN(rgb)    ((uint8_t)(rgb>>8u))
#define GET_BLUE(rgb)     ((uint8_t)(rgb))
#define MASK_16BITS       (0xFFFF)
#define BIT0              (1<<0)
#define BIT1              (1<<1)
#define BIT2              (1<<2)
#define RANDOM_LINE_SIZE  ((uint8_t)(SCREEN_HIGH/48))
#define min(a)            ((a) > 255 ? 255 : a)
#define TABLE_SIZE        ((uint16_t)256)
/* -User specific- */


/* =============================================================================
                        Private constants and variables
============================================================================= */
static SDL_Window   *g_pSDL_Window                              = NULL;
static SDL_Renderer *g_pSDL_Renderer                            = NULL;
static SDL_Surface  *g_pSDL_Image                               = NULL;
static SDL_Surface  *g_pSDL_Screen                              = NULL;
static SDL_Texture  *g_pSDL_Texture                             = NULL;

/* -User specific- */
static uint32_t     *g_pau32OldScreen                           = NULL;
static uint32_t     *g_pau32NewScreen                           = NULL;
static uint32_t     *g_pau32FrameBuffer                         = NULL;
static uint32_t     *g_pau32Mini                                = NULL;
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
static void vMakeParticle       (uint16_t p_u16PosX, uint16_t p_u16PosY);
static void vInitializeFx       (void);
static void vMakeWater2D        (void);
static void vMakeAndDisplayMini (void);
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
      case SDL_KEYDOWN:
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

  if (NULL != g_pSDL_Image)
  {
    SDL_FreeSurface (g_pSDL_Image);
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

  /* Initialize random seed. */
  srand ( time(NULL) );

  /* -User specific- */
  g_pau32FrameBuffer = (uint32_t *)malloc (sizeof (uint32_t) * YMAX * SCREEN_WIDTH);
  if (NULL != g_pau32FrameBuffer)
  {
    g_pau32OldScreen = (uint32_t *)malloc (sizeof (uint32_t) * YMAX * SCREEN_WIDTH);
    if (NULL != g_pau32OldScreen)
    {
      g_pau32NewScreen = (uint32_t *)malloc (sizeof (uint32_t) * YMAX * SCREEN_WIDTH);
      if (NULL != g_pau32NewScreen)
      {
        g_pau32Mini = (uint32_t *)malloc (sizeof (uint32_t) * YMAX * SCREEN_WIDTH);
        if (NULL != g_pau32Mini)
        {
          vInitializeFx ();
          l_bReturn = true;
        }
      }
    }
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
  if (NULL != g_pau32FrameBuffer)
  {
    free (g_pau32FrameBuffer);
  }
  if (NULL != g_pau32OldScreen)
  {
    free (g_pau32OldScreen);
  }
  if (NULL != g_pau32NewScreen)
  {
    free (g_pau32NewScreen);
  }
  if (NULL != g_pau32Mini)
  {
    free (g_pau32Mini);
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
  memset (g_pau32FrameBuffer, 0u, SCREEN_WIDTH * YMAX * sizeof (uint32_t) );
  vMakeWater2D ();
  vMakeAndDisplayMini ();

  /* Display FX. */
  for (uint16_t l_u16IndexY = 0u; l_u16IndexY < YMAX; l_u16IndexY++)
  {
    for (uint16_t l_u16IndexX = 0u;l_u16IndexX < SCREEN_WIDTH; l_u16IndexX++)
    {
      Uint32 *p_ecran =  g_pSDL_Screen->pixels
                       + ( (l_u16IndexY + (SCREEN_HIGH / 6) ) * SCREEN_WIDTH * g_pSDL_Screen->format->BytesPerPixel)
                       + (l_u16IndexX * g_pSDL_Screen->format->BytesPerPixel);
      *p_ecran = g_pau32FrameBuffer[(l_u16IndexY*SCREEN_WIDTH)+l_u16IndexX];
    }
  }

  l_bReturn = true;
  /* -User specific- */

  return (l_bReturn);
}


/* -User specific- */
/*==============================================================================
Function    :   vMakeParticle
Describe    :   Create particle.
Parameters  :   p_u16PosX, p_u16PosY = Position to create.
Returns     :   None.
==============================================================================*/
void vMakeParticle (uint16_t p_u16PosX, uint16_t p_u16PosY)
{
  for (uint16_t l_u16IndexY = 0; l_u16IndexY < 3; l_u16IndexY++)
  {
    uint16_t l_u16SprY = (l_u16IndexY + p_u16PosY) % YMAX;

    for (uint16_t l_u16IndexX = 0; l_u16IndexX < 3; l_u16IndexX++)
    {
       uint16_t l_u16SprX = (l_u16IndexX + p_u16PosX) % SCREEN_WIDTH;

       g_pau32OldScreen[(l_u16SprY * SCREEN_WIDTH) + l_u16SprX] = 255;
    }
  }

}


/*==============================================================================
Function    :   vInitializeFx
Describe    :   Create LUT and initialize FX.
Parameters  :   None.
Returns     :   None.
==============================================================================*/
static void vInitializeFx (void)
{
  memset (g_pau32OldScreen, 0u, SCREEN_WIDTH * YMAX * sizeof (uint32_t) );
  memset (g_pau32NewScreen, 0u, SCREEN_WIDTH * YMAX * sizeof (uint32_t) );
}


/*==============================================================================
Function    :   vMakeWater2D.
Describe    :   Make FX.
Parameters  :   None.
Returns     :   None.
==============================================================================*/
static void vMakeWater2D (void)
{
  /* Locals variables declaration. */
         float l_fDumping = 1.0052f;
  static float l_fAngle1  = 0.0f;
  static float l_fAngle2  = 0.0f;
  static float l_fAngle3  = 0.0f;
  static float l_fAngle4  = 0.0f;

  vMakeParticle (SCREEN_WIDTH/2 + SCREEN_WIDTH/3  * sin(l_fAngle1), YMAX/2 + YMAX/3 * cos(l_fAngle4));
  vMakeParticle (SCREEN_WIDTH/2 + SCREEN_WIDTH/3  * sin(l_fAngle2), YMAX/2 + YMAX/3 * sin(l_fAngle1));
  vMakeParticle (SCREEN_WIDTH/2 + SCREEN_WIDTH/3  * cos(l_fAngle3), YMAX/2 + YMAX/3 * sin(l_fAngle2));
  vMakeParticle (SCREEN_WIDTH/2 + SCREEN_WIDTH/3  * cos(l_fAngle4), YMAX/2 + YMAX/3 * cos(l_fAngle3));
  l_fAngle1 += 0.04f;
  l_fAngle2 += 0.03f;
  l_fAngle3 += 0.024f;
  l_fAngle4 += 0.07f;

  for (uint16_t l_u16IndexY = 1u; l_u16IndexY < (YMAX - 1u); l_u16IndexY++)
  {
    for (uint16_t l_u16IndexX = 1u; l_u16IndexX < (SCREEN_WIDTH - 1u); l_u16IndexX++)
    {
      int16_t l_s16Pixel = (
          g_pau32OldScreen[(l_u16IndexY * SCREEN_WIDTH) + l_u16IndexX - 1u]
        + g_pau32OldScreen[(l_u16IndexY * SCREEN_WIDTH) + l_u16IndexX + 1u]
        + g_pau32OldScreen[((l_u16IndexY - 1u) * SCREEN_WIDTH) + l_u16IndexX]
        + g_pau32OldScreen[((l_u16IndexY + 1u) * SCREEN_WIDTH) + l_u16IndexX]
      ) / 2 - g_pau32NewScreen[(l_u16IndexY * SCREEN_WIDTH) + l_u16IndexX];

      if (0 > l_s16Pixel )
        l_s16Pixel = 0;

      g_pau32NewScreen[(l_u16IndexY * SCREEN_WIDTH) + l_u16IndexX] = l_s16Pixel;
      g_pau32NewScreen[(l_u16IndexY * SCREEN_WIDTH) + l_u16IndexX] *= l_fDumping;

      if (255 < g_pau32NewScreen[(l_u16IndexY * SCREEN_WIDTH) + l_u16IndexX] )
        g_pau32NewScreen[(l_u16IndexY * SCREEN_WIDTH) + l_u16IndexX] = 255;

      if (32 > g_pau32NewScreen[(l_u16IndexY*SCREEN_WIDTH)+l_u16IndexX])
        g_pau32FrameBuffer[(l_u16IndexY*SCREEN_WIDTH)+l_u16IndexX] = 32;
      else
        g_pau32FrameBuffer[(l_u16IndexY*SCREEN_WIDTH)+l_u16IndexX] = g_pau32NewScreen[(l_u16IndexY*SCREEN_WIDTH)+l_u16IndexX];
    }
  }

  /* Swap buffer. */
  uint32_t *temp   = g_pau32OldScreen;
  g_pau32OldScreen = g_pau32NewScreen;
  g_pau32NewScreen = temp;
}


/*==============================================================================
Function    :   vMakeAndDisplayMini.
Describe    :   Display 3 miniatures with scroll down.
Parameters  :   None.
Returns     :   None.
==============================================================================*/
static void vMakeAndDisplayMini (void)
{
  /* Locals variables declaration. */
  static uint32_t l_u32ScrollYValue = 0;

  /* Create miniature. */
  for (uint16_t l_u16IndexY = 0u; l_u16IndexY < (YMAX>>2u); l_u16IndexY++)
  {
    for (uint16_t l_u16IndexX = 0u; l_u16IndexX < (SCREEN_WIDTH>>2u); l_u16IndexX++)
    {
      g_pau32Mini[(l_u16IndexY * (SCREEN_WIDTH>>2u) ) + l_u16IndexX] = g_pau32FrameBuffer[ (  (l_u16IndexY * g_pSDL_Screen->format->BytesPerPixel) * SCREEN_WIDTH)
                                                                    + (l_u16IndexX * g_pSDL_Screen->format->BytesPerPixel)];
    }
  }

  /* Display miniatures. */
  for (uint16_t l_u16IndexY = 0u; l_u16IndexY < (YMAX>>2u); l_u16IndexY++)
  {
    for (uint16_t l_u16IndexX = 0u; l_u16IndexX < (SCREEN_WIDTH>>2u); l_u16IndexX++)
    {
      /* Video inversion. */
      uint32_t l_u32RGB     = g_pau32Mini[l_u16IndexY * (SCREEN_WIDTH>>2u) + l_u16IndexX];
      uint8_t l_u8SrcRed    = UINT8_MAX_VALUE - GET_RED(l_u32RGB);
      uint8_t l_u8SrcGreen  = UINT8_MAX_VALUE - GET_GREEN(l_u32RGB);
      uint8_t l_u8SrcBlue   = UINT8_MAX_VALUE - GET_BLUE(l_u32RGB);

      /* First miniature. */
      uint16_t l_u16MiniPosY  = (l_u16IndexY + l_u32ScrollYValue)%YMAX;
      l_u32RGB                = g_pau32FrameBuffer[(l_u16MiniPosY*SCREEN_WIDTH)+(l_u16IndexX+8)];
      uint8_t l_u8DstRed      = min(l_u8SrcRed    + 3 * GET_RED(l_u32RGB)   )>>2u;
      uint8_t l_u8DstGreen    = min(l_u8SrcGreen  + 3 * GET_GREEN(l_u32RGB) )>>2u;
      uint8_t l_u8DstBlue     = min(l_u8SrcBlue   + 3 * GET_BLUE(l_u32RGB)  )>>2u;
      g_pau32FrameBuffer[(l_u16MiniPosY*SCREEN_WIDTH)+(l_u16IndexX+8)] = MAKE_RGB(l_u8DstRed, l_u8DstGreen, l_u8DstBlue);

      /* Second miniature. */
      l_u16MiniPosY = (l_u16MiniPosY + (YMAX/3) ) % YMAX;
      l_u32RGB      = g_pau32FrameBuffer[(l_u16MiniPosY * SCREEN_WIDTH) + (l_u16IndexX + 8)];
      l_u8DstRed    = min(l_u8SrcRed    + GET_RED(l_u32RGB)   )>>1u;
      l_u8DstGreen  = min(l_u8SrcGreen  + GET_GREEN(l_u32RGB) )>>1u;
      l_u8DstBlue   = min(l_u8SrcBlue   + GET_BLUE(l_u32RGB)  )>>1u;
      g_pau32FrameBuffer[(l_u16MiniPosY*SCREEN_WIDTH)+(l_u16IndexX+8)] = MAKE_RGB(l_u8DstRed, l_u8DstGreen, l_u8DstBlue);

      /* Third miniature. */
      l_u16MiniPosY = (l_u16MiniPosY + (YMAX/3) ) % YMAX;
      l_u32RGB      = g_pau32FrameBuffer[(l_u16MiniPosY * SCREEN_WIDTH) + (l_u16IndexX + 8)];
      l_u8DstRed    = min(3 * l_u8SrcRed    + GET_RED(l_u32RGB)   )>>2u;
      l_u8DstGreen  = min(3 * l_u8SrcGreen  + GET_GREEN(l_u32RGB) )>>2u;
      l_u8DstBlue   = min(3 * l_u8SrcBlue   + GET_BLUE(l_u32RGB)  )>>2u;
      g_pau32FrameBuffer[(l_u16MiniPosY*SCREEN_WIDTH)+(l_u16IndexX+8)] = MAKE_RGB(l_u8DstRed, l_u8DstGreen, l_u8DstBlue);
    }
  }

  /* Display upper and lower border. */
  for (uint16_t l_u16IndexY = 0u; l_u16IndexY < (YMAX>>2u); l_u16IndexY++)
  {
    uint32_t l_u32RGB = MAKE_RGB(255,255,255);

    uint32_t l_u16BorderPosY = (l_u16IndexY + l_u32ScrollYValue) % YMAX;
    g_pau32FrameBuffer[(l_u16BorderPosY * SCREEN_WIDTH) + 8] = l_u32RGB;
    g_pau32FrameBuffer[(l_u16BorderPosY * SCREEN_WIDTH) + 8 + (SCREEN_WIDTH>>2u)]=l_u32RGB;

    l_u16BorderPosY = (l_u16BorderPosY + (YMAX/3))%YMAX;
    g_pau32FrameBuffer[(l_u16BorderPosY * SCREEN_WIDTH) + 8] = l_u32RGB;
    g_pau32FrameBuffer[(l_u16BorderPosY * SCREEN_WIDTH) + 8 + (SCREEN_WIDTH>>2u)]=l_u32RGB;

    l_u16BorderPosY = (l_u16BorderPosY + (YMAX/3))%YMAX;
    g_pau32FrameBuffer[(l_u16BorderPosY * SCREEN_WIDTH) + 8] = l_u32RGB;
    g_pau32FrameBuffer[(l_u16BorderPosY * SCREEN_WIDTH) + 8 + (SCREEN_WIDTH>>2u)]=l_u32RGB;
  }

  /* Display left and right border. */
  for (uint16_t l_u16IndexX=8;l_u16IndexX<(8+(SCREEN_WIDTH>>2u));l_u16IndexX++)
  {
    uint32_t l_u32RGB         = MAKE_RGB(255,255,255);

    uint32_t l_u16BorderPosY  = l_u32ScrollYValue%YMAX;
    g_pau32FrameBuffer[(l_u16BorderPosY * SCREEN_WIDTH) + l_u16IndexX] = l_u32RGB;
    g_pau32FrameBuffer[( ( (l_u16BorderPosY + (YMAX>>2u) )%YMAX) * SCREEN_WIDTH) + l_u16IndexX] = l_u32RGB;

    l_u16BorderPosY = (l_u16BorderPosY + (YMAX/3) )%YMAX;
    g_pau32FrameBuffer[(l_u16BorderPosY*SCREEN_WIDTH)+l_u16IndexX] = l_u32RGB;
    g_pau32FrameBuffer[( ( (l_u16BorderPosY+(YMAX>>2u) )%YMAX) * SCREEN_WIDTH) + l_u16IndexX] = l_u32RGB;

    l_u16BorderPosY = (l_u16BorderPosY + (YMAX/3) )%YMAX;
    g_pau32FrameBuffer[(l_u16BorderPosY*SCREEN_WIDTH) + l_u16IndexX] = l_u32RGB;
    g_pau32FrameBuffer[( ( (l_u16BorderPosY+(YMAX>>2u) )%YMAX) * SCREEN_WIDTH) + l_u16IndexX] = l_u32RGB;
  }

  l_u32ScrollYValue++;
}
/* -User specific- */


