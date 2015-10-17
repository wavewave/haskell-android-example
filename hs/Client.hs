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
import           Data.Monoid
-- import           Data.Text (Text, append, pack)
import qualified Data.Text as T
import           Data.Text.Binary
import qualified Data.Text.Foreign as TF
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
  :: (CString -> CInt -> CString -> CInt -> IO ())
  -> IO (FunPtr (CString -> CInt -> CString -> CInt -> IO ()))

foreign import ccall safe "register_callback_fptr"
  registerCallbackFPtr :: FunPtr (CString -> CInt -> CString -> CInt -> IO ()) -> IO ()

foreign import ccall safe "__android_log_write"
  androidLogWrite :: CInt -> CString -> CString -> IO CInt

foreign import ccall safe "write_message"
  c_write_message :: CString -> CInt -> IO ()

foreign export ccall "chatter" chatter :: IO ()

{-
cStrLen :: CStringLen -> CInt
cStrLen = mkCInt . snd

cStr :: CStringLen -> CString
cStr = fst
-}

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

clientSender :: TVar [Message] -> TMVar (T.Text,T.Text) -> String -> IO ()
clientSender logvar tvar ipaddrstr = forever $ do 
  flip catch exceptionHandler $ do 
    connect ipaddrstr "5003" $ \(sock,_addr) -> forever $ do
      (username,msg) <- atomically $ takeTMVar tvar
      packAndSend sock (username, msg)

chatter :: IO ()
chatter = do
  logvar <- atomically $ newTVar []
  sndvar <- atomically $ newEmptyTMVar
  mkCallbackFPtr (onClick (sndvar,logvar)) >>= registerCallbackFPtr 
  
  forkIO $ clientSender logvar sndvar "ianwookim.org"
  forkIO $ clientReceiver logvar "ianwookim.org" 
  messageViewer logvar

onClick :: (TMVar (T.Text,T.Text), TVar [Message]) -> CString -> CInt -> CString -> CInt
        -> IO ()
onClick (sndvar,logvar) cnick m cmsg n = do
  nick <- TF.peekCStringLen (cnick,fromIntegral m)
  msg <- TF.peekCStringLen (cmsg,fromIntegral n)
  atomically $ putTMVar sndvar (nick,msg)

                                
messageViewer :: TVar [Message] -> IO ()
messageViewer logvar = forever $ do 
  msgs <- atomically $ do
    msgs <- readTVar logvar
    writeTVar logvar []
    return msgs
  let formatted = (map format . reverse) msgs
  mapM_ (\x -> printLog x >> printMsg x) formatted
  -- mapM_ (printMsg . format) . reverse $ msgs
 where format x = T.pack (show (messageNum x)) <> " : " <>
                  messageUser x <> " : " <>
                  messageBody x <> "\n"


printMsg :: T.Text -> IO ()
printMsg msg =
  TF.withCStringLen msg $
    \(cmsg,n) -> c_write_message cmsg (fromIntegral n)

printLog :: T.Text -> IO ()
printLog msg = TF.withCStringLen "UPHERE" $ \(ctag,_) ->
                 TF.withCStringLen msg $ \(cmsg,_) ->
                   androidLogWrite 3 ctag cmsg >> return ()
