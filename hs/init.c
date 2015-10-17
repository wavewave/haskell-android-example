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

int activityId;
jclass ref_class;
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
    ref_mid = (*env)->GetMethodID(env, cls, "sendMsgToChatter", "(Ljava/lang/String;)V");
    (*env)->DeleteLocalRef(env,cls);
  }
}


void callJniTest( JNIEnv* env, char* cmsg )
{
  jstring jmsg = (*env)->NewStringUTF(env,cmsg);
  if( jmsg ) {
    struct my_jobject *s; 
    //int k = 1;
    
    HASH_FIND_INT( ref_objs, &activityId, s );
    if( s ) {
      (*env)->CallVoidMethod(env,s->ref,ref_mid,jmsg);
      
    }
    else {
      __android_log_write(3, "UPHERE", "NON EXIST");
    }
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
  while( 1 ) {
    pthread_mutex_lock(&wlock);
    pthread_cond_wait(&wcond,&wlock);
    callJniTest( env, wmessage );
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
  pthread_cond_destroy(&wcond2);
  pthread_mutex_destroy(&wlock2);
  
  JNIEnv* env ;
  (*vm)->GetEnv(vm,(void**)(&env),JNI_VERSION_1_6);

  /* unregistering 
  int k = 1 ; 
  struct my_jobject* r;

  HASH_FIND_INT( ref_objs, &k, r );
  if( r ) 
    (*env)->DeleteGlobalRef(env,r->ref);
  */
} 

void
Java_com_uphere_vchatter_Chatter_onCreateHS( JNIEnv* env, jobject activity, jint k)
{
  activityId = k;
  pthread_create( &thr_msgread, NULL, &reader_runtime, NULL );
  pthread_create( &thr_msgwrite, NULL, &writer_runtime, NULL );
}


Java_com_uphere_vchatter_ObjectRegisterer_registerJRef( JNIEnv* env, jobject obj, jint k, jobject v )
{

  __android_log_write(3,"UPHERE", " I Am here" );
  char str[1000] ;
  sprintf(str, "k = %d, v = %d ", k, v );
  __android_log_write(3,"UPHERE", str );
  
  jobject ref = (*env)->NewGlobalRef(env,v);
  struct my_jobject *s ;
  s = malloc(sizeof(struct my_jobject));
  s->id = k ;
  s->ref = ref; 
  HASH_ADD_INT( ref_objs, id, s );

  
}
  
void
Java_com_uphere_vchatter_VideoFragment_onCreateHS( JNIEnv* env, jobject f,
						   jint k, jobject tv )
{
  jobject ref  = (*env)->NewGlobalRef(env,tv);
  struct my_jobject *s ;
  s = malloc(sizeof(struct my_jobject));
  s->id = k ;
  s->ref = ref; 
  HASH_ADD_INT( ref_objs, id, s );
}

void
Java_com_uphere_vchatter_VideoFragment_onClickHS( JNIEnv* env, jobject f,
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
 
