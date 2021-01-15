/*
 * ulist.c
 *
 *  Created on: Jun 19, 2012
 *      Author: nicolasaunai
 */




#include <ulist.h>
#include <stdint.h>





UList *ulist_new(void)
{

	/* memory allocation for the  new list */
	UList * list = malloc(sizeof *list);


	/* if allocation failed, write a message and quit */
	if (list == NULL)
	{
		fprintf(stderr, "ulist_new() : Unable to allocate new list. %s (%d)\n", __FILE__, __LINE__);
		return NULL;
	}


	/* by default, create one packet
	 * with 0 element in it */
	list->num_elems = 0;
	list->num_packets = 1;

	/* then create the packet */
	list->head = malloc(sizeof *(list->head));

	if (list->head == NULL)
	{
		fprintf(stderr, "ulist_new() : Unable to allocate first packet. %s (%d)\n", __FILE__, __LINE__);
		free(list);
	}

	/* there is no packet before the head of the list */
	list->head->prev = NULL;


	/* allocate the memory for the data contained by he packet */
	list->head->data = malloc(NELEM_PACKET * sizeof (Particle));


	if (list->head->data == NULL)
	{
		fprintf(stderr, "ulist_new() : Unable to allocate first packet data. %s (%d)\n", __FILE__, __LINE__);
		free(list->head);
		free(list);
	}

	list->head->next = NULL;
	list->head->num_elems = 0;

	list->tail = list->head;



	return list;
}










void ulist_delete(UList *list)
{
	UINT ipacket;
	UList_Packet *packet, *tmp;

	/* first packet is the head */
	packet = list->head;

	/* loop over all packets */
	for (ipacket=0; ipacket < list->num_packets; ipacket++)
	{

		/* save the address of next packet*/
		tmp = packet->next;

		/* so we can erase the current packet */
		if (packet != NULL )
		{
			free(packet->data);
			free(packet);
		}

		/*set the current packet to next one that we saved earlier*/
		packet = tmp;
	}
	free(list);
}









UList_Packet *ulist_addpacket(UList *list)
{
	UList_Packet *new_packet, *prev;


	/* create the new packet */
	new_packet = malloc(sizeof *new_packet);

	/* if the allocation fails we return NULL */
	if(new_packet == NULL)
	{
		fprintf(stderr, "ulist_addpacket() - Unable to add a new packet to the list - %s (%d)\n", __FILE__, __LINE__);
		return NULL;
	}

	/* allocation was ok, now allocate the data within the packet*/
	new_packet->data = malloc(NELEM_PACKET * sizeof (Particle));


	/* if it failed, free the packet and return NULL*/
	if (new_packet->data == NULL)
	{
		fprintf(stderr, "ulist_addpacket() - Unable to add a packet, data allocation failed - %s (%d)\n", __FILE__, __LINE__);
		free(new_packet);
		return NULL;
	}

	/* a new packet is always added at the tail of
	 * the list, so the next packet is NULL.
	 * Also a packet is always created empty.*/
	new_packet->next = NULL;
	new_packet->num_elems = 0;


	/* the current tail of the list
	 * is about to be the packet before the
	 * new one, so save it */
	prev = list->tail;


	/* this, lastly tail, packet
	 * must now be linked to its new next packet*/
	prev->next = new_packet;


	/* and the new packet is linked
	 * to it too*/
	new_packet->prev = prev;


	/* the packet newly created then becomes
	 * the new tail of the list*/
	list->tail = new_packet;


	/* finally the total number of
	 * packets within the list is incremented by 1*/
	list->num_packets ++;


	/* and the new packet (the tail) is returned */
	return list->tail;
}








void ulist_print(const UList *list)
{
	UList_Packet *packet = list->head;
	UINT i=0;

	printf("--------------------------------\n");
	printf("     UList information\n");
	printf("--------------------------------\n");
	printf("# of packets     :   %03d\n", list->num_packets);

	while(packet != NULL)
	{
		printf("packet[%d] has %d elements\n", i,packet->num_elems);
		packet = packet->next;
		i++;
	}

	printf("# of elements    :   %04d\n", list->num_elems);
	printf("--------------------------------\n\n");
}








UList_Packet * ulist_remove_packet(UList *list, UINT ipacket)
{
	UList_Packet *current, *tmp;
	UINT i;

	tmp = NULL;

	if (ipacket !=0) /* we don't delete the first packet */
	{
		current = list->head;

		/* loop over the list until
		 * the current points to the packet to be deleted
	   . */
		for (i=0; i < ipacket; i++)
		{
			current = current->next;
		}

		/* 'current' is the packet to be deleted, so the packet before it
		 * must be linked to the packet next to 'current' */


		(current->prev)->next = current->next;   /*A -> B -> C becomes  A -> C*/
		if (current != list->tail)  /* if the packet is not the tail, link its next packet to its prev*/
			(current->next)->prev = current->prev;   /*A <- B <- C becomes  A <- C*/


		/* now we want to link the particles together*/


		/* packet just before */
		tmp = current->prev;

		/* if the deleted packet was the tail, tmp is now the tail
		 * in this case, and if the number of elements is not 0 : A[last]-->C[first] */
		if (tmp->next !=NULL && tmp->next->num_elems !=0)
		{
			tmp->data[tmp->num_elems-1].next = &(tmp->next->data[0]);
		}


		if(current->data)
		{
			free(current->data);
		}
		free(current);



		(list->num_packets) --;
	}
	/*return the packet just before the one that has been removed*/
	return tmp;
}





/*a revoir car prend pas en compte le nbre de particles*/
UINT ulist_size(const UList *list)
{
	return list->num_elems * sizeof(Particle);
}



