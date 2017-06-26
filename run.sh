# Compile
make -f Makefile

# Image and param to use
graph_def='1'
if [[ -z $1 ]]; then graph_in="test/graph_in/${graph_def}.txt"; else graph_in=$1; fi

# Run stuff
print() {
  echo "--- $1 ---"
}

# print 'Running secuential'
# time ./build/secuential $graph_in
# print 'Done secuential'
# echo ''

print 'Running parallel'
time mpirun -np 1 ./build/parallel $graph_in
print 'Done parallel'
echo ''

# print 'Running parallel (over-CPU)'
# time OMP_NUM_THREADS=10 ./build/parallel $graph_in $kernel $passes $graph_out
# print 'Done parallel over'
# echo ''

echo 'Done!'
