{-# LANGuAGE EmptyDataDecls #-}
{-# LANGUAGE ForeignFunctionInterface #-}
{-# LANGUAGE OverloadedStrings #-} 
{-# LANGUAGE ScopedTypeVariables #-}

module Client where

import           Control.Concurrent
-- import           Control.Concurrent.STM
-- import           Control.Concurrent.STM.TMVar
import           Control.Exception
import           Control.Monad (forever,guard)
-- import           Control.Monad.IO.Class
-- import           Control.Monad.Loops 
-- import           Control.Monad.Trans.Class
-- import           Control.Monad.Trans.State
import           Data.Foldable
import           Data.Function (on)
import           Data.IORef
import           Data.List (sortBy)
-- import           Data.Text (Text, append, pack)
-- import qualified Data.Text as T
-- import           Data.Text.Binary
-- import qualified Data.Text.IO as TIO
import           Foreign.C.String
import           Foreign.C.Types
import           Foreign.Ptr
import           GHC.Conc
-- import           Network.Simple.TCP
import           System.Environment
import           System.Exit
import           System.Mem
import           System.IO
--
-- import           Common

data Command = ViewAfter Int 


data JObjectObj
type JObject = Ptr JObjectObj

data JNINativeInterface_
type JNIEnv = Ptr (Ptr JNINativeInterface_)

exceptionHandler = \(e :: SomeException) -> do
  withCString "UPHERE" $ \ctag -> 
    withCString (show e) $ \cstr -> 
      androidLogWrite 4 ctag cstr
  return ()



test = do
  iref <- newIORef (0 :: Int)
  withCString "UPHERE" $ \ctag -> do
    forkIO $ forever $ do
      n <- readIORef iref
      withCString ("sub thread " ++ show n) $ \cstr ->
        androidLogWrite 3 ctag cstr
      writeIORef iref (n+2)
      threadDelay 100000
      performGC

    forever $ do
      n <- readIORef iref
      withCString ("main thread " ++ show n) $ \cstr ->
        androidLogWrite 3 ctag cstr
      writeIORef iref (n+1)
      threadDelay 100000
      performGC
    

foreign export ccall "test1" test1 :: IO ()

test1 = do
  ref <- newIORef (0 :: Int)
  mkCallbackFPtr (callback1 ref) >>= registerCallbackFPtr 
  flip catch exceptionHandler test >> return ()

-- foreign export ccall "callback1" callback1 :: JNIEnv -> JObject -> IO ()

foreign import ccall safe "wrapper" mkCallbackFPtr
  :: (JNIEnv -> JObject -> IO ()) -> IO (FunPtr (JNIEnv -> JObject -> IO ()))

foreign import ccall safe "register_callback_fptr"
  registerCallbackFPtr :: FunPtr (JNIEnv -> JObject -> IO ()) -> IO ()


callback1 ref env activity = do
  n <- readIORef ref
  writeIORef ref (n+1)
  let msg = "HASKELL I AM ALIVE : " ++ show n ++ "\n"
  {- 
  withCString "UPHERE" $ \ctag -> 
    withCString msg $ \cmsg ->
      androidLogWrite 3 ctag cmsg >> return ()
  -}
  withCString msg $ \cmsg ->
    c_Chatter_sendMsgToChatter env activity cmsg
  
foreign import ccall safe "__android_log_write"
  androidLogWrite :: CInt -> CString -> CString -> IO CInt

foreign import ccall "Chatter_sendMsgToChatter"
  c_Chatter_sendMsgToChatter :: JNIEnv -> JObject -> CString -> IO ()
                                

