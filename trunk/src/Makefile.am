elephant_reasoner_CFLAGS	= -O2 -Wall

bin_PROGRAMS     			= elephant-reasoner

elephant_reasoner_LDADD		= reasoner/libreasoner.a parser/libowl2fsParser.a model/libmodel.a preprocessing/libpreprocessing.a index/libindex.a \
	saturation/libsaturation.a hierarchy/libhierarchy.a hashing/libhashing.a utils/libutils.a  

DISTCLEANFILES   			= .deps Makefile
