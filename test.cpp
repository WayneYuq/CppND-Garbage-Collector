#include <iostream>

class test
{
private:
    /* data */
public:
    test(/* args */);
    ~test();
};

test::test(/* args */)
{
}

test::~test()
{
}

int main(int argc, char const *argv[])
{
    std::cout << new test() << "\n";
    
    return 0;
}
