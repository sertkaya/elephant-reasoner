test_hash_table_CFLAGS	= -O2 -Wall

bin_PROGRAMS     			= test_hash_table test_set test_list test_hash_map test_hash_map_performance test_hash_table_performance test_map_performance test_map test_hash_string

test_hash_table_LDADD			= ../hashing/libhashing.a
test_hash_map_LDADD				= ../hashing/libhashing.a
test_hash_map_performance_LDADD	= ../hashing/libhashing.a
test_map_performance_LDADD		= ../hashing/libhashing.a
test_map_LDADD					= ../hashing/libhashing.a
test_hash_table_performance_LDADD	= ../hashing/libhashing.a
test_set_LDADD					= ../utils/libutils.a ../hashing/libhashing.a
test_list_LDADD					= ../utils/libutils.a
test_hash_string_LDADD			= ../hashing/libhashing.a

DISTCLEANFILES   			= .deps Makefile
