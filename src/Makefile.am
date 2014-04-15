elephant_reasoner_CFLAGS	= -O2 -Wall

bin_PROGRAMS     			= elephant-reasoner

elephant_reasoner_LDADD		= reasoner/libreasoner.a parser/libowl2fsParser.a index/libindex.a saturation/libsaturation.a hierarchy/libhierarchy.a utils/libutils.a \
	model/libmodel.a hashing/libhashing.a preprocessing/libpreprocessing.a model/tbox/libtboxmodel.a model/abox/libaboxmodel.a model/libmodel.a index/tbox/libindextbox.a \
	index/abox/libindexabox.a

DISTCLEANFILES   			= .deps Makefile
