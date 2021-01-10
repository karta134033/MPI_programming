AVXFLAGS = -march=native -mtune=intel -malign-data=compat -mstackrealign -mtls-dialect=gnu2 -maccumulate-outgoing-args 
all: main
main: main.cc
	mpicc -O3 $(AVXFLAGS) -m64 main.cc func.cc -o matmul
