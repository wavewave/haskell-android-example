
cabal --with-ghc=arm-unknown-linux-androideabi-ghc --with-ld=arm-linux-androideabi-ld.gold --with-ghc-pkg=arm-unknown-linux-androideabi-ghc-pkg configure

cabal build

cp dist/build/libhaskell.so/libhaskell.so ../app/src/main/jniLibs/armeabi/libhaskell.so

cd .. ; gradle installArmDebug ; cd hs

