#pragma once

#include "label-button.hpp"
#include "core/input-listener.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <QMainWindow>
#include <QString>
#include <QProcess>
#include <QPushButton>
#include <QSystemTrayIcon>
#include <QKeyEvent>
#include <QCloseEvent>

class Home : public QMainWindow {
	Q_OBJECT

	public:
		Home(QWidget* parent = nullptr);
		
		protected:
		void closeEvent(QCloseEvent* event) override;
		
		private:
		QSystemTrayIcon *m_tray;
		std::string m_selected_device;
		QMenu* m_file_menu;
		QMenu* m_devices_menu;
		QProcess* m_proc;
		QPushButton* m_start_button;
		QPushButton* m_stop_button;
		InputListener* m_input_listener = nullptr;
		LabelButton* m_selected_label_button = nullptr;
		QList<LabelButton*> m_used_label_buttons;
		
	private slots:
		void start_key_listener (LabelButton* lb);
		void recieve_detected_keys(QList<QString> keys);
		void reload_devices();
		void start();
		void stop();
};