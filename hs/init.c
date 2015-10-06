#include <stdio.h>
#include <jni.h>
#include <HsFFI.h>

extern void __stginit_Client(void);

void (*fptr_onclick)(JNIEnv*,jobject,jobject,jstring) ;

void c_register_on_click_fptr( void (*v)(JNIEnv*,jobject,jobject) ) {
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
      jstring tmsg = (*env)->NewStringUTF(env,"HELLOJNI");
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


/*
jstring
Java_com_example_hellojni_HelloJni_stringFromJNI(JNIEnv *env, jobject this)
{
  int a = 300; // secretfunction();
  
  char* str;

  if( a > 300) { str = "hello haskell"; }
  else { str = "there haskell"; } 


  shout(env, "MESSAGE TO LOG : HELLLOOOO THEREEEE ");
  return (*env)->NewStringUTF(env, str); 
}
*/

void
Java_co_uphere_chatter_Chatter_onClickHS( JNIEnv* env, jobject this, jobject that, jstring str)
{
  const char* nativeString = (*env)->GetStringUTFChars(env, str, 0);
  fptr_onclick (env, this, that,nativeString);
  (*env)->ReleaseStringUTFChars(env,str,nativeString);
  
}

void
Java_co_uphere_chatter_Chatter_onIdleHS( JNIEnv* env, jobject activity, jobject textview)
{
  fptr_onidle (env, activity, textview);
}
