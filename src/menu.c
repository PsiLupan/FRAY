#include "menu.h"

//8022BFBC
u32 sub_8022BFBC(u32 result){
  if ( result == 2 )
    return 0x0ED241FF;
  if ( result < 2 )
  {
    if ( result )
    {
      if ( result >= 0 )
        result = 0xFF5A41FF;
    }
    else
    {
      result = 0x5A73FFFF;
    }
  }
  else if ( result == 4 )
  {
    result = 0x9B41FFFF;
  }
  else if ( result < 4 )
  {
    result = 0xF0C85AFF;
  }
  return result;
}

//8022C010
s32 sub_8022C010(s32 result, s32 a2){
  if ( !result )
    return a2;
  switch ( result )
  {
    case 0:
      return result;
    case 1:
    case 6:
    case 7:
    case 8:
    case 9:
    case 10:
    case 33:
      result = 0;
      break;
    case 2:
    case 11:
    case 12:
    case 13:
    case 14:
    case 15:
    case 16:
    case 17:
    case 18:
      result = 1;
      break;
    case 3:
      result = 2;
      break;
    case 4:
    case 19:
    case 20:
    case 21:
    case 22:
    case 23:
    case 24:
      result = 3;
      break;
    case 5:
    case 25:
    case 26:
    case 27:
    case 28:
    case 29:
    case 30:
    case 31:
    case 32:
      result = 4;
      break;
  }
  return result;
}