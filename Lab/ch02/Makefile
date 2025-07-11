myapp: greeting.o myapp.o
	gcc myapp.o greeting.o -o myapp
greeting.o: greeting.c greeting.h
	gcc -c greeting.c
myapp.o: myapp.c greeting.h
	gcc -c myapp.c 
clean:
	rm -rf *.o


