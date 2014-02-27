CXXFLAGS += -Wall -Werror

BUILDDIR = ../../build/lexer/

LEXER_SRC = pl0lexer.lex
LEXER_C = pl0lexer_c.c
LEXER_H = pl0lexer_c.h

SRCS = pl0lexer.cc pl0token.cc

OBJS = $(BUILDDIR)$(LEXER_C:.c=.o) $(patsubst %.cc,$(BUILDDIR)%.o,$(SRCS))
DEBS = $(patsubst %.cc,$(BUILDDIR)%.d,$(SRCS))

all: Makefile $(OBJS)

$(BUILDDIR)$(LEXER_C:.c=.o): $(LEXER_SRC)
	flex -o $(LEXER_C) --yylineno --header-file=$(LEXER_H) $(LEXER_SRC)
	gcc -c $(LEXER_C) -o $(BUILDDIR)$(LEXER_C:.c=.o)

-include $(DEBS)

$(BUILDDIR)%.o: %.cc
	g++ $(CXXFLAGS) -c $< -MD -o $@

clean:
	rm -f $(OBJS) $(DEBS) $(LEXER_C) $(LEXER_H) *~ *.o pl0lexer_test

test: all
	g++ $(CXXFLAGS) pl0lexer_test.cc $(OBJS) -ll -o pl0lexer_test
