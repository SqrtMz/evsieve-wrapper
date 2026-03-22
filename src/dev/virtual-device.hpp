#pragma once

#include <set>
#include <atomic>
#include <libevdev/libevdev-uinput.h>
#include <QDebug>
#include <QThread>

class VirtualDeviceWorker : public QObject {
	Q_OBJECT

	public:
		VirtualDeviceWorker(const std::string& device_path, QObject* parent = nullptr);

	public slots:
		void start();
		void stop();
		
	private:
		std::string m_device_path;
		std::atomic<bool> m_is_listening;

	signals:
		void send_key_events(int code, int value);
		void finished();
};


class VirtualDevice : public QObject {
	Q_OBJECT

	public:
		VirtualDevice(const std::string& device_path, QObject* parent = nullptr);
		virtual ~VirtualDevice();

		void start();
		void stop();

	signals:
		void started();
		void stopped();
	
	protected:
		struct libevdev_uinput* m_uinput = nullptr;
		std::string m_device_path;
		QThread* m_worker_thread;
		VirtualDeviceWorker* m_vd_worker;

		QList<int> m_lr_lj;	// m_left_right_left_joystick
		QList<int> m_ud_lj;	// m_up_down_left_joystick
		QList<int> m_lr_rj;	// m_left_right_right_joystick
		QList<int> m_ud_rj;	// m_up_down_right_joystick

		virtual void generate_key_event(int code, int value) {};
		virtual void generate_abs_event(int axis, int value) {};
		virtual void generate_rel_event(int code, int value) {};
		
	protected slots:
		virtual void handle_key_events(int code, int value) {};
};