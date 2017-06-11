compile: src/main.c
	mkdir -p build
	gcc -o build/secuential src/main.c
	gcc -o build/parallel src/main.c
