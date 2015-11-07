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

  pthread_mutex_init(&choreolock2,NULL);
  pthread_cond_init(&choreocond2,NULL);
}

wqueue::~wqueue() {
  pthread_mutex_destroy(&wlock);
  pthread_cond_destroy(&wcond); 

  pthread_mutex_destroy(&choreolock);
  pthread_cond_destroy(&choreocond); 

  pthread_mutex_destroy(&choreolock2);
  pthread_cond_destroy(&choreocond2); 

}

void wqueue::loop( JNIEnv* env,
		   void (*callback)(JNIEnv*, message*),
		   void (*flush)(JNIEnv*) ) {
  while( 1 ) {
    pthread_cond_wait(&choreocond,&choreolock);

    pthread_cond_signal(&choreocond2);
    
    pthread_mutex_lock(&wlock);
    if( msgs.size() ) { 
      for( auto& it :  msgs ) {
	message* msg = new message;
	msg->tag = TAG_MSG;
	msg->text = it.first;
	msg->sz_text = it.second;
	callback( env, msg );
      }
      msgs.clear();
    }
    //
    message* msg2 = new message;
    msg2->tag = TAG_COORD;
    msg2->x = coord.first;
    msg2->y = coord.second; 
    callback( env, msg2 );
    
    flush( env );
    
    pthread_mutex_unlock(&wlock);
    
  }
}


void wqueue::loop2( void (*callback)() ) { 
  while( 1 ) {
    pthread_cond_wait(&choreocond2,&choreolock2);
    callback();
    //__android_log_write( ANDROID_LOG_DEBUG, "ANDROIDRUNTIME", "loop2" );
  }
}

void wqueue::write_message( char* cmsg, int n )
{
  pthread_mutex_lock(&wlock);
  msgs.push_back( make_pair( cmsg, n ) );
  pthread_mutex_unlock(&wlock);
}

void wqueue::write_coord( int x, int y )
{
  //pthread_mutex_lock(&wlock);
  coord = make_pair(x,y);
  //pthread_mutex_unlock(&wlock);
}

