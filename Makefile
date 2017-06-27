compile: src/main.c src/parallel.c
	mkdir -p build
	gcc ./src/main.c -o ./build/secuential
	mpicc ./src/parallel.c -o ./build/parallel
