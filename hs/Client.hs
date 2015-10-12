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
  :: (JNIEnv -> JObject -> CString -> CString -> IO ())
  -> IO (FunPtr (JNIEnv -> JObject -> CString -> CString -> IO ()))

foreign import ccall safe "register_callback_fptr"
  registerCallbackFPtr :: FunPtr (JNIEnv -> JObject -> CString -> CString -> IO ())
                       -> IO ()

foreign import ccall safe "__android_log_write"
  androidLogWrite :: CInt -> CString -> CString -> IO CInt

foreign import ccall "Chatter_sendMsgToChatter"
  c_Chatter_sendMsgToChatter :: JNIEnv -> JObject -> CString -> CString -> IO ()

foreign export ccall "chatter" chatter :: IO ()

exceptionHandler = \(e :: SomeException) -> do
  withCString "UPHERE" $ \ctag -> 
    withCString (show e) $ \cstr -> 
      androidLogWrite 4 ctag cstr
  return ()


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
  logvar <- atomically $ newTVar []
  sndvar <- atomically $ newEmptyTMVar
  mkCallbackFPtr (onClick (sndvar,logvar)) >>= registerCallbackFPtr 
  
  forkIO $ clientSender logvar sndvar "ianwookim.org"
  forkIO $ clientReceiver logvar "ianwookim.org" 

  messageViewer logvar


onClick :: (TMVar (String,String), TVar [Message]) -> JNIEnv -> JObject
        -> CString -> CString -> IO ()
onClick (sndvar,logvar) env activity cnick cmsg = do
  nick <- peekCString cnick
  msg <- peekCString cmsg
  atomically $ putTMVar sndvar (nick,msg)

                                
messageViewer :: TVar [Message] -> IO ()
messageViewer logvar = forever $ do 
  msgs <- atomically $ do
    msgs <- readTVar logvar
    writeTVar logvar []
    return msgs
  mapM_ (printMsg . format) . reverse $ msgs
 where format x = show (messageNum x) ++ " : " ++
                  T.unpack (messageUser x) ++ " : " ++
                  T.unpack (messageBody x)


printMsg :: String -> IO ()
printMsg msg = do
    withCString "UPHERE" $ \ctag -> 
      withCString msg $ \cmsg ->
        androidLogWrite 3 ctag cmsg >> return ()

