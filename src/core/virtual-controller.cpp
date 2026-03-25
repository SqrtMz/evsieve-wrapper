#include "virtual-controller.hpp"

VirtualController::VirtualController(const std::string& device_path, struct controller_events& events, std::atomic<bool>& input_listener_exists, QObject* parent) : VirtualDevice(device_path, input_listener_exists, parent), m_events(events) {
	struct libevdev* dev = libevdev_new();
	libevdev_set_name(dev, "Key Remapper Virtual Controller");

	struct input_absinfo abs_x = {
		.value = 0,
		.minimum = -32768,
		.maximum = 32767,
		.fuzz = 16,
		.flat = 128
	};
	
	struct input_absinfo abs_y = {
		.value = -1,
		.minimum = -32768,
		.maximum = 32767,
		.fuzz = 16,
		.flat = 128
	};

	libevdev_enable_event_type(dev, EV_ABS);
	libevdev_enable_event_code(dev, EV_ABS, ABS_X, &abs_x);
	libevdev_enable_event_code(dev, EV_ABS, ABS_Y, &abs_y);
	libevdev_enable_event_code(dev, EV_ABS, ABS_RX, &abs_x);
	libevdev_enable_event_code(dev, EV_ABS, ABS_RY, &abs_y);

	libevdev_enable_event_type(dev, EV_KEY);
	libevdev_enable_event_code(dev, EV_KEY, BTN_DPAD_UP, nullptr);
	libevdev_enable_event_code(dev, EV_KEY, BTN_DPAD_RIGHT, nullptr);
	libevdev_enable_event_code(dev, EV_KEY, BTN_DPAD_DOWN, nullptr);
	libevdev_enable_event_code(dev, EV_KEY, BTN_DPAD_LEFT, nullptr);
	libevdev_enable_event_code(dev, EV_KEY, BTN_SOUTH, nullptr);
	libevdev_enable_event_code(dev, EV_KEY, BTN_EAST, nullptr);
	libevdev_enable_event_code(dev, EV_KEY, BTN_NORTH, nullptr);
	libevdev_enable_event_code(dev, EV_KEY, BTN_WEST, nullptr);
	libevdev_enable_event_code(dev, EV_KEY, BTN_TL, nullptr);
	libevdev_enable_event_code(dev, EV_KEY, BTN_TR, nullptr);
	libevdev_enable_event_code(dev, EV_KEY, BTN_TL2, nullptr);
	libevdev_enable_event_code(dev, EV_KEY, BTN_TR2, nullptr);
	libevdev_enable_event_code(dev, EV_KEY, BTN_SELECT, nullptr);
	libevdev_enable_event_code(dev, EV_KEY, BTN_START, nullptr);
	libevdev_enable_event_code(dev, EV_KEY, BTN_MODE, nullptr);
	libevdev_enable_event_code(dev, EV_KEY, BTN_THUMBL, nullptr);
	libevdev_enable_event_code(dev, EV_KEY, BTN_THUMBR, nullptr);

	libevdev_uinput_create_from_device(dev, LIBEVDEV_UINPUT_OPEN_MANAGED, &m_uinput);
	libevdev_free(dev);
}

void VirtualController::generate_key_event(int code, int value) {
	libevdev_uinput_write_event(m_uinput, EV_KEY, code, value);
	libevdev_uinput_write_event(m_uinput, EV_SYN, SYN_REPORT, 0);
}

void VirtualController::generate_abs_event(int axis, int value) {
	libevdev_uinput_write_event(m_uinput, EV_ABS, axis, value);
	libevdev_uinput_write_event(m_uinput, EV_SYN, SYN_REPORT, 0);
}

