#include <iostream>
#include <memory>
#include <cstring>
using namespace std;

#include <ruff.h>
#include <thistle.h>

void verify_linear_transform_output()
{
    const size_t batch_count = 13;

    // image
    srand(1);
    unique_ptr<float[]> buffer(new float[784 * batch_count]);
    for(int k = 0; k < 784 * batch_count; k++)
    {
        buffer[k] = float(rand()) / float(RAND_MAX);
    }

    auto batch = thistle_create_sample_buffer(28, 28, 1, batch_count, nullptr, THISTLE_THROW_ON_ERROR());
    auto size = thistle_get_buffer_element_count(batch, THISTLE_THROW_ON_ERROR());
    auto width = thistle_get_buffer_element_width(batch, THISTLE_THROW_ON_ERROR());
    auto height = thistle_get_buffer_element_height(batch, THISTLE_THROW_ON_ERROR());
    auto channels = thistle_get_buffer_element_channels(batch, THISTLE_THROW_ON_ERROR());

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
}

void verify_label_learning_signal()
{
    const size_t input_count = 29;
    const size_t output_count = 1;
    const size_t weight_count = (input_count + 1) * output_count;
    const size_t batch_count = 59;

    auto init_buffer = [](auto& buffer, float min, float max)
    {
        for(float& val : buffer)
        {
            val = float(rand()) / float(RAND_MAX) * (max - min) - max;
        }
    };

    float true_weights[weight_count];
    init_buffer(true_weights, -1.0f, 1.0f);

    float initial_weights[weight_count];
    init_buffer(initial_weights, -1.0f, 1.0f);

    float input_vector[input_count * batch_count];
    init_buffer(input_vector, 0.0f, 1.0f);

    float label_vector[output_count * batch_count];
    float initial_product = initial_weights[input_count];
    float error;
    float output_delta;
    {
        for(size_t j = 0; j < batch_count; j++)
        {
            float true_product = true_weights[input_count];
            for(size_t k = 0; k < input_count; k++)
            {
                float& input_k = input_vector[k + j * input_count];

                float& tw_k = true_weights[k];
                true_product += input_k * tw_k;
            }
            label_vector[j] = true_product;
            cout << "label_vector[" << j << "] : " << true_product << endl;
        }
    }

    auto input_batch = thistle_create_sample_buffer(input_count, 1, 1, batch_count, input_vector, THISTLE_THROW_ON_ERROR());
    auto output_batch = thistle_create_sample_buffer(output_count, 1, 1, batch_count, nullptr, THISTLE_THROW_ON_ERROR());
    auto label_batch = thistle_create_sample_buffer(output_count, 1, 1, batch_count, label_vector, THISTLE_THROW_ON_ERROR());
    auto error_batch = thistle_create_sample_buffer(1, 1, 1, batch_count, nullptr, THISTLE_THROW_ON_ERROR());
    auto label_delta_batch = thistle_create_sample_buffer(output_count, 1, 1, batch_count, nullptr, THISTLE_THROW_ON_ERROR());
    auto parameter_delta_buffer = thistle_create_flat_buffer(weight_count, nullptr, THISTLE_THROW_ON_ERROR());

    // create nodes
    auto transform_node = thistle_create_linear_transform_node(input_count, output_count, initial_weights, weight_count, THISTLE_THROW_ON_ERROR());
    auto transform_parameter_buffer = thistle_get_node_parameter_buffer(transform_node, THISTLE_THROW_ON_ERROR());
    auto label_node = thistle_create_label_node(output_count, thistle_square_difference, THISTLE_THROW_ON_ERROR());

    // set and init weight updater
    auto weight_updater = thistle_create_sgd_parameter_updater(weight_count, THISTLE_THROW_ON_ERROR());

    float learningRate = 0.01f;
    float momentum = 0.5f;

    for(int k = 0; k < 12000; k++)
    {
        if((k + 0) % 200 == 0)
        {
            learningRate *= 0.99f;

            thistle_set_sgd_parameter_updater_hyperparameters(weight_updater, learningRate, momentum, THISTLE_THROW_ON_ERROR());
        }

        cout << "LearningRate: " << learningRate << endl;
        // feedforward!
        thistle_calc_node_output(transform_node, input_batch, nullptr, output_batch, THISTLE_THROW_ON_ERROR());

        // calculate square error
        thistle_calc_node_output(label_node, output_batch, label_batch, error_batch, THISTLE_THROW_ON_ERROR());
        float calculated_error[batch_count];
        thistle_get_buffer_data(error_batch, batch_count, calculated_error, THISTLE_THROW_ON_ERROR());

        float average_error = 0.0f;
        for(float f : calculated_error)
        {
            average_error += f;
        }
        average_error /= batch_count;

        // verify error calculation
        cout << "Average Error: " << average_error << endl;

        // calculate label error derivatives
        thistle_calc_node_input_deltas(label_node, output_batch, nullptr, label_batch, label_delta_batch, THISTLE_THROW_ON_ERROR());

        // calculate weight matrix derivatives
        thistle_calc_node_parameter_deltas(transform_node, input_batch, label_delta_batch, nullptr, parameter_delta_buffer, THISTLE_THROW_ON_ERROR());

        // copy weights and weight deltas to CPU memory
        float calculated_weight_deltas[weight_count];
        thistle_get_buffer_data(parameter_delta_buffer, weight_count, calculated_weight_deltas, THISTLE_THROW_ON_ERROR());

        float current_weights[weight_count];
        thistle_get_buffer_data(transform_parameter_buffer, weight_count, current_weights, THISTLE_THROW_ON_ERROR());

        cout << "True, Current, Delta" << endl;
        for(size_t k = 0; k < weight_count; k++)
        {
            cout << true_weights[k] << ", " << current_weights[k] << ", " << calculated_weight_deltas[k] << endl;
        }

        thistle_update_node_parameters(weight_updater, transform_parameter_buffer, parameter_delta_buffer, THISTLE_THROW_ON_ERROR());

        cout << "----" << endl;
    }

    thistle_free_parameter_updater(weight_updater, THISTLE_THROW_ON_ERROR());
    thistle_free_buffer(transform_parameter_buffer, THISTLE_THROW_ON_ERROR());
    thistle_free_node(transform_node, THISTLE_THROW_ON_ERROR());
}

int main() try
{
    thistle_begin_session(THISTLE_THROW_ON_ERROR());

    //verify_linear_transform_output();
    verify_label_learning_signal();

    thistle_end_session(THISTLE_THROW_ON_ERROR());
}
catch(const exception& ex)
{
    std::cout << ex.what() << std::endl;
}