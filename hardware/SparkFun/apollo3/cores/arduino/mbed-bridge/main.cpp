/* 
// This file is subject to the terms and conditions defined in
// file 'LICENSE.md', which is part of this source code package.
*/

#include "Arduino.h"

int main(void)
{
  init();
  initVariant();
  setup();
  for (;;)
  {
    loop();
  }
  return 0;
}