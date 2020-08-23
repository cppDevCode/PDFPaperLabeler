# change application name here (executable output name)
TARGET=etiquetador
 
# compiler
CC=gcc 
# debug
DEBUG=-g
# optimisation
OPT=-O0
# warnings
WARN=-Wall
 
PTHREAD=-pthread
 
CCFLAGS=$(DEBUG) $(OPT) $(WARN) $(PTHREAD) -pipe 
 
GTKLIB=`pkg-config --cflags --libs gtk+-3.0 gthread-2.0`

 
# linker
LD=gcc
LDFLAGS=$(PTHREAD) $(GTKLIB) -lhpdf -lcups -rdynamic
#LDFLAGS=$(PTHREAD) $(GTKLIB) -export-dynamic -lhpdf -lcups
#LDFLAGS=$(PTHREAD) $(GTKLIB) -export-dynamic -L/home/alejandro/Documentos/Programacion/Proyectos/etiquetador/lib -lhpdf -lcups

OBJS=    main.o impresora.o generoPDF.o
 
all: $(OBJS)
	$(LD) -o $(TARGET) $(OBJS) $(LDFLAGS)

generoPDF.o: src/generoPDF.c
	$(CC) src/generoPDF.c -c $(CCFLAGS) -lhpdf $(GTKLIB) -o generoPDF.o
impresora.o: src/impresora.c
	$(CC)  src/impresora.c -c $(CCFLAGS) $(GTKLIB) -o impresora.o
main.o: src/main.c
	$(CC) src/main.c -c $(CCFLAGS) $(GTKLIB) -o main.o
clean:
	rm -f *.o $(TARGET)
