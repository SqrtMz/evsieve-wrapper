#include "input-listener.hpp"
#include <iostream>
#include <fcntl.h>
#include <libevdev/libevdev.h>

InputListener::InputListener(const std::string& device_path, QObject* parent) : QThread(parent), m_device_path(device_path) {}

void InputListener::run() {

	if (m_device_path == "") return;

	int fd = ::open(m_device_path.c_str(), O_RDONLY | O_NONBLOCK);
	if (fd < 0) return;
	
	struct libevdev *dev = nullptr;
	int rc = libevdev_new_from_fd(fd, &dev);
	if (rc < 0) {
		::close(fd);
		return;
	}

	libevdev_grab(dev, LIBEVDEV_GRAB);
	m_is_listening = true;
	do {
		struct input_event ev;
		rc = libevdev_next_event(dev, LIBEVDEV_READ_FLAG_NORMAL, &ev);

		if (rc == 0) {
			if (ev.type == 1 && ev.value == 1) {
				m_keys.append(libevdev_event_code_get_name(ev.type, ev.code));
			}
			
			if (ev.type == 1 && ev.value == 0) {
				emit send_detected_keys(m_keys);
				m_keys.clear();
				this->stop();
				break;
			}
		}

		QThread::msleep(10);
	} while (m_is_listening);

	libevdev_grab(dev, LIBEVDEV_UNGRAB);
	libevdev_free(dev);
	::close(fd);
}

void InputListener::stop() {
	m_is_listening = false;
}