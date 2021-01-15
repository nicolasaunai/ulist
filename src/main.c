/*
 * main.c
 *
 *  Created on: Jun 15, 2012
 *      Author: nicolasaunai
 */



#include <stdio.h>
#include <stdlib.h>
#include <ulist.h>
#include <math.h>
#include <time.h>


UINT test_addelem(void)
{
	UList *cell;
	UINT err = 1;
	Particle p;
	int i;
	UList_Packet *packet;


	cell = ulist_new();



	if (ulist_getnbpacket(cell) != 1)
	{
		printf("error nb packet is not 1 after initialization\n");
		err = 0;
	}

	if (ulist_getnelem(cell) != 0)
	{
		printf("error nb elem is not 0 after initialization\n");
		err = 0;
	}


	p.r[0] = 1;
	p.r[1] = 2;
	p.r[3] = 4;

	ulist_addelem(cell, &p);



	if (ulist_getnelem(cell) != 1)
	{
		printf("error nb elem is not 1 after inserting 1 element after init\n");
		err = 0;
	}

	ulist_addelem(cell, &p);

	if (ulist_getnelem(cell) != 2)
	{
		printf("error nb elem is not 2 after inserting a second element\n");
		err = 0;
	}



	if (ulist_getnbpacket(cell) != 1)
	{
		printf("error nb packet is not 1 only 2 elements added (maxelem=100)\n");
		err = 0;
	}


	for (i=0; i< 380; i++)
	{
		p.r[0] = 1*i;
		p.r[1] = 2*i;
		p.r[3] = 4*i;
		ulist_addelem(cell, &p);
	}


	/* now there should be 382+2=382 elements*/

	if (ulist_getnelem(cell) != 382)
	{
		printf("error nb elem is not 382\n");
		err = 0;
	}


	i=0;
	packet = cell->head;
	while(packet != NULL)
	{
		if (packet->num_elems != NELEM_PACKET && packet != cell->tail)
		{
			printf("error - all the first packets should be full, %d/%d, %d\n", packet->num_elems, NELEM_PACKET, i);
			err = 0;
		}
		packet = packet->next;
		i++;
	}


	if (cell->tail->num_elems != 82)
	{
		printf("error, last packet should have 82 particles\n");
		err = 0;
	}

	ulist_print(cell);

	ulist_delete(cell);

	return err;
}











void testgen(void)
{
	UList *cell1, *cell2;
	Particle p, *p2;
	UINT npart = 121, i;

	cell1 = ulist_new();
	cell2 = ulist_new();


	ulist_print(cell1);

	/*ulist_remove_packet(particles, 4);*/


	for (i=0; i <npart; i++)
	{
		p.r[0] = i*0.1;
		p.r[1] = i*0.2;
		p.r[2] = i*0.3;

		ulist_addelem(cell1,&p);

		p.r[0] = -(float)i*0.1;
		p.r[1] = -(float)i*0.2;
		p.r[2] = -(float)i*0.3;

		ulist_addelem(cell2,&p);

	}
	ulist_print(cell1);
	ulist_print(cell2);



	/*for (i=0; i <cell2->num_elems; i++)
	{
		p2 = ulist_getelemi(cell2, i);
		printf("p2.r = %f\n", p2->r[0]);
	}*/

	p2 = ulist_getelemi(cell1, 40);
	p2 = ulist_addelem(cell2,p2);
	ulist_removeelem(cell1,40);

	printf("after cell communication\n");


	ulist_print(cell1);
	ulist_print(cell2);


	printf("p2.r = %f\n", p2->r[0]);
	printf("move cell2[108] to cell1\n");



	p2 = ulist_getelemi(cell2, 108);
	printf("cell2(108).r0 = %f\n", p2->r[0]);
	p2 = ulist_addelem(cell1,p2);
	ulist_removeelem(cell2,108);

	printf("after cell communication\n");


	ulist_print(cell1);
	ulist_print(cell2);


	printf("cell1(new) = %f\n", p2->r[0]);




/*	for (i=0; i <npart; i++)
	{
		p2 =  ulist_getelemi(cell1, i);
		printf("%f\n", p2->r[0]);
	}
*/






	/*ulist_removeelem(particles,0);*/



	printf("size of the UList : %d\n", ulist_size(cell1));

	ulist_delete(cell1);
	ulist_delete(cell2);


}






