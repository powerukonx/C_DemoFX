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
/* -User specific- */

/* =============================================================================
                          Private defines and typedefs
============================================================================= */
/* FX specific. */
#define SDL_SUBSYSTEM_TO_INIT   SDL_INIT_VIDEO
#define FX_NAME                 "FX_Tunnel3D by UKONX"
#define SCREEN_WIDTH            ((uint16_t)800)
#define SCREEN_HEIGHT           ((uint16_t)600)
#define SCREEN_BPP              ((uint8_t)32)
#define YMAX                    ((uint16_t)(SCREEN_HEIGHT - (SCREEN_HEIGHT / 3)))
#define ALPHA_MASK              ((uint32_t)0xFF000000)
#define RED_MASK                ((uint32_t)0x00FF0000)
#define GREEN_MASK              ((uint32_t)0x0000FF00)
#define BLUE_MASK               ((uint32_t)0x000000FF)
#define TEXTURE_FILENAME        "./texture/ukonx.png"

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
typedef struct {
  float x, y, z;
} sVector_t;

typedef struct {
  uint32_t x, y;
} sPoint_t;

typedef float camera_t[3][3];

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
static void vRenderTunnel3D (void);
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
      g_pSDL_Window = SDL_CreateWindow (FX_NAME, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_FLAG);
      if (NULL != g_pSDL_Window)
      {
        /* Create renderer. */
        g_pSDL_Renderer = SDL_CreateRenderer (g_pSDL_Window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
        if (NULL != g_pSDL_Renderer)
        {
          /* Allocate a new RGB surface. */
          g_pSDL_Screen = SDL_CreateRGBSurface (0, SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP, RED_MASK, GREEN_MASK, BLUE_MASK, ALPHA_MASK);
          if (NULL != g_pSDL_Screen)
          {
            /* Create a texture for a rendering context. */
            g_pSDL_Texture = SDL_CreateTexture (g_pSDL_Renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, SCREEN_WIDTH, SCREEN_HEIGHT);
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
  l_bReturn = true;
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
  l_bReturn = true;
  /* -User specific- */

  return (l_bReturn);
}


/* -User specific- */
/*==============================================================================
Function    :
Describe    :   .
Parameters  :   None.
Returns     :   None.
==============================================================================*/
static void vRenderer (void)
{
  /* Locals variables declaration */
  int l_iReturn = -1;
  int x, y;
  sVector_t d;           // vecteur direction pour chaque rayon
  sVector_t inter;       // point d'intersection entre le rayon et le tunnel

  double a, b, c;       // coefficient du polynome
  double delta;         // discriminant du polynome
  double t, t1, t2;     // t solutions

  int rayon   = SCREEN_HEIGHT;// rayon du tunnel
  int hauteur = 256;  // distance des plans sur Oy

  /* calculs constants pour l'image c = (cx² + cy² - r²) */
  c = (camera.x * camera.x) + (camera.y * camera.y) - (rayon*rayon);

  /* Render by 8x8 */
  for (y = 0; y < H_ECRAN + 1; y += INTERPOL)
  {
    for (x = 0; x < L_ECRAN + 1; x += INTERPOL)
    {
      /* Compute direction */
      d.x = ((double)(x - (L_ECRAN>>1))) / FOV;
      d.y = ((double)(y - (H_ECRAN>>1))) / FOV;
      d.z = 1;

      vMatrixProduct (&d, &d, cam_matrix);

      /* Normalize d */
      normalize (&d);

      /* a = dx² + dy² */
      a = (d.x*d.x) + (d.y*d.y);

      /* b = 2 * (cx * dx + cy * dy) */
      b = 2*(camera.x*d.x + camera.y*d.y);

      /* delta = b² - 4ac */
      delta = (b * b) - 4 * a * c;

      /* delta>0 => 2 solutions ? */
      if (delta > 0)
      {
        /* Compute sqrt */
        delta = sqrt(delta);

        /*       -b - racine(delta)
           t1 = --------------------
                      2a              */
        t1 = (-b - delta) / (2 * a);

        /*       -b + racine(delta)
           t2 = --------------------
                      2a             */
        t2 = (-b + delta) / (2 * a);

        /* t is positive solution */
        t = (t1>0)?t1 : t2;

        /* Compute intersection point between ray and tunnel */
        /* inter = camera + t*direction */
        inter.x = camera.x + t*d.x;
        inter.y = camera.y + t*d.y;
        inter.z = camera.z + t*d.z;

        fog[x][y] = (t/2000.0f);
        if (t>1) t = 1;

        interpol[x][y].x = (unsigned short) (fabs( (1024 * atan2(inter.y, inter.x) ) / M_PI));
        interpol[x][y].y = (unsigned short)  fabs( inter.z );
      }
      else
      {
        fog[x][y] = 0;

        /* There is no intersection */
        interpol[x][y].x = 0;
        interpol[x][y].y = 0;
      }
    }
  }
}


/* =============================================================================
Function    :   normalize

Description :   .

Parameters  :   .

Return      :
============================================================================= */
void normalize (VECTEUR *v)
{
  /* Locals variables declaration */
  float l_fVectorLen;

  /* Compute vector length */
  l_fVectorLen = sqrt((v->x * v->x) + (v->y * v->y) + (v->z * v->z));

  /* Normalize vector */
  v->x = v->x / (l_fVectorLen ? l_fVectorLen : 1);
  v->y = v->y / (l_fVectorLen ? l_fVectorLen : 1);
  v->z = v->z / (l_fVectorLen ? l_fVectorLen : 1);
}


/* =============================================================================
Function    :   vMatrixProduct

Description :   Make product between v1 and m1 and place result into v.

Parameters  :   .

Return      :
============================================================================= */
void vMatrixProduct (VECTEUR *v, VECTEUR *v1, TMatrice m1)
{
  /* Locals variables declaration */
  int i;
  float temp[3] = { 0, 0, 0 };

  for(i = 0; i < 3; i++)
  {
    temp[i] += v1->x * m1[i][0];
    temp[i] += v1->y * m1[i][1];
    temp[i] += v1->z * m1[i][2];
  }

  v->x = temp[0];
  v->y = temp[1];
  v->z = temp[2];
}


/* =============================================================================
Function    :   vMatrixRotation

Description :   Apply rx,ry and rz rotation to m matrix.

Parameters  :   .

Return      :
============================================================================= */
void vMatrixRotation (TMatrice m, float rx, float ry, float rz)
{
  /* Locals variables declaration */
  float cosx, sinx;
  float cosy, siny;
  float cosz, sinz;

  /* Compute each cos-sin */
  cosx = (float) cos(rx);
  sinx = (float) sin(rx);
  cosy = (float) cos(ry);
  siny = (float) sin(ry);
  cosz = (float) cos(rz);
  sinz = (float) sin(rz);

  m[0][0] = cosy*cosz;
  m[0][1] = cosy*sinz;
  m[0][2] = -siny;

  m[1][0] = sinx*siny*cosz - cosx*sinz;
  m[1][1] = sinx*siny*sinz + cosx*cosz;
  m[1][2] = sinx*cosy;

  m[2][0] = cosx*siny*cosz + sinx*sinz;
  m[2][1] = cosx*siny*sinz - sinx*cosz;
  m[2][2] = cosx*cosy;
}

/*==============================================================================
Function    :
Describe    :   .
Parameters  :   None.
Returns     :   None
==============================================================================*/
static void vInterpolate (void)
{
/*
   formule d'interpolation :
     v(x,y) = ( (8-x)*(8-y)*c1 + x*(8-y)*c2 + (8-x)*y*c3 + x*y*c4 ) / 64

   d'ou :
     dv(x,y)/dx = ( -(8-y)*v1 + (8-y)*v2 - y*v3 + y*v4 ) / 64
   et
     dv(x,y)/dy = ( -(8-x)*v1 - x*v2 + (8-x)*v3 + x*v4 ) / 64

   dv/dx pas constant : il varie avec y, on calcule
                        donc la variation de dýv(x,y)/dxy

     dýv(x,y)/dxy = ( v1 - v2 - v3 + v4 ) / 64
*/

  int _x, _y;             // coordonnees dans le tableau de vals a interpoler
  int x, y;               // coordonnees a l'interieur du carre
  unsigned long  v;       // valeur courante (interpolee)
  unsigned long _v;       // valeur du debut de la ligne
  unsigned long dvdx;     // variation selon x (= dv(x,y)/dx )
  unsigned long dvdy;     // variation selon y (= dv(x,y)/dy )
  unsigned long dvdxdy;   // variation de dvdx quand y varie
  unsigned long v1, v2, v3, v4;   // valeurs aux 4 coins du carres

  // interpolation des x
  for(_y=0;_y<H_ECRAN;_y+=INTERPOL)      // on scanne les differentes valeurs
    for(_x=0;_x<L_ECRAN;_x+=INTERPOL)    // a interpoler
    // et on interpole les carres 8x8
    {
      v1 = interpol[_x][_y].x<<8;     // on recupere les valeurs aux 4 coins
      v2 = interpol[_x+INTERPOL][_y].x<<8;   // du carre a interpoler
      v3 = interpol[_x][_y+INTERPOL].x<<8;   // (on les passe sur 16 bits pour avoir
      v4 = interpol[_x+INTERPOL][_y+INTERPOL].x<<8; // plus de precision dans la variation)

      _v = v = v1;                 // on initialise le x a interpoler

      dvdx   = (-v1+v2)/INTERPOL;        // variation en x quand y=0
      dvdy   = (-v1+v3)/INTERPOL;        // variation en y quand x=0
      dvdxdy = (v1-v2-v3+v4)/(INTERPOL*INTERPOL);   // variation de dvdx quand y varie

      for(y=_y;y<_y+INTERPOL;y++) {       // on parcourt le carre de haut en bas
        for(x=_x;x<_x+INTERPOL;x++) {     // de gauche a droite
          v += dvdx;               // on fait varier la valeur a interpoler
          interpol[x][y].x = v>>8; // on repasse la valeur sur 8 bits
        }
        _v += dvdy;                // on fait varier la valeur de gauche
        v = _v;                    // on recommence au debut de la ligne
        dvdx += dvdxdy;            // on fait varier dvdx
      }

    }

  // interpolation des y
  for(_y=0;_y<H_ECRAN;_y+=INTERPOL)      // on scanne les differentes valeurs
    for(_x=0;_x<L_ECRAN;_x+=INTERPOL)    // a interpoler
    // et on interpole les carres 8x8
    {
      v1 = interpol[_x][_y].y<<8;     // on recupere les valeurs aux 4 coins
      v2 = interpol[_x+INTERPOL][_y].y<<8;   // du carre a interpoler
      v3 = interpol[_x][_y+INTERPOL].y<<8;   // (on les passe sur 16 bits pour avoir
      v4 = interpol[_x+INTERPOL][_y+INTERPOL].y<<8; // plus de precision dans la variation)

      _v = v = v1;                 // on initialise le x a interpoler

      dvdx   = (-v1+v2)/INTERPOL;        // variation en x quand y=0
      dvdy   = (-v1+v3)/INTERPOL;        // variation en y quand x=0
      dvdxdy = (v1-v2-v3+v4)/(INTERPOL*INTERPOL);   // variation de dvdx quand y varie

      for(y=_y;y<_y+INTERPOL;y++) {       // on parcourt le carre de haut en bas
        for(x=_x;x<_x+INTERPOL;x++) {     // de gauche a droite
          v += dvdx;               // on fait varier la valeur a interpoler
          interpol[x][y].y = v>>8; // on repasse la valeur sur 8 bits
        }
        _v   += dvdy;              // on fait varier la valeur de gauche
        v     = _v;                // on recommence au debut de la ligne
        dvdx += dvdxdy;            // on fait varier dvdx
      }

    }

    float f1,f2,f3,f4;
    float _f,f;
    float dfdx,dfdy,dfdxdy;

  // interpolation des t
  for(_y=0;_y<H_ECRAN;_y+=INTERPOL)      // on scanne les differentes valeurs
    for(_x=0;_x<L_ECRAN;_x+=INTERPOL)    // a interpoler
    // et on interpole les carres 8x8
    {
      f1 = fog[_x][_y];     // on recupere les valeurs aux 4 coins
      f2 = fog[_x+INTERPOL][_y];   // du carre a interpoler
      f3 = fog[_x][_y+INTERPOL];   // (on les passe sur 16 bits pour avoir
      f4 = fog[_x+INTERPOL][_y+INTERPOL]; // plus de precision dans la variation)

      _f = f = f1;                 // on initialise le x a interpoler

      dfdx   = (-f1+f2) / INTERPOL;        // variation en x quand y=0
      dfdy   = (-f1+f3) / INTERPOL;        // variation en y quand x=0
      dfdxdy = (f1-f2-f3+f4) / (INTERPOL*INTERPOL);   // variation de dvdx quand y varie

      for(y=_y;y<_y+INTERPOL;y++) {       // on parcourt le carre de haut en bas
        for(x=_x;x<_x+INTERPOL;x++) {     // de gauche a droite
          f += dfdx;               // on fait varier la valeur a interpoler
          fog[x][y] = f; // on repasse la valeur sur 8 bits
        }
        _f += dfdy;                // on fait varier la valeur de gauche
        f = _f;                    // on recommence au debut de la ligne
        dfdx += dfdxdy;            // on fait varier dvdx
      }

    }
}
/* -User specific- */


