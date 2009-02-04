#include <iostream>
#include "luatest.h"

Plus::Plus() {
  std::cout << "Constructor called" << std::endl;
}

Plus::~Plus() {
  std::cout << "Destructor called" << std::endl;
}

int Plus::addtwo(int a, int b) {
  return a + b;
}
