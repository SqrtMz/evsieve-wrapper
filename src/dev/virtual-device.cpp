#include "virtual-device.hpp"
#include <fcntl.h>

VirtualDeviceWorker::VirtualDeviceWorker (const std::string& device_path, QObject* parent) : QObject(parent), m_device_path(device_path) {}

void VirtualDeviceWorker::start() {

	if (m_device_path == "") return;

	int fd = ::open(m_device_path.c_str(), O_RDONLY | O_NONBLOCK);
	if (fd < 0) return;
	
	struct libevdev *dev = nullptr;
	int rc = libevdev_new_from_fd(fd, &dev);
	if (rc < 0) {
		::close(fd);
		return;
	}

	// libevdev_grab(dev, LIBEVDEV_GRAB);
	m_is_listening = true;

	do {
		struct input_event ev;
		rc = libevdev_next_event(dev, LIBEVDEV_READ_FLAG_NORMAL, &ev);

		if (rc == 0 && ev.type == 1 && ev.value != 2) emit send_key_events(ev.code, ev.value);

		QThread::msleep(10);
	} while (m_is_listening);

	// libevdev_grab(dev, LIBEVDEV_UNGRAB);
	libevdev_free(dev);
	::close(fd);

	emit finished();
}

void VirtualDeviceWorker::stop() {
	m_is_listening = false;
}


VirtualDevice::VirtualDevice(const std::string& device_path, QObject* parent) : QObject(parent), m_device_path(device_path) {

	m_worker_thread = new QThread(this);
	m_vd_worker = new VirtualDeviceWorker(device_path);

	m_vd_worker->moveToThread(m_worker_thread);

	connect(m_worker_thread, &QThread::started, m_vd_worker, &VirtualDeviceWorker::start);
	
	connect(m_vd_worker, &VirtualDeviceWorker::send_key_events, this, &VirtualDevice::handle_key_events);

	connect(m_vd_worker, &VirtualDeviceWorker::finished, m_worker_thread, &QThread::quit);
	connect(m_vd_worker, &VirtualDeviceWorker::finished, m_vd_worker, &QObject::deleteLater);
	connect(m_worker_thread, &QThread::finished, m_worker_thread, &QObject::deleteLater);
}

VirtualDevice::~VirtualDevice() {libevdev_uinput_destroy(m_uinput);}

void VirtualDevice::start() {m_worker_thread->start();}

void VirtualDevice::stop() {

	if (m_vd_worker) m_vd_worker->stop();

	if (m_worker_thread) {
		if (m_worker_thread->isRunning()) {
			m_worker_thread->quit();
			m_worker_thread->wait();
		}
	}
}