#
# Master Makefile for TREK73
#

MAKE = make

all: programs documentation

programs:
	cd src; $(MAKE) all

documentation:
	cd man; $(MAKE) all

install: all
	cd src; $(MAKE) install
	cd man; $(MAKE) install

clean:
	cd src; $(MAKE) clean
	cd man; $(MAKE) clean
