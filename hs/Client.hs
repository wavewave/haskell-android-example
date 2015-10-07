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
import           Data.Function (on)
import           Data.List (sortBy)
import qualified Data.Text as T
import           Data.Text.Binary
import           Network.Simple.TCP
import           System.Environment

import Data.Text (Text, append, pack)
import qualified Data.Text.IO as TIO
import Foreign.C.String
import Foreign.Ptr
import GHC.Conc

import Data.Foldable
import Data.IORef

import System.Exit
import System.IO

--
import           Common

data Command = ViewAfter Int 


data JObjectObj
type JObject = Ptr JObjectObj

data JNINativeInterface_
type JNIEnv = Ptr (Ptr JNINativeInterface_)


addLog :: TVar [Message] -> [Message] -> IO ()
addLog logvar msgs = atomically $ do 
                       log <- readTVar logvar
                       writeTVar logvar (log ++ msgs) 
 

addJavaLog :: TVar [JavaMessage] -> String -> IO ()
addJavaLog msgvar str = atomically $ do
                          msgs <- readTVar msgvar
                          writeTVar msgvar (Msg str : msgs)


clientReceiver :: TVar [JavaMessage] -> TVar [Message] -> String -> IO ()
clientReceiver msgvar logvar ipaddrstr = forever $ do
{-    forever $ do
      threadDelay 10000000
      addLog logvar [Message 30 "disji" "lslssg"] -}
     
  flip catch (exceptionHandler logvar) $ do
    connect ipaddrstr "5002" $ \(sock,addr) -> do
      flip catch (\(e :: SomeException) -> addLog logvar [Message 70 "error" (T.pack (show e))]) $ do
      addJavaLog msgvar ("Connection established to " ++ show addr)
      flip evalStateT 0 $ whileJust_  (lift (recvAndUnpack sock)) $ \(xs :: [Message]) ->
        if null xs 
          then return ()
          else do n <- get
                  let n' = checkLatestMessage xs
                  liftIO $ addLog logvar xs
                  liftIO $ addJavaLog msgvar ("message from server: " ++ show (n,n'))
                  put n'        
  threadDelay 1000000
--    $ \(e :: SomeException)-> do
--        addLog logvar [Message 70 "error" (T.pack (show e))]
{-
                        
-}
exceptionHandler logvar = \(e :: SomeException) -> addLog logvar [Message 70 "error" (T.pack (show e))]


clientSender :: TVar [Message] -> TMVar (String,String) -> String -> IO ()
clientSender logvar tvar ipaddrstr = forever $ do 
  flip catch (exceptionHandler logvar) $ do 
    connect ipaddrstr "5003" $ \(sock,_addr) -> forever $ do
      (username,str) <- atomically $ takeTMVar tvar
      packAndSend sock (T.pack username, T.pack str)


foreign import ccall "wrapper" mkOnClickFPtr
  :: (JNIEnv -> JObject -> JObject -> CString -> CString -> IO ()) 
  -> IO (FunPtr (JNIEnv -> JObject -> JObject -> CString -> CString -> IO ()))

foreign import ccall "c_register_on_click_fptr"
   registerOnClickFPtr :: FunPtr (JNIEnv -> JObject -> JObject -> CString -> CString -> IO ()) -> IO ()



foreign import ccall "shout" shout :: JNIEnv -> CString -> IO ()

foreign import ccall "c_textView_setText" textViewSetText :: JNIEnv -> JObject -> CString -> IO ()

foreign import ccall "c_textView_append" textViewAppend :: JNIEnv -> JObject -> CString -> IO ()

onCreate :: JNIEnv -> JObject -> JObject -> IO ()
onCreate env activity tv =  do
    getNumProcessors >>= setNumCapabilities
    caps <- getNumCapabilities
    -- let txt  = "MESSAGE FROM HASKELL:\n\nRunning on " ++ show caps ++ " CPUs!"
    -- cstr <- newCString txt
    iref <- newIORef 0

    logvar <- atomically $ newTVar []
    sndvar <- atomically $ newEmptyTMVar
    msgvar <- atomically $ newTVar []
    forkIO $ clientReceiver msgvar logvar "ianwookim.org" 
    forkIO $ clientSender logvar sndvar "ianwookim.org"
    mkOnClickFPtr (onClick (iref,sndvar,logvar)) >>= registerOnClickFPtr
    mkOnIdleFPtr (onIdle (msgvar,logvar)) >>= registerOnIdleFPtr
    return ()



foreign export ccall
  "Java_com_uphere_chatter_Chatter_onCreateHS"
  onCreate :: JNIEnv -> JObject -> JObject -> IO ()

data JavaMessage = Msg String

onClick :: (IORef Int, TMVar (String,String), TVar [Message]) -> JNIEnv -> JObject -> JObject -> CString -> CString -> IO ()
onClick (ref,tvar,logvar) env activity tv cnick cmsg = do
  n <- readIORef ref
  writeIORef ref (n+1)
  nick <- peekCString cnick
  msg <- peekCString cmsg
  forkIO $ atomically $ putTMVar tvar (nick,msg) 
  return ()
  

onIdle :: (TVar [JavaMessage], TVar [Message]) -> JNIEnv -> JObject -> JObject -> IO ()
onIdle (msgvar,logvar) env _a tv = do
  -- printMsg env (Msg "onIdle called")
  msgs <- atomically $ do
    msgs <- readTVar msgvar
    writeTVar msgvar []
    return msgs
  mapM_ (printMsg env) . reverse $ msgs


  msgs' <- atomically $ do
    msgs' <- readTVar logvar
    writeTVar logvar []
    return msgs'
  mapM_ (textViewMsg env tv) . reverse $ msgs'


textViewMsg :: JNIEnv -> JObject -> Message -> IO ()
textViewMsg env tv msg = do
  cstr <- newCString (prettyPrintMessage msg ++ "\n") 
  textViewAppend env tv cstr

printMsg :: JNIEnv -> JavaMessage -> IO ()
printMsg env (Msg msg) = newCString msg >>= shout env

foreign import ccall "wrapper" mkOnIdleFPtr
  :: (JNIEnv -> JObject -> JObject -> IO ())
     -> IO (FunPtr (JNIEnv -> JObject -> JObject -> IO ()))

foreign import ccall "c_register_on_idle_fptr"
  registerOnIdleFPtr :: FunPtr (JNIEnv -> JObject -> JObject -> IO ()) -> IO ()

