#include "lda_gibbs.hpp"
#include <iostream>

int main() {
    lda_model lda(50, 0.5, 0.01, 0);
    lda.Inference(" ", " ", 0.9, 2);
    return 1;
} 
