#pragma once

struct thistle_parameter_updater
{
    virtual ~thistle_parameter_updater() = default;

    virtual void update_parameters(
        device_buffer1d<float>& paramBuffer,
        const device_buffer1d<float>& paramDeltaBuffer)const = 0;
};
typedef thistle_parameter_updater thistle_parameter_updater_t;