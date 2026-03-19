#include "home.hpp"
#include "core/remapper.hpp"
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

int is_event_device(const struct dirent* dev) {
	return strncmp("event", dev->d_name, 5) == 0;
}

Home::Home(QWidget* parent) : QMainWindow(parent) {
	setWindowTitle("Evsieve Wrapper");

	m_tray = new QSystemTrayIcon(QIcon::fromTheme("application-system"), this);
	QMenu* tray_menu = new QMenu(this);

	QAction* tray_show = tray_menu->addAction("Show");
	connect(tray_show, &QAction::triggered, this, &Home::show);
	
	QAction* tray_quit = tray_menu->addAction("Quit");
	connect(tray_quit, &QAction::triggered, this, &QApplication::quit);

	m_tray->setContextMenu(tray_menu);
	m_tray->show();

	m_file_menu = menuBar()->addMenu("&File");

	QAction* quit_action = m_file_menu->addAction("Quit");
	connect(quit_action, &QAction::triggered, this, &QApplication::quit);

	m_devices_menu = menuBar()->addMenu("&Devices");
	connect(m_devices_menu, &QMenu::aboutToShow, this, &Home::reload_devices);

	QWidget* w = new QWidget(this);
	setCentralWidget(w);

	QVBoxLayout* layout = new QVBoxLayout(w);

	QFormLayout* top_layout = new QFormLayout();
	layout->addLayout(top_layout);

	
	LabelButton* btn_a = new LabelButton("A", "No key assigned");
	top_layout->addRow(btn_a);
	connect(btn_a->button, &QPushButton::clicked, this, [this, btn_a](){start_key_listener(btn_a);});
	
	LabelButton* btn_b = new LabelButton("B", "No key assigned");
	top_layout->addRow(btn_b);
	connect(btn_b->button, &QPushButton::clicked, this, [this, btn_b](){start_key_listener(btn_b);});

	QHBoxLayout* bottom_layout = new QHBoxLayout();
	layout->addLayout(bottom_layout);

	m_start_button = new QPushButton("Start");
	bottom_layout->addWidget(m_start_button);
	connect(m_start_button, &QPushButton::clicked, this, &Home::start);

	m_stop_button = new QPushButton("Stop");
	m_stop_button->setDisabled(true);
	bottom_layout->addWidget(m_stop_button);
	connect(m_stop_button, &QPushButton::clicked, this, &Home::stop);
}

void Home::reload_devices() {

	m_devices_menu->clear();

	struct dirent** devices;
	int num_devs = scandir("/dev/input", &devices, is_event_device, versionsort);

	if (num_devs <= 0) {m_devices_menu->addAction("No devices available")->setDisabled(true);}
	else {
		for (int i = 0; i < num_devs; i++) {

			struct libevdev* dev = nullptr;
			std::string event_path = "/dev/input/" + (std::string)devices[i]->d_name;
			int fd = ::open(event_path.c_str(), O_RDONLY | O_NONBLOCK);

			if (libevdev_new_from_fd(fd, &dev) == 0) {
				std::string name = libevdev_get_name(dev);
				std::string action_name = name + '\t' + event_path;

				QAction* tmp_action = m_devices_menu->addAction(action_name.c_str());
				connect(tmp_action, &QAction::triggered, this, [this, event_path](bool checked){m_selected_device = event_path;});
			}

			libevdev_free(dev);
			::close(fd);
		}
	}
}

void Home::start_key_listener(LabelButton* lb) {

	if (m_selected_device == "") return;

	if (m_input_listener) {
		m_input_listener->stop();
		m_input_listener = nullptr;
	}

	lb->button->setText("Waiting for key...");
	m_selected_label_button = lb;

	m_input_listener = new InputListener(m_selected_device, this);

	connect(m_input_listener, &InputListener::send_detected_keys, this, &Home::recieve_detected_keys);
	connect(m_input_listener, &QThread::finished, m_input_listener, &QThread::deleteLater);

	m_input_listener->start();
}

void Home::recieve_detected_keys(QList<QString> keys) {
	
	if (m_selected_label_button) {

		QString text;

		for (QString k : keys) {
			text.append(k);
			text.append(" + ");
		}
		
		text.chop(3);

		m_selected_label_button->button->setText(text);
		m_selected_label_button = nullptr;
		m_input_listener = nullptr;
	}
}

void Home::start() {
	std::string HOME = getenv("HOME");
	std::string NAME = "EVSIEVE-WRAPPER-DEVICE";
	std::string COMMAND = "--input grab " + m_selected_device + " --map key:k btn:south --map key:l btn:east --map key:j btn:north --map key:i btn:west --map key:space btn:tr --map key:u btn:tl --map key:e btn:tr2 --map key:q btn:tl2 --map key:0 btn:start --map key:9 btn:select --map key:up btn:dpad_up --map key:down btn:dpad_down --map key:left btn:dpad_left --map key:right btn:dpad_right --map key:w:1 abs:y:-32768 --map key:s:1 abs:y:32767 --map key:w:0 abs:y:0 --map key:s:0 abs:y:0 --map key:a:1 abs:x:-32768 --map key:d:1 abs:x:32767 --map key:a:0 abs:x:0 --map key:d:0 abs:x:0 --output create-link=" + HOME + "/" + NAME + " name=" + NAME + " repeat=disable btn:south btn:east btn:west btn:north btn:tl btn:tr btn:tl2 btn:tr2 btn:start btn:select btn:dpad_up btn:dpad_down btn:dpad_left btn:dpad_right abs:y abs:x --output";
	QStringList ARGUMENTS = QString::fromStdString(COMMAND).split(' ');

	m_proc = new QProcess(this);
	m_proc->start("evsieve", ARGUMENTS);
	m_start_button->setDisabled(true);
	m_stop_button->setDisabled(false);
}

void Home::stop() {
	m_proc->terminate();
	m_start_button->setDisabled(false);
	m_stop_button->setDisabled(true);
}

void Home::closeEvent(QCloseEvent* event) {
	hide();
	event->ignore();
}