void VirtualController::handle_key_events(int code, int value) {

	if (value == 1) m_held_keys.insert(code);
	else m_held_keys.erase(code);

	if (m_events.abs_ljoy_left->contains(code)) {
		if (std::all_of(m_events.abs_ljoy_left->begin(), m_events.abs_ljoy_left->end(), [this](int k){return m_held_keys.contains(k);}) && value == 1 && !m_lr_lj.contains(0)) m_lr_lj.prepend(0);
		else m_lr_lj.removeOne(0);
	}

	if (m_events.abs_ljoy_right->contains(code)) {
		if (std::all_of(m_events.abs_ljoy_right->begin(), m_events.abs_ljoy_right->end(), [this](int k){return m_held_keys.contains(k);}) && value == 1 && !m_lr_lj.contains(1)) m_lr_lj.prepend(1);
		else m_lr_lj.removeOne(1);
	}

	if (!m_lr_lj.isEmpty()) {
		if (m_lr_lj.at(0) == 0) generate_abs_event(ABS_X, -32768);
		else if (m_lr_lj.at(0) == 1) generate_abs_event(ABS_X, 32767);
	} else {
		generate_abs_event(ABS_X, 0);
	}

	if (m_events.abs_ljoy_up->contains(code)) {
		if (std::all_of(m_events.abs_ljoy_up->begin(), m_events.abs_ljoy_up->end(), [this](int k){return m_held_keys.contains(k);}) && value == 1 && !m_ud_lj.contains(0)) m_ud_lj.prepend(0);
		else m_ud_lj.removeOne(0);
	}

	if (m_events.abs_ljoy_down->contains(code)) {
		if (std::all_of(m_events.abs_ljoy_down->begin(), m_events.abs_ljoy_down->end(), [this](int k){return m_held_keys.contains(k);}) && value == 1 && !m_ud_lj.contains(1)) m_ud_lj.prepend(1);
		else m_ud_lj.removeOne(1);
	}

	if (!m_ud_lj.isEmpty()) {
		if (m_ud_lj.at(0) == 0) generate_abs_event(ABS_Y, -32768);
		else if (m_ud_lj.at(0) == 1) generate_abs_event(ABS_Y, 32767);
	} else {
		generate_abs_event(ABS_Y, 0);
	}

	if (m_events.abs_rjoy_left->contains(code)) {
		if (std::all_of(m_events.abs_rjoy_left->begin(), m_events.abs_rjoy_left->end(), [this](int k){return m_held_keys.contains(k);}) && value == 1 && !m_lr_rj.contains(0)) m_lr_rj.prepend(0);
		else m_lr_rj.removeOne(0);
	}

	if (m_events.abs_rjoy_right->contains(code)) {
		if (std::all_of(m_events.abs_rjoy_right->begin(), m_events.abs_rjoy_right->end(), [this](int k){return m_held_keys.contains(k);}) && value == 1 && !m_lr_rj.contains(1)) m_lr_rj.prepend(1);
		else m_lr_rj.removeOne(1);
	}

	if (!m_lr_rj.isEmpty()) {
		if (m_lr_rj.at(0) == 0) generate_abs_event(ABS_RX, -32768);
		else if (m_lr_rj.at(0) == 1) generate_abs_event(ABS_RX, 32767);
	} else {
		generate_abs_event(ABS_RX, 0);
	}

	if (m_events.abs_rjoy_up->contains(code)) {
		if (std::all_of(m_events.abs_rjoy_up->begin(), m_events.abs_rjoy_up->end(), [this](int k){return m_held_keys.contains(k);}) && value == 1 && !m_ud_rj.contains(0)) m_ud_rj.prepend(0);
		else m_ud_rj.removeOne(0);
	}

	if (m_events.abs_rjoy_down->contains(code)) {
		if (std::all_of(m_events.abs_rjoy_down->begin(), m_events.abs_rjoy_down->end(), [this](int k){return m_held_keys.contains(k);}) && value == 1 && !m_ud_rj.contains(1)) m_ud_rj.prepend(1);
		else m_ud_rj.removeOne(1);
	}

	if (!m_ud_rj.isEmpty()) {
		if (m_ud_rj.at(0) == 0) generate_abs_event(ABS_RY, -32768);
		else if (m_ud_rj.at(0) == 1) generate_abs_event(ABS_RY, 32767);
	} else {
		generate_abs_event(ABS_RY, 0);
	}


	if (m_events.btn_thumbl->contains(code)) {
		if (std::all_of(m_events.btn_thumbl->begin(), m_events.btn_thumbl->end(), [this](int k){return m_held_keys.contains(k);})) generate_key_event(BTN_THUMBL, 1);
		else generate_key_event(BTN_THUMBL, 0);
	}

	if (m_events.btn_thumbr->contains(code)) {
		if (std::all_of(m_events.btn_thumbr->begin(), m_events.btn_thumbr->end(), [this](int k){return m_held_keys.contains(k);})) generate_key_event(BTN_THUMBR, 1);
		else generate_key_event(BTN_THUMBR, 0);
	}


	if (m_events.btn_west->contains(code)) {
		if (std::all_of(m_events.btn_west->begin(), m_events.btn_west->end(), [this](int k){return m_held_keys.contains(k);})) generate_key_event(BTN_WEST, 1);
		else generate_key_event(BTN_WEST, 0);
	}

	if (m_events.btn_east->contains(code)) {
		if (std::all_of(m_events.btn_east->begin(), m_events.btn_east->end(), [this](int k){return m_held_keys.contains(k);})) generate_key_event(BTN_EAST, 1);
		else generate_key_event(BTN_EAST, 0);
	}

	if (m_events.btn_south->contains(code)) {
		if (std::all_of(m_events.btn_south->begin(), m_events.btn_south->end(), [this](int k){return m_held_keys.contains(k);})) generate_key_event(BTN_SOUTH, 1);
		else generate_key_event(BTN_SOUTH, 0);
	}

	if (m_events.btn_north->contains(code)) {
		if (std::all_of(m_events.btn_north->begin(), m_events.btn_north->end(), [this](int k){return m_held_keys.contains(k);})) generate_key_event(BTN_NORTH, 1);
		else generate_key_event(BTN_NORTH, 0);
	}


	if (m_events.btn_select->contains(code)) {
		if (std::all_of(m_events.btn_select->begin(), m_events.btn_select->end(), [this](int k){return m_held_keys.contains(k);})) generate_key_event(BTN_SELECT, 1);
		else generate_key_event(BTN_SELECT, 0);
	}

	if (m_events.btn_mode->contains(code)) {
		if (std::all_of(m_events.btn_mode->begin(), m_events.btn_mode->end(), [this](int k){return m_held_keys.contains(k);})) generate_key_event(BTN_MODE, 1);
		else generate_key_event(BTN_MODE, 0);
	}

	if (m_events.btn_start->contains(code)) {
		if (std::all_of(m_events.btn_start->begin(), m_events.btn_start->end(), [this](int k){return m_held_keys.contains(k);})) generate_key_event(BTN_START, 1);
		else generate_key_event(BTN_START, 0);
	}


	if (m_events.btn_dpad_up->contains(code)) {
		if (std::all_of(m_events.btn_dpad_up->begin(), m_events.btn_dpad_up->end(), [this](int k){return m_held_keys.contains(k);})) generate_key_event(BTN_DPAD_UP, 1);
		else generate_key_event(BTN_DPAD_UP, 0);
	}

	if (m_events.btn_dpad_right->contains(code)) {
		if (std::all_of(m_events.btn_dpad_right->begin(), m_events.btn_dpad_right->end(), [this](int k){return m_held_keys.contains(k);})) generate_key_event(BTN_DPAD_RIGHT, 1);
		else generate_key_event(BTN_DPAD_RIGHT, 0);
	}

	if (m_events.btn_dpad_down->contains(code)) {
		if (std::all_of(m_events.btn_dpad_down->begin(), m_events.btn_dpad_down->end(), [this](int k){return m_held_keys.contains(k);})) generate_key_event(BTN_DPAD_DOWN, 1);
		else generate_key_event(BTN_DPAD_DOWN, 0);
	}

	if (m_events.btn_dpad_left->contains(code)) {
		if (std::all_of(m_events.btn_dpad_left->begin(), m_events.btn_dpad_left->end(), [this](int k){return m_held_keys.contains(k);})) generate_key_event(BTN_DPAD_LEFT, 1);
		else generate_key_event(BTN_DPAD_LEFT, 0);
	}


	if (m_events.btn_tl->contains(code)) {
		if (std::all_of(m_events.btn_tl->begin(), m_events.btn_tl->end(), [this](int k){return m_held_keys.contains(k);})) generate_key_event(BTN_TL, 1);
		else generate_key_event(BTN_TL, 0);
	}

	if (m_events.btn_tr->contains(code)) {
		if (std::all_of(m_events.btn_tr->begin(), m_events.btn_tr->end(), [this](int k){return m_held_keys.contains(k);})) generate_key_event(BTN_TR, 1);
		else generate_key_event(BTN_TR, 0);
	}

	if (m_events.btn_tl2->contains(code)) {
		if (std::all_of(m_events.btn_tl2->begin(), m_events.btn_tl2->end(), [this](int k){return m_held_keys.contains(k);})) generate_key_event(BTN_TL2, 1);
		else generate_key_event(BTN_TL2, 0);
	}

	if (m_events.btn_tr2->contains(code)) {
		if (std::all_of(m_events.btn_tr2->begin(), m_events.btn_tr2->end(), [this](int k){return m_held_keys.contains(k);})) generate_key_event(BTN_TR2, 1);
		else generate_key_event(BTN_TR2, 0);
	}
}