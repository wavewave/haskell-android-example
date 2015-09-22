#include <jni.h>
#include <HsFFI.h>

extern void hs_set_java_vm(HsPtr a1);
extern void __stginit_HaskellActivity(void);


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


JNIEXPORT jint JNICALL JNI_OnLoad( JavaVM *vm, void *pvt ) {
    static char *argv[] = { "libhaskell.so", 0 }, **argv_ = argv;
    static int argc = 1;

    hs_init(&argc, &argv_);
    //hs_init( 0, 0);
    hs_add_root(__stginit_HaskellActivity);
    hs_set_java_vm(vm);
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

