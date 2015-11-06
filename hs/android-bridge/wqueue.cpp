#include <android/log.h>
#include <jni.h>
#include <pthread.h>

#include "wqueue.h"

wqueue::wqueue() {
  pthread_mutex_init(&wlock, NULL);
  pthread_cond_init(&wcond,NULL);
}

wqueue::~wqueue() {
  pthread_mutex_destroy(&wlock);
  pthread_cond_destroy(&wcond); 
}

void wqueue::loop( JNIEnv* env, void (*callback)(JNIEnv*, char*, int) ) {
  while( 1 ) {
    pthread_mutex_lock(&wlock);
    pthread_cond_wait(&wcond,&wlock);
    for( auto& it :  msgs ) {
      callback( env, it.first, it.second );
    }
    msgs.clear();
    
    pthread_cond_signal(&wcond);
    pthread_mutex_unlock(&wlock);
    
  }
}

void wqueue::write_message( char* cmsg, int n )
{
  pthread_mutex_lock(&wlock);
  msgs.push_back( make_pair( cmsg, n ) );
  pthread_cond_signal(&wcond);
  pthread_cond_wait(&wcond,&wlock);
  pthread_mutex_unlock(&wlock);
}
