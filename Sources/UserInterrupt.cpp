#include "UserInterrupt.h"  

bool UserInterrupt::isInterrupt_ = false ;

void UserInterrupt::clear()
{
  isInterrupt_ = false;
}

void UserInterrupt::stopIfOrdered()
{
  if (isInterrupt_)
  {
    clear();
    throw UserInterrupt();
  }
}

void UserInterrupt::orderStop()
{
  isInterrupt_ = true;
}

const char* UserInterrupt::what() const 
{ 
  return "Current action was interrupted by user";
}
