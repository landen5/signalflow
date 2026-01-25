#include <signalflow/buffer.hpp>
#include <iostream>

int main() {
    std::cout << "Testing wrap-around..." << std::endl;
    signalflow::CircularBuffer<int> buffer(3);
    buffer.push(10);
    buffer.push(20);
    buffer.push(30);
    buffer.push(40); // This should overwrite the oldest value (10)

    std::cout << "Value at index 0: " << buffer.at(0) << std::endl; // Should print 40
    std::cout << "Value at index 1: " << buffer.at(1) << std::endl; // Should print 20
    std::cout << "Value at index 2: " << buffer.at(2) << std::endl; // Should print 10
    return 0;
}