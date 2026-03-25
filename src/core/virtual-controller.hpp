#pragma once

#include "input-listener.hpp"
#include "virtual-device.hpp"
#include "device-events.hpp"
#include <libevdev/libevdev-uinput.h>

class VirtualController : public VirtualDevice {
	Q_OBJECT

	public:
		VirtualController(const std::string& device_path, struct controller_events& events, std::atomic<bool>& input_listener_exists, QObject* parent = nullptr);

		void generate_key_event(int code, int value) override;
		void generate_abs_event(int axis, int value) override;

	protected slots:
	void handle_key_events(int code, int value) override;

	private:
		// Didn't know how to make everything on the key buffer, so made as a "fuck it"
		QList<int> m_lr_lj;	// ABS_X buffer - 0 means left, 1 means right
		QList<int> m_ud_lj;	// ABS_Y buffer - 0 means up, 1 means down
		QList<int> m_lr_rj;	// ABS_RX buffer - 0 means left, 1 means right
		QList<int> m_ud_rj;	// ABS_RY buffer - 0 means up, 1 means right

		std::set<int> m_held_keys;

		struct controller_events m_events;
};