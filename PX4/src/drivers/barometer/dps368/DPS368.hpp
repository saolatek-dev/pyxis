/****************************************************************************
 *
 *   Copyright (c) 2019-2021 PX4 Development Team. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 * 3. Neither the name PX4 nor the names of its contributors may be
 *    used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 ****************************************************************************/

/**
 * @file DPS368.hpp
 *
 * Driver for the Infineon DPS368 barometer connected via I2C or SPI.
 *
 * The DPS368 is the waterproof/dust-proof variant of the DPS310: same
 * register map, same Product ID (0x10), same calibration coefficient layout
 * and the same compensation formulas (see docs/Saolah743/new_baro_docs/
 * infineon-dps368-datasheet-en.pdf, chapters 4.9 and 7). This driver is
 * therefore a sibling of drivers/barometer/dps310, kept separate so the two
 * parts can be built and debugged independently on the same board.
 */

#pragma once

#include <drivers/device/Device.hpp>
#include <uORB/PublicationMulti.hpp>
#include <uORB/topics/sensor_baro.h>
#include <lib/perf/perf_counter.h>
#include <px4_platform_common/px4_work_queue/ScheduledWorkItem.hpp>
#include <px4_platform_common/i2c_spi_buses.h>

#include "Infineon_DPS368_Registers.hpp"

namespace dps368
{

using Infineon_DPS368::CalibrationCoefficients;
using Infineon_DPS368::Register;

class DPS368 : public I2CSPIDriver<DPS368>
{
public:
	DPS368(const I2CSPIDriverConfig &config, device::Device *interface);
	virtual ~DPS368();

	static I2CSPIDriverBase *instantiate(const I2CSPIDriverConfig &config, int runtime_instance);
	static void print_usage();

	int			init();

	void			print_status();
	void			RunImpl();

private:

	void			start();
	int			reset();

	uint8_t			RegisterRead(Register reg);
	void			RegisterWrite(Register reg, uint8_t val);
	void			RegisterSetBits(Register reg, uint8_t setbits);
	void			RegisterClearBits(Register reg, uint8_t clearbits);

	static constexpr uint32_t SAMPLE_RATE{32};

	uORB::PublicationMulti<sensor_baro_s> _sensor_baro_pub{ORB_ID(sensor_baro)};

	device::Device		*_interface;

	CalibrationCoefficients	_calibration{};

	perf_counter_t		_sample_perf;
	perf_counter_t		_comms_errors;
};

} // namespace dps368
