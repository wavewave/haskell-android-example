#include <Rts.h>
#include <RtsAPI.h>

void haskell_init()
{
  static char *argv[] = { "libhaskell.so", 0 }, **argv_ = argv;
  static int argc = 1;
  static RtsConfig rtsopts = { RtsOptsAll, "-C0.005"}; // -H128m -K64m" };
  hs_init_ghc(&argc,&argv_, rtsopts);
  //hs_init(&argc,&argv_);
  //chatter();
  //return NULL;
}
