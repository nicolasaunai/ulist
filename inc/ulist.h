

#ifndef ULIST_H_
#define ULIST_H_


#include <stdlib.h>
#include <stdio.h>

typedef unsigned int UINT;

#define NELEM_PACKET 100



typedef struct _particle
{
	float r[3];
	float v[3];
	float q;
	float m;

	UINT pushme;

	struct _particle *next;

} Particle;






typedef struct _ulist_packet
{
	struct _ulist_packet *next;      /* points to the next packet */
	struct _ulist_packet *prev; 	 /* points to the previous packet */
	Particle *data;                  /* array of NumElemt particles */
	UINT num_elems;                  /* nb of elements in the packet */
} UList_Packet;



typedef struct _ulist
{
    UINT num_elems;
    UINT num_packets;
    UList_Packet *head;
    UList_Packet *tail;
} UList;




UList *ulist_new(void);

UList_Packet *ulist_addpacket(UList *list);

UList_Packet *ulist_remove_packet(UList *list, UINT ipacket);

void ulist_print(const UList *list);

void ulist_delete(UList *list);

UINT ulist_size(const UList *list);

Particle * ulist_getelemi(const UList *list, UINT i);

void ulist_removeelem(UList *list, UINT ielem);

Particle * ulist_addelem(UList *list, const Particle *p);

UINT ulist_getnbpacket(const UList *list);

UINT ulist_getnelem(const UList *list);



#endif /* ULIST_H_ */
