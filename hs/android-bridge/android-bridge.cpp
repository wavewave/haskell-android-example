#include <map>
#include <queue>
#include "android-bridge.h"
#include "wqueue.h"

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

wqueue* wq; 
rqueue* rq;

void (*fptr_callback)(char*, int, char*, int);

void register_callback_fptr ( void (*v)(char*, int, char*, int) ) {
  fptr_callback = v;
}

void (*fptr_calljava)( JNIEnv*, char*, int );
void (*fptr_flushjava) (JNIEnv* );

JavaVM* jvm;

jmethodID ref_mid1;
jmethodID ref_mid2;

pthread_t thr_haskell;
pthread_t thr_msgread; 
pthread_t thr_msgwrite;

   
void prepareJni( JNIEnv* env ) {
  jclass cls1 = env->FindClass("com/uphere/vchatter/Chatter"); 
  if( cls1 ) {
    ref_mid1 = env->GetMethodID(cls1, "sendMsgToChatter", "([B)V");
    ref_mid2 = env->GetMethodID(cls1, "flushMsg", "()V");
    env->DeleteLocalRef(cls1);
  } else {
    __android_log_write( ANDROID_LOG_ERROR, "ANDROIDRUNTIME", "No such class Chatter");
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
  rq->loop( fptr_callback );
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
  wq->loop( env, fptr_calljava, fptr_flushjava ); 
  return NULL;
}

void write_message( char* cmsg, int n)
{
  wq->write_message( cmsg, n );
}  


JNIEXPORT jint JNICALL JNI_OnLoad( JavaVM *vm, void *pvt ) {
  jvm = vm;
  JNIEnv* env;
  jvm->GetEnv((void **)&env, JNI_VERSION_1_6);

  __android_log_write( ANDROID_LOG_DEBUG, "ANDROIDRUNTIME", "C++ test 2"  );
  wq = new wqueue();
  rq = new rqueue();
  prepareJni(env);
  
  pthread_create( &thr_haskell, NULL, &haskell_runtime, NULL );
  
  return JNI_VERSION_1_6;
} 




JNIEXPORT void JNICALL JNI_OnUnload( JavaVM *vm, void *pvt ) {
  hs_exit();
  
  JNIEnv* env ;
  vm->GetEnv((void**)(&env),JNI_VERSION_1_6);

  for(auto& it : ref_objs ) { 
    env->DeleteGlobalRef(it.second);
  }

  delete wq;
} 


