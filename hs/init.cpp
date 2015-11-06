#include <map>
//#include <wqueue.h>
#include <rqueue.h>
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
  int nnick = env->GetArrayLength(nick);
  int nmsg = env->GetArrayLength(msg);
  rq->sendMessageFromJavaToHaskell( (char*)cnick, nnick, (char*)cmsg, nmsg);
  //pthread_mutex_lock(&lock);
  //strcpy( messagebox , (char*)cmsg);
  //strcpy( nickbox, (char*)cnick);
  //size_nickbox = ;
  //size_messagebox = 
  //pthread_cond_signal(&cond);
  //pthread_mutex_unlock(&lock);
  env->ReleaseByteArrayElements(msg,cmsg,NULL);
  env->ReleaseByteArrayElements(nick,cnick,NULL);
}

