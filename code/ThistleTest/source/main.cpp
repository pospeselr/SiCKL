#include <iostream>
#include <memory>
#include <cstring>
using namespace std;

#include <ruff.h>
#include <thistle.h>

int main() try
{
    const size_t batch_count = 13;

    // image
    srand(1);
    unique_ptr<float[]> buffer(new float[784 * batch_count]);
    for(int k = 0; k < 784 * batch_count; k++)
    {
        buffer[k] = float(rand()) / float(RAND_MAX);
    }

    thistle_begin_session(THISTLE_THROW_ON_ERROR());

    auto batch = thistle_create_sample_buffer(28, 28, 1, batch_count, nullptr, THISTLE_THROW_ON_ERROR());
    auto size = thistle_get_buffer_batch_size(batch, THISTLE_THROW_ON_ERROR());
    auto width = thistle_get_buffer_sample_width(batch, THISTLE_THROW_ON_ERROR());
    auto height = thistle_get_buffer_sample_height(batch, THISTLE_THROW_ON_ERROR());
    auto channels = thistle_get_buffer_sample_channels(batch, THISTLE_THROW_ON_ERROR());

    thistle_set_buffer_data(batch, 784 * batch_count, buffer.get(), THISTLE_THROW_ON_ERROR());

    cout << "batch size : " << size << endl;
    cout << "width      : " << width << endl;
    cout << "height     : " << height << endl;
    cout << "channels   : " << channels << endl;

    // identity matrix
    unique_ptr<float[]> weights(new float[785 * 784]);
    for(int i = 0; i < 784; i++)
    {
        // identity matrix
        for(int j = 0; j < 784; j++)
        {
            weights[j + 785 * i] = (i == j) ? 1.0f : 0.0f;
        }
        // bias
        weights[784 + 785 * i] = 0.0f;
    }

    auto transformed_batch = thistle_create_sample_buffer(784, 1, 1, batch_count, nullptr, THISTLE_THROW_ON_ERROR());
    auto transform_node = thistle_create_linear_transform_node(784, 784, weights.get(), (785*784), THISTLE_THROW_ON_ERROR());

    auto parameterCount = thistle_get_node_parameters_count(transform_node, THISTLE_THROW_ON_ERROR());

    cout << "parameters : " << parameterCount << endl;
    auto deltaBuffer = thistle_create_flat_buffer(parameterCount, nullptr, THISTLE_THROW_ON_ERROR());

    thistle_calc_node_output(transform_node, batch, nullptr, transformed_batch, THISTLE_THROW_ON_ERROR());

    unique_ptr<float[]> transformed(new float[784 * batch_count]);
    memset(transformed.get(), 784 * batch_count * sizeof(float), 0x00);
    thistle_get_buffer_data(transformed_batch, 784 * batch_count, transformed.get(), THISTLE_THROW_ON_ERROR());

    for(int i = 0; i < 784 * batch_count; i++)
    {
        RUFF_ASSERT(transformed[i] == buffer[i]);
        //cout << i << " " << transformed[i] << " : " << buffer[i] << endl;
    }

    thistle_free_buffer(deltaBuffer, THISTLE_THROW_ON_ERROR());
    thistle_free_buffer(batch, THISTLE_THROW_ON_ERROR());
    thistle_free_buffer(transformed_batch, THISTLE_THROW_ON_ERROR());
    thistle_end_session(THISTLE_THROW_ON_ERROR());
}
catch(const exception& ex)
{
    std::cout << ex.what() << std::endl;
}