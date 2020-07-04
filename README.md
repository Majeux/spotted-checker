Practical assignment for Advances in Model Checking. Performs LTL based model checking using the [SPOT](https://spot.lrde.epita.fr/) platform. 
Explores the tools in the Spot library to perform various model checking task:
- Building Kripke Structures
- Reading LTL formulas
- Building Buchi automata
- Computing cross products of Kripke and Buchi

Also provides an abstraction layer for defining Kripke structures and Buchi automata, making it more approachable and requiring less foreknowledge of C++ and Spot.

Contains an example implementation of Peterson's algorithm for N processes using the abstractions and some properties to check.

## Abstract Kripke Classes:
- `inc/model_abstract/state.h`: Contains the `AbstractState` class, which implements the `hash()` and `compare()` functions from `spot::state` for a generic set of state variables.
- `inc/model_abstract/iterator.h`: Contains the `AbstractIterator` class, which implements the `recycle` function from `spot::kripke_succ_iter` for a generic set of state variables.
- `inc/model_abstract/kripke.h`: Contains the `AbstractKripke` class, which implements the `succ_iter()` function from `spot::kripke`.

## Template Kripke Classes:
Contains classes inheriting our `inc/model_abstract` classes. Implementations of their virtual functions are left empty, with some instructions.

## Example Kripke Classes:
`inc/model_example` provides an example implementation of Peterson's algorithm for N processes. Made by implementing the functions from `inc/model_empty`.

## Wrapping/Implementing Model Checking:
- `inc/cross_product.h`: Contains the `CrossProduct` class which provides a generic (otherwise empty) function that can move through successors of pairs of states and implements a DFS search for an accepting run of the cross-product of a Kripke structure and Büchi automaton.
- `inc/checker.h`: Contains functions for explicitely creating Kripke structures and Büchi automata using SPOT, computing accepting runs between a Kripke and Büchi (using both SPOT and the `CrossProduct` class.
- `main/main.cc`: Constructs our example and performs some model checking on them.
