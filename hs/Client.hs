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


{-
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
  flip catch exceptionHandler $ do
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


  -- addLog logvar [Message 70 "error" (T.pack (show e))]


clientSender :: TVar [Message] -> TMVar (String,String) -> String -> IO ()
clientSender logvar tvar ipaddrstr = forever $ do 
  flip catch exceptionHandler $ do 
    connect ipaddrstr "5003" $ \(sock,_addr) -> forever $ do
      (username,str) <- atomically $ takeTMVar tvar
      packAndSend sock (T.pack username, T.pack str)
-}

foreign import ccall safe "wrapper" mkOnClickFPtr
  :: (JNIEnv -> JObject -> JObject -> CString -> CString -> IO ()) 
  -> IO (FunPtr (JNIEnv -> JObject -> JObject -> CString -> CString -> IO ()))

foreign import ccall safe "c_register_on_click_fptr"
   registerOnClickFPtr :: FunPtr (JNIEnv -> JObject -> JObject -> CString -> CString -> IO ()) -> IO ()



-- foreign import ccall "shout" shout :: JNIEnv -> CString -> IO ()

foreign import ccall safe "c_textView_setText" textViewSetText :: JNIEnv -> JObject -> CString -> IO ()

foreign import ccall safe "c_textView_append" textViewAppend :: JNIEnv -> JObject -> CString -> IO ()

test = do
  putStrLn "test"

  iref <- newIORef (0 :: Int)

  -- let n = 10
  
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
    


onCreate :: JNIEnv -> JObject -> JObject -> IO ()
onCreate env activity tv =  do
    getNumProcessors >>= setNumCapabilities
    caps <- getNumCapabilities
{-
    logvar <- atomically $ newTVar []
    sndvar <- atomically $ newEmptyTMVar
    msgvar <- atomically $ newTVar []

    -- forkOS $
    --   flip catch exceptionHandler $ test
    -- forkIO $ clientReceiver msgvar logvar "ianwookim.org" 
    -- forkIO $ clientSender logvar sndvar "ianwookim.org"
-}

    -- forkIO $
    --   flip catch exceptionHandler $ test

    mkOnClickFPtr (onClick {- (sndvar,logvar) -}) >>= registerOnClickFPtr 
    mkOnIdleFPtr (onIdle {- (msgvar,logvar) -}) >>= registerOnIdleFPtr 
    return ()



foreign export ccall
  "Java_com_uphere_chatter_Chatter_onCreateHS"
  onCreate :: JNIEnv -> JObject -> JObject -> IO ()

foreign export ccall
  "test1"
  test1 :: IO ()

test1 = flip catch exceptionHandler test >> return ()


data JavaMessage = Msg String

onClick :: {- (TMVar (String,String), TVar [Message]) -> -} JNIEnv -> JObject -> JObject -> CString -> CString -> IO ()
onClick {- (tvar,logvar) -} env activity tv cnick cmsg = do
  -- withCString "UPHERE" $ \ctag -> 
  --   androidLogWrite 3 ctag cmsg  
  -- forkOS $
{-  forkIO $ do
    nick <- peekCString cnick
    msg <- peekCString cmsg
    atomically $ putTMVar tvar (nick,msg)
  -- print "hello"
  performGC -}
  return ()
  

onIdle :: {- (TVar [JavaMessage], TVar [Message]) -> -} JNIEnv -> JObject -> JObject -> IO ()
onIdle {- (msgvar,logvar) -} env _a tv = do
  
  withCString "UPHERE" $ \ctag ->
    withCString "OnIdle called" $ \cstr -> 
      androidLogWrite 3 ctag cstr >> return ()

  performGC 
  -- putStrLn "onIdle"
{-  forkIO $ do 
    msgs <- atomically $ do
      msgs <- readTVar msgvar
      writeTVar msgvar []
      return msgs
    mapM_ printMsg . reverse $ msgs

  -- forkIO $ do
  -- threadDelay 100000

  msgs' <- atomically $ do
    msgs' <- readTVar logvar
    writeTVar logvar []
    return msgs'
  mapM_ (textViewMsg env tv) . reverse $  msgs'

    -- (textViewMsg env tv) . reverse $ msgs'
  
  return ()
-}

  -- printMsg env (Msg (show (fib 35)))
  -- performGC

 {- 
  msgs' <- atomically $ do
      msgs' <- readTVar logvar
      writeTVar logvar []
      return msgs'
     -}
  -- mapM_ (printMsg env) . reverse . map (Msg . prettyPrintMessage) $ msgs'
  --  performGC

{-
textViewMsg :: JNIEnv -> JObject -> Message -> IO ()
textViewMsg env tv msg = do
  cstr <- newCString (prettyPrintMessage msg ++ "\n") 
  textViewAppend env tv cstr
-}

printMsg :: JavaMessage -> IO ()
printMsg (Msg msg) = do
    withCString "UPHERE" $ \ctag -> 
      withCString msg $ \cmsg ->
        androidLogWrite 3 ctag cmsg >> return ()


foreign import ccall safe "wrapper" mkOnIdleFPtr
  :: (JNIEnv -> JObject -> JObject -> IO ())
     -> IO (FunPtr (JNIEnv -> JObject -> JObject -> IO ()))

foreign import ccall safe "c_register_on_idle_fptr"
  registerOnIdleFPtr :: FunPtr (JNIEnv -> JObject -> JObject -> IO ()) -> IO ()

foreign import ccall safe "__android_log_write"
  androidLogWrite :: CInt -> CString -> CString -> IO CInt

