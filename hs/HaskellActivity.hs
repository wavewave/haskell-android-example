{-# LANGUAGE ForeignFunctionInterface,OverloadedStrings #-}
module HaskellActivity where

import Foreign.JNI
import Foreign.JNI.Lookup
import Control.Monad.IO.Class
import Data.Text (Text, append, pack)
import qualified Data.Text.IO as TIO
import GHC.Conc

import System.IO
{- 
-}

 

textView_class :: JClass
textView_class = jclass "android/widget/TextView"

textView_new :: JObject -> JNI JObject
textView_new ctx = do
  newObject textView_class (jmethodid textView_class "<init>" "(Landroid/content/Context;)V") [jv ctx]

textView_setText :: JObject -> Text -> JNI ()
textView_setText tv txt = do
  jtxt <- newString txt
  callMethod tv (jmethodid textView_class "setText" "(Ljava/lang/CharSequence;)V") [jv jtxt]

onCreate :: JNIEnv -> JObject -> JObject -> IO ()
onCreate env activity tv {- _bundle -} = runJNISafe () env $ do
  msg <- liftIO $ do
    getNumProcessors >>= setNumCapabilities
    caps <- getNumCapabilities
    let txt  = "Hello World!\nRunning on " `append` pack (show caps) `append` " CPUs!"
    -- TIO.writeFile "/data/local/tmp/mytmp.txt" txt
    putStrLn " HIHIHIHIH" 
    return txt
   
  -- activityClass <- getObjectClass activity

  {- tv <- textView_new activity -}
  textView_setText tv msg

  {- 
  activity_setContentView <- getMethodID activityClass "setContentView" "(Landroid/view/View;)V"
  callMethod activity activity_setContentView [JVObject tv] :: JNI () -}
  return ()

foreign export ccall
  "Java_com_example_hellojni_HelloJni_onCreateHS"
  onCreate :: JNIEnv -> JObject -> JObject -> IO ()


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
