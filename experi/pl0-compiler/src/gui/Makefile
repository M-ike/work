CXXFLAGS += -Wall -Werror

BUILDDIR = ../../build/gui/

MOCABLE_HEADERS = mainwindow.h sourcewindow.h tokentablewindow.h parsetreewindow.h\
                  symboltablewindow.h lexertoolbar.h parsertoolbar.h grammarwindow.h\
                  codewindow.h codetoolbar.h interpreterwindow.h interpretertoolbar.h

SRC_FILES = mainwindow.cc sourcewindow.cc tokentablewindow.cc lexertoolbar.cc\
            parsertoolbar.cc parsetreewindow.cc symboltablewindow.cc languagemanager.cc\
            parsetreevisitor.cc grammarwindow.cc codewindow.cc codetoolbar.cc\
            interpreterwindow.cc interpretertoolbar.cc

RES = icon.qrc

ADD_SRCS += $(patsubst %.h,%_moc.cc,$(MOCABLE_HEADERS))
ADD_SRCS += $(patsubst %.qrc,%_qrc.cc,$(RES))

SRCS += $(SRC_FILES) $(ADD_SRCS)

INCPATH = -I/usr/share/qt4/mkspecs/linux-g++ -I/usr/include/qt4/QtCore\
          -I/usr/include/qt4/QtGui -I/usr/include/qt4

DEFINES = -DQT_NO_DEBUG -DQT_GUI_LIB -DQT_CORE_LIB -DQT_SHARED

OBJS = $(patsubst %.cc,$(BUILDDIR)%.o,$(SRCS))
DEBS = $(patsubst %.cc,$(BUILDDIR)%.d,$(SRCS))

all: Makefile $(ADD_SRCS) $(OBJS)

-include $(DEBS)

%_qrc.cc: %.qrc
	/usr/bin/rcc -name icon  $< -o $@

$(BUILDDIR)%.o: %.cc
	g++ $(CXXFLAGS) $(INCPATH) -c $< -MD -o $@

%_moc.cc: %.h
	/usr/bin/moc-qt4 $(DEFINES) $(INCPATH) $< -o $@

clean:
	rm -f $(OBJS) $(DEBS) $(ADD_SRCS) *~
