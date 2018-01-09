test_hash_table_CFLAGS	= -O2 -Wall
test_ts_stack_CFLAGS	= -lpthread -pthread
bin_PROGRAMS			= test_stack_fixed_size test_stack test_ts_stack test_hash_table test_set test_list test_hash_map test_hash_map_performance test_hash_table_performance test_map_performance test_map test_hash_string

test_hash_table_LDADD			= ../hashing/libhashing.a
test_hash_map_LDADD				= ../hashing/libhashing.a
test_hash_map_performance_LDADD	= ../hashing/libhashing.a
test_map_performance_LDADD		= ../hashing/libhashing.a
test_map_LDADD					= ../hashing/libhashing.a
test_hash_table_performance_LDADD	= ../hashing/libhashing.a
test_set_LDADD					= ../utils/libutils.a ../hashing/libhashing.a
test_list_LDADD					= ../utils/libutils.a
test_hash_string_LDADD			= ../hashing/libhashing.a
test_ts_stack_LDADD				= ../utils/libutils.a
test_stack_LDADD				= ../utils/libutils.a
test_stack_fixed_size_LDADD		= ../utils/libutils.a

DISTCLEANFILES   			= .deps Makefile
