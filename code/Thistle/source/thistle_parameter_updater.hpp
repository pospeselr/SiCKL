#pragma once

struct thistle_parameter_updater
{
    virtual ~thistle_parameter_updater() = default;

    virtual void update_parameters(
        thistle_node_t* node,
        const device_buffer1d<float>& parameterUpdateBuffer) const = 0;
};
typedef thistle_parameter_updater thistle_parameter_updater_t;