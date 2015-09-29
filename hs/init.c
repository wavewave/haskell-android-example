#include <stdio.h>
#include <jni.h>
#include <HsFFI.h>

//extern void hs_set_java_vm(HsPtr a1);
extern void __stginit_Client(void);

void (*fptr_onclick)(JNIEnv*,jobject,jobject) ;

void c_register_on_click_fptr( void (*v)(JNIEnv*,jobject,jobject) ) {

  fptr_onclick = v;
  
}



// static int Log.D(String tag, String msg)
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


// static int Log.D(String tag, String msg)
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




JNIEXPORT jint JNICALL JNI_OnLoad( JavaVM *vm, void *pvt ) {
    static char *argv[] = { "libhaskell.so", 0 }, **argv_ = argv;
    static int argc = 1;

    hs_init(&argc, &argv_);
    //hs_init( 0, 0);
    hs_add_root(__stginit_Client);
    //hs_set_java_vm(vm);
    return JNI_VERSION_1_2;
}

JNIEXPORT void JNICALL JNI_OnUnload( JavaVM *vm, void *pvt ) {
    hs_exit();
}



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

void
Java_com_example_hellojni_Sub_onClickHS( JNIEnv* env, jobject this, jobject that)
{
  //int c = fptr();
  //  char str[15];
  //  sprintf(str, "%d", c);
   
  //  shout(env, str) ; //"NEWMESSAGE :HERE ");
  fptr_onclick (env, this, that);
}
