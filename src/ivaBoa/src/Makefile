# Generated automatically from Makefile.in by configure.
# $Id: Makefile.in,v 1.59 2002/03/24 22:20:19 jnelson Exp $

.SUFFIXES:
.SUFFIXES: .o .c
.PHONY: clean mrclean distclean depend all dist



GCC_FLAGS = -Wstrict-prototypes -Wpointer-arith -Wcast-align -Wcast-qual\
  -Wtraditional\
  -Wshadow\
  -Wconversion\
  -Waggregate-return\
  -Wmissing-prototypes\
  -Wnested-externs\
  -Wall \
  -Wundef -Wwrite-strings -Wredundant-decls -Winline


srcdir = .
VPATH = .:./../extras
LDFLAGS =  -g
LDFLAGS += -liconv
LDFLAGS += -L$(LIB_DIR)

LIBS =  $(MQ_LIB) $(OAL_LIB)
CFLAGS = -g -O2 -pipe -Wall -I.
CFLAGS += -I$(MQ_INCLUDE_DIR) -I$(OAL_INCLUDE_DIR)

# Change these if necessary

YACC = bison -y 
LEX = flex 
CC = $(BUILD_TOOL_PREFIX)gcc 
CPP = $(BUILD_TOOL_PREFIX)gcc -E

SOURCES = alias.c boa.c buffer.c cgi.c cgi_header.c config.c escape.c \
	get.c hash.c ip.c log.c mmap_cache.c pipe.c queue.c read.c \
	request.c response.c select.c signals.c util.c sublog.c isapi.c isapi_formdata.c

OBJS = y.tab.o lex.yy.o $(SOURCES:.c=.o) timestamp.o 

all: boa boa_indexer
	$(BUILD_TOOL_PREFIX)strip boa
	cp boa ivaBoa

boa:	$(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS) $(LIBS)
	
boa_indexer:	index_dir.o escape.o   
	$(CC) -o $@ $^ $(LDFLAGS) $(LIBS)

clean:
	rm -f $(OBJS) boa core lex.yy.c y.tab.c y.tab.h *~ boa_indexer index_dir.o   

install: ivaBoa
	install -d $(BIN_DIR)
	install ivaBoa $(BIN_DIR)
	install ../ivaBoa.conf $(BIN_DIR)
	install ../mime.types $(BIN_DIR)
distclean:	mrclean

mrclean:	clean
	rm -f config.status config.cache config.h Makefile config.log

# parser dependencies

y.tab.c y.tab.h:	boa_grammar.y
	$(YACC) -d $^

lex.yy.c:	boa_lexer.l
	$(LEX) $^

# timestamp

timestamp.o:	$(SOURCES) boa_grammar.y boa_lexer.l

# depend stuff
.depend:
	$(CPP) -MM $(SOURCES) > .depend
        
depend:
	-rm -f .depend
	$(MAKE) .depend
        
include .depend

# tags
tags:	$(SOURCES)
	ctags -o tags $^ *.h

# dist
dist:
	$(MAKE) clean
	./makedist.sh
        
# object dump
boa.objdump:    boa
	objdump --disassemble-all --source boa > $@

