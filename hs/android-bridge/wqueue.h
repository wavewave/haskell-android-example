#ifndef __WQUEUE__
#define __WQUEUE__

#include <pthread.h>

class wqueue {

  //private:
  
 public:
  int size_wmessage = 0;
  char wmessage[4096];
  
  pthread_mutex_t wlock; 
  pthread_cond_t  wcond; 

public:
  wqueue();
  ~wqueue();

  void loop( JNIEnv* env, void (*callback)(JNIEnv*, char*, int) );
  void write_message( char* cmsg, int n ); 
};

#endif
