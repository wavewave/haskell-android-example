{-# LANGUAGE EmptyDataDecls #-}
{-# LANGUAGE ForeignFunctionInterface #-}
{-# LANGUAGE OverloadedStrings #-}

module HaskellActivity where

import Control.Monad.IO.Class
import Data.Text (Text, append, pack)
import qualified Data.Text.IO as TIO
import Foreign.C.String
-- import Foreign.JNI
-- import Foreign.JNI.Lookup
import Foreign.Ptr
import GHC.Conc

import System.IO
import Control.Concurrent
import Control.Monad
 
{- 
textView_class :: JClass
textView_class = jclass "android/widget/TextView"

textView_new :: JObject -> JNI JObject
textView_new ctx = do
  newObject textView_class (jmethodid textView_class "<init>" "(Landroid/content/Context;)V") [jv ctx]

textView_setText :: JObject -> Text -> JNI ()
textView_setText tv txt = do
  jtxt <- newString txt
  callVoidMethod tv (jmethodid textView_class "setText" "(Ljava/lang/CharSequence;)V") [jv jtxt]
-}
data JObjectObj
type JObject = Ptr JObjectObj

data JNINativeInterface_
type JNIEnv = Ptr (Ptr JNINativeInterface_)
  
onCreate :: JNIEnv -> JObject -> JObject -> IO ()
onCreate env activity tv =  do
    getNumProcessors >>= setNumCapabilities
    caps <- getNumCapabilities
    let txt  = "MESSAGE FROM HASKELL:\n\nRunning on " ++ show caps ++ " CPUs!"
    cstr <- newCString txt
    shout env cstr
    textViewSetText env tv cstr



foreign export ccall
  "Java_com_example_hellojni_HelloJni_onCreateHS"
  onCreate :: JNIEnv -> JObject -> JObject -> IO ()

foreign import ccall "shout" shout :: JNIEnv -> CString -> IO ()

foreign import ccall "c_textView_setText" textViewSetText :: JNIEnv -> JObject -> CString -> IO ()



{-                              
foreign export ccall
  "Java_com_example_hellojni_HelloJni_stringFromJNI"
  stringFromJNI :: JNIEnv -> JObject -> IO JString


stringFromJNI :: JNIEnv -> JObject -> IO JString
stringFromJNI env _ = runJNI env $ do
                        jstr <- newString "MESSAGE FROM HASKELL: HELLO WORLD!"
                        return jstr


getNumProcessors >>= setNumCapabilities
    caps <- getNumCapabilities
    return $ "Hello World!\nRunning on " `append` pack (show caps) `append` " CPUs!"
-}
