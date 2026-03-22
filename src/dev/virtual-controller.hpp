#pragma once

#include "virtual-device.hpp"
#include <libevdev/libevdev-uinput.h>

class VirtualController : public VirtualDevice {
	Q_OBJECT

	public:
		VirtualController(const std::string& device_path, QObject* parent = nullptr);
		int init();

		void generate_key_event(int code, int value) override;
		void generate_abs_event(int axis, int value) override;

	protected slots:
		void handle_key_events(int code, int value) override;
};