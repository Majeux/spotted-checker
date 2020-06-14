#ifndef CHECKER_H
#define CHECKER_H

class Checker {
    public:
        Checker();

    /* TODO Read Kripke model
        * Build automata
        * Recognize state (variables)
    */

    /* TODO Create property checks
        * Standard method for deadlocks/liveness and stuff
        * Parse LTL formula to evaluate model dependent property
        * Convert property to Buchi
    */

    /* TODO Evaluate model
        * Cross-product Model x !property (Check inverse of desired property)
        * Check emptiness
        * Report violation ??
    */
};

#endif
