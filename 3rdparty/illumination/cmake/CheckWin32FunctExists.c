#ifdef CHECK_FUNCTION_EXISTS

#include <time.h>
#include <windows.h>

#ifdef __CLASSIC_C__
int main(){
  int ac;
  char*av[];
#else
int main(int ac, char*av[]){
#endif
  return CHECK_FUNCTION_EXISTS==NULL;
}

#else  /* CHECK_FUNCTION_EXISTS */

#  error "CHECK_FUNCTION_EXISTS has to specify the function"

#endif /* CHECK_FUNCTION_EXISTS */
