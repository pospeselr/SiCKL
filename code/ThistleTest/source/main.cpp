#include <iostream>
#include <memory>
using namespace std;

#include <ruff.h>
#include <thistle.h>

int main() try
{
    //RUFF_ASSERT(&main == nullptr);
    //auto node = thistle_create_node_linear_transform(784, 500);

    unique_ptr<float[]> buffer(new float[784 * 100]);

    thistle_begin_session(THISTLE_THROW_ON_ERROR());

    auto batch = thistle_create_sample_batch(28, 28, 1, 100, buffer.get(), THISTLE_THROW_ON_ERROR());
    auto size = thistle_get_batch_size(batch, THISTLE_THROW_ON_ERROR());
    auto width = thistle_get_sample_width(batch, THISTLE_THROW_ON_ERROR());
    auto height = thistle_get_sample_height(batch, THISTLE_THROW_ON_ERROR());
    auto channels = thistle_get_sample_channels(batch, THISTLE_THROW_ON_ERROR   ());

    cout << "batch size : " << size << endl;
    cout << "width      : " << width << endl;
    cout << "height     : " << height << endl;
    cout << "channels   : " << channels << endl;


    cout << "Hello World" << endl;

    thistle_end_session(THISTLE_THROW_ON_ERROR());
} catch(const exception& ex)
{
    std::cout << ex.what() << std::endl;
}