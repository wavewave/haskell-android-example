
cabal --with-ghc=arm-unknown-linux-androideabi-ghc --with-ld=arm-linux-androideabi-ld.gold --with-ghc-pkg=arm-unknown-linux-androideabi-ghc-pkg --extra-include-dir=$ANDROID_NDK_HOME/platforms/android-21/arch-arm/usr/include  --extra-include-dir=$ANDROID_NDK_HOME/sources/cxx-stl/llvm-libc++/libcxx/include configure

#--extra-include-dir=$ANDROID_NDK_HOME/sources/cxx-stl/llvm-libc++/libcxx/include
# --extra-include-dir=$ANDROID_NDK_HOME/sources/cxx-stl/gnu-libstdc++/4.9/include
#--extra-include-dir=$ANDROID_NDK_HOME/sources/cxx-stl/stlport/stlport

cabal build

cp dist/build/libhaskell.so/libhaskell.so ../app/src/main/jniLibs/armeabi/libhaskell.so

cd .. ; gradle installArmDebug ; cd hs

