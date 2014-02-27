CXXFLAGS += -Wall -Werror

BUILDDIR = ../build/

TARGET = $(BUILDDIR)pl0-compiler

INCPATH = -I. -I/usr/share/qt4/mkspecs/linux-g++ -I/usr/include/qt4/QtCore -I/usr/include/qt4/QtCore\
          -I/usr/include/qt4/QtGui -I/usr/include/qt4/QtGui -I/usr/include/qt4
LIBS  = -L/usr/lib -ll -lQtGui -L/usr/lib -L/usr/X11R6/lib -laudio -lXt -lSM -lICE\
        -lX11 -lQtCore -lm -pthread -lrt -ldl -lpthread


SRCS = main.cc pl0controller.cc common.cc

OBJS = $(patsubst %.cc,$(BUILDDIR)%.o,$(SRCS))

DEBS = $(patsubst %.cc,$(BUILDDIR)%.d,$(SRCS)) $(wildcard $(BUILDDIR)gui/*.d) $(wildcard $(BUILDDIR)lexer/*.d)\
       $(wildcard $(BUILDDIR)parser/*.d) $(wildcard $(BUILDDIR)codegenerator/*.d)\
       $(wildcard $(BUILDDIR)interpreter/*.d)


all: Makefile buils_dirs $(TARGET)
	make -C lang
	echo "BUILD SUCCESSFULL"

-include $(DEBS)

buils_dirs:
	mkdir -p $(BUILDDIR)lexer
	mkdir -p $(BUILDDIR)parser
	mkdir -p $(BUILDDIR)codegenerator
	mkdir -p $(BUILDDIR)interpreter
	mkdir -p $(BUILDDIR)gui

$(TARGET): $(OBJS) $(wildcard $(BUILDDIR)gui/*.o) $(wildcard $(BUILDDIR)lexer/*.o) $(wildcard $(BUILDDIR)parser/*.o)\
           $(wildcard $(BUILDDIR)codegenerator/*.o) $(wildcard $(BUILDDIR)interpreter/*.o)
	make -C lexer
	make -C parser
	make -C codegenerator
	make -C interpreter
	make -C gui
	g++ $(OBJS) `ls $(BUILDDIR)gui/*.o` `ls $(BUILDDIR)lexer/*.o` `ls $(BUILDDIR)parser/*.o`\
                    `ls $(BUILDDIR)codegenerator/*.o` `ls $(BUILDDIR)interpreter/*.o` $(LIBS) -o $(TARGET)

$(BUILDDIR)%.o: %.cc
	g++ $(CXXFLAGS) $(INCPATH) -c $< -MD -o $@

clean:
	make -C lexer clean
	make -C parser clean
	make -C codegenerator clean
	make -C interpreter clean
	make -C lang clean
	make -C gui clean
	rm -f $(TARGET) $(OBJS) $(DEBS) *~
