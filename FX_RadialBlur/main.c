/* =============================================================================

      88        88  88      a8P     ,ad8888ba,    888b      88  8b        d8
      88        88  88    ,88'     d8"'    `"8b   8888b     88   Y8,    ,8P
      88        88  88  ,88"      d8'        `8b  88 `8b    88    `8b  d8'
      88        88  88,d88'       88          88  88  `8b   88      Y88P
      88        88  8888"88,      88          88  88   `8b  88      d88b
      88        88  88P   Y8b     Y8,        ,8P  88    `8b 88    ,8P  Y8,
      Y8a.    .a8P  88     "88,    Y8a.    .a8P   88     `8888   d8'    `8b
       `"Y8888Y"'   88       Y8b    `"Y8888Y"'    88      `888  8P        Y8

  File name:     main.c
  Date:          21 07 2017
  Author:        Power(code)/Deemphasis(algo).
  Description:   Radial blur.
============================================================================= */

/* =============================================================================
                                 DEBUG Section
============================================================================= */
//#define TRUE_FULLSCREEN
// #define DESKTOP_FULLSCREEN
#define OCTANT0
#define OCTANT1
#define OCTANT2
#define OCTANT3
#define OCTANT4
#define OCTANT5
#define OCTANT6
#define OCTANT7

#define LINE01
#define LINE12
#define LINE23
#define LINE34
#define LINE45
#define LINE56
#define LINE67
#define LINE70

//#define CLEAR_BACKGROUND


/* =============================================================================
                                 Include Files
============================================================================= */
#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <math.h>
#include <SDL2/SDL_image.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/* =============================================================================
                          Private defines and typedefs
============================================================================= */

#define WINDOW_WIDTH  512
#define WINDOW_HEIGHT 384
#if defined(TRUE_FULLSCREEN)
  #define SDL_FLAG SDL_WINDOW_FULLSCREEN
#elif defined(DESKTOP_FULLSCREEN)
  #define SDL_FLAG SDL_WINDOW_FULLSCREEN_DESKTOP
#else
  #define SDL_FLAG 0
#endif
/* =============================================================================
                          Private defines and typedefs
============================================================================= */
#define IMG_PATH      "texture/ukonx.png"
//#define MASK_VIDEO    0x00FFFFFF
#define MASK_VIDEO    0x00000000
#define PIXEL_COLOR   0x00FFFFFF
#define min(a,b)      (a>b)?b:a
#define max(a,b)      (a>b)?a:b
#define OFFSET_X      WINDOW_WIDTH/2
#define OFFSET_Y      WINDOW_HEIGHT/2

/* =============================================================================
                        Private constants and variables
============================================================================= */


/* =============================================================================
                         Private functions declarations
============================================================================= */
void vRenderer (unsigned long*, short, short, float);


/* =============================================================================
                        Public constants and variables
============================================================================= */


/* =============================================================================
                               Public functions
============================================================================= */

