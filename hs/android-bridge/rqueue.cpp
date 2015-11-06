#include <jni.h>
#include <pthread.h>
#include "rqueue.h"

rqueue::rqueue() {
  pthread_mutex_init(&lock, NULL);
  pthread_cond_init(&cond,NULL);
}

rqueue::~rqueue() {
  pthread_mutex_destroy(&lock);
  pthread_cond_destroy(&cond); 
}

void rqueue::loop( void (*callback)(char*, int, char*, int) ) {
  while( 1 ) {
    pthread_mutex_lock(&lock);
    pthread_cond_wait(&cond,&lock);
    pthread_mutex_unlock(&lock);
    callback(nickbox,size_nickbox,messagebox,size_messagebox);
  }
}


void rqueue::sendMessageFromJavaToHaskell( char* nick, int nnick, char* msg, int nmsg )
{
  pthread_mutex_lock(&lock);
  strcpy( messagebox , msg);
  strcpy( nickbox, nick);
  size_nickbox = nnick; 
  size_messagebox = nmsg;
  pthread_cond_signal(&cond);
  pthread_mutex_unlock(&lock);
  
}
