
cabal --with-ghc=arm-unknown-linux-androideabi-ghc --with-ld=arm-linux-androideabi-ld.gold --with-ghc-pkg=arm-unknown-linux-androideabi-ghc-pkg --extra-include-dir=$ANDROID_NDK_HOME/platforms/android-21/arch-arm/usr/include configure

cabal build

cabal copy

cabal register
 

