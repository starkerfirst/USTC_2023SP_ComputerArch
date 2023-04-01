OPT ?= -O3 -ffast-math -ftree-vectorize

CFLAGS   = -std=gnu11 $(OPT) -Wall -Wextra -Wpedantic -Wstrict-aliasing -static

EXEC=lfsr merge mm sieve spmv

all: $(EXEC)

%: %.c *.h
	$(CC) -o $@ $< $(CFLAGS)

clean: 
	@rm -f $(EXEC)

gen_arr:
	python rand_c_arr.py --len=8192 --range=1000000
	python rand_spmv_arrs.py
