BUILDDIR = ../../build/

SRC_FILES = $(wildcard ../gui/*.cc)

all: Makefile
	lupdate-qt4 $(SRC_FILES) -ts de.ts
	lrelease-qt4 de.ts -qm $(BUILDDIR)de.qm

clean:
	rm -f $(BUILDDIR)*.qm *~