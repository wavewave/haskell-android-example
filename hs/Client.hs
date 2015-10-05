{-# LANGuAGE EmptyDataDecls #-}
{-# LANGUAGE ForeignFunctionInterface #-}
{-# LANGUAGE OverloadedStrings #-} 
{-# LANGUAGE ScopedTypeVariables #-}

module Client where

import           Control.Concurrent
import           Control.Concurrent.STM
import           Control.Concurrent.STM.TMVar
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

import System.IO

--
import           Common

data Command = ViewAfter Int 


data JObjectObj
type JObject = Ptr JObjectObj

data JNINativeInterface_
type JNIEnv = Ptr (Ptr JNINativeInterface_)


commander :: TVar [Message] -> Command -> IO ()
commander logvar (ViewAfter n) = do
    xs <- atomically (readTVar logvar) 
    (mapM_ (putStrLn . prettyPrintMessage) 
     . sortBy (compare `on` messageNum)
     . filter ( (> n) . messageNum ) 
     ) xs

addLog :: TVar [Message] -> [Message] -> IO ()
addLog logvar msgs = atomically $ do 
                       log <- readTVar logvar
                       writeTVar logvar (log ++ msgs) 
 

clientReceiver :: TVar [Message] -> String -> IO ()
clientReceiver logvar ipaddrstr = 
  connect ipaddrstr "5002" $ \(sock,addr) -> do 
    putStrLn $ "Connection established to " ++ show addr
    flip evalStateT 0 $ whileJust_  (lift (recvAndUnpack sock)) $ \xs -> 
      if null xs 
        then return ()
        else do n <- get
                let n' = checkLatestMessage xs
                liftIO $ addLog logvar xs
                liftIO $ commander logvar (ViewAfter n)
                put n'
                              

clientSender :: TMVar String -> String -> String -> IO ()
clientSender tvar ipaddrstr username = 
  connect ipaddrstr "5003" $ \(sock,_addr) -> forever $ do
    str <- atomically $ takeTMVar tvar
    packAndSend sock (T.pack username, T.pack str)


foreign import ccall "wrapper" mkOnClickFPtr
  :: (JNIEnv -> JObject -> JObject -> IO ()) 
  -> IO (FunPtr (JNIEnv -> JObject -> JObject -> IO ()))

foreign import ccall "c_register_on_click_fptr"
   registerOnClickFPtr :: FunPtr (JNIEnv -> JObject -> JObject -> IO ()) -> IO ()



foreign import ccall "shout" shout :: JNIEnv -> CString -> IO ()

foreign import ccall "c_textView_setText" textViewSetText :: JNIEnv -> JObject -> CString -> IO ()

foreign import ccall "c_textView_append" textViewAppend :: JNIEnv -> JObject -> CString -> IO ()

onCreate :: JNIEnv -> JObject -> JObject -> IO ()
onCreate env activity tv =  do
    getNumProcessors >>= setNumCapabilities
    caps <- getNumCapabilities
    putStrLn "hello there"
    let txt  = "MESSAGE FROM HASKELL:\n\nRunning on " ++ show caps ++ " CPUs!"
    cstr <- newCString txt
    iref <- newIORef 0
    


    logvar <- atomically $ newTVar []
    sndvar <- atomically $ newEmptyTMVar
    msgvar <- atomically $ newTVar []
    forkIO $ clientReceiver logvar "ianwookim.org" 
    forkIO $ clientSender sndvar "ianwookim.org" "wavewave"
    mkOnClickFPtr (onClick (iref,sndvar,msgvar)) >>= registerOnClickFPtr

    mkOnIdleFPtr (onIdle msgvar) >>= registerOnIdleFPtr


    return ()



foreign export ccall
  "Java_com_example_hellojni_HelloJni_onCreateHS"
  onCreate :: JNIEnv -> JObject -> JObject -> IO ()

data JavaMessage = Msg String

onClick :: (IORef Int, TMVar String, TVar [JavaMessage]) -> JNIEnv -> JObject -> JObject -> IO ()
onClick (ref,tvar,msgvar) env activity tv = do
  n <- readIORef ref
  writeIORef ref (n+1)
  cstr <- newCString (show n) 
  shout env cstr
  -- textViewSetText env tv cstr

  forkIO $ do 
    atomically $ putTMVar tvar ("Hi There " ++ show n)

    atomically $ do
      msgs <- readTVar msgvar
      writeTVar msgvar (Msg ("message recorded: " ++ show n) : msgs)

    -- (shout env cstr)
    -- forkIO $ do
  return ()
  

onIdle :: TVar [JavaMessage] -> JNIEnv -> JObject -> JObject -> IO ()
onIdle msgvar env _a tv = do
  msgs <- atomically $ do
    msgs <- readTVar msgvar
    writeTVar msgvar []
    return msgs
  mapM_ (printMsg env tv) msgs
  
  -- cstr <- newCString "HAHAHA"
  -- shout env cstr


printMsg :: JNIEnv -> JObject -> JavaMessage -> IO ()
printMsg env tv (Msg msg) = do
  cstr <- newCString msg
  -- shout env cstr
  textViewAppend env tv cstr

foreign import ccall "wrapper" mkOnIdleFPtr
  :: (JNIEnv -> JObject -> JObject -> IO ())
     -> IO (FunPtr (JNIEnv -> JObject -> JObject -> IO ()))

foreign import ccall "c_register_on_idle_fptr"
  registerOnIdleFPtr :: FunPtr (JNIEnv -> JObject -> JObject -> IO ()) -> IO ()

--foreign export ccall
--  "Java_com_example_hellojni_Sub_onIdleHS"
--  onIdle :: JNIEnv -> JObject -> IO ()
