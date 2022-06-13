# name of the program (Minix service)
PROG=proj

# source code files to be compiled
SRCS = proj.c vg.c kbd.c mouse.c timer.c utils.c game.c letters.c mouse_collision.c

.PATH: ${.CURDIR}
.PATH: ${.CURDIR}/devices/graphics
.PATH: ${.CURDIR}/devices/kbc
.PATH: ${.CURDIR}/devices/timer
.PATH: ${.CURDIR}/devices/utils

# additional compilation flags
# "-Wall -Wextra -Werror -I . -std=c11 -Wno-unused-parameter" are already set
CFLAGS += -pedantic -Wunreachable-code -D __LCOM_OPTIMIZED__

# list of library dependencies
DPADD += ${LIBLCF}
LDADD += -llcf

# include LCOM's makefile that does all the "heavy lifting"
.include <minix.lcom.mk>