UList_Packet *ulist_gotopacketi(const UList *list, UINT i)
{
	UINT p;
	UList_Packet *packet;


	packet = list->head;
	for (p=0; p < i; p++)
	{
		packet = packet->next;
	}

	return packet;
}






Particle *ulist_getelemi(const UList *list, UINT i)
{
	UINT ipacket, idata;
	UList_Packet *packet;
	/*uintptr_t tmp;*/

	if (i >= list->num_elems)
	{
		fprintf(stderr, "ulist_getelemi() - element requested outside the list - %s (%d)\n", __FILE__, __LINE__);
		return NULL;
	}

	ipacket = i/NELEM_PACKET;  /* index of the packet within which the element i is */

	idata   = i%NELEM_PACKET;  /*position in the data of the packet 'ipacket' where the element is*/


	packet = ulist_gotopacketi(list, ipacket);

	/*printf("accessing data at packet[%d] element[%d]... %f\n", ipacket, idata, packet->data[idata]);*/
	/* so now 'packet' is the current packet
	 * so we can directly return the element */


	return &packet->data[idata];
}







/*void swap(void *array, uint *size, uint elem)
{
	array[elem] = array[*size-1];
	(*size) --;
}*/




UINT ulist_getnbpacket(const UList *list)
{
	return list->num_packets;
}



UINT ulist_getnelem(const UList *list)
{
	return list->num_elems;
}





void ulist_removeelem(UList *list, UINT ielem)
{
	UINT ipacket, idata;
	UList_Packet *packet, *prev;
	UINT nelems;

	ipacket = ielem / NELEM_PACKET;
	idata = ielem % NELEM_PACKET;

	packet = ulist_gotopacketi(list,ipacket);

	nelems = packet->num_elems;

	if (packet == list->head && packet->num_elems -1 == 0)
	{
		/* this is the last element of the head packet
		 * we don't want to delete the first packet
		 * so simply decrement the number of its elements */
		printf("heeeey\n");
		packet->num_elems --;
	}


	/* if this is the last element of the packet and the packet
	 * is not the head of list, simply delete the packet */
	else if (packet->num_elems-1 == 0)
	{

		/*the previous particle has no next particle*/
		/*packet->prev->data[packet->prev->num_elems-1].next = NULL;*/

		prev = ulist_remove_packet(list, ipacket);
		list->num_packets --;

		/* last particle of the previous packet
		 * is linked to the first particle in the next packet*/
		if (prev != list->tail)
		{
			printf("last\n");
			prev->data[prev->num_elems-1].next = &prev->next->data[0];
			printf("last2\n");
		}
		else prev->data[prev->num_elems-1].next = NULL;
	}



	/*the particle is not the only one in the packet*/
	else
	{
		/* swap the current element of the array with the last element (unless it is the last already) */
		if (idata != packet->num_elems-1)
		{
			packet->data[idata] = packet->data[packet->num_elems-1];
			packet->data[idata].next = &packet->data[idata+1];
		}

		/* now decrement the size of the array */
		packet->num_elems --;


		if (idata != 0 && idata != packet->num_elems-1)
		{
			/*printf("test\n");*/
			/* the particle was inside a packet
			 * so the particle before is in the same packet */
			packet->data[idata-1].next = &packet->data[idata];
		}


		/* if the removed particle was the first, then the new first
		 * of the packet is linked to the previous packet */
		else if (idata==0 && packet != list->head)
		{
			packet->prev->data[packet->prev->num_elems-1].next = &packet->prev->data[idata];
		}

		/* if the removed particle was the last one of the packet
		 * then the particle before it must be linked to the first
		 * particle in the next packet*/
		else if (idata == packet->num_elems-1 && packet != list->tail)
		{
			packet->data[idata-1].next = &packet->next->data[0];
		}


	}

	/* decrement the number of elements in the list */
	list->num_elems --;
}









Particle * ulist_addelem(UList *list, const Particle *p)
{
	UList_Packet *packet, *prev;
	UINT found;
	packet = list->head;
	prev = NULL;

	found = 0;

	while (packet != NULL && found == 0)
	{
		if (packet->num_elems != NELEM_PACKET)
			found = 1;
		else
		{
			prev = packet;         /* save current packet as the previous*/
			packet = packet->next; /* move the cursor to the next packet*/
		}

	}

	/* all the packets are full, need a new one */
	if (packet == NULL)
	{
		/*printf("all packets are full\n");*/
		packet = ulist_addpacket(list);  /*this is the packet just added */
	}

	/*copy the particle in the first available spot*/
	packet->data[packet->num_elems] = *p;

	packet->num_elems ++;
	list->num_elems ++;


	/* now link the lastly added particle to the previous one
	 * that can't be done if the current packet is the head AND the element
	 * is the first because is that case, there is no previous element*/
	if (packet != list->head && packet->num_elems ==1)
	{
		/*printf("no head but first element\n");*/
		prev->data[prev->num_elems-1].next =  &packet->data[packet->num_elems-1];
	}

	/* we can be in a packet different from the head but not at the first element
	 * in this case the previous particle is in the same array*/
	else if(packet->num_elems != 1)
	{
		/*printf("not first element (any packet)\n");*/
		packet->data[packet->num_elems-2].next = &packet->data[packet->num_elems-1];
	}
	/*else
		printf("must be the head\n");*/

	/*since a particle is always added at the end
	 * of a packet, it has no 'next' */
	packet->data[packet->num_elems-1].next = NULL;

	return &packet->data[packet->num_elems-1];
}