int test_exchange(UINT index)
{
	UList *cell1, *cell2;
	int p;
	Particle part, *p_part;
	int err = 1;
	UINT ipacket, idata;
	float r0, r1;

	cell1 = ulist_new();
	cell2 = ulist_new();


	/*add 102 particles to cell1*/
	for (p=0; p<102;p++)
	{
		part.r[0] = p*0.1;
		part.r[1] = p*0.2;
		part.r[2] = p*0.3;

		ulist_addelem(cell1, &part);
	}


	/*add 33 particles to cell1*/
	for (p=0; p < 33;p++)
	{
		part.r[0] = p*0.1;
		part.r[1] = p*0.2;
		part.r[2] = p*0.3;

		ulist_addelem(cell2, &part);
	}



	ipacket = index / NELEM_PACKET;
	idata   = index % NELEM_PACKET;






	p_part = ulist_getelemi(cell2, index);

	r0 = p_part->r[0];

	p_part = ulist_addelem(cell1, p_part);
	ulist_removeelem(cell2,index);

	r1 = p_part->r[0];

	if (r1 != r0)
	{
		err = 0;
		printf("error - the position of the particle displaced from cell2 to cell1 has changed\n");
	}



	ulist_delete(cell1);
	ulist_delete(cell2);

	return err;


}





UINT test_send(void)
{
	UList *cell1, *cell2;
	int p;
	Particle part, *p_part, *p_tmp;
	int err = 1;
	UINT npart1[2], npart2[2];


	cell1 = ulist_new();
	cell2 = ulist_new();


	/*add 102 particles to cell1*/
	for (p=0; p<102;p++)
	{
		part.r[0] = p*0.1;
		part.r[1] = p*0.2;
		part.r[2] = p*0.3;

		ulist_addelem(cell1, &part);
	}


	/*add 33 particles to cell1*/
	for (p=0; p < 33; p++)
	{
		part.r[0] = p*0.1;
		part.r[1] = p*0.2;
		part.r[2] = p*0.3;

		ulist_addelem(cell2, &part);
	}


	npart1[0] = ulist_getnelem(cell1);
	npart2[0] = ulist_getnelem(cell2);


	p_part = ulist_getelemi(cell2, 0);
	p = 0;
	while (p_part != NULL)
	{
		p_tmp = ulist_getelemi(cell2,0);
		ulist_addelem(cell1,p_tmp);
		ulist_removeelem(cell2,0);
		p_part = p_part->next;
	}

	npart1[1] = ulist_getnelem(cell1);
	npart2[1] = ulist_getnelem(cell2);

	if (npart2[1] != 0)
	{
		printf("error - cell2 should be empty and nelem is  %d\n", npart2[1]);
		err = 0;
	}

	if (npart1[1] != npart1[0] + npart2[0])
	{
		printf("error - cell1 should have %d particles and has %d\n", npart2[0] + npart1[0], npart1[2]);
		err = 0;
	}

	return err;

}




