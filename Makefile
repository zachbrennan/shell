# Makefile for the CS:APP Shell Lab

TEAM = NOBODY
VERSION = 1
DRIVER = ./sdriver.pl
TSH = ./snsh
TRACEPATH = ./traces/
TSHREF = ./referenceShell/tshref
TSHARGS = "-p"
CC = gcc
CFLAGS = -Wall -O2 -g
FILES = $(TSH) ./myspin ./mysplit ./mystop ./myint

all: $(FILES)


##################
# Regression tests
##################

# Run tests using the student's shell program
test01:
	$(DRIVER) -t $(TRACEPATH)trace01.txt -s $(TSH) -a $(TSHARGS)
test02:
	$(DRIVER) -t $(TRACEPATH)trace02.txt -s $(TSH) -a $(TSHARGS)
test03:
	$(DRIVER) -t $(TRACEPATH)trace03.txt -s $(TSH) -a $(TSHARGS)
test04:
	$(DRIVER) -t $(TRACEPATH)trace04.txt -s $(TSH) -a $(TSHARGS)
test05:
	$(DRIVER) -t $(TRACEPATH)trace05.txt -s $(TSH) -a $(TSHARGS)
test06:
	$(DRIVER) -t $(TRACEPATH)trace06.txt -s $(TSH) -a $(TSHARGS)
test07:
	$(DRIVER) -t $(TRACEPATH)trace07.txt -s $(TSH) -a $(TSHARGS)
test08:
	$(DRIVER) -t $(TRACEPATH)trace08.txt -s $(TSH) -a $(TSHARGS)
test09:
	$(DRIVER) -t $(TRACEPATH)trace09.txt -s $(TSH) -a $(TSHARGS)
test10:
	$(DRIVER) -t $(TRACEPATH)trace10.txt -s $(TSH) -a $(TSHARGS)
test11:
	$(DRIVER) -t $(TRACEPATH)trace11.txt -s $(TSH) -a $(TSHARGS)
test12:
	$(DRIVER) -t $(TRACEPATH)trace12.txt -s $(TSH) -a $(TSHARGS)
test13:
	$(DRIVER) -t $(TRACEPATH)trace13.txt -s $(TSH) -a $(TSHARGS)
test14:
	$(DRIVER) -t $(TRACEPATH)trace14.txt -s $(TSH) -a $(TSHARGS)
test15:
	$(DRIVER) -t $(TRACEPATH)trace15.txt -s $(TSH) -a $(TSHARGS)
test16:
	$(DRIVER) -t $(TRACEPATH)trace16.txt -s $(TSH) -a $(TSHARGS)

# Run the tests using the reference shell program
rtest01:
	$(DRIVER) -t $(TRACEPATH)trace01.txt -s $(TSHREF) -a $(TSHARGS)
rtest02:
	$(DRIVER) -t $(TRACEPATH)trace02.txt -s $(TSHREF) -a $(TSHARGS)
rtest03:
	$(DRIVER) -t $(TRACEPATH)trace03.txt -s $(TSHREF) -a $(TSHARGS)
rtest04:
	$(DRIVER) -t $(TRACEPATH)trace04.txt -s $(TSHREF) -a $(TSHARGS)
rtest05:
	$(DRIVER) -t $(TRACEPATH)trace05.txt -s $(TSHREF) -a $(TSHARGS)
rtest06:
	$(DRIVER) -t $(TRACEPATH)trace06.txt -s $(TSHREF) -a $(TSHARGS)
rtest07:
	$(DRIVER) -t $(TRACEPATH)trace07.txt -s $(TSHREF) -a $(TSHARGS)
rtest08:
	$(DRIVER) -t $(TRACEPATH)trace08.txt -s $(TSHREF) -a $(TSHARGS)
rtest09:
	$(DRIVER) -t $(TRACEPATH)trace09.txt -s $(TSHREF) -a $(TSHARGS)
rtest10:
	$(DRIVER) -t $(TRACEPATH)trace10.txt -s $(TSHREF) -a $(TSHARGS)
rtest11:
	$(DRIVER) -t $(TRACEPATH)trace11.txt -s $(TSHREF) -a $(TSHARGS)
rtest12:
	$(DRIVER) -t $(TRACEPATH)trace12.txt -s $(TSHREF) -a $(TSHARGS)
rtest13:
	$(DRIVER) -t $(TRACEPATH)trace13.txt -s $(TSHREF) -a $(TSHARGS)
rtest14:
	$(DRIVER) -t $(TRACEPATH)trace14.txt -s $(TSHREF) -a $(TSHARGS)
rtest15:
	$(DRIVER) -t $(TRACEPATH)trace15.txt -s $(TSHREF) -a $(TSHARGS)
rtest16:
	$(DRIVER) -t $(TRACEPATH)trace16.txt -s $(TSHREF) -a $(TSHARGS)


# clean up
clean:
	rm -f $(FILES) *.o *~


