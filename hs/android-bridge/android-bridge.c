#include <stdio.h>
#include <jni.h>
//#include <HsFFI.h>
#include <Rts.h>
#include <RtsAPI.h>

#include <android/log.h>


#include <unistd.h>
#include <sys/types.h>
#include <errno.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>

#include "uthash.h"

void (*fptr_callback)(char*, int, char*, int);

void register_callback_fptr ( void (*v)(char*, int, char*, int) ) {
  fptr_callback = v;
}

void (*fptr_calljava)( JNIEnv*, char*, int );

static JavaVM* jvm; 

pthread_t thr_haskell;
pthread_t thr_msgread; 
pthread_t thr_msgwrite;

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t  cond = PTHREAD_COND_INITIALIZER;

pthread_mutex_t wlock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t wcond = PTHREAD_COND_INITIALIZER;

int size_nickbox =0;
char nickbox[4096];

int size_messagebox = 0;
char messagebox[4096]; 

int size_wmessage = 0;
char wmessage[4096];

//JNIEnv* ref_env;
//jobject ref_act; 

int activityId;
jmethodID ref_mid;

struct my_jobject {
  int id;
  jobject ref;
  UT_hash_handle hh;
};

struct my_jobject *ref_objs = NULL;
 
void prepareJni( JNIEnv* env ) {
  jclass cls = (*env)->FindClass(env,"com/uphere/vchatter/Chatter"); 
  if( cls ) {
    ref_mid = (*env)->GetMethodID(env, cls, "sendMsgToChatter", "([B)V");
    (*env)->DeleteLocalRef(env,cls);
  }
}

void* haskell_runtime( void* d )
{
  static char *argv[] = { "libhaskell.so", 0 }, **argv_ = argv;
  static int argc = 1;
  static RtsConfig rtsopts = { RtsOptsAll, "-H128m -K64m" };
  hs_init_ghc(&argc,&argv_, rtsopts);
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
  (*jvm)->AttachCurrentThread(jvm,(void**)&env, &args);
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
  (*jvm)->AttachCurrentThread(jvm,(void**)&env, &args);
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
  (*jvm)->GetEnv(jvm, (void **)&env, JNI_VERSION_1_6);
  prepareJni(env);
  
  pthread_create( &thr_haskell, NULL, &haskell_runtime, NULL );
  
  return JNI_VERSION_1_6;
} 

JNIEXPORT void JNICALL JNI_OnUnload( JavaVM *vm, void *pvt ) {
  hs_exit();
  pthread_cond_destroy(&cond);
  pthread_mutex_destroy(&lock);
  pthread_cond_destroy(&wcond);
  pthread_mutex_destroy(&wlock);
  
  JNIEnv* env ;
  (*vm)->GetEnv(vm,(void**)(&env),JNI_VERSION_1_6);

  struct my_jobject *s, *tmp;

  HASH_ITER(hh, ref_objs, s, tmp ) {
    HASH_DEL( ref_objs, s );
    (*env)->DeleteGlobalRef(env,s->ref);
    free(s);
  }
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
 
