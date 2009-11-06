# Makefile for UNIX version of GENESIS
# Modified by CBC to eliminate ga.a archive

# Note: select the appropriate compiler options
# CFLAGS = -O 
CFLAGS = -g		# debugging
CFLAGS += -pedantic -Wall		# help finding errors

COMP = gcc	        # to easily switch between cc and gcc

GA = best.o checkpnt.o convert.o cross.o \
    done.o elitist.o error.o evaluate.o \
    generate.o init.o input.o main.o measure.o \
    mutate.o restart.o schema.o select.o

H = define.h extern.h global.h format.h

f = f1

ga.$f:  $(GA) $f.o
	cc $(CFLAGS) -o ga.$f $f.o $(GA) -lm

$(GA) : $(H)

install : report setup
	chmod ugo+x go

report : report.o error.o
	cc $(CFLAGS) -o report report.o error.o

report.o : $(H)

setup : setup.o
	cc $(CFLAGS) -o setup setup.o

clean:
	rm *.o; rm report; rm setup
