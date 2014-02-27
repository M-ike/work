#ifndef TOKEN_H
#define TOKEN_H

#include <string>
#include "../common.h"

template <typename T>
class Token
{
   public:
      Token() {}
      T type;
      position_t position;
      std::string value;
};

#endif
