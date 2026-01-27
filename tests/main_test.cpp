#include <signalflow/buffer.hpp>
#include <signalflow/window.hpp>
#include <iostream>

int main() {
    // --- Test 1: Buffer Logic ---
    std::cout << "--- Testing Circular Buffer ---" << std::endl;
    signalflow::CircularBuffer<int> buffer(3);
    buffer.push(10);
    buffer.push(20);
    buffer.push(30);
    buffer.push(40); 
    std::cout << "Latest (index 0): " << buffer.at(0) << std::endl; // 40
    std::cout << "Oldest (index 2): " << buffer.at(2) << std::endl; // 20
    std::cout << std::endl;

    // --- Test 2: Windowing Logic ---
    std::cout << "--- Testing Hann Window ---" << std::endl;
    signalflow::CircularBuffer<float> buf(5);
    for(int i=0; i<5; i++) buf.push(1.0f); // Fill with 1.0

    signalflow::Window hann(5, signalflow::Window::Type::Hann);
    auto result = hann.apply(buf);

    for (float val : result) {
        std::cout << val << " "; 
    }
    std::cout << "\n---------------------------" << std::endl;
    
    return 0;
}