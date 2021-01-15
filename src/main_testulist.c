
/* test of the density interpolation time for the ULL implementation
 * with varying particle per cell number for a fixed grid. */


#include <stdio.h>
#include <stdlib.h>
#include <ulist.h>
#include <math.h>
#include <time.h>





typedef struct _cell
{
	float e[3];
	float b[3];
	float n;
	UList *plist;
} Cell;





UINT test_piculist(int nppcell)
{
	int i,j,k, ijk;
	int nx, ny, nz, p;
	Particle part;
	float x,y,z;
	float dx, dy, dz;
	Cell *grid;
	Particle *p_part;
	float w1, w2, w3, w4, w5, w6, w7, w8;
	int ijk1, ijk2, ijk3, ijk4, ijk5, ijk6, ijk7, ijk8;
	float lx, ly, lz;
	int cpt=0;
	clock_t start, end;
	double duration = 0;
	float min, max;
	float xm, ym, zm;
	float odx, ody, odz;

	xm = 10.;
	ym = 10.;
	zm = 10.;

	nx = 100;
	ny = 100;
	nz = 100;

	dx = xm/(float)(nx-1);
	dy = ym/(float)(ny-1);
	dz = zm/(float)(nz-1);


	odx = 1./dx;
	ody = 1./dy;
	odz = 1./dz;

	min = 0;
	max = 0;


	grid = malloc(nx * ny * nz * sizeof *grid);

	for (i=0; i< nx-1; i++)
	{
		x = i*dx;

		for (j = 0; j < ny-1; j++)
		{

			y = j*dy;
			for (k = 0; k < nz-1; k++)
			{
				z = k*dz;

				ijk = k + j*nz + i*ny*nz;


				grid[ijk].plist = ulist_new();
				grid[ijk].n = 0;

/*				if (grid[ijk].plist == NULL) printf("erreur plist\n");*/

				for (p = 0; p < nppcell; p++)
				{
					part.r[0] = x + 0.5*dx+ 0.5*(2*(float)rand()/RAND_MAX-1)*dx;
					part.r[1] = y + 0.5*dy+ 0.5*(2*(float)rand()/RAND_MAX-1)*dy;
					part.r[2] = z + 0.5*dz+ 0.5*(2*(float)rand()/RAND_MAX-1)*dz;

					if (part.r[0] >= xm) part.r[0] -= 1e-4;
					if (part.r[1] >= xm) part.r[1] -= 1e-4;
					if (part.r[2] >= xm) part.r[2] -= 1e-4;

					if (part.r[0] > max) max = part.r[0];
					if (part.r[0] < min) min = part.r[0];

					ulist_addelem(grid[ijk].plist, &part);
					cpt ++;
				}
			}
		}
	} /* end of the initialization*/


	printf("cpt = %d, min = %f, max = %f\n", cpt, min, max);

	cpt = 0;

	start = clock();

	/* now loop on the particles */
	for (i=0; i< nx-1; i++)
	{
		for (j = 0; j < ny-1; j++)
		{
			for (k = 0; k < nz-1; k++)
			{
				ijk = k + j*nz + i*ny*nz;

				/* get the first particle */
				p_part = ulist_getelemi(grid[ijk].plist,0);

				/* these indices can be outside the particle loop
				 * which is much faster. */

				ijk1 = (k  ) + (j  )*nz + (i  )*nz*ny;
				ijk2 = (k  ) + (j+1)*nz + (i  )*nz*ny;
				ijk3 = (k  ) + (j+1)*nz + (i+1)*nz*ny;
				ijk4 = (k  ) + (j  )*nz + (i+1)*nz*ny;
				ijk5 = (k+1) + (j  )*nz + (i  )*nz*ny;
				ijk6 = (k+1) + (j+1)*nz + (i  )*nz*ny;
				ijk7 = (k+1) + (j+1)*nz + (i+1)*nz*ny;
				ijk8 = (k+1) + (j  )*nz + (i  )*nz*ny;

				while (p_part != NULL)
				{

					lx = p_part->r[0]*odx - i;
					ly = p_part->r[1]*ody - j;
					lz = p_part->r[2]*odz - k;

			        w1 = (1.0-lx)*(1.0-ly)*(1.0-lz);
			        w2 = (1.0-lx)*(    ly)*(1.0-lz);
			        w3 = (    lx)*(    ly)*(1.0-lz);
			        w4 = (    lx)*(1.0-ly)*(1.0-lz);
			        w5 = (1.0-lx)*(1.0-ly)*(    lz);
			        w6 = (1.0-lx)*(    ly)*(    lz);
			        w7 = (    lx)*(    ly)*(    lz);
			        w8 = (    lx)*(1.0-ly)*(    lz);


			        grid[ijk1].n += w1;
			        grid[ijk2].n += w2;
			        grid[ijk3].n += w3;
			        grid[ijk4].n += w4;
			        grid[ijk5].n += w5;
			        grid[ijk6].n += w6;
			        grid[ijk7].n += w7;
			        grid[ijk8].n += w8;

					p_part = p_part->next;
					cpt++;
				}

			}
		}
	} /* end of interpolation loop*/
	end = clock();
	duration = ((double)end - (double)start)/CLOCKS_PER_SEC;

	printf("cpt = %d\n", cpt);
	printf("duration of the interpolation : %f\n",duration);

	for (i=0; i< nx-1; i++)
	{
		x = i*dx;

		for (j = 0; j < ny-1; j++)
		{

			y = j*dy;
			for (k = 0; k < nz-1; k++)
			{
				z = k*dz;

				ijk = k + j*nz + i*ny*nz;


				ulist_delete(grid[ijk].plist);
			}
		}
	} /* end of the initialization*/

	free(grid);

	return 0;
}









int main(void)
{
	int nppcell;

	for(nppcell = 54; nppcell < 304; nppcell+=50)  test_piculist(nppcell);


	return 0;
}





