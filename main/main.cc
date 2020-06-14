#include <iostream>

#include <spot/twaalgos/dot.hh>

#include "checker.h"

int main()
{
  Checker checker;
  std::cerr << "Printing test kripke" << std::endl;
  spot::print_dot(std::cout, checker.explicit_door_kripke());

  std::cerr << "-- done" << std::endl;
  return 1;
}
