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

void wqueue::loop( JNIEnv* env, void (*callback)(JNIEnv*, message*), void (*flush)(JNIEnv*) ) {
  while( 1 ) {
    pthread_cond_wait(&choreocond,&choreolock);

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

void wqueue::write_message( char* cmsg, int n )
{
  pthread_mutex_lock(&wlock);
  msgs.push_back( make_pair( cmsg, n ) );
  pthread_mutex_unlock(&wlock);
}

void wqueue::write_coord( int x, int y )
{
  pthread_mutex_lock(&wlock);
  coord = make_pair(x,y);
  pthread_mutex_unlock(&wlock);
}

