#include "home.hpp"
#include <remapper/remapper.hpp>
#include <cstdlib>
#include <fcntl.h>
#include <dirent.h>
#include <csignal>
#include <QApplication>
#include <QWidget>
#include <QHBoxLayout>
#include <QMenuBar>
#include <QFormLayout>
#include <QLabel>
#include <libevdev/libevdev.h>

int is_event_device(const struct dirent *dev) {
	return strncmp("event", dev->d_name, 5) == 0;
}

Home::Home() {
	setWindowTitle("Evsieve Wrapper");

	tray = new QSystemTrayIcon(QIcon::fromTheme("application-system"), this);
	QMenu *tray_menu = new QMenu();
	QAction *tray_show = tray_menu->addAction("Show");
	connect(tray_show, &QAction::triggered, this, &Home::show);
	
	QAction *tray_quit = tray_menu->addAction("Quit");
	connect(tray_quit, &QAction::triggered, this, &QApplication::quit);

	tray->setContextMenu(tray_menu);
	tray->show();

	file_menu = menuBar()->addMenu("&File");

	QAction *quit_action = file_menu->addAction("Quit");
	connect(quit_action, &QAction::triggered, this, &QApplication::quit);

	devices_menu = menuBar()->addMenu("&Devices");
	connect(devices_menu, &QMenu::aboutToShow, this, &Home::reload_devices);

	QWidget *w = new QWidget();
	setCentralWidget(w);

	QVBoxLayout *layout = new QVBoxLayout(w);

	QFormLayout *top_layout = new QFormLayout();
	layout->addLayout(top_layout);

	QLabel *label1 = new QLabel("Label Text 1");
	QPushButton *btn1 = new QPushButton("TEST1");
	connect(btn1, &QPushButton::clicked, this, &Home::assign_key);
	QHBoxLayout *lyt1 = new QHBoxLayout();
	lyt1->addWidget(label1);
	lyt1->addWidget(btn1);
	top_layout->addRow(lyt1);

	QLabel *label2 = new QLabel("Label Text 2");
	QPushButton *btn2 = new QPushButton("TEST2");
	QHBoxLayout *lyt2 = new QHBoxLayout();
	lyt2->addWidget(label2);
	lyt2->addWidget(btn2);
	top_layout->addRow(lyt2);

	QHBoxLayout *bottom_layout = new QHBoxLayout();
	layout->addLayout(bottom_layout);

	start_button = new QPushButton("Start");
	bottom_layout->addWidget(start_button);
	connect(start_button, &QPushButton::clicked, this, &Home::start);

	stop_button = new QPushButton("Stop");
	stop_button->setDisabled(true);
	bottom_layout->addWidget(stop_button);
	connect(stop_button, &QPushButton::clicked, this, &Home::stop);
}

void Home::reload_devices() {

	devices_menu->clear();

	struct dirent **devices;
	int num_devs = scandir("/dev/input", &devices, is_event_device, versionsort);

	if (num_devs <= 0) {devices_menu->addAction("No devices available")->setDisabled(true);}
	else {
		for (int i = 0; i < num_devs; i++) {

			struct libevdev *dev = nullptr;
			std::string event_path = "/dev/input/" + (std::string)devices[i]->d_name;
			int fd = ::open(event_path.c_str(), O_RDONLY | O_NONBLOCK);

			if (libevdev_new_from_fd(fd, &dev) == 0) {
				std::string name = libevdev_get_name(dev);
				std::string action_name = name + '\t' + event_path;

				QAction *tmp_action = devices_menu->addAction(action_name.c_str());
				connect(tmp_action, &QAction::triggered, this, [this, event_path](bool checked){selected_device = QString::fromStdString(event_path);});
			}

			libevdev_free(dev);
			::close(fd);
		}
	}
}

void Home::assign_key() {
	qDebug() << selected_device;
}

void Home::start() {
	QString HOME = getenv("HOME");
	QString NAME = "EVSIEVE-WRAPPER-DEVICE";
	QString COMMAND = "--input grab " + selected_device + " --map key:k btn:south --map key:l btn:east --map key:j btn:north --map key:i btn:west --map key:space btn:tr --map key:u btn:tl --map key:e btn:tr2 --map key:q btn:tl2 --map key:0 btn:start --map key:9 btn:select --map key:up btn:dpad_up --map key:down btn:dpad_down --map key:left btn:dpad_left --map key:right btn:dpad_right --map key:w:1 abs:y:-32768 --map key:s:1 abs:y:32767 --map key:w:0 abs:y:0 --map key:s:0 abs:y:0 --map key:a:1 abs:x:-32768 --map key:d:1 abs:x:32767 --map key:a:0 abs:x:0 --map key:d:0 abs:x:0 --output create-link=" + HOME + "/" + NAME + " name=" + NAME + " repeat=disable btn:south btn:east btn:west btn:north btn:tl btn:tr btn:tl2 btn:tr2 btn:start btn:select btn:dpad_up btn:dpad_down btn:dpad_left btn:dpad_right abs:y abs:x --output";
	QStringList ARGUMENTS = COMMAND.split(' ');

	proc = new QProcess(this);
	proc->start("evsieve", ARGUMENTS);
	start_button->setDisabled(true);
	stop_button->setDisabled(false);
}

void Home::stop() {
	proc->terminate();
	start_button->setDisabled(false);
	stop_button->setDisabled(true);
}

void Home::closeEvent(QCloseEvent *event) {
	hide();
	event->ignore();
}