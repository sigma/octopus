bin_PROGRAMS = server
server_SOURCES = Main.cpp PluginManager.cpp PluginManager.h

# set the include path found by configure
INCLUDES = -I$(top_srcdir)/interfaces -I$(top_srcdir)/utils $(all_includes)

server_LDADD = $(top_builddir)/interfaces/libinterfaces.la

# the library search path.
server_LDFLAGS = $(all_libraries) -ldl
AM_CFLAGS = 
AM_CXXFLAGS = -Wall -Werror -ansi -pedantic
AM_FFLAGS = 
SUBDIRS = 
