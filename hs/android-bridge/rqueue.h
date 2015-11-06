#ifndef __RQUEUE__
#define __RQUEUE__

#include <pthread.h>

class rqueue {
 public:
  pthread_mutex_t lock;
  pthread_cond_t  cond;

  int size_nickbox = 0;
  char nickbox[4096];
  
  int size_messagebox = 0;
  char messagebox[4096]; 

 public:
  rqueue();
  ~rqueue();
  void loop( void (*callback)(char*, int, char*, int) );
  void sendMessageFromJavaToHaskell( char* nick, int nnick, char* msg, int nmsg );
  
  
};

#endif

