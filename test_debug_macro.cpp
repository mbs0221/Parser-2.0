#include <iostream>

int main() {
    #ifdef _DEBUG
        std::cout << "_DEBUG macro is defined" << std::endl;
    #else
        std::cout << "_DEBUG macro is NOT defined" << std::endl;
    #endif
    
    #ifdef NDEBUG
        std::cout << "NDEBUG macro is defined" << std::endl;
    #else
        std::cout << "NDEBUG macro is NOT defined" << std::endl;
    #endif
    
    return 0;
} 