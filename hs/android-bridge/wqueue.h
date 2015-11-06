#ifndef __WQUEUE__
#define __WQUEUE__

#include <pthread.h>
//
#include <vector>
#include <string>

using namespace std;

class wqueue {

 private:
  vector<pair<char*,int> > msgs;
  
  pthread_mutex_t wlock; 
  pthread_cond_t  wcond; 

public:
  wqueue();
  ~wqueue();

  void loop( JNIEnv* env, void (*callback)(JNIEnv*, char*, int) );
  void write_message( char* cmsg, int n ); 
};

#endif
