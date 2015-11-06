cp $ANDROID_NDK_HOME/sources/cxx-stl/llvm-libc++/libs/armeabi/libc++_shared.so ../app/src/main/jniLibs/armeabi/libc++_shared.so

cd android-bridge ; ./build.sh ; cd ..

cabal --with-ghc=arm-unknown-linux-androideabi-ghc --with-ld=arm-linux-androideabi-ld.gold --with-ghc-pkg=arm-unknown-linux-androideabi-ghc-pkg --extra-include-dir=$ANDROID_NDK_HOME/platforms/android-21/arch-arm/usr/include  --extra-include-dir=$ANDROID_NDK_HOME/sources/cxx-stl/llvm-libc++/libcxx/include --extra-lib-dir=$ANDROID_NDK_HOME/sources/cxx-stl/llvm-libc++/libs/armeabi configure -v3 ;
    cabal build -v \
    && cp dist/build/libhaskell.so/libhaskell.so ../app/src/main/jniLibs/armeabi/libhaskell.so \
    && cd .. && gradle installArmDebug && cd hs


#--extra-include-dir=$ANDROID_NDK_HOME/sources/cxx-stl/llvm-libc++/libcxx/include
# --extra-include-dir=$ANDROID_NDK_HOME/sources/cxx-stl/gnu-libstdc++/4.9/include
#--extra-include-dir=$ANDROID_NDK_HOME/sources/cxx-stl/stlport/stlport

