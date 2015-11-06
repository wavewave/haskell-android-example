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
  pthread_mutex_t choreolock;
  pthread_cond_t  choreocond;
  
 public:
  wqueue();
  ~wqueue();

  void loop( JNIEnv* env, void (*callback)(JNIEnv*, char*, int), void (*flush)(JNIEnv*) );
  void write_message( char* cmsg, int n ); 
};

#endif
