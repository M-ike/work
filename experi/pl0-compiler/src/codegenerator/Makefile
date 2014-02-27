CXXFLAGS += -Wall -Werror

BUILDDIR = ../../build/codegenerator/

SRCS = codegenerator.cc

OBJS = $(patsubst %.cc,$(BUILDDIR)%.o,$(SRCS))
DEBS = $(patsubst %.cc,$(BUILDDIR)%.d,$(SRCS))

all: Makefile $(OBJS)

-include $(DEBS)

$(BUILDDIR)%.o: %.cc
	g++ $(CXXFLAGS) $(INCPATH) -c $< -MD -o $@

clean:
	rm -f $(OBJS) $(DEBS) *~
