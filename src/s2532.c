/*****************************************************************************/
/*                                                                           */
/*                                                                           */
/* (c) Copyright 1995 by                                                     */
/*     SINTEF, Oslo, Norway.                                                 */
/*     All rights reserved. See the copyright.h for more details.            */
/*                                                                           */
/*****************************************************************************/

#include "copyright.h"

/*
 *
 * $Id: s2532.c,v 1.1 1995-08-21 13:22:45 jka Exp $
 *
 */

#define S2532

#include "sislP.h"
/* Prelimenary declaration */
#if defined(SISLNEEDPROTOTYPES)
#ifndef s2512
extern
#endif
void
   s2512(SISLSurf *surf, int ider, int iside1, int iside2, double parvalue[],
      int *leftknot1,int *leftknot2, double gaussian[], int *stat);
#else
#ifndef s2512
extern
#endif
void s2512();
#endif
/* *********************** */



#if defined(SISLNEEDPROTOTYPES)
void
s2532(SISLSurf *surf, int u_continuity, int v_continuity, 
      int *u_surfnumb, int *v_surfnumb, SISLSurf ***gauss_surf, int *stat)
#else
void s2532(surf, u_continuity, v_continuity, u_surfnumb, 
	   v_surfnumb, gauss_surf, stat)
     SISLSurf *surf;
     int      u_continuity;
     int      v_continuity;
     int      *u_surfnumb;
     int      *v_surfnumb;
     SISLSurf ***gauss_surf;
     int      *stat;
