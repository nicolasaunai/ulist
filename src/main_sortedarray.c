

#include <stdio.h>
#include <stdlib.h>
/*#include <ulist.h>*/
#include <math.h>
#include <time.h>

typedef unsigned int UINT;

#define EPS 1e-4

typedef struct _particle
{
	float r[3];
	float v[3];
	float q;
	float m;

	struct _particle *next;

} Particle;





typedef struct _cell
{
	float e[3];
	float b[3];
	float n;
/*	UList *plist;*/
} Cell;





void minmax(Particle *p, int size, float *min, float *max)
{
	int m;


	*min = p[0].r[0];
	*max = p[0].r[0];

	for (m=0; m < size; m++)
	{
		if (p[m].r[0] > *max) *max = p[m].r[0];
		if (p[m].r[0] < *min) *min = p[m].r[0];
	}

}





UINT test_picarraysorted(int nppcell)
{
	int i,j,k, ijk;
	int nx, ny, nz, p;
	float x,y,z;
	float dx, dy, dz;
	Cell *grid;
	Particle *p_part;
	float w1, w2, w3, w4, w5, w6, w7, w8;
	int ijk1, ijk2, ijk3, ijk4, ijk5, ijk6, ijk7, ijk8;
	float lx, ly, lz;
	int cpt=0;
	int nptot;
	clock_t start, end;
	double duration = 0;
	float min, max;
	float xm, ym, zm;
	float odx, ody, odz;
	float ix, jy, kz;

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


	grid = malloc(nx * ny * nz * sizeof *grid);
	p_part = malloc(nx*ny*nz * nppcell*sizeof *p_part);
	if (p_part == NULL)
	{
		printf("particle allocation impossible\n");
		exit(-1);
	}

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

				grid[ijk].n = 0;


				for (p=0; p < nppcell; p++)
				{
					p_part[cpt].r[0]   = x + 0.5*dx+ 0.5*(2*(float)rand()/RAND_MAX-1)*dx;
					p_part[cpt].r[1]   = y + 0.5*dy+ 0.5*(2*(float)rand()/RAND_MAX-1)*dy;
					p_part[cpt++].r[2] = z + 0.5*dz+ 0.5*(2*(float)rand()/RAND_MAX-1)*dz;
				}
			}
		}
	} /* end of the initialization*/


	minmax(p_part, cpt, &min, &max);

	printf("cpt = %d, min = %f, max = %f\n", cpt, min, max);
	nptot = cpt;
	cpt = 0;

	start = clock();
	for (p=0; p < nptot; p++ )
	{
		ix = (p_part[cpt].r[0])*odx;
		jy = (p_part[cpt].r[1])*ody;
		kz = (p_part[cpt].r[2])*odz;

		ix  = (ix >=  nx-1) ? nx -1- EPS: ix;
		jy  = (jy >=  ny-1) ? ny -1- EPS: jy;
		kz  = (kz >=  nz-1) ? nz -1- EPS: kz;


		i = (int)ix;
		j = (int)jy;
		k = (int)kz;

		lx = ix - i;
		ly = jy - j;
		lz = kz - k;

		w1 = (1.0-lx)*(1.0-ly)*(1.0-lz);
		w2 = (1.0-lx)*(    ly)*(1.0-lz);
		w3 = (    lx)*(    ly)*(1.0-lz);
		w4 = (    lx)*(1.0-ly)*(1.0-lz);
		w5 = (1.0-lx)*(1.0-ly)*(    lz);
		w6 = (1.0-lx)*(    ly)*(    lz);
		w7 = (    lx)*(    ly)*(    lz);
		w8 = (    lx)*(1.0-ly)*(    lz);

		ijk1 = (k  ) + (j  )*nz + (i  )*nz*ny;
		ijk2 = (k  ) + (j+1)*nz + (i  )*nz*ny;
		ijk3 = (k  ) + (j+1)*nz + (i+1)*nz*ny;
		ijk4 = (k  ) + (j  )*nz + (i+1)*nz*ny;
		ijk5 = (k+1) + (j  )*nz + (i  )*nz*ny;
		ijk6 = (k+1) + (j+1)*nz + (i  )*nz*ny;
		ijk7 = (k+1) + (j+1)*nz + (i+1)*nz*ny;
		ijk8 = (k+1) + (j  )*nz + (i  )*nz*ny;

		grid[ijk1].n += w1;
		grid[ijk2].n += w2;
		grid[ijk3].n += w3;
		grid[ijk4].n += w4;
		grid[ijk5].n += w5;
		grid[ijk6].n += w6;
		grid[ijk7].n += w7;

		cpt++;

	} /* end of interpolation loop*/
	end = clock();

	free(p_part);
	free(grid);


	printf("cpt = %d\n", cpt);

	duration = ((double)end - (double)start)/CLOCKS_PER_SEC;

	printf("sorted particle array : the interpolation duration is %f\n", duration);

	return 0;
}






int main(void)
{
	int nppcell;

	for(nppcell = 54; nppcell < 304; nppcell+=50)  test_picarraysorted(nppcell);


	return 0;
}
