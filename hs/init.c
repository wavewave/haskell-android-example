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

extern void __stginit_Client(void);

extern void chatter( void );

void (*fptr_callback)(char*, char*);

void register_callback_fptr ( void (*v)(char*, char*) ) {
  fptr_callback = v;
}

static JavaVM* jvm; 

pthread_t thr_haskell;
pthread_t thr_msgread; 
pthread_t thr_msgwrite;

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t  cond = PTHREAD_COND_INITIALIZER;

pthread_mutex_t wlock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t wcond = PTHREAD_COND_INITIALIZER;

pthread_mutex_t wlock2 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t wcond2 = PTHREAD_COND_INITIALIZER;


char nickbox[4096];
char messagebox[4096]; 

char wmessage[4096];

//JNIEnv* ref_env;
//jobject ref_act; 
jclass ref_class;
jmethodID ref_mid;

struct my_jobject {
  int id;
  jobject ref;
  UT_hash_handle hh;
};

struct my_jobject *ref_objs = NULL;


/*
void Chatter_sendMsgToChatter ( JNIEnv* env, jobject activity, char* cmsg ) { 
  jclass cls = (*env)->GetObjectClass(env, activity);
  if( cls ) {
    jmethodID mid =
      (*env)->GetMethodID(env, cls, "sendMsgToChatter",
			  "(Ljava/lang/String;)V");
    if( mid ) {
      jstring jmsg = (*env)->NewStringUTF(env,cmsg);
      if( jmsg ) { 
        (*env)->CallVoidMethod(env,activity,mid,jmsg);
      }
    }
    (*env)->DeleteLocalRef(env,cls);
  }
}
*/

void prepareJni( JNIEnv* env ) {
  struct my_jobject* s;
  int k = 1;
  HASH_FIND_INT( ref_objs, &k, s );
  jclass cls = (*env)->GetObjectClass(env, s->ref);
  //jclass cls = (*env)->FindClass(env,"com/uphere/jchatter/Chatter"); 
  if( cls ) {
    ref_mid = (*env)->GetMethodID(env, cls, "sendMsgToChatter", "(Ljava/lang/String;)V");
    // (*env)->DeleteLocalRef(env,cls);
  }
}


void callJniTest( JNIEnv* env, char* cmsg )
{
  jstring jmsg = (*env)->NewStringUTF(env,cmsg);
  if( jmsg ) {
    struct my_jobject *s; 
    int k = 1;
    HASH_FIND_INT( ref_objs, &k, s );
    (*env)->CallVoidMethod(env,s->ref,ref_mid,jmsg);
  }  
}  


void* haskell_runtime( void* d )
{
  static char *argv[] = { "libhaskell.so", 0 }, **argv_ = argv;
  static int argc = 1;
  static RtsConfig rtsopts = { RtsOptsAll, "-H128m -K64m" };
  // hs_init(&argc, &argv_);
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

    fptr_callback(nickbox,messagebox);
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
  prepareJni(env);
  while( 1 ) {
    pthread_mutex_lock(&wlock);
    pthread_cond_wait(&wcond,&wlock);
    //Chatter_sendMsgToChatter( env, ref_act, wmessage );
    callJniTest( env, wmessage );
    pthread_cond_signal(&wcond);
    pthread_mutex_unlock(&wlock);
    
  }
  return NULL;
}

JNIEXPORT jint JNICALL JNI_OnLoad( JavaVM *vm, void *pvt ) {
  jvm = vm; 
  pthread_create( &thr_haskell, NULL, &haskell_runtime, NULL );
  
  return JNI_VERSION_1_6;
} 

JNIEXPORT void JNICALL JNI_OnUnload( JavaVM *vm, void *pvt ) {
  hs_exit();
  pthread_cond_destroy(&cond);
  pthread_mutex_destroy(&lock);
  pthread_cond_destroy(&wcond);
  pthread_mutex_destroy(&wlock);
  pthread_cond_destroy(&wcond2);
  pthread_mutex_destroy(&wlock2);
  
  JNIEnv* env ;
  (*vm)->GetEnv(vm,(void**)(&env),JNI_VERSION_1_6);
  int k = 1 ; 
  struct my_jobject* r;

  HASH_FIND_INT( ref_objs, &k, r );
  if( r ) 
    (*env)->DeleteGlobalRef(env,r->ref);
} 

void
Java_com_uphere_vchatter_Chatter_onCreateHS( JNIEnv* env, jobject activity)
{
  jobject ref_act = (*env)->NewGlobalRef(env,activity);
  struct my_jobject *s ;
  s = malloc(sizeof(struct my_jobject));
  s->id = 1 ;
  s->ref = ref_act; 
  HASH_ADD_INT( ref_objs, id, s );
  pthread_create( &thr_msgread, NULL, &reader_runtime, NULL );
  pthread_create( &thr_msgwrite, NULL, &writer_runtime, NULL );
}
  
void
Java_com_uphere_vchatter_VideoFragment_onClickHS( JNIEnv* env, jobject f, jobject v,
				     	          jstring nick, jstring msg)
{
  char* cmsg = (*env)->GetStringUTFChars(env,msg,0);
  char* cnick = (*env)->GetStringUTFChars(env,nick,0);
  pthread_mutex_lock(&lock);
  strcpy( messagebox , cmsg);
  strcpy( nickbox, cnick);
  pthread_cond_signal(&cond);
  pthread_mutex_unlock(&lock);
  (*env)->ReleaseStringUTFChars(env,msg,cmsg);
  (*env)->ReleaseStringUTFChars(env,nick,cnick);
  
}

void write_message( char* cmsg )
{
  pthread_mutex_lock(&wlock);
  strcpy( wmessage , cmsg);
  pthread_cond_signal(&wcond);
  pthread_cond_wait(&wcond,&wlock);
  pthread_mutex_unlock(&wlock);
}
 
