# Makefile intital

CC = cc

all: compress

# added CFFLAGS, LDFLAGS and package lm to both
compress: compress.c compress.h compress_params.c
	$(CC) $(CFLAGS) compress.c $(LDFLAGS) -lnetcdf -lm -o compress.exe

compress_github: compress.c compress.h compress_params.c
	$(CC) $(CFLAGS) compress.c $(LDFLAGS) -lnetcdf -lm -o compress.exe

clean:
	rm -rf *exe core *nc
