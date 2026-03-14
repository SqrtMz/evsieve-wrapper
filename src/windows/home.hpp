#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <QMainWindow>
#include <QString>
#include <QProcess>
#include <QPushButton>
#include <QSystemTrayIcon>
#include <QCloseEvent>

class Home : public QMainWindow {

	Q_OBJECT

	public:
		Home();
		QSystemTrayIcon *tray;
		
	protected:
		void closeEvent(QCloseEvent *event) override;
		
	private:
		QString selected_device;
		QMenu *file_menu;
		QMenu *devices_menu;
		QProcess *proc;
		QPushButton *start_button;
		QPushButton *stop_button;
	
	private slots:
		void assign_key ();
		void reload_devices();
		void start();
		void stop();
};