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

#include <uthash.h>
#include <android-bridge.h>

extern void __stginit_Client(void);

// extern void chatter( void );

extern void (*fptr_callback)(char*, int, char*, int);

extern void (*fptr_calljava)( JNIEnv*, char*, int );

extern struct my_jobject *ref_objs;

extern pthread_t thr_haskell;
extern pthread_t thr_msgread; 
extern pthread_t thr_msgwrite;

extern pthread_mutex_t lock;
extern pthread_cond_t  cond;


extern int size_nickbox;
extern char nickbox[4096];

extern int size_messagebox;
extern char messagebox[4096]; 

extern int size_wmessage;
extern char wmessage[4096];


int activityId;
jmethodID ref_mid;


/* 
void prepareJni( JNIEnv* env ) {
  jclass cls = (*env)->FindClass(env,"com/uphere/vchatter/Chatter"); 
  if( cls ) {
    ref_mid = (*env)->GetMethodID(env, cls, "sendMsgToChatter", "([B)V");
    (*env)->DeleteLocalRef(env,cls);
  }
}
*/

void callJniTest( JNIEnv* env, char* cmsg, int n )
{
  // jstring jmsg = (*env)->NewStringUTF(env,cmsg);
  jbyteArray jmsg = (*env)->NewByteArray(env,n);
  (*env)->SetByteArrayRegion(env,jmsg,0,n,(jbyte*)cmsg);
  struct my_jobject *s; 

  HASH_FIND_INT( ref_objs, &activityId, s );
  if( s ) {
    (*env)->CallVoidMethod(env,s->ref,ref_mid,jmsg);
    
  }
  else {
    __android_log_write(3, "UPHERE", "NON EXIST");
  }

}  



void
Java_com_uphere_vchatter_Chatter_onCreateHS( JNIEnv* env, jobject activity, jint k)
{
  fptr_calljava = callJniTest;
  activityId = k;
  pthread_create( &thr_msgread, NULL, &reader_runtime, NULL );
  pthread_create( &thr_msgwrite, NULL, &writer_runtime, NULL );
}


Java_com_uphere_vchatter_ObjectRegisterer_registerJRef( JNIEnv* env, jobject obj, jint k, jobject v )
{
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
				     	          jbyteArray nick, jbyteArray msg)
{
  char* cnick = (*env)->GetByteArrayElements(env,nick,NULL);
  char* cmsg  = (*env)->GetByteArrayElements(env,msg,NULL);
  pthread_mutex_lock(&lock);
  strcpy( messagebox , cmsg);
  strcpy( nickbox, cnick);
  size_nickbox = (*env)->GetArrayLength(env,nick);
  size_messagebox = (*env)->GetArrayLength(env,msg);
  pthread_cond_signal(&cond);
  pthread_mutex_unlock(&lock);
  (*env)->ReleaseByteArrayElements(env,msg,cmsg,NULL);
  (*env)->ReleaseByteArrayElements(env,nick,cnick,NULL);
}

