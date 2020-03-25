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
  Description:  Oldschool demoeffect - FX Particle.
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
#include <math.h>
#include <string.h>
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
#define YMAX                    SCREEN_HIGH//((uint16_t)(SCREEN_HIGH - (SCREEN_HIGH / 3)))
#define ALPHA_MASK              ((uint32_t)0xFF000000)
#define RED_MASK                ((uint32_t)0x00FF0000)
#define GREEN_MASK              ((uint32_t)0x0000FF00)
#define BLUE_MASK               ((uint32_t)0x000000FF)

//#define TRUE_FULLSCREEN
//#define DESKTOP_FULLSCREEN
#if defined(TRUE_FULLSCREEN)
  #define SDL_FLAG (SDL_WINDOW_FULLSCREEN | SDL_WINDOW_RESIZABLE)
#elif defined(DESKTOP_FULLSCREEN)
  #define SDL_FLAG (SDL_WINDOW_FULLSCREEN_DESKTOP | SDL_WINDOW_RESIZABLE)
#else
  #define SDL_FLAG (SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE)
#endif
/* FX specific. */

/* -User specific- */
#define UINT8_MAX_VALUE   ((uint8_t)255)
#define MAKE_RGB(r,g,b)   ( ( (r)<<16u) | ((g)<<8) | (b) )
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