UINT test_loop(void)
{
	UList *cell;
	int p;
	Particle part, *p_part;
	int err = 1;
	float r0, r1;

	cell = ulist_new();


	/*add 102 particles to cell1*/
	for (p=0; p<102;p++)
	{
		part.r[0] = p*0.1;
		part.r[1] = p*0.2;
		part.r[2] = p*0.3;

		ulist_addelem(cell, &part);
	}

	ulist_print(cell);

	p_part = ulist_getelemi(cell, cell->num_elems-1);

	r0= p_part->r[0];

	p_part =  ulist_getelemi(cell, 0);
	p = 0;

	while (p_part->next != NULL)
	{
		p_part = p_part->next;
		p ++;
	}

	r1 = p_part->r[0];

	if (r0!= r1)
	{
		printf("error - last particle of the list doesn't have the same position as the last inserted\n");
		err = 0;
	}

	return err;
}




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

	nx = 100;
	ny = 100;
	nz = 100;

	dx = 0.1;
	dy = 0.1;
	dz = 0.1;

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
		x = i*dx;
		for (j = 0; j < ny-1; j++)
		{
			y = j*dy;
			for (k = 0; k < nz-1; k++)
			{
				z = k*dz;
				ijk = k + j*nz + i*ny*nz;

				/* get the first particle */
				p_part = ulist_getelemi(grid[ijk].plist,0);

				while (p_part != NULL)
				{

					lx = p_part->r[0] - i;
					ly = p_part->r[1] - j;
					lz = p_part->r[2] - k;

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


	nx = 100;
	ny = 100;
	nz = 100;

	dx = 0.1;
	dy = 0.1;
	dz = 0.1;


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

		i = (int)floor(p_part[cpt].r[0]);
		j = (int)floor(p_part[cpt].r[1]);
		k = (int)floor(p_part[cpt].r[2]);

		lx = p_part[cpt].r[0] - i;
		ly = p_part[cpt].r[1] - j;
		lz = p_part[cpt].r[2] - k;


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

	printf("cpt = %d\n", cpt);

	duration = ((double)end - (double)start)/CLOCKS_PER_SEC;

	printf("sorted particle array : the interpolation duration is %f\n", duration);

	return 0;
}









UINT test_picarrayrandom(int nppcell)
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


	nx = 100;
	ny = 100;
	nz = 100;

	dx = 0.1;
	dy = 0.1;
	dz = 0.1;


	grid = malloc(nx * ny * nz * sizeof *grid);
	p_part = malloc(nx*ny*nz * nppcell*sizeof *p_part);
	if (p_part == NULL) printf("particle allocation impossible\n");

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
			}
		}
	} /* end of the initialization*/


	printf("positioning particles...\n");
	for (p=0; p < nppcell * (nx-1)*(ny-1)*(nz-1); p++)
	{
		p_part[cpt].r[0]   = (float)rand()/RAND_MAX * (nx-1)*dx;
		p_part[cpt].r[1]   = (float)rand()/RAND_MAX * (ny-1)*dy;
		p_part[cpt++].r[2] = (float)rand()/RAND_MAX * (nz-1)*dz;
	}
	printf("particle in place\n");

	minmax(p_part, cpt, &min, &max);

	printf("cpt = %d, min = %f, max = %f\n", cpt, min, max);

	nptot = cpt;
	cpt = 0;

	/*time(&start);*/
	start = clock();

	for (p=0; p < nptot; p++ )
	{

		i = (int)floor(p_part[cpt].r[0]);
		j = (int)floor(p_part[cpt].r[1]);
		k = (int)floor(p_part[cpt].r[2]);

		lx = p_part[cpt].r[0] - i;
		ly = p_part[cpt].r[1] - j;
		lz = p_part[cpt].r[2] - k;


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
	/*time(&end);*/
	end = clock();
	/*duration = difftime(end, start);*/
	duration = ((double)end-start)/CLOCKS_PER_SEC;
	printf("cpt = %d\n", cpt);
	printf("particle array randomized : the interpolation duration is %f\n", duration);


	return 0;
}











int main(void)
{
	int method = 3;
	int nppcell;
	/*if(test_addelem() == 1) printf("test_addelem() SUCCESSFUL\n");
	else printf("go debugging ulist_addelem() !!!!\n");

	if (test_exchange(30) == 1) printf("test_exchange() SUCCESSFUL\n");
	else printf("go debugging ulist_addelem() and ulist_removeelem()!!!!\n");
*/
	/*if (test_send() == 1) printf("test_send() SUCCESSFUL\n");
	else printf("emptying a cell does not work!!!!\n");*/

/*	if (test_loop() ==1 ) printf("test_loop() SUCCESSFULL\n");
	else printf("lopp does not work, particle chain must be invalid\n");
*/

	if (method == 1)  for(nppcell = 54; nppcell < 304; nppcell+=50)  test_piculist(nppcell);
	if (method == 2)  for(nppcell = 54; nppcell < 304; nppcell+=50) test_picarraysorted(nppcell);
	if (method == 3)  for(nppcell = 54; nppcell < 304; nppcell+=50)  test_picarrayrandom(nppcell);


	return 0;
}




