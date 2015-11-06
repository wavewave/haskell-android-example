#include <map>
#include <android-bridge.h>


extern "C" { 
  void
  Java_com_uphere_vchatter_Chatter_onCreateHS( JNIEnv* env, jobject activity, jint k);
  
  void
  Java_com_uphere_vchatter_ObjectRegisterer_registerJRef( JNIEnv* env, jobject obj, jint k, jobject v );

  void
Java_com_uphere_vchatter_VideoFragment_onCreateHS( JNIEnv* env, jobject f,
						   jint k, jobject tv );

  void
Java_com_uphere_vchatter_VideoFragment_onClickHS( JNIEnv* env, jobject f,
				     	          jbyteArray nick, jbyteArray msg);




}


#include <stdio.h>
#include <jni.h>

#include <android/log.h>

#include <unistd.h>
#include <sys/types.h>
#include <errno.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>


extern void __stginit_Client(void);

// extern void chatter( void );

extern void (*fptr_callback)(char*, int, char*, int);

extern void (*fptr_calljava)( JNIEnv*, char*, int );

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



void callJniTest( JNIEnv* env, char* cmsg, int n )
{
  jbyteArray jmsg = env->NewByteArray(n);
  env->SetByteArrayRegion(jmsg,0,n,(jbyte*)cmsg);

  auto it = ref_objs.find(activityId);
  if( it != ref_objs.end() ) {
    env->CallVoidMethod(it->second,ref_mid,jmsg);
  }  else {
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


void
Java_com_uphere_vchatter_ObjectRegisterer_registerJRef( JNIEnv* env, jobject obj, jint k, jobject v )
{
  jobject ref = env->NewGlobalRef(v);
  ref_objs[k] = ref; 
}
  
void
Java_com_uphere_vchatter_VideoFragment_onCreateHS( JNIEnv* env, jobject f,
						   jint k, jobject tv )
{
  jobject ref  = env->NewGlobalRef(tv);
  ref_objs[k] = ref;
}

void
Java_com_uphere_vchatter_VideoFragment_onClickHS( JNIEnv* env, jobject f,
				     	          jbyteArray nick, jbyteArray msg)
{
  jbyte* cnick = env->GetByteArrayElements(nick,NULL);
  jbyte* cmsg  = env->GetByteArrayElements(msg,NULL);
  pthread_mutex_lock(&lock);
  strcpy( messagebox , (char*)cmsg);
  strcpy( nickbox, (char*)cnick);
  size_nickbox = env->GetArrayLength(nick);
  size_messagebox = env->GetArrayLength(msg);
  pthread_cond_signal(&cond);
  pthread_mutex_unlock(&lock);
  env->ReleaseByteArrayElements(msg,cmsg,NULL);
  env->ReleaseByteArrayElements(nick,cnick,NULL);
}

// }
