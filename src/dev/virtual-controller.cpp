#include "virtual-controller.hpp"

VirtualController::VirtualController(const std::string& device_path, QObject* parent) : VirtualDevice(device_path, parent) {init();}

int VirtualController::init() {
	struct libevdev* dev = libevdev_new();
	libevdev_set_name(dev, "NON-EVSIEVE-VIRTUAL-DEVICE");

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

	int rc = libevdev_uinput_create_from_device(dev, LIBEVDEV_UINPUT_OPEN_MANAGED, &m_uinput);

	libevdev_free(dev);
	return rc;
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

	if (code == KEY_A || code == KEY_D) {
		if (value == 1) m_lr_lj.prepend(code);
		else m_lr_lj.removeOne(code);
	}

	if (!m_lr_lj.isEmpty()) {
		if (m_lr_lj.at(0) == KEY_A) generate_abs_event(ABS_X, -32768);
		else if (m_lr_lj.at(0) == KEY_D) generate_abs_event(ABS_X, 32767);
	} else {
		generate_abs_event(ABS_X, 0);
	}

	if (code == KEY_W || code == KEY_S) {
		if (value == 1) m_ud_lj.prepend(code);
		else m_ud_lj.removeOne(code);
	}

	if (!m_ud_lj.isEmpty()) {
		if (m_ud_lj.at(0) == KEY_W) generate_abs_event(ABS_Y, -32768);
		else if (m_ud_lj.at(0) == KEY_S) generate_abs_event(ABS_Y, 32767);
	} else {
		generate_abs_event(ABS_Y, 0);
	}

	if (code == KEY_G || code == KEY_B) {
		if (value == 1) m_lr_rj.prepend(code);
		else m_lr_rj.removeOne(code);
	}

	if (!m_lr_rj.isEmpty()) {
		if (m_lr_rj.at(0) == KEY_G) generate_abs_event(ABS_RY, -32768);
		else if (m_lr_rj.at(0) == KEY_B) generate_abs_event(ABS_RY, 32767);
	} else {
		generate_abs_event(ABS_RY, 0);
	}

	if (code == KEY_V || code == KEY_N) {
		if (value == 1) m_ud_rj.prepend(code);
		else m_ud_rj.removeOne(code);
	}

	if (!m_ud_rj.isEmpty()) {
		if (m_ud_rj.at(0) == KEY_V) generate_abs_event(ABS_RX, -32768);
		else if (m_ud_rj.at(0) == KEY_N) generate_abs_event(ABS_RX, 32767);
	} else {
		generate_abs_event(ABS_RX, 0);
	}
	
	if (code == KEY_COMMA) generate_key_event(BTN_THUMBL, value);
	if (code == KEY_DOT) generate_key_event(BTN_THUMBR, value);

	if (code == KEY_8) generate_key_event(BTN_SELECT, value);
	if (code == KEY_9) generate_key_event(BTN_MODE, value);
	if (code == KEY_0) generate_key_event(BTN_START, value);

	if (code == KEY_UP) generate_key_event(BTN_DPAD_UP, value);
	if (code == KEY_RIGHT) generate_key_event(BTN_DPAD_RIGHT, value);
	if (code == KEY_DOWN) generate_key_event(BTN_DPAD_DOWN, value);
	if (code == KEY_LEFT) generate_key_event(BTN_DPAD_LEFT, value);

	if (code == KEY_U) generate_key_event(BTN_TL, value);
	if (code == KEY_O) generate_key_event(BTN_TR, value);
	if (code == KEY_Q) generate_key_event(BTN_TL2, value);
	if (code == KEY_E) generate_key_event(BTN_TR2, value);

	if (code == KEY_I) generate_key_event(BTN_WEST, value);
	if (code == KEY_J) generate_key_event(BTN_NORTH, value);
	if (code == KEY_K) generate_key_event(BTN_SOUTH, value);
	if (code == KEY_L) generate_key_event(BTN_EAST, value);
}