
arm-unknown-linux-androideabi-ghc -c HaskellActivity.hs

arm-linux-androideabi-gcc -I/nix/store/70qz24akm0fyq01p8dibl5zigzzpg4rf-ghc-7.10.2/lib/ghc-7.10.2/include -fPIC -c init.c

arm-unknown-linux-androideabi-ghc -shared -no-hs-main  HaskellActivity.o init.o ~/.cabal/lib/arm-linux-android-ghc-7.10.2/foreign-jni-0.1.0.0-BLasI58wFFn6cArKa6ThDA/HSforeign-jni-0.1.0.0-BLasI58wFFn6cArKa6ThDA.o ~/.cabal/lib/arm-linux-android-ghc-7.10.2/text-1.2.1.3-DTuBjTofvAs1uCsYv9oEzS/HStext-1.2.1.3-DTuBjTofvAs1uCsYv9oEzS.o ~/.cabal/lib/arm-linux-android-ghc-7.10.2/mtl-2.2.1-65LuOlgNTrwDU3OLh1aCnC/HSmtl-2.2.1-65LuOlgNTrwDU3OLh1aCnC.o -lHSrts -L/nix/store/w623l8rbx4i6jmrw1zndxx6ag50l3j7l-ghc-android/lib/arm-unknown-linux-androideabi-ghc-7.10.2/trans_4zGCyGM416E0qnMIQb9mM2  -lHStransformers-0.4.2.0-4zGCyGM416E0qnMIQb9mM2 -L/nix/store/w623l8rbx4i6jmrw1zndxx6ag50l3j7l-ghc-android/lib/arm-unknown-linux-androideabi-ghc-7.10.2/binar_DZKxWoHV7QiEmTOuUJSvWL -lHSbinary-0.7.5.0-DZKxWoHV7QiEmTOuUJSvWL -L/nix/store/w623l8rbx4i6jmrw1zndxx6ag50l3j7l-ghc-android/lib/arm-unknown-linux-androideabi-ghc-7.10.2/bytes_3ta8QVxlpYNKR7sUcHtpIE -lHSbytestring-0.10.6.0-3ta8QVxlpYNKR7sUcHtpIE  -L/nix/store/w623l8rbx4i6jmrw1zndxx6ag50l3j7l-ghc-android/lib/arm-unknown-linux-androideabi-ghc-7.10.2/deeps_2I70b8M8s5p23eitWtXIeR -lHSdeepseq-1.4.1.1-2I70b8M8s5p23eitWtXIeR -lCffi -L/nix/store/w623l8rbx4i6jmrw1zndxx6ag50l3j7l-ghc-android/lib/arm-unknown-linux-androideabi-ghc-7.10.2/conta_Eag7kM77Y7fCgl1OTcgXDg -lHScontainers-0.5.6.2-Eag7kM77Y7fCgl1OTcgXDg -L/nix/store/w623l8rbx4i6jmrw1zndxx6ag50l3j7l-ghc-android/lib/arm-unknown-linux-androideabi-ghc-7.10.2/array_HnDHE6WOgoj97o9lI9ovvx -lHSarray-0.5.1.0-HnDHE6WOgoj97o9lI9ovvx -lm -o libhaskell.so

cp libhaskell.so ../app/src/main/jniLibs/armeabi/libhaskell.so


