elephant_reasoner_CFLAGS	= -DNDEBUG -Wall

bin_PROGRAMS     			= elephant-reasoner

elephant_reasoner_LDADD		= reasoner/libreasoner.a parser/libowl2fsParser.a index/libindex.a saturation/libsaturation.a hierarchy/libhierarchy.a utils/libutils.a model/libmodel.a  /usr/local/lib/libJudy.a

DISTCLEANFILES   			= .deps Makefile
