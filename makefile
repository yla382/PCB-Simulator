run: list.o pcb_sim.o
	gcc -o run list.o pcb_sim.o

list.o: list.c list.h
	gcc -c list.c

pcb_sim.o: pcb_sim.c list.h
	gcc -c pcb_sim.c

clean:
	rm  run pcb_sim.o list.o
