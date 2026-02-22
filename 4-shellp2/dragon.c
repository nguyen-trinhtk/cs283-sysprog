#include <stdio.h>
#include "dragon.h"

extern void print_dragon(){
  fwrite(dragon_txt, 1, dragon_txt_len, stdout);
  printf("\n");
}