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
  Description:  Oldschool demoeffect - FX Bump2D.
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
/* -User specific- */

/* =============================================================================
                          Private defines and typedefs
============================================================================= */
/* FX specific. */
#define SDL_SUBSYSTEM_TO_INIT   SDL_INIT_VIDEO
#define FX_NAME                 "FX_Bump2D by UKONX"
#define SCREEN_WIDTH            ((uint16_t)800)
#define SCREEN_HIGH             ((uint16_t)600)
#define SCREEN_BPP              ((uint8_t)32)
#define YMAX                    (SCREEN_HIGH - (SCREEN_HIGH / 3) )
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

/* -User specific- */
#define UINT8_MAX_VALUE   ((uint8_t)255)
#define MAKE_RGB(r,g,b)   ( ( (r)<<16u) | ((g)<<8) | (b) )

#define RGB2GRAY(r,g,b)   ((255-(0.2126f * (r))) + (255-(0.7152f * (g))) + (255 - (0.0722f * (b))))

#define GET_RED(rgb)      ((uint8_t)(rgb>>16u))
#define GET_GREEN(rgb)    ((uint8_t)(rgb>>8u))
#define GET_BLUE(rgb)     ((uint8_t)(rgb))

#define min(a)            ((a) > 255 ? 255 : a)
#define TEXTURE_WIDTH     ((uint16_t)512)
#define TEXTURE_HEIGHT    ((uint16_t)512)

