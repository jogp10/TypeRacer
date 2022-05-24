# name of the program (Minix service)
PROG=proj

# source code files to be compiled
SRCS = proj.c vc.c kbd.c timer.c utils.c

.PATH: ${.CURDIR}/devices/graphics
.PATH: ${.CURDIR}/devices/kbd
.PATH: ${.CURDIR}/devices/mouse
.PATH: ${.CURDIR}/devices/timer
.PATH: ${.CURDIR}/devices/utils


# additional compilation flags
# "-Wall -Wextra -Werror -I . -std=c11 -Wno-unused-parameter" are already set
CFLAGS += -pedantic

# list of library dependencies
DPADD += ${LIBLCF}
LDADD += -llcf

# include LCOM's makefile that does all the "heavy lifting"
.include <minix.lcom.mk>