typedef struct PARTICLE
{
  uint16_t u16PosX;
  uint16_t u16PosY;
  int16_t   s8Vx;
  int16_t   s8Vy;
  int16_t   s8Sx;
  int16_t   s8Sy;
  int16_t   s8Ax;
  int16_t   s8Ay;
  uint32_t u32Time;
  uint8_t  u8Intensity;
  uint8_t  u8Red;
  uint8_t  u8Green;
  uint8_t  u8Blue;
} s_Particle_t;

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
static uint8_t *g_pau8Sinus                                     = NULL;
static uint32_t *g_pau32Palette                                 = NULL;
static uint32_t *g_pau32Buffer                                  = NULL;
static uint32_t *g_pau32Mini                                    = NULL;
static const char g_cachFilename[]                              = "./texture/particleTexture_16x16.png";
static s_Particle_t g_asParticle[3000];
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
      case SDL_KEYDOWN:
      {
        switch(l_sEvent.key.keysym.sym)
        {
          case SDLK_ESCAPE:
          {
            l_bReturn = false;
            break;
          }
          default:
          {
            break;
          }
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


static void vMakeParticle (s_Particle_t *p_psParticle)
{
  static float angle1 = 0.0f;
  static float angle2 = 0.0f;

  if (NULL != p_psParticle)
  {
    p_psParticle->u16PosX     = (SCREEN_WIDTH>>1u) + ((SCREEN_WIDTH>>1u) -(g_pSDL_Image->h)) *sin (angle2) ;//*/(SCREEN_WIDTH>>1u) - (g_pSDL_Image->w>>1u);
    p_psParticle->u16PosY     = (YMAX>>1u) + ((YMAX>>1u) - (g_pSDL_Image->h))*cos (angle1) *sin(angle2);//(YMAX>>0u)  ;//+ (YMAX>>1u)*sin (angle1)*sin(angle1*angle2*angle3) ;//(g_pSDL_Image->h>>1u);//YMAX         - (g_pSDL_Image->h>>1u);
    p_psParticle->s8Vx        = 4 - (int8_t)(rand()%8);
    p_psParticle->s8Vy        = /*-*/(int8_t)(rand()%15) ;
    p_psParticle->s8Ax        = rand()%1;
    p_psParticle->s8Ay        = rand()%1;
    p_psParticle->s8Sx        = 1;
    p_psParticle->s8Sy        = 1;
    p_psParticle->u32Time     = 0;
    p_psParticle->u8Intensity = 1 + rand()%254;
    p_psParticle->u8Red       = rand()%256;
    p_psParticle->u8Green     = rand()%256;
    p_psParticle->u8Blue      = rand()%256;
  }

  angle1 += 0.00031f;
  angle2 -= 0.00077f;
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
  srand(time(NULL));

  /* Load a surface from Image file. */
  g_pSDL_Image = IMG_Load (g_cachFilename);
  if (NULL != g_pSDL_Image)
  {
    g_pau8Sinus = (uint8_t *)malloc (sizeof (uint8_t) * TABLE_SIZE);
    if (NULL != g_pau8Sinus)
    {
      g_pau32Palette = (uint32_t *)malloc (sizeof (uint32_t) * TABLE_SIZE);
      if (NULL != g_pau32Palette)
      {
        g_pau32Buffer = (uint32_t *)malloc (sizeof (uint32_t) * YMAX * SCREEN_WIDTH);
        if (NULL != g_pau32Buffer)
        {
          g_pau32Mini = (uint32_t *)malloc (sizeof (uint32_t) * YMAX * SCREEN_WIDTH);
          if (NULL != g_pau32Mini)
          {
            for (int i = 0; i < sizeof(g_asParticle)/sizeof(g_asParticle[0]); i++)
            {
              vMakeParticle (&g_asParticle[i]);
            }
            l_bReturn = true;
          }
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
  if (NULL != g_pau8Sinus)
  {
    free (g_pau8Sinus);
  }
  if (NULL != g_pau32Palette)
  {
    free (g_pau32Palette);
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
  memset (g_pau32Buffer, 0u, sizeof(uint32_t) * YMAX * SCREEN_WIDTH );

  /* Update particle. */
  for (int i = 0; i < sizeof(g_asParticle)/sizeof(g_asParticle[0]); i++)
  {
    for (uint16_t l_u16TexY = 0; l_u16TexY < g_pSDL_Image->h; l_u16TexY++)
    {
      for (uint16_t l_u16TexX = 0; l_u16TexX < g_pSDL_Image->w; l_u16TexX++)
      {
        Uint8 *p_texture = (Uint8 *)g_pSDL_Image->pixels + l_u16TexY * g_pSDL_Image->pitch + l_u16TexX * g_pSDL_Image->format->BytesPerPixel;

        uint16_t l_u16PosX = g_asParticle[i].u16PosX + l_u16TexX ;
        uint16_t l_u16PosY = g_asParticle[i].u16PosY + l_u16TexY ;

        if ((l_u16PosX < SCREEN_WIDTH) && (l_u16PosY < YMAX))
        {
          uint32_t r = (g_pau32Buffer[(l_u16PosY * SCREEN_WIDTH) + l_u16PosX]>>16)&0xff;
          uint32_t g = (g_pau32Buffer[(l_u16PosY * SCREEN_WIDTH) + l_u16PosX]>>8)&0xff;
          uint32_t b = (g_pau32Buffer[(l_u16PosY * SCREEN_WIDTH) + l_u16PosX]>>0)&0xff;

          r += /*g_asParticle[i].u8Intensity * */ (*(p_texture + 2))*g_asParticle[i].u8Red / 255;
          g += /*g_asParticle[i].u8Intensity * */ (*(p_texture + 1))*g_asParticle[i].u8Green / 255;
          b += /*g_asParticle[i].u8Intensity * */ (*(p_texture + 0))*g_asParticle[i].u8Blue / 255;

          if (r>255) r = 255;
          if (g>255) g = 255;
          if (b>255) b = 255;

          g_pau32Buffer[(l_u16PosY * SCREEN_WIDTH) + l_u16PosX] =  (r<<16) | (g<<8)|(b);
        }
      }
    }

    g_asParticle[i].u16PosX += g_asParticle[i].s8Vx*g_asParticle[i].s8Sx;
    g_asParticle[i].u16PosY += g_asParticle[i].s8Vy*g_asParticle[i].s8Sy + 0.3f*g_asParticle[i].u32Time;
    g_asParticle[i].u32Time++;
    g_asParticle[i].s8Sx += g_asParticle[i].s8Ax;
    g_asParticle[i].s8Sy += g_asParticle[i].s8Ay;

    if (3 < g_asParticle[i].u8Red)
    {
      g_asParticle[i].u8Red-=3;
    }
    if (3 < g_asParticle[i].u8Green)
    {
      g_asParticle[i].u8Green-=3;
    }

    if (3 < g_asParticle[i].u8Blue)
    {
      g_asParticle[i].u8Blue-=3;
    }


    if (3 > g_asParticle[i].u8Intensity)
    {
      vMakeParticle (&g_asParticle[i]);
    }
    else
    {
      g_asParticle[i].u8Intensity-=3;
    }
  }

  /* Display FX. */
  for (uint16_t l_u16IndexY = 0u; l_u16IndexY < YMAX; l_u16IndexY++)
  {
    for (uint16_t l_u16IndexX = 0u;l_u16IndexX < SCREEN_WIDTH; l_u16IndexX++)
    {
      Uint32 *p_ecran =  g_pSDL_Screen->pixels
                       + ( (l_u16IndexY /*+ (SCREEN_HIGH / 6)*/ ) * SCREEN_WIDTH * g_pSDL_Screen->format->BytesPerPixel)
                       + (l_u16IndexX * g_pSDL_Screen->format->BytesPerPixel);
      *p_ecran = g_pau32Buffer[(l_u16IndexY*SCREEN_WIDTH)+l_u16IndexX];
    }
  }

  l_bReturn = true;
  /* -User specific- */

  return (l_bReturn);
}


/* -User specific- */

/* -User specific- */


