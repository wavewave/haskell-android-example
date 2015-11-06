#include <malloc.h>
#include <map>
#include <rqueue.h>
#include <android-bridge.h>


extern "C" { 
  void
  Java_com_uphere_vchatter_Chatter_onCreateHS( JNIEnv* env, jobject activity, jint k);

  void
  Java_com_uphere_vchatter_Chatter_onFrameHS( JNIEnv* env );
  

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

void callSendMsgToChatter( JNIEnv* env, char* cmsg, int n )
{
  jbyteArray jmsg = env->NewByteArray(n);
  env->SetByteArrayRegion(jmsg,0,n,(jbyte*)cmsg);

  auto it = ref_objs.find(activityId);
  if( it != ref_objs.end() ) {
    env->CallVoidMethod(it->second,ref_mid1,jmsg);
  }  else {
    __android_log_write(3, "UPHERE", "NON EXIST");
  }
  free(cmsg);
}  


void callFlushMsg( JNIEnv* env )
{
  auto it = ref_objs.find(activityId);
  if( it != ref_objs.end() ) {
    env->CallVoidMethod(it->second,ref_mid2);
  }  else {
    __android_log_write(3, "UPHERE", "NON EXIST");
  }
}  


void
Java_com_uphere_vchatter_Chatter_onCreateHS( JNIEnv* env, jobject activity, jint k)
{
  fptr_calljava = callSendMsgToChatter;
  fptr_flushjava = callFlushMsg;
  activityId = k;
  pthread_create( &thr_msgread, NULL, &reader_runtime, NULL );
  pthread_create( &thr_msgwrite, NULL, &writer_runtime, NULL );
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
  env->ReleaseByteArrayElements(msg,cmsg,NULL);
  env->ReleaseByteArrayElements(nick,cnick,NULL);
}

