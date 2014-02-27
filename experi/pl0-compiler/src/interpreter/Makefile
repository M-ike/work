CXXFLAGS += -Wall -Werror

BUILDDIR = ../../build/interpreter/

SRCS = interpreter.cc

INCPATH = -I/usr/share/qt4/mkspecs/linux-g++ -I/usr/include/qt4/QtCore\
          -I/usr/include/qt4/QtGui -I/usr/include/qt4

OBJS = $(patsubst %.cc,$(BUILDDIR)%.o,$(SRCS))
DEBS = $(patsubst %.cc,$(BUILDDIR)%.d,$(SRCS))

all: Makefile $(OBJS)

-include $(DEBS)

$(BUILDDIR)%.o: %.cc
	g++ $(CXXFLAGS) $(INCPATH) -c $< -MD -o $@

clean:
	rm -f $(OBJS) $(DEBS) *~
