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


extern void __stginit_Client(void);

extern void chatter( void );

void (*fptr_callback)(JNIEnv*, jobject, char*, char*);

void register_callback_fptr ( void (*v)(JNIEnv*,jobject, char*, char*) ) {
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

jobject ref_act; 

char nickbox[4096];
char messagebox[4096]; 

char wmessage[4096];

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




void* haskell_runtime( void* d )
{
  static char *argv[] = { "libhaskell.so", 0 }, **argv_ = argv;
  static int argc = 1;
  static RtsConfig rtsopts = { RtsOptsAll, "-H128m -K64m" };
  // hs_init(&argc, &argv_);
  hs_init_ghc(&argc,&argv_, rtsopts);
  __android_log_write( 3, "UPHERE", "start" ) ; 
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

    fptr_callback(env,ref_act,nickbox,messagebox);
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
    //__android_log_write(3,"UPHERE","checkpoint1");
    pthread_cond_wait(&wcond,&wlock);
    //__android_log_write(3,"UPHERE","checkpoint2");
    Chatter_sendMsgToChatter( env, ref_act, wmessage );
    //__android_log_write(3,"UPHERE","checkpoint3");
    pthread_cond_signal(&wcond);
    //__android_log_write(3,"UPHERE","checkpoint4");   
    pthread_mutex_unlock(&wlock);

    
    //pthread_mutex_lock(&wlock2);
    //pthread_mutex_unlock(&wlock2);
    //__android_log_write(3,"UPHERE","I've got here 2");    
    
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
  (*env)->DeleteGlobalRef(env,ref_act);
} 

void
Java_com_uphere_vchatter_Chatter_onCreateHS( JNIEnv* env, jobject activity)
{
  ref_act = (*env)->NewGlobalRef(env,activity);
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
  //__android_log_write(3,"UPHERE","write_message");
  //__android_log_write(3,"UPHERE",cmsg);
  pthread_cond_wait(&wcond,&wlock);
  pthread_mutex_unlock(&wlock);

  //pthread_mutex_lock(&wlock2);
  //pthread_mutex_lock(&wlock2);

}
 
