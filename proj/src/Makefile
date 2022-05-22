# name of the program (Minix service)
PROG=proj

# source code files to be compiled
SRCS = proj.c devices/graphics/vc.c devices/graphics/kbd.c devices/graphics/timer.c devices/graphics/utils.c

# additional compilation flags
# "-Wall -Wextra -Werror -I . -std=c11 -Wno-unused-parameter" are already set
CFLAGS += -pedantic

# list of library dependencies
DPADD += ${LIBLCF}
LDADD += -llcf

# include LCOM's makefile that does all the "heavy lifting"
.include <minix.lcom.mk>
