#pragma once

#include <atomic>
#include <QThread>

class InputListener : public QThread {
	Q_OBJECT

	public:
		InputListener(const std::string& device_path, QObject* parent = nullptr);
		void stop();

	protected:
		void run() override;

	signals:
		void send_detected_keys(QList<QString> keys);

	private:
		std::string m_device_path;
		std::atomic<bool> m_running_flag = false;
		QList<QString> m_keys;
};