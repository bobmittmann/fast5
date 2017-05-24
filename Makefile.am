## Process this file with automake to produce Makefile.in

AUTOMAKE_OPTIONS = subdir-objects

#AM_CPPFLAGS = -DPACKAGE_DATA_DIR=\""$(datadir)"\" 
#AM_CPPFLAGS = -I$(top_srcdir)/../../include
#ACLOCAL_AMFLAGS = -I m4 --install

AM_CPPFLAGS = -I$(top_srcdir)/include

AM_CFLAGS = -O2 -Wall

bin_PROGRAMS = f5dump f5vcd

f5dump_SOURCES = src/f5dump.c src/fast5.c

f5vcd_SOURCES = src/f5vcd.c src/fast5.c src/vcd.c