/* =============================================================================
Function    :   main

Description :   Main function.

Parameters  :   None.

Return      :   None.
============================================================================= */
int main (int arc, char *argv[]) {

  /* Locals variables declaration and initialization */
  unsigned char l_ui8Loop = 1;
  short l_i16PosX;
  short l_i16PosY;
  unsigned long l_ui32Pixel;
  float l_fAngle = 0.0f;
  int l_iReturn = EXIT_FAILURE;
  int l_iPitch;
  SDL_Event l_sEvent;
  SDL_Renderer* l_psRenderer = NULL;
  SDL_Window* l_psWindow = NULL;
  SDL_Texture* l_psTexture = NULL;
  SDL_Surface* l_psSurface = NULL;
  void* l_pvBackBuffer;

  unsigned long* l_paui32BackBuffer = NULL;
  unsigned long* l_paui32Texture = NULL;

  unsigned short l_ui16OffX = 0;


  /* Allocate memory for video buffer */
  l_paui32BackBuffer = (unsigned long*) malloc (WINDOW_WIDTH*WINDOW_HEIGHT*4);
  if (NULL == l_paui32BackBuffer) {

    printf ("malloc l_paui32BackBuffer failed !\n");
  }
  else {

    /* Init SDL video system */
    if (0 > SDL_Init (SDL_INIT_EVERYTHING)) {

      printf ("SDL_Init failed: %s\n", SDL_GetError());
    }
    else {

      /* Create mainwindow */
      l_psWindow = SDL_CreateWindow ("FX_RadialBlur",
                                      SDL_WINDOWPOS_UNDEFINED,
                                      SDL_WINDOWPOS_UNDEFINED,
                                      WINDOW_WIDTH,
                                      WINDOW_HEIGHT,
                                      SDL_FLAG);
      if (NULL == l_psWindow) {

        printf ("SDL_CreateWindow failed: %s\n", SDL_GetError());
      }
      else {

        /* Create renderer to window */
        l_psRenderer = SDL_CreateRenderer (l_psWindow, -1, SDL_RENDERER_ACCELERATED);
        if (NULL == l_psRenderer) {

          printf ("SDL_CreateRenderer failed: %s\n", SDL_GetError());
        }
        else {

          /* Load image into surface */
          l_psSurface = IMG_Load (IMG_PATH);
          if (NULL == l_psSurface) {

            printf("IMG_Load failed: %s\n", IMG_GetError());
          }
          else {

            /* Create texture for renderer */
            l_psTexture = SDL_CreateTexture (l_psRenderer, SDL_PIXELFORMAT_ABGR8888, SDL_TEXTUREACCESS_STREAMING, WINDOW_WIDTH, WINDOW_HEIGHT);
            if (NULL == l_psRenderer) {

              printf ("SDL_CreateTexture failed: %s\n", SDL_GetError());
            }
            else {

              /* Lock surface if need */
              if (SDL_MUSTLOCK (l_psSurface)) {

                if (0 > SDL_LockSurface (l_psSurface)) {

                  printf ("SDL_LockSurface failed: %s\n", SDL_GetError());
                }
                else {

                  /* Nothing to do */
                }
              }
              else {

                /* Allocate memory for texture */
                l_paui32Texture = malloc (l_psSurface->h*l_psSurface->w*sizeof (unsigned long));
                if (NULL == l_paui32Texture) {


                  printf ("malloc l_paui32Texture failed !\n");
                }
                else {

                  /* Clear texture buffer */
                  memset (l_paui32Texture, 0, l_psSurface->h*l_psSurface->w*sizeof (unsigned long));

                  /* For each line */
                  for (l_i16PosY = 0; l_i16PosY < l_psSurface->h; l_i16PosY++) {

                    /* For each column */
                    for (l_i16PosX = 0; l_i16PosX < l_psSurface->w; l_i16PosX++) {

                      /* Copy to pixel to texture buffer  */
                      l_ui32Pixel = (*(unsigned long*)(l_psSurface->pixels + l_i16PosY*(l_psSurface->pitch) + (l_i16PosX * (l_psSurface->pitch/l_psSurface->w))))&0x00FFFFFF;
                      l_paui32Texture[(l_i16PosY*l_psSurface->w) + l_i16PosX] = l_ui32Pixel;

                    }
                  }

                  /* Unlock surface if need */
                  if (SDL_MUSTLOCK(l_psSurface)) {

                    SDL_UnlockSurface(l_psSurface);
                  }
                  else {

                    /* Nothing to do */
                  }

                  /* Main loop */
                  while (l_ui8Loop) {

                    /* If event pending */
                    while (1 == SDL_PollEvent (&l_sEvent)) {

                      switch (l_sEvent.type) {

                        case SDL_QUIT: {

                          l_ui8Loop = 0;

                          break;
                        }

                        case SDL_KEYDOWN: {

                          //if (l_sEvent.key.keysym.sym == SDLK_ESCAPE) {

                            l_ui8Loop = 0;
                         // }

                          break;
                        }

                        default :{

                          break;
                        }
                      }
                    }

#ifdef CLEAR_BACKGROUND
                    SDL_SetRenderDrawColor (l_psRenderer, 0, 0, 0, 0);
                    SDL_RenderClear (l_psRenderer);
#endif /* CLEAR_BACKGROUND */

                    /* Lock texture for update */
                    if (0 == SDL_LockTexture (l_psTexture, NULL, &l_pvBackBuffer, &l_iPitch)) {

                      memset (l_pvBackBuffer, 0, WINDOW_WIDTH*WINDOW_HEIGHT*sizeof (unsigned long));

                      /* Make distorsion into texture */
                      for (l_i16PosY = 0; l_i16PosY < WINDOW_HEIGHT; l_i16PosY++) {

                        for (l_i16PosX = 0; l_i16PosX < WINDOW_WIDTH; l_i16PosX++) {

                          unsigned int xx = l_i16PosX + l_ui16OffX;//(WINDOW_WIDTH>>2)*sin (l_fAngle);
                          unsigned int yy = l_i16PosY;// + (WINDOW_HEIGHT>>3)*cos (l_fAngle);

                          l_ui32Pixel = l_paui32Texture[((yy % l_psSurface->h) * l_psSurface->w) + (xx % l_psSurface->w)];
                          ((unsigned long*)l_pvBackBuffer)[(l_i16PosY * WINDOW_WIDTH) + l_i16PosX] = l_ui32Pixel;
                        }
                      }

                      /* Apply radial blur on backbuffer */
                      vRenderer (l_pvBackBuffer,
                                  (WINDOW_WIDTH>>1) + (WINDOW_WIDTH/3)*sin (l_fAngle)*cos (-3.5*l_fAngle),
                                  (WINDOW_HEIGHT>>1) + (WINDOW_HEIGHT>>2)*cos (l_fAngle*4)*cos (l_fAngle/2.0),
                                  0.991f);

                      /* Move radial blur */
                      l_fAngle += 0.002;
                      l_ui16OffX = (l_ui16OffX + 1) % WINDOW_WIDTH;

                      SDL_Delay(10);

                      /* Unlock texture */
                      SDL_UnlockTexture (l_psTexture);

                      /* Update video buffer */
                      SDL_RenderCopy (l_psRenderer, l_psTexture, NULL, NULL);

                      /* Flip back buffer */
                      SDL_RenderPresent (l_psRenderer);
                    }
                    else {

                      /* Nothing to do */
                    }
                  }

                  free (l_paui32Texture);

                  l_iReturn = EXIT_SUCCESS;
                }
              }

              /* Destroy SDL ressources */
              SDL_DestroyTexture (l_psTexture);
            }

            /* Destroy SDL ressources */
            SDL_FreeSurface (l_psSurface);
          }

          /* Destroy SDL ressources */
          SDL_DestroyRenderer (l_psRenderer);
        }

        /* Destroy SDL ressources */
        SDL_DestroyWindow (l_psWindow);
      }

      /* Deinit SDL */
      SDL_Quit ();
    }

    /* Free memory */
    free (l_paui32BackBuffer);
  }

  return (l_iReturn);
}


/* ============================================================================
                               Private functions
============================================================================ */

