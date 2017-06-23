compile: src/main.c
	mkdir -p build
	# gcc -o build/secuential src/main.c
	mpicc ./src/main.c -o ./build/parallel
