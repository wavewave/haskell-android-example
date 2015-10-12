{-# LANGuAGE EmptyDataDecls #-}
{-# LANGUAGE ForeignFunctionInterface #-}
{-# LANGUAGE OverloadedStrings #-} 
{-# LANGUAGE ScopedTypeVariables #-}

module Client where

import           Control.Concurrent
import           Control.Concurrent.STM
import           Control.Concurrent.STM.TMVar
import           Control.Exception
import           Control.Monad (forever,guard)
import           Control.Monad.IO.Class
import           Control.Monad.Loops 
import           Control.Monad.Trans.Class
import           Control.Monad.Trans.State
import           Data.Foldable
import           Data.Function (on)
import           Data.IORef
import           Data.List (sortBy)
import           Data.Text (Text, append, pack)
import qualified Data.Text as T
import           Data.Text.Binary
import qualified Data.Text.IO as TIO
import           Foreign.C.String
import           Foreign.C.Types
import           Foreign.Ptr
import           GHC.Conc
import           Network.Simple.TCP
import           System.Environment
import           System.Exit
import           System.Mem
import           System.IO
--
import           Common

data Command = ViewAfter Int 

data JObjectObj
type JObject = Ptr JObjectObj

data JNINativeInterface_
type JNIEnv = Ptr (Ptr JNINativeInterface_)

foreign import ccall safe "wrapper" mkCallbackFPtr
  :: (JNIEnv -> JObject -> IO ()) -> IO (FunPtr (JNIEnv -> JObject -> IO ()))

foreign import ccall safe "register_callback_fptr"
  registerCallbackFPtr :: FunPtr (JNIEnv -> JObject -> IO ()) -> IO ()

foreign import ccall safe "__android_log_write"
  androidLogWrite :: CInt -> CString -> CString -> IO CInt

foreign import ccall "Chatter_sendMsgToChatter"
  c_Chatter_sendMsgToChatter :: JNIEnv -> JObject -> CString -> IO ()

foreign export ccall "chatter" chatter :: IO ()

exceptionHandler = \(e :: SomeException) -> do
  withCString "UPHERE" $ \ctag -> 
    withCString (show e) $ \cstr -> 
      androidLogWrite 4 ctag cstr
  return ()

{-
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

-- foreign export ccall "test1" test1 :: IO ()
-}

addLog :: TVar [Message] -> [Message] -> IO ()
addLog logvar msgs = atomically $ do 
                       log <- readTVar logvar
                       writeTVar logvar (log ++ msgs) 
 

clientReceiver :: TVar [Message] -> String -> IO ()
clientReceiver logvar ipaddrstr = forever $ do
  flip catch exceptionHandler $ do
    connect ipaddrstr "5002" $ \(sock,addr) -> do
      flip catch (\(e :: SomeException) -> addLog logvar [Message 70 "error" (T.pack (show e))]) $ do
      flip evalStateT 0 $ whileJust_  (lift (recvAndUnpack sock)) $ \(xs :: [Message]) ->
        if null xs 
          then return ()
          else do n <- get
                  let n' = checkLatestMessage xs
                  liftIO $ addLog logvar xs
                  put n'        
  threadDelay 1000000

clientSender :: TVar [Message] -> TMVar (String,String) -> String -> IO ()
clientSender logvar tvar ipaddrstr = forever $ do 
  flip catch exceptionHandler $ do 
    connect ipaddrstr "5003" $ \(sock,_addr) -> forever $ do
      (username,str) <- atomically $ takeTMVar tvar
      packAndSend sock (T.pack username, T.pack str)

chatter :: IO ()
chatter = do
  -- ref <- newIORef (0 :: Int)
  logvar <- atomically $ newTVar []
  sndvar <- atomically $ newEmptyTMVar
  mkCallbackFPtr (onClick (sndvar,logvar)) >>= registerCallbackFPtr 
  
  forkIO $ clientSender   logvar sndvar "ianwookim.org"

  clientReceiver logvar        "ianwookim.org" 

  -- flip catch exceptionHandler test >> return ()


onClick :: (TMVar (String,String), TVar [Message]) -> JNIEnv -> JObject -> IO ()
onClick (sndvar,logvar) env activity = do
  atomically $ putTMVar sndvar ("UPHERE","HELLO")
  withCString "hello there\n" $ \cmsg ->
    c_Chatter_sendMsgToChatter env activity cmsg

                                

