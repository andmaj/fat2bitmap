CFLAGS = -D_FILE_OFFSET_BITS=64
DOSFS = dosfstools/src

all:
	$(CC) $(CFLAGS) -I ${DOSFS} -Wall -lm -o bin/fat2bitmap \
	src/fat2bitmap.c ${DOSFS}/boot.c ${DOSFS}/charconv.c \
	${DOSFS}/check.c ${DOSFS}/common.c ${DOSFS}/fat.c ${DOSFS}/file.c \
	${DOSFS}/lfn.c ${DOSFS}/io.c 

clean:
	rm bin/fat2bitmap
