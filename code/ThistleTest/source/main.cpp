#include <iostream>
#include <memory>
using namespace std;

#define RUFF_PRINT_ASSERT(SRC, LINE, MSG) std::cout << "cout " << SRC << "(" << LINE << ") : " << MSG << std::endl
#include <ruff.h>
#include <spark.h>
#include <thistle.h>

int main()
{
    RUFF_ASSERT(&main == nullptr);
    auto node = thistle_create_node_linear_transform(784, 500);
    cout << "Hello World" << endl;
}