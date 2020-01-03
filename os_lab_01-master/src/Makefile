FILES = 1.c 2.c 3.c 4.c 5.c
OFILES = $(FILES:.c=.out)
.SUFFIXES: .c .out

all: $(OFILES)
	echo "Done!"

.c.out:
	gcc $< -o $@