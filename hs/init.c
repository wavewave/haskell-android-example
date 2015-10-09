#include <stdio.h>
#include <jni.h>
#include <HsFFI.h>

extern void __stginit_Client(void);

void (*fptr_onclick)(JNIEnv*,jobject,jobject,const char*,const char*) ;

void c_register_on_click_fptr( void (*v)(JNIEnv*,jobject,jobject,const char*,const char *) ) {
  fptr_onclick = v;
}

void (*fptr_onidle)(JNIEnv*, jobject, jobject);

void c_register_on_idle_fptr( void (*v)(JNIEnv*,jobject, jobject) ) {
  fptr_onidle = v;
}



void shout ( JNIEnv* env,  char* cmsg ) { 
  jclass cls = (*env)->FindClass(env, "android/util/Log");
  if( cls ) {
    jmethodID mid = (*env)->GetStaticMethodID(env, cls, "d", "(Ljava/lang/String;Ljava/lang/String;)I");
    if( mid ) {
      jstring tmsg = (*env)->NewStringUTF(env,"UPHERE");
      jstring jmsg = (*env)->NewStringUTF(env,cmsg);
      if( jmsg ) { 
        (*env)->CallStaticIntMethod(env,cls,mid,tmsg,jmsg);
      }
    }
  }
}


void c_textView_setText ( JNIEnv* env,  jobject tv, char* cmsg ) { 
  jclass cls = (*env)->FindClass(env, "android/widget/TextView");
  if( cls ) {
    jmethodID mid = (*env)->GetMethodID(env, cls, "setText", "(Ljava/lang/CharSequence;)V");
    if( mid ) {
      jstring jmsg = (*env)->NewStringUTF(env,cmsg);
      if( jmsg ) { 
        (*env)->CallVoidMethod(env,tv,mid,jmsg);
      }
    }
  }
}

void c_textView_append ( JNIEnv* env,  jobject tv, char* cmsg ) { 
  jclass cls = (*env)->FindClass(env, "android/widget/TextView");
  if( cls ) {
    jmethodID mid = (*env)->GetMethodID(env, cls, "append", "(Ljava/lang/CharSequence;)V");
    if( mid ) {
      jstring jmsg = (*env)->NewStringUTF(env,cmsg);
      if( jmsg ) { 
        (*env)->CallVoidMethod(env,tv,mid,jmsg);
      }
    }
  }
}


JNIEXPORT jint JNICALL JNI_OnLoad( JavaVM *vm, void *pvt ) {
    static char *argv[] = { "libhaskell.so", 0 }, **argv_ = argv;
    static int argc = 1;

    hs_init(&argc, &argv_);
    hs_add_root(__stginit_Client);
    return JNI_VERSION_1_2;
}

JNIEXPORT void JNICALL JNI_OnUnload( JavaVM *vm, void *pvt ) {
    hs_exit();
}

void
Java_com_uphere_chatter_Chatter_onClickHS( JNIEnv* env, jobject this, jobject that,
					   jstring nick, jstring msg)
{
  const char* cnick = (*env)->GetStringUTFChars(env, nick, 0);
  const char* cmsg = (*env)->GetStringUTFChars(env, msg, 0);
  fptr_onclick (env, this, that, cnick, cmsg);
  (*env)->ReleaseStringUTFChars(env,nick,cnick);
  (*env)->ReleaseStringUTFChars(env,msg,cmsg);
}

void
Java_com_uphere_chatter_Chatter_onIdleHS( JNIEnv* env, jobject activity, jobject textview)
{
  fptr_onidle (env, activity, textview);
}
