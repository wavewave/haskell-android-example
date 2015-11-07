#ifndef __WQUEUE__
#define __WQUEUE__

#include <pthread.h>
//
#include <vector>
#include <string>

using namespace std;

#define TAG_MSG   0
#define TAG_COORD 1

struct message {
  int tag;
  char* text;
  int sz_text;
  int x;
  int y;
};

class wqueue {

 private:
  vector<pair<char*,int> > msgs;
  pair<int,int> coord;
  
  pthread_mutex_t wlock; 
  pthread_cond_t  wcond; 

 public:
  pthread_mutex_t choreolock;
  pthread_cond_t  choreocond;
  
 public:
  wqueue();
  ~wqueue();

  void loop( JNIEnv* env,
	     void (*callback)(JNIEnv*, message* ),
	     void (*flush)(JNIEnv*) );
  
  void write_message( char* cmsg, int n );
  void write_coord( int x, int y );
};

#endif
