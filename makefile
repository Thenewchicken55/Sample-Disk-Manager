build:
	gcc -o filesys filesys.c

run:
	gcc -o filesys filesys.c
	./filesys

L:
	gcc -o filesys filesys.c
	./filesys L

P:
	gcc -o filesys filesys.c
	./filesys P msg

M:
	gcc -o filesys filesys.c
	./filesys M doggie

D:
	gcc -o filesys filesys.c
	./filesys D