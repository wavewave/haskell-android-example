#include <string.h>
#include <stdlib.h>
#include <jni.h>

// int secretfunction( void );
//void test( void ) {
//  return 0;
//}

 
jstring
Java_com_example_hellojni_HelloJni_stringFromJNI(JNIEnv *env, jobject this)
{
  int a = 300; // secretfunction();
  
  char* str;

  if( a > 300) { str = "hello"; }
  else { str = "there"; } 
  
  return (*env)->NewStringUTF(env, str); 
}

