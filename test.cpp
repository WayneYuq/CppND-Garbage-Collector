#include <iostream>
#include <string>

template<int size = 0>
class test
{
private:
    int arraysize;
public:
    test() : arraysize(size) { std::cout << arraysize << "\n"; };
    ~test() {};
};

// template<int size>
// test<size>::test()
// {
//     std::cout << 132 << "\n";
// }



int main(int argc, char const *argv[])
{
    test<> p; 
    return 0;
}
