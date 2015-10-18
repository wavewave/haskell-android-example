#include <uthash.h>
#include <jni.h>

struct my_jobject {
  int id;
  jobject ref;
  UT_hash_handle hh;
};

void* haskell_runtime( void* d );

void* reader_runtime( void* d );

void* writer_runtime( void* d );

