#include <android/log.h>
#include <jni.h>
#include <pthread.h>

#include <unistd.h>

#include "wqueue.h"

wqueue::wqueue() {
  pthread_mutex_init(&wlock, NULL);
  pthread_cond_init (&wcond,NULL);

  pthread_mutex_init(&choreolock,NULL);
  pthread_cond_init (&choreocond,NULL);
}

wqueue::~wqueue() {
  pthread_mutex_destroy(&wlock);
  pthread_cond_destroy(&wcond); 
}

void wqueue::loop( JNIEnv* env,
		   void (*callback)(JNIEnv*, char*, int), void (*flush)(JNIEnv*) ) {
  while( 1 ) {
    //__android_log_write(3, "ANDROIDRUNTIME", "hey I am looping" );
    //sleep(5);
    pthread_cond_wait(&choreocond,&choreolock);
    __android_log_write(3, "ANDROIDRUNTIME", "hey I am looping" );
    
    pthread_mutex_lock(&wlock);
    //pthread_cond_wait(&wcond,&wlock);
    if( msgs.size() ) { 
      for( auto& it :  msgs ) {
	callback( env, it.first, it.second );
      }
      msgs.clear();
      flush( env );
    }
    // pthread_cond_signal(&wcond);
    pthread_mutex_unlock(&wlock);
    
  }
}

void wqueue::write_message( char* cmsg, int n )
{
  pthread_mutex_lock(&wlock);
  msgs.push_back( make_pair( cmsg, n ) );
  //pthread_cond_signal(&wcond);
  //pthread_cond_wait(&wcond,&wlock);
  pthread_mutex_unlock(&wlock);
}
