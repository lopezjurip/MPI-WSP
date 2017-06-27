compile: src/common.c src/main.c src/parallel.c
	mkdir -p build
	gcc  src/common.c ./src/main.c -o ./build/secuential
	mpicc  src/common.c ./src/parallel.c -o ./build/parallel
