#include <map>
#include "android-bridge.h"

std::map<int, jobject> ref_objs;

#include <stdio.h>
#include <jni.h>
#include <HsFFI.h>

#include <android/log.h>


#include <unistd.h>
#include <sys/types.h>
#include <errno.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>

  

void (*fptr_callback)(char*, int, char*, int);

void register_callback_fptr ( void (*v)(char*, int, char*, int) ) {
  fptr_callback = v;
}

void (*fptr_calljava)( JNIEnv*, char*, int );

JavaVM* jvm; 

pthread_t thr_haskell;
pthread_t thr_msgread; 
pthread_t thr_msgwrite;

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t  cond = PTHREAD_COND_INITIALIZER;

pthread_mutex_t wlock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t wcond  = PTHREAD_COND_INITIALIZER;

int size_nickbox = 0;
char nickbox[4096];

int size_messagebox = 0;
char messagebox[4096]; 

int size_wmessage = 0;
char wmessage[4096];

jmethodID ref_mid;

   
void prepareJni( JNIEnv* env ) {
  jclass cls = env->FindClass("com/uphere/vchatter/Chatter"); 
  if( cls ) {
    ref_mid = env->GetMethodID(cls, "sendMsgToChatter", "([B)V");
    env->DeleteLocalRef(cls);
  }
}

void* haskell_runtime( void* d )
{
  static char *argv[] = { "libhaskell.so", 0 }, **argv_ = argv;
  static int argc = 1;
  //static RtsConfig rtsopts = { RtsOptsAll, "-H128m -K64m" };
  // hs_init_ghc(&argc,&argv_, rtsopts);
  hs_init(&argc,&argv_);
  chatter();
  return NULL;
}

void* reader_runtime( void* d )
{
  JNIEnv* env;
  JavaVMAttachArgs args;
  args.version = JNI_VERSION_1_6;
  args.name = NULL;
  args.group = NULL;
  jvm->AttachCurrentThread(&env, &args);
  while( 1 ) {
    pthread_mutex_lock(&lock);
    pthread_cond_wait(&cond,&lock);
    pthread_mutex_unlock(&lock);
    fptr_callback(nickbox,size_nickbox,messagebox,size_messagebox);
  }
  return NULL;
}

void* writer_runtime( void* d )
{
  JNIEnv* env;
  JavaVMAttachArgs args;
  args.version = JNI_VERSION_1_6;
  args.name = NULL;
  args.group = NULL;
  jvm->AttachCurrentThread(&env, &args);
  while( 1 ) {
    pthread_mutex_lock(&wlock);
    pthread_cond_wait(&wcond,&wlock);
    fptr_calljava( env, wmessage, size_wmessage );
    pthread_cond_signal(&wcond);
    pthread_mutex_unlock(&wlock);
    
  }
  return NULL;
}

JNIEXPORT jint JNICALL JNI_OnLoad( JavaVM *vm, void *pvt ) {
  jvm = vm;
  JNIEnv* env;
  jvm->GetEnv((void **)&env, JNI_VERSION_1_6);

  __android_log_write( ANDROID_LOG_DEBUG, "ANDROIDRUNTIME", "C++ test 2"  );
  
  prepareJni(env);
  
  pthread_create( &thr_haskell, NULL, &haskell_runtime, NULL );
  
  return JNI_VERSION_1_6;
} 


void write_message( char* cmsg, int n )
{
  pthread_mutex_lock(&wlock);
  strcpy( wmessage , cmsg);
  size_wmessage = n;
  pthread_cond_signal(&wcond);
  pthread_cond_wait(&wcond,&wlock);
  pthread_mutex_unlock(&wlock);
}


JNIEXPORT void JNICALL JNI_OnUnload( JavaVM *vm, void *pvt ) {
  hs_exit();
  pthread_cond_destroy(&cond);
  pthread_mutex_destroy(&lock);
  pthread_cond_destroy(&wcond);
  pthread_mutex_destroy(&wlock);
  
  JNIEnv* env ;
  vm->GetEnv((void**)(&env),JNI_VERSION_1_6);

  for(auto& it : ref_objs ) { 
    env->DeleteGlobalRef(it.second);
  }

} 
