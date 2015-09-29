module Common where 

import qualified Data.Binary as Bi
import qualified Data.ByteString as S 
import qualified Data.ByteString.Lazy as L
import           Data.List (sortBy)
import qualified Data.Text as T
import           Data.Text.Binary
import qualified Data.Text.Encoding as TE

import Network.Simple.TCP


data Message = Message { messageNum :: Int 
                       , messageUser :: T.Text
                       , messageBody :: T.Text }
             deriving Show

instance Bi.Binary Message where 
  -- put :: Message -> Put
  put (Message n user txt) = do Bi.put n
                                Bi.put user
                                Bi.put txt
                           -- Bi.put (TE.encodeUtf8 txt)
  -- get :: Get Message 
  get = do n <- Bi.get
           -- txtbstr <- Bi.get
           -- let txt = TE.decodeUtf8 txtbstr
           user <- Bi.get
           txt <- Bi.get 
           return (Message n user txt)

packNumBytes :: S.ByteString -> S.ByteString
packNumBytes bstr = 
  let len = (fromIntegral . S.length) bstr :: Bi.Word32
  in L.toStrict (Bi.encode len)   


packAndSend :: (Bi.Binary a) => Socket -> a -> IO ()
packAndSend sock x = do 
    let msg = (L.toStrict . Bi.encode) x 
        sizebstr = packNumBytes msg
    send sock sizebstr
    send sock msg 


recvAndUnpack :: (Bi.Binary a) => Socket -> IO (Maybe a)
recvAndUnpack sock = do 
    msizebstr <- recv sock 4
    case msizebstr of 
      Nothing -> return Nothing
      Just sizebstr -> do 
        let s32 = (Bi.decode . L.fromStrict) sizebstr :: Bi.Word32
            s = fromIntegral s32 :: Int
        mmsg <- recv sock s

        case mmsg of 
          Nothing -> return Nothing
          Just msg -> (return . Just . Bi.decode . L.fromStrict) msg  


prettyPrintMessage :: Message -> String
prettyPrintMessage (Message n user txt) = show n ++ " : " ++ T.unpack user ++ " : " ++ T.unpack txt

checkLatestMessage :: [Message] -> Int
checkLatestMessage msgs = (head . sortBy (flip compare) . map messageNum) msgs