/* =============================================================================
Function    :   vRenderer

Description :   Radial blur renderer.

                Schema des octants du radial blur

                        \ 4|3 /       <= Vers le haut de l'ecran (y = 0 )
                         \ | /
                        5 \|/ 2
                        ---|---
                        6 /|\ 1
                         / | \
                        / 7|0 \

Parameters  :   p_paui32Source =,
                p_pu16Centrex = ,
                p_u16Centrey = ,
                p_fLevel.

Return      :   None.
============================================================================= */
void vRenderer (unsigned long* p_paui32Source, short p_pu16Centrex, short p_u16Centrey, float p_fLevel) {

  /* Locals variables declaration */
  register unsigned long l_ui32PixelColor;
  register unsigned long r,g,b;
  register unsigned long tol;
  long l_lIdx;
  long xx, yy;
  long xx2, yy2;
  int col;
  long iLevel;

  /* Initialisation des parametres du radial blur */
  col = 65536;
  iLevel = 259 * p_fLevel;
  tol = 65535;

  /* Ligne separant octant 0 et 1 */
#ifdef LINE01
  for (l_lIdx = 1; l_lIdx < WINDOW_WIDTH; l_lIdx++) {

    /* Diagonale (+1, +1) */
    xx = p_pu16Centrex + l_lIdx;
    yy = p_u16Centrey + l_lIdx;
    if (yy >= WINDOW_HEIGHT) yy = WINDOW_HEIGHT - 1;
    if (xx >= WINDOW_WIDTH)  xx = WINDOW_WIDTH - 1;

    /* Get pixel color from (XX, YY) */
    l_ui32PixelColor = p_paui32Source[xx + (yy * WINDOW_WIDTH)];

    /* R,G and B separation */
    r = col * (l_ui32PixelColor>>16);
    g = col * ((l_ui32PixelColor>>8) & 0xff);
    b = col * (l_ui32PixelColor & 0xff) ;

    /* Get pixel color from (XX-1, YY-1) */
    l_ui32PixelColor = p_paui32Source[xx - 1 + (yy - 1)*WINDOW_WIDTH];

    /* R,G and B separation and add with last results */
    r += (tol * ((((l_ui32PixelColor>>16) & 0xff) * iLevel)>>8 ))  & 0xffff0000;
    g += (tol * ((((l_ui32PixelColor>>8)  & 0xff) * iLevel)>>8 ))  & 0xffff0000;
    b += (tol * (((l_ui32PixelColor & 0xff) * iLevel)>>8)) & 0xffff0000;

    /* Color clipping */
    if(r>0xff0000)  r=0xff0000;    g=g>>8;
    if(g>0xff00)    g=0xff00;      b=b>>16;
    if(b>255)        b=255;

    /* Copy to (XX,YY) */
    p_paui32Source[xx + yy * WINDOW_WIDTH] = r|g|b;
  }
#endif

  /* Ligne separant octant 1 et 2 */
#ifdef LINE12
  for(l_lIdx = 1; l_lIdx < WINDOW_WIDTH; l_lIdx++) {

    /* Horizontale (+1, 0) */
    xx = p_pu16Centrex + l_lIdx;
    yy = p_u16Centrey;

    if (xx >= WINDOW_WIDTH) xx = WINDOW_WIDTH - 1;

    /* Get pixel color from (XX,YY) */
    l_ui32PixelColor = p_paui32Source[xx + yy*WINDOW_WIDTH];

    /* R,G and B separation */
    r = col * (l_ui32PixelColor>>16);
    g = col * ((l_ui32PixelColor>>8)&0xff);
    b = col * (l_ui32PixelColor&0xff) ;

    /* Get pixel color from (XX - 1, YY) */
    l_ui32PixelColor=p_paui32Source[xx - 1 + yy*WINDOW_WIDTH];

    /* R,G and B separation and add with last results */
    r += (tol * ((((l_ui32PixelColor>>16) &0xff  )*iLevel)>>8 ))  &0xffff0000;
    g += (tol * ((((l_ui32PixelColor>>8)  &0xff  )*iLevel)>>8 ))  &0xffff0000;
    b += (tol * ((( l_ui32PixelColor      &0xff  )*iLevel)>>8 ))  &0xffff0000;

    /* Color clipping */
    if(r>0xff0000)  r=0xff0000;    g=g>>8;
    if(g>0xff00)    g=0xff00;      b=b>>16;
    if(b>255)        b=255;

    /* Copy to (XX,YY) */
    p_paui32Source[xx + yy * WINDOW_WIDTH] = r|g|b;
  }
#endif

  /* Ligne separant octant 2 et 3 */
#ifdef LINE23
  for (l_lIdx = 1; l_lIdx < WINDOW_WIDTH; l_lIdx++) {

    /* Diagonale (+1,-1) */
    xx = p_pu16Centrex + l_lIdx;
    yy = p_u16Centrey - l_lIdx;
    if (yy < 0) yy = 0;
    if (xx >= WINDOW_WIDTH) xx = WINDOW_WIDTH - 1;

    /* Get pixel color from (XX, YY) */
    l_ui32PixelColor = p_paui32Source[xx + yy*WINDOW_WIDTH];

    /* R,G and B separation */
    r = col * (l_ui32PixelColor >> 16);
    g = col * ((l_ui32PixelColor >> 8) & 0xff);
    b = col * (l_ui32PixelColor & 0xff) ;

    /* Get pixel color from (XX-1, YY+1) */
    l_ui32PixelColor=p_paui32Source[xx /*+ WINDOW_WIDTH*/ - 1 + (yy + 1) *WINDOW_WIDTH];

    /* R,G and B separation and add with last results */
    r += (tol*( (((l_ui32PixelColor>>16) &0xff  )*iLevel)>>8 ))  &0xffff0000;
    g += (tol*( (((l_ui32PixelColor>>8)  &0xff  )*iLevel)>>8 ))  &0xffff0000;
    b += (tol*( (( l_ui32PixelColor      &0xff  )*iLevel)>>8 ))  &0xffff0000;

    /* Color clipping */
    if(r>0xff0000)  r=0xff0000;    g=g>>8;
    if(g>0xff00)    g=0xff00;      b=b>>16;
    if(b>0xff)      b=0xff;

    /* Copy to (XX,YY) */
    p_paui32Source[xx + yy * WINDOW_WIDTH] = r|g|b;
  }

#endif

  /* Ligne separant octant 3 et 4 */
#ifdef LINE34
  for (l_lIdx = 1; l_lIdx < WINDOW_WIDTH; l_lIdx++) {

    /* Verticale (0, -1) */
    xx = p_pu16Centrex;
    yy = p_u16Centrey - l_lIdx;
    if (yy < 0) yy = 0;

    /* Get pixel color from (XX, YY) */
    l_ui32PixelColor=p_paui32Source[xx + yy*WINDOW_WIDTH];

    /* R,G and B separation */
    r=col*(l_ui32PixelColor>>16);
    g=col*((l_ui32PixelColor>>8)&0xff);
    b=col*(l_ui32PixelColor&0xff) ;

    /* Get pixel color from (XX,YY+1) */
    l_ui32PixelColor=p_paui32Source[xx + /*WINDOW_WIDTH+*/ (yy + 1)*WINDOW_WIDTH];

    /* R,G and B separation and add with last results */
    r+=(tol*( (((l_ui32PixelColor>>16) &0xff  )*iLevel)>>8 ))  &0xffff0000;
    g+=(tol*( (((l_ui32PixelColor>>8)  &0xff  )*iLevel)>>8 ))  &0xffff0000;
    b+=(tol*( (( l_ui32PixelColor      &0xff  )*iLevel)>>8 ))  &0xffff0000;

    /* Color clipping */
    if(r>0xff0000)  r=0xff0000;    g=g>>8;
    if(g>0xff00)    g=0xff00;      b=b>>16;
    if(b>255)        b=255;

    /* Copy to (XX,YY) */
    p_paui32Source[xx + yy * WINDOW_WIDTH] = r|g|b;
  }

#endif

  /* Ligne separant octant 5 er 4 */
#ifdef LINE45
  for(l_lIdx = 1; l_lIdx < WINDOW_WIDTH; l_lIdx++) {

    /* Diagonale (-1,-1) */
    xx = p_pu16Centrex - l_lIdx;
    yy = p_u16Centrey - l_lIdx;
    if (yy < 0) yy = 0;
    if (xx < 0) xx = 0;

    /* Get pixel color from (XX,YY) */
    l_ui32PixelColor=p_paui32Source[xx + yy*WINDOW_WIDTH];

    /* R,G and B separation */
    r=col*(l_ui32PixelColor>>16);
    g=col*((l_ui32PixelColor>>8)&0xff);
    b=col*(l_ui32PixelColor&0xff) ;

    /* Get pixel color from (XX + 1, YY + 1) */
    l_ui32PixelColor=p_paui32Source[xx /*+ WINDOW_WIDTH*/+ 1 + (yy + 1)*WINDOW_WIDTH];

    /* R,G and B separation and add with last results */
    r+=(tol*( (((l_ui32PixelColor>>16) &0xff  )*iLevel)>>8 ))  &0xffff0000;
    g+=(tol*( (((l_ui32PixelColor>>8)  &0xff  )*iLevel)>>8 ))  &0xffff0000;
    b+=(tol*( (( l_ui32PixelColor      &0xff  )*iLevel)>>8 ))  &0xffff0000;

    /* Color clipping */
    if(r>0xff0000)  r=0xff0000;    g=g>>8;
    if(g>0xff00)    g=0xff00;      b=b>>16;
    if(b>255)        b=255;

    /* Copy to (XX,YY) */
    p_paui32Source[xx + yy * WINDOW_WIDTH] = r|g|b;
  }
#endif

  /* Ligne separant octant 5 et 6 */
#ifdef LINE56
  for (l_lIdx = 1; l_lIdx < WINDOW_WIDTH; l_lIdx++) {

    /* Horizontale (-1,0) */
    xx=p_pu16Centrex - l_lIdx;
    yy=p_u16Centrey;
    if (xx < 0) xx = 0;

    /* Get pixel color from (XX, YY) */
    l_ui32PixelColor=p_paui32Source[xx + yy*WINDOW_WIDTH];

    /* R,G and B separation */
    r=col*(l_ui32PixelColor>>16);
    g=col*((l_ui32PixelColor>>8)&0xff);
    b=col*(l_ui32PixelColor&0xff) ;

    /* Get pixel color from (XX + 1, YY) */
    l_ui32PixelColor=p_paui32Source[xx + 1 + yy*WINDOW_WIDTH];

    /* R,G and B separation and add with last results */
    r+=(tol*( (((l_ui32PixelColor>>16) &0xff  )*iLevel)>>8 ))  &0xffff0000;
    g+=(tol*( (((l_ui32PixelColor>>8)  &0xff  )*iLevel)>>8 ))  &0xffff0000;
    b+=(tol*( (( l_ui32PixelColor      &0xff  )*iLevel)>>8 ))  &0xffff0000;

    /* Color clipping */
    if(r>0xff0000)  r=0xff0000;    g=g>>8;
    if(g>0xff00)    g=0xff00;      b=b>>16;
    if(b>255)        b=255;

    /* Copy to (XX,YY) */
    p_paui32Source[xx + yy * WINDOW_WIDTH] = r|g|b;
  }
#endif

  /* Ligne separant octant 6 et 7 */
#ifdef LINE67
  for (l_lIdx = 1; l_lIdx < WINDOW_WIDTH; l_lIdx++) {

    /* Diagonale (-1,+1) */
    xx = p_pu16Centrex - l_lIdx;
    yy = p_u16Centrey + l_lIdx;
    if (xx < 0) xx = 0;
    if (yy >= WINDOW_HEIGHT) yy = WINDOW_HEIGHT - 1;

    /* Get pixel color from (XX,YY) */
    l_ui32PixelColor=p_paui32Source[xx + yy*WINDOW_WIDTH];

    /* R,G and B separation */
    r=col*(l_ui32PixelColor>>16);
    g=col*((l_ui32PixelColor>>8)&0xff);
    b=col*(l_ui32PixelColor&0xff) ;

    /* Get pixel color from (XX-1,YY-1) */
    l_ui32PixelColor=p_paui32Source[xx/*-WINDOW_WIDTH*/ + 1 +(yy - 1)*WINDOW_WIDTH];

    /* R,G and B separation and add with last results */
    r+=(tol*( (((l_ui32PixelColor>>16) &0xff  )*iLevel)>>8 ))  &0xffff0000;
    g+=(tol*( (((l_ui32PixelColor>>8)  &0xff  )*iLevel)>>8 ))  &0xffff0000;
    b+=(tol*( (( l_ui32PixelColor      &0xff  )*iLevel)>>8 ))  &0xffff0000;

    /* Color clipping */
    if(r>0xff0000)  r=0xff0000;    g=g>>8;
    if(g>0xff00)    g=0xff00;      b=b>>16;
    if(b>255)        b=255;

    /* Copy to (XX,YY) */
    p_paui32Source[xx + yy * WINDOW_WIDTH] = r|g|b;
  }
#endif

  /* Ligne separant octant 7 et 0 */
#ifdef LINE70
  for (l_lIdx = 1; l_lIdx < WINDOW_WIDTH; l_lIdx++) {

    /* Verticale (0,+1) */
    xx = p_pu16Centrex;
    yy = p_u16Centrey + l_lIdx;
    if (yy >= WINDOW_HEIGHT) yy = WINDOW_HEIGHT - 1;

    /* Get pixel color from (XX,YY) */
    l_ui32PixelColor=p_paui32Source[xx+yy*WINDOW_WIDTH];

    /* R,G and B separation */
    r=col*(l_ui32PixelColor>>16);
    g=col*((l_ui32PixelColor>>8)&0xff);
    b=col*(l_ui32PixelColor&0xff) ;

    /* Get pixel color from (XX, YY - 1) */
    l_ui32PixelColor=p_paui32Source[xx /*- WINDOW_WIDTH*/ + (yy - 1)*WINDOW_WIDTH];

    /* R,G and B separation and add with last results */
    r+=(tol*( (((l_ui32PixelColor>>16) &0xff  )*iLevel)>>8 ))  &0xffff0000;
    g+=(tol*( (((l_ui32PixelColor>>8)  &0xff  )*iLevel)>>8 ))  &0xffff0000;
    b+=(tol*( (( l_ui32PixelColor      &0xff  )*iLevel)>>8 ))  &0xffff0000;

    /* Color clipping */
    if(r>0xff0000)  r=0xff0000;    g=g>>8;
    if(g>0xff00)    g=0xff00;      b=b>>16;
    if(b>255)        b=255;

    /* Copy to (XX,YY) */
    p_paui32Source[xx + yy * WINDOW_WIDTH] = r|g|b;
  }
#endif

  /* Gestion des octants 0 et 1 */
  l_lIdx = 1;
  for (yy2 = p_u16Centrey + 1; yy2 < WINDOW_HEIGHT; yy2++) {

    /* Octant 0 */
#ifdef OCTANT0
    for (xx2 = p_pu16Centrex + 1; xx2 < p_pu16Centrex + l_lIdx; xx2++)  {

      if (xx2 >= WINDOW_WIDTH) xx = WINDOW_WIDTH - 1; else xx=xx2;
      if (yy2 >= WINDOW_HEIGHT) yy = WINDOW_HEIGHT - 1; else yy=yy2;

      /* Get pixel color from (XX, YY) */
      l_ui32PixelColor = p_paui32Source[xx + yy*WINDOW_WIDTH];

      /* R,G and B separation */
      r=col*(l_ui32PixelColor>>16);
      g=col*((l_ui32PixelColor>>8)&0xff);
      b=col*(l_ui32PixelColor&0xff) ;
      tol=((xx-p_pu16Centrex)<<16)/(yy-p_u16Centrey);

      /* Get pixel color from (XX - 1, YY - 1) */
      l_ui32PixelColor = p_paui32Source[xx - 1 + ((yy - 1)*WINDOW_WIDTH)/* - (WINDOW_WIDTH + 1)*/];

      r+=(tol*( (((l_ui32PixelColor>>16) &0xff  )*iLevel)>>8 ))  &0xffff0000;
      g+=(tol*( (((l_ui32PixelColor>>8)  &0xff  )*iLevel)>>8 ))  &0xffff0000;
      b+=(tol*( (( l_ui32PixelColor      &0xff  )*iLevel)>>8 ))  &0xffff0000;

      /* Get pixel color from (XX, YY - 1) */
      l_ui32PixelColor = p_paui32Source[xx + ((yy - 1)*WINDOW_WIDTH)/* - WINDOW_WIDTH*/];

      tol=65536-tol;
      r+=(tol*( (((l_ui32PixelColor>>16)&0xff  )*iLevel)>>8 ))  &0xffff0000;
      g+=(tol*( (((l_ui32PixelColor>>8)  &0xff  )*iLevel)>>8 ))  &0xffff0000;
      b+=(tol*( (( l_ui32PixelColor      &0xff  )*iLevel)>>8 ))  &0xffff0000 ;

      /* Color clipping */
      if(r>0xff0000)  r=0xff0000;    g=g>>8;
      if(g>0xff00)    g=0xff00;      b=b>>16;
      if(b>255)        b=255;

      /* Copy to (XX,YY) */
      p_paui32Source[xx + yy * WINDOW_WIDTH] = r|g|b;
    }
#endif

     /* Octant 1 */
#ifdef OCTANT1
    for (xx2 = p_pu16Centrex + l_lIdx + 1; xx2 < WINDOW_WIDTH; xx2++) {

      if (xx2 >= WINDOW_WIDTH) xx = WINDOW_WIDTH - 1; else xx=xx2;
      if (yy2 >= WINDOW_HEIGHT) yy = WINDOW_HEIGHT - 1; else yy=yy2;

      /* Get pixel color from (XX,YY) */
      l_ui32PixelColor = p_paui32Source[xx + yy*WINDOW_WIDTH];

      /* R,G and B separation */
      r=col*(l_ui32PixelColor>>16);    //col = define
      g=col*((l_ui32PixelColor>>8)&0xff);
      b=col*(l_ui32PixelColor&0xff) ;
      tol=((yy-p_u16Centrey)<<16)/(xx-p_pu16Centrex);    //vy/vx

      /* Get pixel color from (XX - 1, YY - 1) */
      l_ui32PixelColor = p_paui32Source[xx - 1 + (yy - 1)*WINDOW_WIDTH/* - (WINDOW_WIDTH+1)*/];

      r+=(tol*( (((l_ui32PixelColor>>16) &0xff  )*iLevel)>>8 ))  &0xffff0000;
      g+=(tol*( (((l_ui32PixelColor>>8)  &0xff  )*iLevel)>>8 ))  &0xffff0000;
      b+=(tol*( (( l_ui32PixelColor      &0xff  )*iLevel)>>8 ))  &0xffff0000;

      /* Get pixel color from (XX-1,YY) */
      l_ui32PixelColor = p_paui32Source[xx + yy*WINDOW_WIDTH - 1];

      tol=65536-tol;
      r+=(tol*( (((l_ui32PixelColor>>16) &0xff  )*iLevel)>>8 ))  &0xffff0000;
      g+=(tol*( (((l_ui32PixelColor>>8)  &0xff  )*iLevel)>>8 ))  &0xffff0000;
      b+=(tol*( (( l_ui32PixelColor      &0xff  )*iLevel)>>8 ))  &0xffff0000;

      /* Color clipping */
      if(r>0xff0000)  r=0xff0000;    g=g>>8;
      if(g>0xff00)    g=0xff00;      b=b>>16;
      if(b>255)        b=255;

      /* Copy to (XX,YY) */
      p_paui32Source[xx + yy * WINDOW_WIDTH] = r|g|b;
    }
#endif

    l_lIdx++;
  }


  /* Gestion des octants 7 et 6 */
  l_lIdx = 1;
  for (yy2 = p_u16Centrey + 1; yy2 < WINDOW_HEIGHT; yy2++) {

    /* Octant 7 */
#ifdef OCTANT7
    for (xx2 = p_pu16Centrex - 1; xx2 > p_pu16Centrex - l_lIdx; xx2--) {

      if (xx2 < 0) xx = 0; else xx=xx2;
      if (yy2 >= WINDOW_HEIGHT) yy = WINDOW_HEIGHT - 1; else yy=yy2;


      /* Get pixel color from (XX,YY) */
      l_ui32PixelColor = p_paui32Source[xx + yy*WINDOW_WIDTH];

      /* R,G and B separation */
      r=col*(l_ui32PixelColor>>16);    //col = define
      g=col*((l_ui32PixelColor>>8)&0xff) ;
      b=col*(l_ui32PixelColor&0xff) ;
      tol=((p_pu16Centrex-xx)<<16)/(yy-p_u16Centrey);    //-vx/vy

      /* Get pixel color from (XX+1,YY-1) */
      l_ui32PixelColor = p_paui32Source[xx + 1 + (yy - 1)*WINDOW_WIDTH/* -(WINDOW_WIDTH-1)*/];

      r+=(tol*( (((l_ui32PixelColor>>16) &0xff  )*iLevel)>>8 ))  &0xffff0000;
      g+=(tol*( (((l_ui32PixelColor>>8)  &0xff  )*iLevel)>>8 ))  &0xffff0000;
      b+=(tol*( (( l_ui32PixelColor      &0xff  )*iLevel)>>8 ))  &0xffff0000;

      /* Get pixel color from (XX,YY-1) */
      l_ui32PixelColor=p_paui32Source[xx + (yy - 1)*WINDOW_WIDTH/* - WINDOW_WIDTH*/];

      tol=65536-tol;
      r+=(tol*( (((l_ui32PixelColor>>16) &0xff  )*iLevel)>>8 ))  &0xffff0000;
      g+=(tol*( (((l_ui32PixelColor>>8)  &0xff  )*iLevel)>>8 ))  &0xffff0000;
      b+=(tol*( (( l_ui32PixelColor      &0xff  )*iLevel)>>8 ))  &0xffff0000;

      /* Color clipping */
      if(r>0xff0000)  r=0xff0000;    g=g>>8;
      if(g>0xff00)    g=0xff00;      b=b>>16;
      if(b>255)        b=255;


      /* Copy to (XX,YY) */
      p_paui32Source[xx + yy * WINDOW_WIDTH] = r|g|b;
    }
#endif

    /* Octant 6 */
#ifdef OCTANT6
    for (xx2 = p_pu16Centrex - l_lIdx -1 ;xx2 >= 0; xx2--) {

      if (xx2 < 0) xx = 0; else xx=xx2;
      if (yy2 >= WINDOW_HEIGHT) yy = WINDOW_HEIGHT - 1; else yy=yy2;


      /* Get pixel color from (XX,YY) */
      l_ui32PixelColor = p_paui32Source[xx + yy*WINDOW_WIDTH];

      /* R,G and B separation */
      r=col*(l_ui32PixelColor>>16);    //col = define
      g=col*((l_ui32PixelColor>>8)&0xff) ;
      b=col*(l_ui32PixelColor&0xff) ;
      tol=((p_u16Centrey-yy)<<16)/(xx-p_pu16Centrex);    //-vy/vx

      /* Get pixel color from (XX+1,YY-1) */
      l_ui32PixelColor = p_paui32Source[xx + 1 + (yy - 1)*WINDOW_WIDTH /*- (WINDOW_WIDTH - 1)*/ ];

      r+=(tol*( (((l_ui32PixelColor>>16) &0xff  )*iLevel)>>8 ))  &0xffff0000;
      g+=(tol*( (((l_ui32PixelColor>>8)  &0xff  )*iLevel)>>8 ))  &0xffff0000;
      b+=(tol*( (( l_ui32PixelColor      &0xff  )*iLevel)>>8 ))  &0xffff0000;

      /* Get pixel color from (XX+1,YY) */
      l_ui32PixelColor=p_paui32Source[xx + yy*WINDOW_WIDTH + 1];

      tol=65536-tol;
      r+=(tol*( (((l_ui32PixelColor>>16) &0xff  )*iLevel)>>8 ))  &0xffff0000;
      g+=(tol*( (((l_ui32PixelColor>>8)  &0xff  )*iLevel)>>8 ))  &0xffff0000;
      b+=(tol*( (( l_ui32PixelColor      &0xff  )*iLevel)>>8 ))  &0xffff0000;

      /* Color clipping */
      if(r>0xff0000)  r=0xff0000;    g=g>>8;
      if(g>0xff00)    g=0xff00;      b=b>>16;
      if(b>255)        b=255;


      /* Copy to (XX,YY) */
      p_paui32Source[xx + yy * WINDOW_WIDTH] = r|g|b;
    }
#endif

    l_lIdx++;
  }

  /* Gestion des octants 3 et 2 */
  l_lIdx = 1;
  for (yy2 = p_u16Centrey - 1; yy2 >= 0; yy2--) {

    /* Octant 3 */
#ifdef OCTANT3
    for (xx2 = p_pu16Centrex + 1; xx2 < p_pu16Centrex + l_lIdx; xx2++) {

      if (xx2 >= WINDOW_WIDTH) xx = WINDOW_WIDTH - 1; else xx=xx2;
      if (yy2 < 0) yy = 0; else yy=yy2;

      /* Get pixel color from (XX,YY) */
      l_ui32PixelColor = p_paui32Source[xx + yy*WINDOW_WIDTH];

      /* R,G and B separation */
      r=col*(l_ui32PixelColor>>16);    //col = define
      g=col*((l_ui32PixelColor>>8)&0xff) ;
      b=col*(l_ui32PixelColor&0xff) ;
      tol=((xx-p_pu16Centrex)<<16)/(p_u16Centrey-yy);    //-vx/vy

      /* Get pixel color from (XX - 1,YY + 1) */
      l_ui32PixelColor = p_paui32Source[xx - 1 + (yy + 1)*WINDOW_WIDTH/*+(WINDOW_WIDTH-1)*/];

      r+=(tol*( (((l_ui32PixelColor>>16) &0xff  )*iLevel)>>8 ))  &0xffff0000;
      g+=(tol*( (((l_ui32PixelColor>>8)  &0xff  )*iLevel)>>8 ))  &0xffff0000;
      b+=(tol*( (( l_ui32PixelColor      &0xff  )*iLevel)>>8 ))  &0xffff0000;

      /* Get pixel color from (XX,YY + 1) */
      l_ui32PixelColor = p_paui32Source[xx + (yy + 1)*WINDOW_WIDTH/*+ WINDOW_WIDTH*/];

      tol=65536-tol;
      r+=(tol*( (((l_ui32PixelColor>>16) &0xff  )*iLevel)>>8 ))  &0xffff0000;
      g+=(tol*( (((l_ui32PixelColor>>8)  &0xff  )*iLevel)>>8 ))  &0xffff0000;
      b+=(tol*( (( l_ui32PixelColor      &0xff  )*iLevel)>>8 ))  &0xffff0000;

      /* Color clipping */
      if(r>0xff0000)  r=0xff0000;    g=g>>8;
      if(g>0xff00)    g=0xff00;      b=b>>16;
      if(b>255)        b=255;


      /* Copy to (XX,YY) */
      p_paui32Source[xx + yy * WINDOW_WIDTH] = r|g|b;

    }
#endif

    /* Octant 2 */
    //for(xx=finx+1;xx<WINDOW_WIDTH;xx++) {
#ifdef OCTANT2
    for(xx2 = p_pu16Centrex + l_lIdx + 1; xx2 < WINDOW_WIDTH; xx2++) {

      if (xx2 >= WINDOW_WIDTH) xx = WINDOW_WIDTH - 1; else xx=xx2;
      if (yy2 < 0) yy = 0; else yy=yy2;

      /* Get pixel color from (XX,YY) */
      l_ui32PixelColor = p_paui32Source[xx+yy*WINDOW_WIDTH];

      /* R,G and B separation */
      r=col*(l_ui32PixelColor>>16);    //col = define
      g=col*((l_ui32PixelColor>>8)&0xff) ;
      b=col*(l_ui32PixelColor&0xff) ;

      tol=((p_u16Centrey-yy)<<16)/(xx-p_pu16Centrex);    //-vy/vx

      /* Get pixel color from (XX,YY-1) */
      l_ui32PixelColor = p_paui32Source[xx - 1 + (yy + 1)*WINDOW_WIDTH/* + (WINDOW_WIDTH - 1)*/];

      r+=(tol*( (((l_ui32PixelColor>>16) &0xff  )*iLevel)>>8 ))  &0xffff0000;
      g+=(tol*( (((l_ui32PixelColor>>8)  &0xff  )*iLevel)>>8 ))  &0xffff0000;
      b+=(tol*( (( l_ui32PixelColor      &0xff  )*iLevel)>>8 ))  &0xffff0000;

      /* Get pixel color from (XX-1,YY) */
      l_ui32PixelColor=p_paui32Source[xx + yy*WINDOW_WIDTH - 1];

      tol=65536-tol;
      r+=(tol*( (((l_ui32PixelColor>>16) &0xff  )*iLevel)>>8 ))  &0xffff0000;
      g+=(tol*( (((l_ui32PixelColor>>8)  &0xff  )*iLevel)>>8 ))  &0xffff0000;
      b+=(tol*( (( l_ui32PixelColor      &0xff  )*iLevel)>>8 ))  &0xffff0000;

      /* Color clipping */
      if(r>0xff0000)  r=0xff0000;    g=g>>8;
      if(g>0xff00)    g=0xff00;      b=b>>16;
      if(b>255)        b=255;


      /* Copy to (XX,YY) */
      p_paui32Source[xx + yy * WINDOW_WIDTH] = r|g|b;
    }

#endif

    l_lIdx++;
  }

  /* Gestion des octants 4 et 5 */
  l_lIdx = 1;
  for (yy2 = p_u16Centrey - 1; yy2 >= 0; yy2--) {

    /* Octant 4 */
#ifdef OCTANT4
     for (xx2 = p_pu16Centrex - 1; xx2 > p_pu16Centrex - l_lIdx; xx2--) {

      if (xx2 < 0) xx = 0; else xx = xx2;
      if (yy2 < 0) yy = 0; else yy = yy2;

      /* Get pixel color from (XX,YY) */
      l_ui32PixelColor = p_paui32Source[xx + yy * WINDOW_WIDTH];

      /* R,G and B separation */
      r = col * (l_ui32PixelColor  >> 16);
      g = col * ((l_ui32PixelColor >> 8) & 0xff);
      b = col * (l_ui32PixelColor        & 0xff);
      tol = ((xx - p_pu16Centrex) << 16) / (yy - p_u16Centrey);

      /* Get pixel color from (XX + 1, YY + 1) */
      l_ui32PixelColor = p_paui32Source[xx + 1 + (yy + 1)*WINDOW_WIDTH];

      r += (tol * ((((l_ui32PixelColor >> 16) & 0xff) * iLevel) >> 8)) & 0xffff0000;
      g += (tol * ((((l_ui32PixelColor >> 8)  & 0xff) * iLevel) >> 8)) & 0xffff0000;
      b += (tol * ((( l_ui32PixelColor        & 0xff) * iLevel) >> 8)) & 0xffff0000;

      /* Get pixel color from (XX, YY + 1) */
      l_ui32PixelColor=p_paui32Source[xx + (yy + 1) * WINDOW_WIDTH];

      tol = 65536 - tol;
      r += (tol * ((((l_ui32PixelColor >> 16) & 0xff) * iLevel) >> 8 ))  & 0xffff0000;
      g += (tol * ((((l_ui32PixelColor >> 8)  & 0xff) * iLevel) >> 8 ))  & 0xffff0000;
      b += (tol * ((( l_ui32PixelColor        & 0xff) * iLevel) >> 8 ))  & 0xffff0000;

      /* Color clipping */
      if (r > 0xff0000) r=0xff0000;

      g>>=8;
      if (g > 0xff00) g=0xff00;

      b>>=16;
      if (b > 0xff) b=0xff;

      /* Copy to (XX,YY) */
      p_paui32Source[xx + yy * WINDOW_WIDTH] = r|g|b;
    }
#endif

    /* Octant 5 */
#ifdef OCTANT5
    for (xx2 = p_pu16Centrex - l_lIdx - 1; xx2 >= 0; xx2--) {

      if (xx2 < 0) xx = 0; else xx = xx2;
      if (yy2 < 0) yy = 0; else yy = yy2;

      /* Get pixel color from (XX,YY) */
      l_ui32PixelColor = p_paui32Source[xx + yy * WINDOW_WIDTH];

      /* R,G and B separation */
      r = col * ( l_ui32PixelColor >> 16);
      g = col * ((l_ui32PixelColor >> 8) & 0xff);
      b = col * ( l_ui32PixelColor       & 0xff);

      tol = ((yy - p_u16Centrey) << 16) / (xx - p_pu16Centrex);    //vy/vx

      /* Get pixel color from (XX + 1,YY + 1) */
      l_ui32PixelColor = p_paui32Source[xx + 1 + (yy + 1) * WINDOW_WIDTH];

      r += (tol * ((((l_ui32PixelColor>>16) & 0xff) * iLevel) >> 8)) & 0xffff0000;
      g += (tol * ((((l_ui32PixelColor>>8)  & 0xff) * iLevel) >> 8)) & 0xffff0000;
      b += (tol * ((( l_ui32PixelColor      & 0xff) * iLevel) >> 8)) & 0xffff0000;

      /* Get pixel color from (XX + 1,YY) */
      l_ui32PixelColor = p_paui32Source[xx + yy*WINDOW_WIDTH + 1];

      tol = 65536 - tol;
      r += (tol * ((((l_ui32PixelColor>>16) & 0xff) * iLevel) >> 8)) & 0xffff0000;
      g += (tol * ((((l_ui32PixelColor>>8)  & 0xff) * iLevel) >> 8)) & 0xffff0000;
      b += (tol * ((( l_ui32PixelColor      & 0xff) * iLevel) >> 8)) & 0xffff0000;

      /* Color clipping */
      if (r > 0xff0000) r = 0xff0000;

      g = g>>8;
      if (g > 0xff00)   g = 0xff00;

      b = b>>16;
      if (b > 0xff)     b = 0xff;

      /* Copy to (XX,YY) */
      p_paui32Source[xx + yy * WINDOW_WIDTH] = r|g|b;
    }
#endif

    l_lIdx++;
  }
}