#define FORCE             8
#define SATURATION        2

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
static uint8_t *g_pau8Enviromentmap                             = NULL;
static uint8_t  *g_pau8Buffer                                   = NULL;
static uint32_t *g_pau32Buffer                                  = NULL;
static uint32_t *g_pau32Mini                                    = NULL;
static const char sznamefile[]                                  = {"./texture/ukonx.png"};
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
static void vComputeEnvironmentMap(void);
static void vMakeBump (void);
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
      g_pSDL_Window = SDL_CreateWindow (FX_NAME, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HIGH , SDL_FLAG);
      if (NULL != g_pSDL_Window)
      {
        /* Create renderer. */
        g_pSDL_Renderer = SDL_CreateRenderer (g_pSDL_Window, -1, SDL_RENDERER_ACCELERATED| SDL_RENDERER_PRESENTVSYNC);
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
          break;
        }
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
  vFxQuit();

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
  /* Load a surface from Image file. */
  g_pSDL_Image = IMG_Load (sznamefile);
  if (NULL != g_pSDL_Image)
  {
    g_pau8Enviromentmap = (uint8_t *)malloc (sizeof (uint8_t) * g_pSDL_Image->h * g_pSDL_Image->w);
    if (NULL != g_pau8Enviromentmap)
    {
      g_pau8Buffer = (uint8_t *)malloc (sizeof (uint8_t) * YMAX * SCREEN_WIDTH);
      if (NULL != g_pau8Buffer)
      {
        g_pau32Buffer = (uint32_t *)malloc (sizeof (uint32_t) * YMAX * SCREEN_WIDTH);
        if (NULL != g_pau32Buffer)
        {
          g_pau32Mini = (uint32_t *)malloc (sizeof (uint32_t) * YMAX * SCREEN_WIDTH);
          if (NULL != g_pau32Mini)
          {
            vComputeEnvironmentMap();
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
  if (NULL != g_pau8Enviromentmap)
  {
    free (g_pau8Enviromentmap);
  }
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
  vMakeBump();
  vMakeAndDisplayMini();

  /* Display fx. */
  for (uint16_t l_u16IndexY = 0u; l_u16IndexY < YMAX; l_u16IndexY++)
  {
    for (uint16_t l_u16IndexX = 0u;l_u16IndexX < SCREEN_WIDTH; l_u16IndexX++)
    {
      Uint32 *p_ecran =   g_pSDL_Screen->pixels
                        + ( (l_u16IndexY + (SCREEN_HIGH / 6) ) * SCREEN_WIDTH * g_pSDL_Screen->format->BytesPerPixel)
                        + (l_u16IndexX * g_pSDL_Screen->format->BytesPerPixel);
      *p_ecran = g_pau32Buffer[(l_u16IndexY*SCREEN_WIDTH)+l_u16IndexX];
    }
  }
  l_bReturn = true;
  /* -User specific- */

  return (l_bReturn);
}


/* -User specific- */
/*==============================================================================
Function    :   vComputeEnvironmentMap.
Describe    :   Compute environment map.
Parameters  :   None.
Returns     :   None.
==============================================================================*/
static void vComputeEnvironmentMap(void)
{
  for (uint16_t l_u16IndexY = 0u; l_u16IndexY < g_pSDL_Image->h; l_u16IndexY++)
  {
    for (uint16_t l_u16IndexX = 0u; l_u16IndexX < g_pSDL_Image->w; l_u16IndexX++)
    {
      float nX = ((float)l_u16IndexX - g_pSDL_Image->w/2.0f) / (float)(g_pSDL_Image->w/2.0f);
      float nY = ((float)l_u16IndexY - g_pSDL_Image->h/2.0f) / (float)(g_pSDL_Image->h/2.0f);
      float nZ = 1 - sqrt (nX*nX + nY*nY);
      if (nZ < 0)
        nZ = 0.0f;

      g_pau8Enviromentmap[l_u16IndexX + (l_u16IndexY * g_pSDL_Image->w)] = nZ*256;
    }
  }
}


/*==============================================================================
Function    :   vMakeBump.
Describe    :   Make 2d bump mapping FX.
Parameters  :   None.
Returns     :   None.
==============================================================================*/
static void vMakeBump (void)
{
  /* Locals variables declaration. */
  static float l_fAngle  = 0.0f;
  static float l_fAngle2 = 0.0f;
  int16_t l_s16Lightx    = (SCREEN_WIDTH>>1u) + (SCREEN_WIDTH>>1u) * sin(l_fAngle) * cos(l_fAngle2);
  int16_t l_s16Lighty    = (SCREEN_HIGH>>2u)  + (SCREEN_HIGH>>3u)  * cos(l_fAngle);

  /* Compute 2D bump mapping. */
  for (uint16_t l_u16PosY = 1u; l_u16PosY < YMAX - 1u; l_u16PosY++)
  {
    for (uint16_t l_u16PosX = 1u; l_u16PosX < SCREEN_WIDTH - 1u; l_u16PosX++)
    {
      /* bumpmap(x+1,y) */
      uint8_t *p_texture = (uint8_t *)g_pSDL_Image->pixels
                        + ((l_u16PosX + 1) % TEXTURE_WIDTH) * g_pSDL_Image->pitch
                        + ((l_u16PosY + 0) % TEXTURE_HEIGHT) * g_pSDL_Image->format->BytesPerPixel;
      int16_t l_s16DiffX = RGB2GRAY (*(p_texture + 2u), *(p_texture + 1u), *p_texture);

      /* bumpmap(x-1,y) */
      p_texture = (uint8_t *)g_pSDL_Image->pixels
                        + ((l_u16PosX - 1) % TEXTURE_WIDTH) * g_pSDL_Image->pitch
                        + ((l_u16PosY + 0) % TEXTURE_HEIGHT) * g_pSDL_Image->format->BytesPerPixel;

      l_s16DiffX -= RGB2GRAY (*(p_texture + 2u), *(p_texture + 1u), *p_texture);


      /* bumpmap(x,y+1) */
      p_texture = (uint8_t *)g_pSDL_Image->pixels
                        + ((l_u16PosX + 0) % TEXTURE_WIDTH) * g_pSDL_Image->pitch
                        + ((l_u16PosY + 1) % TEXTURE_HEIGHT) * g_pSDL_Image->format->BytesPerPixel;
      int16_t l_s16DiffY = RGB2GRAY (*(p_texture + 2u), *(p_texture + 1u), *p_texture);

      /* bumpmap(x,y-1) */
      p_texture = (uint8_t *)g_pSDL_Image->pixels
                        + ((l_u16PosX + 0u) % TEXTURE_WIDTH) * g_pSDL_Image->pitch
                        + ((l_u16PosY - 1u) % TEXTURE_HEIGHT) * g_pSDL_Image->format->BytesPerPixel;

      l_s16DiffY -= RGB2GRAY (*(p_texture + 2u), *(p_texture + 1u), *p_texture);

      l_s16DiffX = (l_s16DiffX * FORCE) - (l_u16PosX - l_s16Lightx) + 128u;
      l_s16DiffY = (l_s16DiffY * FORCE) - (l_u16PosY - l_s16Lighty) + 128u;
      if (   (l_s16DiffX < 0)
          || (l_s16DiffX > (TEXTURE_WIDTH - 1u) ) )
        l_s16DiffX = 0;

      if (   (l_s16DiffY < 0)
          || (l_s16DiffY > (TEXTURE_HEIGHT - 1u) ) )
        l_s16DiffY = 0;

      p_texture = (uint8_t *)g_pSDL_Image->pixels
                        + (l_u16PosX % TEXTURE_WIDTH) * g_pSDL_Image->pitch
                        + (l_u16PosY % TEXTURE_HEIGHT) * g_pSDL_Image->format->BytesPerPixel;

      uint16_t l_u16Red   = *(p_texture + 0u) * g_pau8Enviromentmap[l_s16DiffX + l_s16DiffY*g_pSDL_Image->w]/256;
      uint16_t l_u16Green = *(p_texture + 1u) * g_pau8Enviromentmap[l_s16DiffX + l_s16DiffY*g_pSDL_Image->w]/256;
      uint16_t l_u16Blue  = *(p_texture + 2u) * g_pau8Enviromentmap[l_s16DiffX + l_s16DiffY*g_pSDL_Image->w]/256;

      l_u16Red   *= SATURATION;
      l_u16Green *= SATURATION;
      l_u16Blue  *= SATURATION;
      if (l_u16Red > 255u)
        l_u16Red = 255u;
      if (l_u16Green > 255u)
        l_u16Green = 255u;
      if (l_u16Blue > 255u)
        l_u16Blue = 255u;

      g_pau32Buffer[(l_u16PosY * SCREEN_WIDTH) + l_u16PosX] = MAKE_RGB(l_u16Red, l_u16Green, l_u16Blue);
    }
  }
  l_fAngle  += 0.03f;
  l_fAngle2 += 0.02f;
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
  static uint32_t l_u32ScrollYValue = 0u;

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
      l_u32RGB                = g_pau32Buffer[(l_u16MiniPosY*SCREEN_WIDTH)+(l_u16IndexX + 8u)];
      uint8_t l_u8DstRed      = min(l_u8SrcRed    + 3u * GET_RED(l_u32RGB)   )>>2u;
      uint8_t l_u8DstGreen    = min(l_u8SrcGreen  + 3u * GET_GREEN(l_u32RGB) )>>2u;
      uint8_t l_u8DstBlue     = min(l_u8SrcBlue   + 3u * GET_BLUE(l_u32RGB)  )>>2u;
      g_pau32Buffer[(l_u16MiniPosY*SCREEN_WIDTH)+(l_u16IndexX + 8u)] = MAKE_RGB(l_u8DstRed, l_u8DstGreen, l_u8DstBlue);

      /* Second miniature. */
      l_u16MiniPosY = (l_u16MiniPosY + (YMAX/3) ) % YMAX;
      l_u32RGB      = g_pau32Buffer[(l_u16MiniPosY * SCREEN_WIDTH) + (l_u16IndexX + 8u)];
      l_u8DstRed    = min(l_u8SrcRed    + GET_RED(l_u32RGB)   )>>1u;
      l_u8DstGreen  = min(l_u8SrcGreen  + GET_GREEN(l_u32RGB) )>>1u;
      l_u8DstBlue   = min(l_u8SrcBlue   + GET_BLUE(l_u32RGB)  )>>1u;
      g_pau32Buffer[(l_u16MiniPosY*SCREEN_WIDTH)+(l_u16IndexX + 8u)] = MAKE_RGB(l_u8DstRed, l_u8DstGreen, l_u8DstBlue);

      /* Third miniature. */
      l_u16MiniPosY = (l_u16MiniPosY + (YMAX/3) ) % YMAX;
      l_u32RGB      = g_pau32Buffer[(l_u16MiniPosY * SCREEN_WIDTH) + (l_u16IndexX + 8u)];
      l_u8DstRed    = min(3 * l_u8SrcRed    + GET_RED(l_u32RGB)   )>>2u;
      l_u8DstGreen  = min(3 * l_u8SrcGreen  + GET_GREEN(l_u32RGB) )>>2u;
      l_u8DstBlue   = min(3 * l_u8SrcBlue   + GET_BLUE(l_u32RGB)  )>>2u;
      g_pau32Buffer[(l_u16MiniPosY*SCREEN_WIDTH)+(l_u16IndexX + 8u)] = MAKE_RGB(l_u8DstRed, l_u8DstGreen, l_u8DstBlue);
    }
  }

  /* Display upper and lower border. */
  for (uint16_t l_u16IndexY = 0u; l_u16IndexY < (YMAX>>2u); l_u16IndexY++)
  {
    uint32_t l_u32RGB = MAKE_RGB(255u, 255u, 255u);

    uint32_t l_u16BorderPosY = (l_u16IndexY + l_u32ScrollYValue) % YMAX;
    g_pau32Buffer[(l_u16BorderPosY * SCREEN_WIDTH) + 8u] = l_u32RGB;
    g_pau32Buffer[(l_u16BorderPosY * SCREEN_WIDTH) + 8u + (SCREEN_WIDTH>>2u)]=l_u32RGB;

    l_u16BorderPosY = (l_u16BorderPosY + (YMAX/3))%YMAX;
    g_pau32Buffer[(l_u16BorderPosY * SCREEN_WIDTH) + 8u] = l_u32RGB;
    g_pau32Buffer[(l_u16BorderPosY * SCREEN_WIDTH) + 8u + (SCREEN_WIDTH>>2u)]=l_u32RGB;

    l_u16BorderPosY = (l_u16BorderPosY + (YMAX/3))%YMAX;
    g_pau32Buffer[(l_u16BorderPosY * SCREEN_WIDTH) + 8u] = l_u32RGB;
    g_pau32Buffer[(l_u16BorderPosY * SCREEN_WIDTH) + 8u + (SCREEN_WIDTH>>2u)]=l_u32RGB;
  }

  /* Display left and right border. */
  for (uint16_t l_u16IndexX = 8u;l_u16IndexX < (8u + (SCREEN_WIDTH>>2u)); l_u16IndexX++)
  {
    uint32_t l_u32RGB         = MAKE_RGB(255u, 255u, 255u);

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


