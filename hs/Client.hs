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
import           Foreign.Marshal.Alloc (mallocBytes)
import           Foreign.Marshal.Utils (copyBytes)
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

foreign import ccall safe "wrapper" mkChoreoFPtr
  :: (CULLong -> IO ()) -> IO (FunPtr (CULLong -> IO ()))


foreign import ccall safe "register_callback_fptr"
  registerCallbackFPtr :: FunPtr (CString -> CInt -> CString -> CInt -> IO ()) -> IO ()

foreign import ccall safe "register_choreo_fptr"
  registerChoreoFPtr :: FunPtr (CULLong -> IO ()) -> IO ()

foreign import ccall safe "__android_log_write"
  androidLogWrite :: CInt -> CString -> CString -> IO CInt

foreign import ccall safe "write_message"
  c_write_message :: CString -> CInt -> IO ()

foreign import ccall safe "write_coord"
  c_write_coord :: CInt -> CInt -> IO ()


foreign export ccall "hs_android_main" hs_android_main :: IO ()


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
    \(cmsg,n) -> do
      dest <- mallocBytes n
      copyBytes dest cmsg n
      c_write_message dest (fromIntegral n)

printLog :: T.Text -> IO ()
printLog msg = TF.withCStringLen "UPHERE" $ \(ctag,n1) ->
                 TF.withCStringLen msg $ \(cmsg,n2) -> do
                   d_ctag <- mallocBytes n1
                   copyBytes d_ctag ctag n1
                   d_cmsg <- mallocBytes  n2
                   copyBytes d_cmsg cmsg n2
                   androidLogWrite 3 d_ctag d_cmsg >> return ()


hs_android_main :: IO ()
hs_android_main = do
  logvar <- atomically $ newTVar []
  sndvar <- atomically $ newEmptyTMVar
  timevar <- atomically $ newTVar Nothing
  mkCallbackFPtr (onClick (sndvar,logvar)) >>= registerCallbackFPtr
  mkChoreoFPtr (choreo' timevar) >>= registerChoreoFPtr

  printLog "CHATTER2"
  
  forkIO $ clientSender logvar sndvar "ianwookim.org"
  forkIO $ clientReceiver logvar "ianwookim.org"
  messageViewer logvar


choreo' :: TVar (Maybe CULLong) -> CULLong -> IO ()
choreo' ref t = do
  mu <- atomically $ readTVar ref
  case mu of
    Nothing -> atomically $ writeTVar ref (Just t)
    Just u -> do
      let n = (t-u) `div` 16666667
      c_write_coord (fromIntegral n) (fromIntegral n)
  