#endif
/*
***************************************************************************
*
***************************************************************************
* PURPOSE : To interpolate the Gaussian curvature of a B-SPLINE or NURBS 
*           surface, as a NURBS surface. 
*           The desired continuity of the Gauss-curvature surface is given as 
*           input, this may lead to a patchwork of output surfaces. 
*           THE INTERPOLATION RESULTS IN A HIGH ORDER SURFACE (IF THE ORIGINAL 
*           SURFACE IS A B-SPLINE OF ORDER K, THE RESULT IS OF ORDER 8K - 11, 
*           IN THE NURBS CASE THE ORDER IS 32K - 35). TO AVOID UNSTABILITY
*           BECAUSE OF THIS, A MAX. ORDER IS APPLIED. THIS MAY LEAD TO AN
*           APPROXIMATION INSTEAD OF AN INTERPOLATION.
*
*
* INPUT   : surf         - The original surface.
*           u_continuity - Desired continuity of the Gauss-curvature surfaces
*                          in u direction:
*                          = 0 : Positional continuity,
*                          = 1 : Tangential continuity,
*                          and so on.
*                          SISL only accepts surfaces of continuity 0 or larger.
*                          If the surface is to be intersected with another,
*                          the continuity must be 1 or larger to find all the
*                          intersection curves.
*           v_continuity - Desired continuity of the Gauss-curvature surfaces
*                          in v direction:
*                          = 0 : Positional continuity,
*                          = 1 : Tangential continuity,
*                          and so on.
*                          SISL only accepts surfaces of continuity 0 or larger.
*                          If the surface is to be intersected with another,
*                          the continuity must be 1 or larger to find all the
*                          intersection curves.
*
*
* OUTPUT  : u_surfnumb   - Number of Gauss-curvature surface patches
*                          in u direction.
*           v_surfnumb   - Number of Gauss-curvature surface patches
*                          in v direction.
*           gauss_surf   - The Gaussian curvature interpolation surfaces.
*                          This will be a pointer to an array of length
*                          u_surfnum*v_surfnumb of SISLSurf pointers,
*                          where the indexing is running fastest in the
*                          u direction.
*           stat         - Status message.
*                          > 0      : Warning.
*                          = 2      : The surface is degenerated.
*                          = 0      : Ok.
*                          < 0      : Error.
*
*
* METHOD  :
*
*
* CALLS   : s2535(), s2534()
*
* WRITTEN BY : Johannes Kaasa, SINTEF, Oslo, Norway, Aug. 1995.
*
*********************************************************************
*/
{
   int ki;                     /* Index in for loops.                    */
   int u_multinc;              /* Increased knot multiplicity.           */
   int v_multinc;              /* Increased knot multiplicity.           */
   int newik1;                 /* New order in u direction.              */
   int newik2;                 /* New order in v direction.              */
   int eval_dim;               /* Evaluation dimention.                  */
   int max_order = 20;         /* Max. order of the curvature surface.   */
   SISLSurf *temp = NULL;      /* Temporary surface.                     */
   SISLSurf *regular = NULL;   /* k-regular surface.                     */
   SISLSurf **org_surf = NULL; /* Array of pointers to original patches. */
   
   
   /* Check input. */
   
   if (surf == NULL || u_continuity < 0 || v_continuity < 0)
      goto err150;
   
   /* Curvature continuity decrease. */
   
   u_multinc = 2;
   v_multinc = 2;
   
   /* Make sure we have a k-regular surface. */

   if (surf->cuopen_1 == SISL_SURF_PERIODIC || 
       surf->cuopen_2 == SISL_SURF_PERIODIC )
   {
      make_sf_kreg(surf, &regular, stat);
      if (*stat < 0) goto error;
   }
   else
      regular = surf;
   
   /* Split the surface to meet the continuity requirements. */
   
   s2535(regular, (u_continuity + u_multinc), (v_continuity + v_multinc), 
	 u_surfnumb, v_surfnumb, &org_surf, stat);
   if (*stat < 0) goto error;
   
   /* Allocate output array. */
   
   if ((*gauss_surf = newarray((*u_surfnumb)*(*v_surfnumb), SISLSurf*)) 
       == NULL) goto err101;
   for (ki = 0; ki < (*u_surfnumb)*(*v_surfnumb); ki++)
      (*gauss_surf)[ki] = NULL;
   
   /* Calculate curvature order. */
   
   if (regular->ikind == 1 || regular->ikind == 3)
   {
      newik1 = 8*regular->ik1 - 11;
      newik2 = 8*regular->ik2 - 11;
   }
   else
   {
      newik1 = 32*regular->ik1 - 35;
      newik2 = 32*regular->ik2 - 35;
   }
   newik1 = min(newik1, max_order);
   newik2 = min(newik2, max_order);
   
   eval_dim = 2;
   
   /* Interpolate. */
   
   if (*u_surfnumb == 1 && *v_surfnumb == 1)
   {
      s2534(regular, u_multinc, v_multinc, newik1, newik2, s2512, eval_dim,
	    &temp, stat);
      if (*stat < 0) goto error;
      if (*stat == 2) goto war002;
      
      (*gauss_surf)[0] = newSurf(temp->in1, temp->in2, temp->ik1, temp->ik2,
				 temp->et1, temp->et2, temp->ecoef, 2, 1, 1);
      
      if (temp != NULL)
      {
	 freeSurf(temp);
	 temp = NULL;
      }
   }
   else
   {
      for (ki = 0; ki < (*u_surfnumb)*(*v_surfnumb); ki++)
      {
	 s2534(org_surf[ki], u_multinc, v_multinc, newik1, newik2, s2512, 
	       eval_dim, &temp, stat);
         if (*stat < 0) goto error;
	 if (*stat == 2) goto war002;
      
         (*gauss_surf)[ki] = newSurf(temp->in1, temp->in2, temp->ik1, 
				     temp->ik2, temp->et1, temp->et2, 
				     temp->ecoef, 2, 1, 1);
	 
	 if (temp != NULL)
         {
	    freeSurf(temp);
	    temp = NULL;
         }
      }
   }


   goto out;



   /* ---------------------- ERROR EXITS ------------------------------- */
   
   /* The surface is degenerated at (u,v) */
   
 war002:
   if (*gauss_surf != NULL)
   {
      for (ki = 0; ki < ((*u_surfnumb)*(*v_surfnumb)); ki++)
	 if ((*gauss_surf)[ki] != NULL) freeSurf((*gauss_surf)[ki]);
      freearray(*gauss_surf);
   }
   *u_surfnumb = 0;
   *v_surfnumb = 0;
   goto out;
  
  /* Error in space allocation */
   
 err101: 
   *stat = -101;
   s6err("s2532", *stat, 0);
   goto out;

   /* Error in input. */
   
 err150:
   *stat = -150;
   s6err("s2532", *stat, 0);
   goto out;

   /* Error in lower level routine. */
   
 error:
   s6err("s2532", *stat, 0);
   goto out;

   /* ---------------------- NORMAL EXIT ------------------------------- */

 out:
   if (regular != surf ) freeSurf(regular);
   if (org_surf != NULL)
   {
      for (ki = 0; ki < ((*u_surfnumb)*(*v_surfnumb)); ki++)
	 if (org_surf[ki] != NULL) freeSurf(org_surf[ki]);
      freearray(org_surf);
   }
   
   return;

}