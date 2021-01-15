#prout
# -------------------------------
#   NA 2008
#   Project      : PIHYB2D
#   Function     : MAKEFILE
#   Module       : --
#   File         : makefileMPI
#   Creation     : 17-06-2008
#   Modification : 17-06-2008
#   -----------------------------
#   Notes :
#   -----------------------------




#Compiler's name
NCOMPILO = gcc
#NCOMPILO = mpicc

#chemins
DSRC   = ./src
DINC   = ./inc
OBJOUT = ./obj
DEXE   = ./bin



#Binary file name
EXEN   = ulist.exe

#options and libraries
#OPT    = -Wall -g -pg -D_DEBUG_PRINTF_
OPT = -Wall -ansi -pedantic -g   #-DCHRONO
LIB    =  -lm 


all: $(DEXE)/$(EXEN)


#compilation commands
CC     = $(NCOMPILO) -I$(DINC) $(OPT) -c
CCL    = $(NCOMPILO)  $(OPT)  -o


#dependancies
OBJECTS =  $(OBJOUT)/ulist.o
OBJECTS += $(OBJOUT)/main_testulist_move.o


#link
$(DEXE)/$(EXEN) : $(OBJECTS)
	$(CCL) $@ $(OBJECTS) $(LIB)

# compile every C file of the main directory
$(OBJOUT)/%.o:$(DSRC)/%.c
	$(CC) -o $@ $<

#clean object files and executable
clean:
	rm $(OBJOUT)/*.o
	rm $(DEXE)/$(EXEN)


