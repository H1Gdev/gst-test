# Note: plugindir is set in configure

##############################################################################
# TODO: change libgstplugin.la to something else, e.g. libmysomething.la     #
##############################################################################
plugin_LTLIBRARIES = libgsttestfilter.la

##############################################################################
# TODO: for the next set of variables, name the prefix if you named the .la, #
#  e.g. libmysomething.la => libmysomething_la_SOURCES                       #
#                            libmysomething_la_CFLAGS                        #
#                            libmysomething_la_LIBADD                        #
#                            libmysomething_la_LDFLAGS                       #
##############################################################################

## Plugin 1

# sources used to compile this plug-in
libgsttestfilter_la_SOURCES = gsttestfilter.c gsttestfilter.h

# compiler and linker flags used to compile this plugin, set in configure.ac
libgsttestfilter_la_CFLAGS = $(GST_CFLAGS)
libgsttestfilter_la_LIBADD = $(GST_LIBS)
libgsttestfilter_la_LDFLAGS = $(GST_PLUGIN_LDFLAGS)
libgsttestfilter_la_LIBTOOLFLAGS = --tag=disable-static

# headers we need but don't want installed
noinst_HEADERS = gsttestfilter.h
