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
  Description:  Oldschool demoeffect - FX xxxx.
============================================================================= */

/* =============================================================================
                                 DEBUG Section
============================================================================= */
#define OCTANT0
#define OCTANT1
#define OCTANT2
#define OCTANT3
#define OCTANT4
#define OCTANT5
#define OCTANT6
#define OCTANT7
#define LINE01
#define LINE23
#define LINE34
#define LINE45
#define LINE12
#define LINE56
#define LINE67
#define LINE70


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
#include <math.h>
/* -User specific- */

/* =============================================================================
                          Private defines and typedefs
============================================================================= */
/* FX specific. */
#define SDL_SUBSYSTEM_TO_INIT   SDL_INIT_VIDEO
#define FX_NAME                 "FX_RadialBlur by UKONX"
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
  #define SDL_FLAG (SDL_WINDOW_FULLSCREEN | SDL_WINDOW_RESIZABLE)
#elif defined(DESKTOP_FULLSCREEN)
  #define SDL_FLAG (SDL_WINDOW_FULLSCREEN_DESKTOP | SDL_WINDOW_RESIZABLE)
#else
  #define SDL_FLAG (SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE)
#endif
/* FX specific. */

/* -User specific- */
#define UINT8_MAX_VALUE         ((uint8_t)255)
#define MAKE_RGB(r,g,b)         ( ( (r)<<16u) | ((g)<<8) | (b) )
#define GET_RED(rgb)            ((uint8_t)(rgb>>16u))
#define GET_GREEN(rgb)          ((uint8_t)(rgb>>8u))
#define GET_BLUE(rgb)           ((uint8_t)(rgb))
#define MASK_16BITS             (0xFFFF)
#define BIT0                    (1<<0)
#define BIT1                    (1<<1)
#define BIT2                    (1<<2)
#define RANDOM_LINE_SIZE        ((uint8_t)(SCREEN_HIGH/48))
#define min(a)                  ((a) > 255 ? 255 : a)
#define TABLE_SIZE              256
#define TEXTURE_FILENAME        "./texture/ukonx.png"
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
static uint8_t  *g_pau8Buffer                                   = NULL;
static uint32_t *g_pau32Buffer                                  = NULL;
static uint32_t *g_pau32Mini                                    = NULL;
static uint8_t *g_pau8Texture                                   = NULL;
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
static void vMakeRadialBlur (void);
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
        if (l_sEvent.key.keysym.sym == SDLK_ESCAPE)
        {
          l_bReturn = false;
        }
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

  /* -User specific- */
  g_pau8Buffer = (uint8_t *)malloc (sizeof (uint8_t) * YMAX * SCREEN_WIDTH);
  if (NULL != g_pau8Buffer)
  {
    g_pau32Buffer = (uint32_t *)malloc (sizeof (uint32_t) * YMAX * SCREEN_WIDTH);
    if (NULL != g_pau32Buffer)
    {
      g_pau32Mini = (uint32_t *)malloc (sizeof (uint32_t) * YMAX * SCREEN_WIDTH);
      if (NULL != g_pau32Mini)
      {
        /* Load a surface from Image file. */
        g_pSDL_Image = IMG_Load (TEXTURE_FILENAME);
        if (NULL != g_pSDL_Image)
        {
          g_pau8Texture = (uint8_t *)malloc (sizeof (uint8_t) * g_pSDL_Image->h * g_pSDL_Image->w);
          if (NULL != g_pSDL_Image)
          {
            /* For each line */
            for (uint16_t l_u16PosY = 0u; l_u16PosY < g_pSDL_Image->h; l_u16PosY++)
            {
              /* For each column */
              for (uint16_t l_u16PosX = 0u; l_u16PosX < g_pSDL_Image->w; l_u16PosX++)
              {
                /* Copy to pixel to texture buffer  */
                uint32_t l_u32Pixel = (* (uint32_t *)(  g_pSDL_Image->pixels
                                                      + l_u16PosY * g_pSDL_Image->pitch
                                                      + l_u16PosX * g_pSDL_Image->format->BytesPerPixel ) ) & 0x00FFFFFF;

                g_pau8Texture[(l_u16PosY * g_pSDL_Image->w) + l_u16PosX] = (uint8_t)(   0.21f * GET_RED(l_u32Pixel)
                                                                                      + 0.72f * GET_GREEN(l_u32Pixel)
                                                                                      + 0.07f * GET_BLUE(l_u32Pixel) );
              }
            }

            l_bReturn = true;
          }
        }
        else
        {
          printf ("IMG_Load failed: %s\n", IMG_GetError () );
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
  if (NULL != g_pau8Buffer)
  {
    free (g_pau8Buffer);
  }
  if (NULL != g_pau32Buffer)
  {
    free (g_pau32Buffer);
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
  static float l_fAngle  = 0.0f;
  static uint16_t l_u16OffsetX = 0u;//128 + 127*sin(l_fAngle);
  uint16_t l_u16OffsetY  = 272 + 128*cos(l_fAngle);

  /* Copy texture to frame buffer */
  for (uint16_t l_u16IndexY = 0u; l_u16IndexY < YMAX; l_u16IndexY++)
  {
    for (uint16_t l_u16IndexX = 0u; l_u16IndexX < SCREEN_WIDTH; l_u16IndexX++)
    {
      uint8_t l_u8Color = g_pau8Texture[  ( ( (l_u16IndexY + l_u16OffsetY) % g_pSDL_Image->h) * g_pSDL_Image->w)
                                        + ( (l_u16IndexX + l_u16OffsetX) % g_pSDL_Image->w)];


      g_pau8Buffer[(l_u16IndexY * SCREEN_WIDTH) + l_u16IndexX] = l_u8Color;
    }
  }

  l_fAngle += 0.08f;
  l_u16OffsetX+=4;

  /* Apply radial blur to back buffer */
  vMakeRadialBlur ();
  vMakeAndDisplayMini ();

  /* Display effect. */
  for (uint16_t l_u16IndexY = 0u; l_u16IndexY < YMAX; l_u16IndexY++)
  {
    for (uint16_t l_u16IndexX = 0u; l_u16IndexX < SCREEN_WIDTH; l_u16IndexX++)
    {
      uint32_t *l_pau32Screen =   g_pSDL_Screen->pixels
                        + ( (l_u16IndexY + (SCREEN_HIGH/6u) ) * SCREEN_WIDTH * g_pSDL_Screen->format->BytesPerPixel)
                        + (l_u16IndexX * g_pSDL_Screen->format->BytesPerPixel);
      *l_pau32Screen = g_pau32Buffer[(l_u16IndexY * SCREEN_WIDTH) + l_u16IndexX];
    }
  }

  l_bReturn = true;
  /* -User specific- */

  return (l_bReturn);
}


/* -User specific- */
/*==============================================================================
Function    :   vMakeRadialBlur
Describe    :   Make Fx rotozoom.
Parameters  :   None.
Returns     :   None.
==============================================================================*/
static void vMakeRadialBlur (void)
{
  /* Locals variables declaration. */
  const uint32_t col = 10000;
  uint32_t tol = 65278;

  /*
    Radial-blur octantSchema des octants du radial blur

          \ 4|3 /       <= To upper screen (y = 0 )
           \ | /
          5 \|/ 2
          ---|---
          6 /|\ 1
           / | \
          / 7|0 \
  */

	/* Line between octant 0 and 1. */
#ifdef LINE01
	for (uint16_t l_u8IndexX = 1u; l_u8IndexX < (SCREEN_WIDTH>>1u); l_u8IndexX++)
  {
    /* Diagonal (+1, +1) */
		uint16_t l_u16DiagX = (SCREEN_WIDTH>>1u) + l_u8IndexX;
		uint16_t l_u16DiagY = (YMAX>>1u)         + l_u8IndexX;

    /* Cropping. */
		if (   (l_u16DiagX >= 0)
        && (l_u16DiagX < SCREEN_WIDTH)
        && (l_u16DiagY >= 0)
        && (l_u16DiagY < YMAX) )
    {
      /* Get pixel from (XX, YY) */
      uint32_t l_u32Val = (col * g_pau8Buffer[l_u16DiagX + (l_u16DiagY * SCREEN_WIDTH)])>>8;

      /* Get pixel from (XX - 1, YY - 1) and add saturations */
      l_u32Val += (tol * g_pau8Buffer[l_u16DiagX - 1 + ((l_u16DiagY - 1) * SCREEN_WIDTH)])>>8 & 0xffffff00;

      /* Clipping */
      l_u32Val = l_u32Val>>8;
      if (l_u32Val > 255) l_u32Val = 255;

      /* Copy to (XX,YY) */
      g_pau8Buffer[l_u16DiagX + l_u16DiagY * SCREEN_WIDTH] = l_u32Val;
    }
	}
#endif

  /* Ligne separant octant 1 et 2. */
#ifdef LINE12
	for (uint16_t l_u8IndexX = 1u; l_u8IndexX < (SCREEN_WIDTH>>1u); l_u8IndexX++)
  {
    /* Horizontale (+1, 0) */
		uint16_t l_u16DiagX = (SCREEN_WIDTH>>1u) + l_u8IndexX;
		uint16_t l_u16DiagY = YMAX>>1u;

    if (  (l_u16DiagX >= 0) && (l_u16DiagX < SCREEN_WIDTH)
        &&(l_u16DiagY >= 0) && (l_u16DiagY < YMAX) )
    {
      /* Get pixel from (XX, YY) */
      uint32_t l_u32Val = (col * g_pau8Buffer[l_u16DiagX + l_u16DiagY*SCREEN_WIDTH])>>8;

      /* Get pixel from (XX - 1, YY) and add saturations */
      l_u32Val += (tol * g_pau8Buffer[l_u16DiagX - 1 + l_u16DiagY*SCREEN_WIDTH])>>8 & 0xffffff00;

      /* Clipping */
      l_u32Val = l_u32Val>>8;
      if (l_u32Val > 255) l_u32Val = 255;

      /* Copy to (XX,YY) */
      g_pau8Buffer[l_u16DiagX + l_u16DiagY*SCREEN_WIDTH] = l_u32Val;
    }
	}
#endif

  /* Ligne separant octant 2 et 3. */
#ifdef LINE23
	for (uint16_t l_u8IndexX = 1u; l_u8IndexX < (SCREEN_WIDTH>>1u); l_u8IndexX++)
  {
    /* Diagonale (+1,-1) */
		uint16_t l_u16DiagX = (SCREEN_WIDTH>>1u)  + l_u8IndexX;
		uint16_t l_u16DiagY = (YMAX>>1u)          - l_u8IndexX;

    if (  (l_u16DiagX >= 0) && (l_u16DiagX < SCREEN_WIDTH)
        &&(l_u16DiagY >= 0) && (l_u16DiagY < YMAX) )
    {
      /* Get pixel from (XX, YY) */
      uint32_t l_u32Val = (col * g_pau8Buffer[l_u16DiagX + l_u16DiagY*SCREEN_WIDTH])>>8;

      /* Get pixel from (XX - 1, YY - 1) and add saturations */
      l_u32Val += (tol * g_pau8Buffer[l_u16DiagX - 1 + (l_u16DiagY + 1) *SCREEN_WIDTH])>>8 & 0xffffff00;

      /* Clipping */
      l_u32Val = l_u32Val>>8;
      if (l_u32Val > 255) l_u32Val = 255;

      /* Copy to (XX,YY) */
      g_pau8Buffer[l_u16DiagX + l_u16DiagY*SCREEN_WIDTH] = l_u32Val;
    }
	}
#endif

  /* Ligne separant octant 3 et 4. */
#ifdef LINE34
	for (uint16_t l_u8IndexX = 1u; l_u8IndexX < (SCREEN_WIDTH>>1u); l_u8IndexX++)
  {
    /* Verticale (0, -1) */
		uint16_t l_u16DiagX = (SCREEN_WIDTH>>1u);
		uint16_t l_u16DiagY = (YMAX>>1u) - l_u8IndexX;

    if (  (l_u16DiagX >= 0) && (l_u16DiagX < SCREEN_WIDTH)
        &&(l_u16DiagY >= 0) && (l_u16DiagY < YMAX) )
    {
      /* Get pixel from (XX, YY) */
      uint32_t l_u32Val = (col * g_pau8Buffer[l_u16DiagX + l_u16DiagY*SCREEN_WIDTH])>>8;

      /* Get pixel from (XX, YY - 1) and add saturations */
      l_u32Val += (tol* g_pau8Buffer[l_u16DiagX + (l_u16DiagY + 1)*SCREEN_WIDTH])>>8 & 0xffffff00;

      /* Clipping */
      l_u32Val = l_u32Val>>8;
      if (l_u32Val > 255) l_u32Val = 255;

      /* Copy to (XX,YY) */
      g_pau8Buffer[l_u16DiagX + l_u16DiagY*SCREEN_WIDTH] = l_u32Val;
    }
	}
#endif

	/* Ligne separant octant 5 er 4. */
#ifdef LINE45
	for (uint16_t l_u8IndexX = 1u; l_u8IndexX < (SCREEN_WIDTH>>1u); l_u8IndexX++)
  {
    /* Diagonale (-1,-1) */
		uint16_t l_u16DiagX = (SCREEN_WIDTH>>1u) - l_u8IndexX;
		uint16_t l_u16DiagY = (YMAX>>1u) - l_u8IndexX;

    if (  (l_u16DiagX >= 0) && (l_u16DiagX < SCREEN_WIDTH)
        &&(l_u16DiagY >= 0) && (l_u16DiagY < YMAX) )
    {
      /* Get pixel from (XX, YY) */
      uint32_t l_u32Val = (col * g_pau8Buffer[l_u16DiagX + l_u16DiagY*SCREEN_WIDTH])>>8;

      /* Get pixel from (XX + 1, YY + 1) and add saturations */
      l_u32Val += (tol * g_pau8Buffer[l_u16DiagX + 1 + (l_u16DiagY + 1)*SCREEN_WIDTH])>>8 & 0xffffff00;

      /* Clipping */
      l_u32Val = l_u32Val>>8;
      if (l_u32Val > 255) l_u32Val = 255;

      /* Copy to (XX,YY) */
      g_pau8Buffer[l_u16DiagX + l_u16DiagY*SCREEN_WIDTH] = l_u32Val;
    }
	}
#endif

  /* Ligne separant octant 5 et 6. */
#ifdef LINE56
	for (uint16_t l_u8IndexX = 1u; l_u8IndexX < (SCREEN_WIDTH>>1u); l_u8IndexX++)
  {
    /* Horizontale (-1,0) */
		uint16_t l_u16DiagX = (SCREEN_WIDTH>>1u) - l_u8IndexX;
		uint16_t l_u16DiagY = (YMAX>>1u);

    if (  (l_u16DiagX >= 0) && (l_u16DiagX < SCREEN_WIDTH)
        &&(l_u16DiagY >= 0) && (l_u16DiagY < YMAX) )
    {
      /* Get pixel from (XX, YY) */
      uint32_t l_u32Val = (col * g_pau8Buffer[l_u16DiagX + l_u16DiagY*SCREEN_WIDTH])>>8;

      /* Get pixel from (XX + 1, YY) and add saturations */
      l_u32Val += (tol * g_pau8Buffer[l_u16DiagX + 1 + l_u16DiagY*SCREEN_WIDTH])>>8 & 0xffffff00;

      /* Clipping */
      l_u32Val = l_u32Val>>8;
      if (l_u32Val > 255) l_u32Val = 255;

      /* Copy to (XX,YY) */
      g_pau8Buffer[l_u16DiagX + l_u16DiagY*SCREEN_WIDTH] = l_u32Val;
    }
	}
#endif

  /* Ligne separant octant 6 et 7. */
#ifdef LINE67
	for (uint16_t l_u8IndexX = 1u; l_u8IndexX < (SCREEN_WIDTH>>1u); l_u8IndexX++)
  {
    /* Diagonale (-1,+1) */
		uint16_t l_u16DiagX = (SCREEN_WIDTH>>1u) - l_u8IndexX;
		uint16_t l_u16DiagY = (YMAX>>1u) + l_u8IndexX;

    if (  (l_u16DiagX >= 0) && (l_u16DiagX < SCREEN_WIDTH)
        &&(l_u16DiagY >= 0) && (l_u16DiagY < YMAX) )
    {
      /* Get pixel from (XX, YY) */
      uint32_t l_u32Val = (col * g_pau8Buffer[l_u16DiagX + l_u16DiagY*SCREEN_WIDTH])>>8;

      /* Get pixel from (XX + 1, YY - 1) and add saturations */
      l_u32Val += (tol * g_pau8Buffer[l_u16DiagX + 1 +(l_u16DiagY - 1)*SCREEN_WIDTH])>>8 & 0xffffff00;

      /* Clipping */
      l_u32Val = l_u32Val>>8;
      if (l_u32Val > 255) l_u32Val = 255;

      /* Copy to (XX,YY) */
      g_pau8Buffer[l_u16DiagX + l_u16DiagY*SCREEN_WIDTH] = l_u32Val;
    }
	}
#endif

	/* Ligne separant octant 7 et 0. */
#ifdef LINE70
	for (uint16_t l_u8IndexX = 1u; l_u8IndexX < (SCREEN_WIDTH>>1u); l_u8IndexX++)
  {
    /* Verticale (0,+1) */
		uint16_t l_u16DiagX = (SCREEN_WIDTH>>1u);
		uint16_t l_u16DiagY = ((YMAX>>1u) + l_u8IndexX);

    if (  (l_u16DiagX >= 0) && (l_u16DiagX < SCREEN_WIDTH)
        &&(l_u16DiagY >= 0) && (l_u16DiagY < YMAX) )
    {

    /* Get pixel from (XX, YY) */
    uint32_t l_u32Val = (col * g_pau8Buffer[l_u16DiagX + l_u16DiagY*SCREEN_WIDTH])>>8;

    /* Get pixel from (XX, YY - 1) and add saturations */
    l_u32Val += (tol * g_pau8Buffer[l_u16DiagX + (l_u16DiagY - 1)*SCREEN_WIDTH])>>8 & 0xffffff00;

    /* Clipping */
    l_u32Val = l_u32Val>>8;
    if (l_u32Val > 255) l_u32Val = 255;

    /* Copy to (XX,YY) */
    g_pau8Buffer[l_u16DiagX + l_u16DiagY*SCREEN_WIDTH] = l_u32Val;
    }
	}
#endif

	/* Gestion des octants 0 et 1. */
	uint16_t l_u16Offset = 1;
	for (uint16_t l_u8IndexY = (YMAX>>1u) + 1; l_u8IndexY < YMAX; l_u8IndexY++)
  {
    /* Octant 0 */
#ifdef OCTANT0
    for (uint16_t l_u8IndexX = (SCREEN_WIDTH>>1u) + 1; l_u8IndexX < (SCREEN_WIDTH>>1u) + l_u16Offset; l_u8IndexX++)
    {
      /* Get pixel from (XX, YY) */
      uint32_t l_u32Val = (col * g_pau8Buffer[l_u8IndexX + l_u8IndexY*SCREEN_WIDTH])>>8;

      //
      tol=( (l_u8IndexX - (SCREEN_WIDTH>>1u) )<<8) / (l_u8IndexY - (YMAX>>1u) );

      /* Get pixel from (XX - 1, YY - 1) and add saturations */
      l_u32Val += (tol * g_pau8Buffer[l_u8IndexX - 1 + ( (l_u8IndexY - 1) * SCREEN_WIDTH)]) & 0xffffff00;

      //
      tol = 256 - tol;

      /* Get pixel from (XX, YY - 1) and add saturations */
      l_u32Val += (tol * g_pau8Buffer[l_u8IndexX + ( (l_u8IndexY - 1) * SCREEN_WIDTH)]) & 0xffffff00;

      /* Clipping */
      l_u32Val = l_u32Val>>8;
      if (l_u32Val > 255)
        l_u32Val = 255;

      /* Copy to (XX,YY) */
      g_pau8Buffer[l_u8IndexX + l_u8IndexY*SCREEN_WIDTH] = l_u32Val;
		}
#endif

     /* Octant 1 */
#ifdef OCTANT1
		for (uint16_t l_u8IndexX = (SCREEN_WIDTH>>1u) + l_u16Offset + 1; l_u8IndexX < SCREEN_WIDTH; l_u8IndexX++)
    {
      /* Get pixel from (XX, YY) */
      uint32_t l_u32Val = (col * g_pau8Buffer[l_u8IndexX + l_u8IndexY*SCREEN_WIDTH])>>8;

      //vy/vx
      tol = ((l_u8IndexY-(YMAX>>1u)) << 8) / (l_u8IndexX - (SCREEN_WIDTH>>1u) );

      /* Get pixel from (XX - 1, YY - 1) and add saturations */
      l_u32Val += (tol * g_pau8Buffer[l_u8IndexX - 1 + (l_u8IndexY - 1)*SCREEN_WIDTH]) & 0xffffff00;

      //
      tol = 256-tol;

      /* Get pixel from (XX-1, YY) and add saturations */
      l_u32Val += (tol * g_pau8Buffer[l_u8IndexX + l_u8IndexY*SCREEN_WIDTH - 1]) & 0xffffff00;

      /* Clipping */
      l_u32Val = l_u32Val>>8;
      if (l_u32Val > 255) l_u32Val = 255;

      /* Copy to (XX,YY) */
      g_pau8Buffer[l_u8IndexX + l_u8IndexY*SCREEN_WIDTH] = l_u32Val;
		}
#endif

		l_u16Offset++;
	}

	/* Gestion des octants 7 et 6. */
	l_u16Offset = 1;
	for (int16_t l_s16IndexY = (YMAX>>1u) + 1; l_s16IndexY < YMAX; l_s16IndexY++)
  {
    /* Octant 7 */
#ifdef OCTANT7
		for (int16_t l_s16IndexX = (SCREEN_WIDTH>>1u) - 1; l_s16IndexX > (SCREEN_WIDTH>>1u) - l_u16Offset; l_s16IndexX--)
    {
      /* Get pixel from (XX, YY) */
      uint32_t l_u32Val = (col * g_pau8Buffer[l_s16IndexX + l_s16IndexY*SCREEN_WIDTH])>>8;

      //-vx/vy
      tol = (((SCREEN_WIDTH>>1u) - l_s16IndexX) << 8) / (l_s16IndexY - (YMAX>>1u));

      /* Get pixel from (XX + 1, YY - 1) and add saturations */
      l_u32Val += (tol * g_pau8Buffer[l_s16IndexX + 1 + (l_s16IndexY - 1)*SCREEN_WIDTH]) & 0xffffff00;

      //
      tol = 256 - tol;

      /* Get pixel from (XX, YY - 1) and add saturations */
      l_u32Val += (tol * g_pau8Buffer[l_s16IndexX + (l_s16IndexY - 1)*SCREEN_WIDTH]) & 0xffffff00;

      /* Clipping */
      l_u32Val = l_u32Val>>8;
      if (l_u32Val > 255) l_u32Val = 255;

      /* Copy to (XX,YY) */
      g_pau8Buffer[l_s16IndexX + l_s16IndexY*SCREEN_WIDTH] = l_u32Val;
    }
#endif

    /* Octant 6 */
#ifdef OCTANT6
		for (int16_t l_s16IndexX = (SCREEN_WIDTH>>1u) - l_u16Offset -1 ;l_s16IndexX >= 0; l_s16IndexX--)
    {
      /* Get pixel from (XX, YY) */
      uint32_t l_u32Val = (col * g_pau8Buffer[l_s16IndexX + l_s16IndexY*SCREEN_WIDTH])>>8;

      //-vy/vx
      tol=(((YMAX>>1u)-l_s16IndexY)<<8)/(l_s16IndexX-(SCREEN_WIDTH>>1u));

      /* Get pixel from (XX + 1, YY - 1) and add saturations */
      l_u32Val += (tol * g_pau8Buffer[l_s16IndexX + 1 + (l_s16IndexY - 1)*SCREEN_WIDTH]) & 0xffffff00;

      //
      tol = 256 - tol;

      /* Get pixel from (XX + 1, YY) and add saturations */
      l_u32Val += (tol * g_pau8Buffer[l_s16IndexX + l_s16IndexY*SCREEN_WIDTH + 1]) & 0xffffff00;

      /* Clipping */
      l_u32Val = l_u32Val>>8;
      if (l_u32Val > 255) l_u32Val = 255;

      /* Copy to (XX,YY) */
      g_pau8Buffer[l_s16IndexX + l_s16IndexY*SCREEN_WIDTH] = l_u32Val;
		}
#endif

    l_u16Offset++;
	}

	/* Gestion des octants 3 et 2. */
	l_u16Offset = 1;
  for (int16_t l_s16IndexY = (YMAX>>1u) - 1; l_s16IndexY >= 0; l_s16IndexY--)
  {
    /* Octant 3 */
#ifdef OCTANT3
		for (int16_t l_s16IndexX = (SCREEN_WIDTH>>1u) + 1; l_s16IndexX < (SCREEN_WIDTH>>1u) + l_u16Offset; l_s16IndexX++)
    {
			/* Get pixel from (XX, YY) */
      uint32_t l_u32Val = (col * g_pau8Buffer[l_s16IndexX + l_s16IndexY*SCREEN_WIDTH])>>8;

      //-vx/vy
      tol = ((l_s16IndexX - (SCREEN_WIDTH>>1u)) << 8) / ((YMAX>>1u) - l_s16IndexY);

      /* Get pixel from (XX - 1, YY + 1) and add saturations */
      l_u32Val += (tol * g_pau8Buffer[l_s16IndexX - 1 + (l_s16IndexY + 1)*SCREEN_WIDTH]) & 0xffffff00;

      //
      tol = 256 - tol;

      /* Get pixel from (XX, YY + 1) and add saturations */
      l_u32Val += (tol * g_pau8Buffer[l_s16IndexX + (l_s16IndexY + 1)*SCREEN_WIDTH]) & 0xffffff00;

      /* Clipping */
      l_u32Val = l_u32Val>>8;
      if (l_u32Val > 255) l_u32Val = 255;

      /* Copy to (XX,YY) */
      g_pau8Buffer[l_s16IndexX + l_s16IndexY*SCREEN_WIDTH] = l_u32Val;

		}
#endif

    /* Octant 2 */
#ifdef OCTANT2
		for(int16_t l_s16IndexX = (SCREEN_WIDTH>>1u) + l_u16Offset + 1; l_s16IndexX < SCREEN_WIDTH; l_s16IndexX++)
    {
      /* Get pixel from (XX, YY) */
      uint32_t l_u32Val = (col * g_pau8Buffer[l_s16IndexX + l_s16IndexY*SCREEN_WIDTH])>>8;

      //-vy/vx
      tol = (((YMAX>>1u) - l_s16IndexY) << 8) / (l_s16IndexX - (SCREEN_WIDTH>>1u));

      /* Get pixel from (XX - 1, YY + 1) and add saturations */
      l_u32Val += (tol * g_pau8Buffer[l_s16IndexX - 1 + (l_s16IndexY + 1)*SCREEN_WIDTH]) & 0xffffff00;

      //
      tol = 256 - tol;

      /* Get pixel from (XX, YY - 1) and add saturations */
      l_u32Val += (tol * g_pau8Buffer[l_s16IndexX + l_s16IndexY*SCREEN_WIDTH - 1]) & 0xffffff00;

      /* Clipping */
      l_u32Val = l_u32Val>>8;
      if (l_u32Val > 255) l_u32Val = 255;

      /* Copy to (XX,YY) */
      g_pau8Buffer[l_s16IndexX + l_s16IndexY*SCREEN_WIDTH] = l_u32Val;
		}

#endif

		l_u16Offset++;
	}

	/* Gestion des octants 4 et 5. */
	l_u16Offset = 1;
	for (int16_t l_s16IndexY = (YMAX>>1u) - 1; l_s16IndexY >= 0; l_s16IndexY--)
  {
		/* Octant 4 */
#ifdef OCTANT4
 		for (int16_t l_s16IndexX = (SCREEN_WIDTH>>1u) - 1; l_s16IndexX > (SCREEN_WIDTH>>1u) - l_u16Offset; l_s16IndexX--)
    {
      /* Get pixel from (XX, YY) */
      uint32_t l_u32Val = (col * g_pau8Buffer[l_s16IndexX+l_s16IndexY*SCREEN_WIDTH])>>8;

      //vx/vy
      tol = ((l_s16IndexX-(SCREEN_WIDTH>>1u))<<8)/(l_s16IndexY-(YMAX>>1u));

      /* Get pixel from (XX + 1, YY + 1) and add saturations */
      l_u32Val += (tol * g_pau8Buffer[l_s16IndexX + 1 + (l_s16IndexY + 1) * SCREEN_WIDTH]) & 0xffffff00;

      //
      tol = 256 - tol;

      /* Get pixel from (XX, YY + 1) and add saturations */
      l_u32Val += (tol * g_pau8Buffer[l_s16IndexX + (l_s16IndexY + 1) * SCREEN_WIDTH]) & 0xffffff00;

      /* Clipping */
      l_u32Val = l_u32Val>>8;
      if (l_u32Val > 255) l_u32Val = 255;

      /* Copy to (XX,YY) */
      g_pau8Buffer[l_s16IndexX + l_s16IndexY*SCREEN_WIDTH] = l_u32Val;
		}
#endif

    /* Octant 5 */
#ifdef OCTANT5
		for (int16_t l_s16IndexX = (SCREEN_WIDTH>>1u) - l_u16Offset - 1; l_s16IndexX >= 0; l_s16IndexX--)
    {
      /* Get pixel from (XX, YY) */
      uint32_t l_u32Val = (col * g_pau8Buffer[l_s16IndexX+l_s16IndexY*SCREEN_WIDTH])>>8;

      // vy/vx
      tol = ((l_s16IndexY - (YMAX>>1u))<<8)/(l_s16IndexX - (SCREEN_WIDTH>>1u));

      /* Get pixel from (XX + 1, YY + 1) and add saturations */
      l_u32Val += (tol * g_pau8Buffer[l_s16IndexX + 1 + (l_s16IndexY + 1)*SCREEN_WIDTH]) & 0xffffff00;

      //
      tol = 256 - tol;

      /* Get pixel from (XX + 1, YY) and add saturations */
      l_u32Val += (tol * g_pau8Buffer[l_s16IndexX + l_s16IndexY*SCREEN_WIDTH + 1]) & 0xffffff00;

      /* Clipping */
      l_u32Val = l_u32Val>>8;
      if (l_u32Val > 255) l_u32Val = 255;

      /* Copy to (XX,YY) */
      g_pau8Buffer[l_s16IndexX + l_s16IndexY*SCREEN_WIDTH] = l_u32Val;
		}
#endif

    l_u16Offset++;
	}

  /* Copy backbuffer to sdl texture. */
  for (uint16_t l_u16PosY = 0; l_u16PosY < YMAX; l_u16PosY++)
  {
    for (uint16_t l_u16PosX = 0; l_u16PosX < SCREEN_WIDTH; l_u16PosX++)
    {
      unsigned char l_ui8Pixel = g_pau8Buffer[(l_u16PosY * SCREEN_WIDTH) + l_u16PosX];
      g_pau32Buffer[(l_u16PosY * SCREEN_WIDTH) + l_u16PosX] = MAKE_RGB(l_ui8Pixel, l_ui8Pixel, l_ui8Pixel);
    }
  }
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
      g_pau32Mini[(l_u16IndexY * (SCREEN_WIDTH>>2u) ) + l_u16IndexX] = g_pau32Buffer[ (  (l_u16IndexY * g_pSDL_Screen->format->BytesPerPixel) * SCREEN_WIDTH)
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
      l_u32RGB                = g_pau32Buffer[(l_u16MiniPosY*SCREEN_WIDTH)+(l_u16IndexX+8)];
      uint8_t l_u8DstRed      = min(l_u8SrcRed    + 3 * GET_RED(l_u32RGB)   )>>2u;
      uint8_t l_u8DstGreen    = min(l_u8SrcGreen  + 3 * GET_GREEN(l_u32RGB) )>>2u;
      uint8_t l_u8DstBlue     = min(l_u8SrcBlue   + 3 * GET_BLUE(l_u32RGB)  )>>2u;
      g_pau32Buffer[(l_u16MiniPosY*SCREEN_WIDTH)+(l_u16IndexX+8)] = MAKE_RGB(l_u8DstRed, l_u8DstGreen, l_u8DstBlue);

      /* Second miniature. */
      l_u16MiniPosY = (l_u16MiniPosY + (YMAX/3) ) % YMAX;
      l_u32RGB      = g_pau32Buffer[(l_u16MiniPosY * SCREEN_WIDTH) + (l_u16IndexX + 8)];
      l_u8DstRed    = min(l_u8SrcRed    + GET_RED(l_u32RGB)   )>>1u;
      l_u8DstGreen  = min(l_u8SrcGreen  + GET_GREEN(l_u32RGB) )>>1u;
      l_u8DstBlue   = min(l_u8SrcBlue   + GET_BLUE(l_u32RGB)  )>>1u;
      g_pau32Buffer[(l_u16MiniPosY*SCREEN_WIDTH)+(l_u16IndexX+8)] = MAKE_RGB(l_u8DstRed, l_u8DstGreen, l_u8DstBlue);

      /* Third miniature. */
      l_u16MiniPosY = (l_u16MiniPosY + (YMAX/3) ) % YMAX;
      l_u32RGB      = g_pau32Buffer[(l_u16MiniPosY * SCREEN_WIDTH) + (l_u16IndexX + 8)];
      l_u8DstRed    = min(3 * l_u8SrcRed    + GET_RED(l_u32RGB)   )>>2u;
      l_u8DstGreen  = min(3 * l_u8SrcGreen  + GET_GREEN(l_u32RGB) )>>2u;
      l_u8DstBlue   = min(3 * l_u8SrcBlue   + GET_BLUE(l_u32RGB)  )>>2u;
      g_pau32Buffer[(l_u16MiniPosY*SCREEN_WIDTH)+(l_u16IndexX+8)] = MAKE_RGB(l_u8DstRed, l_u8DstGreen, l_u8DstBlue);
    }
  }

  /* Display upper and lower border. */
  for (uint16_t l_u16IndexY = 0u; l_u16IndexY < (YMAX>>2u); l_u16IndexY++)
  {
    uint32_t l_u32RGB = MAKE_RGB(255,255,255);

    uint32_t l_u16BorderPosY = (l_u16IndexY + l_u32ScrollYValue) % YMAX;
    g_pau32Buffer[(l_u16BorderPosY * SCREEN_WIDTH) + 8] = l_u32RGB;
    g_pau32Buffer[(l_u16BorderPosY * SCREEN_WIDTH) + 8 + (SCREEN_WIDTH>>2u)]=l_u32RGB;

    l_u16BorderPosY = (l_u16BorderPosY + (YMAX/3))%YMAX;
    g_pau32Buffer[(l_u16BorderPosY * SCREEN_WIDTH) + 8] = l_u32RGB;
    g_pau32Buffer[(l_u16BorderPosY * SCREEN_WIDTH) + 8 + (SCREEN_WIDTH>>2u)]=l_u32RGB;

    l_u16BorderPosY = (l_u16BorderPosY + (YMAX/3))%YMAX;
    g_pau32Buffer[(l_u16BorderPosY * SCREEN_WIDTH) + 8] = l_u32RGB;
    g_pau32Buffer[(l_u16BorderPosY * SCREEN_WIDTH) + 8 + (SCREEN_WIDTH>>2u)]=l_u32RGB;
  }

  /* Display left and right border. */
  for (uint16_t l_u16IndexX=8;l_u16IndexX<(8 + (SCREEN_WIDTH>>2u));l_u16IndexX++)
  {
    uint32_t l_u32RGB         = MAKE_RGB(255,255,255);

    uint32_t l_u16BorderPosY  = l_u32ScrollYValue%YMAX;
    g_pau32Buffer[(l_u16BorderPosY * SCREEN_WIDTH) + l_u16IndexX] = l_u32RGB;
    g_pau32Buffer[( ( (l_u16BorderPosY + (YMAX>>2u) )%YMAX) * SCREEN_WIDTH) + l_u16IndexX] = l_u32RGB;

    l_u16BorderPosY = (l_u16BorderPosY + (YMAX/3) )%YMAX;
    g_pau32Buffer[(l_u16BorderPosY*SCREEN_WIDTH)+l_u16IndexX] = l_u32RGB;
    g_pau32Buffer[( ( (l_u16BorderPosY+(YMAX>>2u) )%YMAX) * SCREEN_WIDTH) + l_u16IndexX] = l_u32RGB;

    l_u16BorderPosY = (l_u16BorderPosY + (YMAX/3) )%YMAX;
    g_pau32Buffer[(l_u16BorderPosY*SCREEN_WIDTH) + l_u16IndexX] = l_u32RGB;
    g_pau32Buffer[( ( (l_u16BorderPosY+(YMAX>>2u) )%YMAX) * SCREEN_WIDTH) + l_u16IndexX] = l_u32RGB;
  }

  l_u32ScrollYValue++;
}
/* -User specific- */


