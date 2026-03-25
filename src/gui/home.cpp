#include "home.hpp"
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

	LabelButton* btn_west = new LabelButton("Y", "No key assigned");
	top_layout->addRow(btn_west);
	connect(btn_west->button, &QPushButton::clicked, this, [this, btn_west](){start_key_listener(btn_west);});
	m_events.btn_west = &btn_west->key_code;
	
	LabelButton* btn_east = new LabelButton("B", "No key assigned");
	top_layout->addRow(btn_east);
	connect(btn_east->button, &QPushButton::clicked, this, [this, btn_east](){start_key_listener(btn_east);});
	m_events.btn_east = &btn_east->key_code;
	
	LabelButton* btn_south = new LabelButton("A", "No key assigned");
	top_layout->addRow(btn_south);
	connect(btn_south->button, &QPushButton::clicked, this, [this, btn_south](){start_key_listener(btn_south);});
	m_events.btn_south = &btn_south->key_code;

	LabelButton* btn_north = new LabelButton("X", "No key assigned");
	top_layout->addRow(btn_north);
	connect(btn_north->button, &QPushButton::clicked, this, [this, btn_north](){start_key_listener(btn_north);});
	m_events.btn_north = &btn_north->key_code;
	
	LabelButton* btn_dpad_up = new LabelButton("Dpad Up", "No key assigned");
	top_layout->addRow(btn_dpad_up);
	connect(btn_dpad_up->button, &QPushButton::clicked, this, [this, btn_dpad_up](){start_key_listener(btn_dpad_up);});
	m_events.btn_dpad_up = &btn_dpad_up->key_code;

	LabelButton* btn_dpad_right = new LabelButton("Dpad Right", "No key assigned");
	top_layout->addRow(btn_dpad_right);
	connect(btn_dpad_right->button, &QPushButton::clicked, this, [this, btn_dpad_right](){start_key_listener(btn_dpad_right);});
	m_events.btn_dpad_right = &btn_dpad_right->key_code;

	LabelButton* btn_dpad_down = new LabelButton("Dpad Down", "No key assigned");
	top_layout->addRow(btn_dpad_down);
	connect(btn_dpad_down->button, &QPushButton::clicked, this, [this, btn_dpad_down](){start_key_listener(btn_dpad_down);});
	m_events.btn_dpad_down = &btn_dpad_down->key_code;

	LabelButton* btn_dpad_left = new LabelButton("Dpad Left", "No key assigned");
	top_layout->addRow(btn_dpad_left);
	connect(btn_dpad_left->button, &QPushButton::clicked, this, [this, btn_dpad_left](){start_key_listener(btn_dpad_left);});
	m_events.btn_dpad_left = &btn_dpad_left->key_code;

	LabelButton* btn_start = new LabelButton("Start", "No key assigned");
	top_layout->addRow(btn_start);
	connect(btn_start->button, &QPushButton::clicked, this, [this, btn_start](){start_key_listener(btn_start);});
	m_events.btn_start = &btn_start->key_code;

	LabelButton* btn_mode = new LabelButton("Brand Button", "No key assigned");
	top_layout->addRow(btn_mode);
	connect(btn_mode->button, &QPushButton::clicked, this, [this, btn_mode](){start_key_listener(btn_mode);});
	m_events.btn_mode = &btn_mode->key_code;

	LabelButton* btn_select = new LabelButton("Select", "No key assigned");
	top_layout->addRow(btn_select);
	connect(btn_select->button, &QPushButton::clicked, this, [this, btn_select](){start_key_listener(btn_select);});
	m_events.btn_select = &btn_select->key_code;

	LabelButton* btn_tl = new LabelButton("LB", "No key assigned");
	top_layout->addRow(btn_tl);
	connect(btn_tl->button, &QPushButton::clicked, this, [this, btn_tl](){start_key_listener(btn_tl);});
	m_events.btn_tl = &btn_tl->key_code;

	LabelButton* btn_tr = new LabelButton("RB", "No key assigned");
	top_layout->addRow(btn_tr);
	connect(btn_tr->button, &QPushButton::clicked, this, [this, btn_tr](){start_key_listener(btn_tr);});
	m_events.btn_tr = &btn_tr->key_code;

	LabelButton* btn_tl2 = new LabelButton("LT", "No key assigned");
	top_layout->addRow(btn_tl2);
	connect(btn_tl2->button, &QPushButton::clicked, this, [this, btn_tl2](){start_key_listener(btn_tl2);});
	m_events.btn_tl2 = &btn_tl2->key_code;

	LabelButton* btn_tr2 = new LabelButton("RT", "No key assigned");
	top_layout->addRow(btn_tr2);
	connect(btn_tr2->button, &QPushButton::clicked, this, [this, btn_tr2](){start_key_listener(btn_tr2);});
	m_events.btn_tr2 = &btn_tr2->key_code;

	LabelButton* btn_ljoy_up = new LabelButton("L-Joystick Up", "No key assigned");
	top_layout->addRow(btn_ljoy_up);
	connect(btn_ljoy_up->button, &QPushButton::clicked, this, [this, btn_ljoy_up](){start_key_listener(btn_ljoy_up);});
	m_events.abs_ljoy_up = &btn_ljoy_up->key_code;

	LabelButton* btn_ljoy_right = new LabelButton("L-Joystick Right", "No key assigned");
	top_layout->addRow(btn_ljoy_right);
	connect(btn_ljoy_right->button, &QPushButton::clicked, this, [this, btn_ljoy_right](){start_key_listener(btn_ljoy_right);});
	m_events.abs_ljoy_right = &btn_ljoy_right->key_code;

	LabelButton* btn_ljoy_down = new LabelButton("L-Joystick Down", "No key assigned");
	top_layout->addRow(btn_ljoy_down);
	connect(btn_ljoy_down->button, &QPushButton::clicked, this, [this, btn_ljoy_down](){start_key_listener(btn_ljoy_down);});
	m_events.abs_ljoy_down = &btn_ljoy_down->key_code;

	LabelButton* btn_ljoy_left = new LabelButton("L-Joystick Left", "No key assigned");
	top_layout->addRow(btn_ljoy_left);
	connect(btn_ljoy_left->button, &QPushButton::clicked, this, [this, btn_ljoy_left](){start_key_listener(btn_ljoy_left);});
	m_events.abs_ljoy_left = &btn_ljoy_left->key_code;

	LabelButton* btn_lthumb = new LabelButton("L-Joystick Button", "No key assigned");
	top_layout->addRow(btn_lthumb);
	connect(btn_lthumb->button, &QPushButton::clicked, this, [this, btn_lthumb](){start_key_listener(btn_lthumb);});
	m_events.btn_thumbl = &btn_lthumb->key_code;

	LabelButton* btn_rjoy_up = new LabelButton("R-Joystick Up", "No key assigned");
	top_layout->addRow(btn_rjoy_up);
	connect(btn_rjoy_up->button, &QPushButton::clicked, this, [this, btn_rjoy_up](){start_key_listener(btn_rjoy_up);});
	m_events.abs_rjoy_up = &btn_rjoy_up->key_code;

	LabelButton* btn_rjoy_right = new LabelButton("R-Joystick Right", "No key assigned");
	top_layout->addRow(btn_rjoy_right);
	connect(btn_rjoy_right->button, &QPushButton::clicked, this, [this, btn_rjoy_right](){start_key_listener(btn_rjoy_right);});
	m_events.abs_rjoy_right = &btn_rjoy_right->key_code;

	LabelButton* btn_rjoy_down = new LabelButton("R-Joystick Down", "No key assigned");
	top_layout->addRow(btn_rjoy_down);
	connect(btn_rjoy_down->button, &QPushButton::clicked, this, [this, btn_rjoy_down](){start_key_listener(btn_rjoy_down);});
	m_events.abs_rjoy_down = &btn_rjoy_down->key_code;

	LabelButton* btn_rjoy_left = new LabelButton("R-Joystick Left", "No key assigned");
	top_layout->addRow(btn_rjoy_left);
	connect(btn_rjoy_left->button, &QPushButton::clicked, this, [this, btn_rjoy_left](){start_key_listener(btn_rjoy_left);});
	m_events.abs_rjoy_left = &btn_rjoy_left->key_code;

	LabelButton* btn_rthumb = new LabelButton("R-Joystick Button", "No key assigned");
	top_layout->addRow(btn_rthumb);
	connect(btn_rthumb->button, &QPushButton::clicked, this, [this, btn_rthumb](){start_key_listener(btn_rthumb);});
	m_events.btn_thumbr = &btn_rthumb->key_code;

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

			if (fd < 0) return;

			int rc = libevdev_new_from_fd(fd, &dev);
			if (rc < 0) {
				::close(fd);
				return;
			}

			if (rc == 0) {
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
	m_input_listener_exists = true;

	connect(m_input_listener, &InputListener::send_detected_keys, this, &Home::recieve_detected_keys);
	connect(m_input_listener, &QThread::finished, m_input_listener, &QThread::deleteLater);

	m_input_listener->start();
}

void Home::recieve_detected_keys(QList<int> keys) {
	
	if (m_selected_label_button) {

		m_selected_label_button->key_code = QList<int>() << 0;

		if (keys.size() < 1) {
			m_selected_label_button->button->setText("No key assigned");
			return;
		}

		QString text;

		for (int k : keys) {
			text.append(libevdev_event_code_get_name(EV_KEY, k));
			text.append(" + ");
		}
		text.chop(3);

		m_selected_label_button->button->setText(text);
		m_selected_label_button->key_code = keys;

		m_selected_label_button = nullptr;
		m_input_listener = nullptr;
		m_input_listener_exists = false;
		text.clear();
	}
}

void Home::start() {

	m_virtual_controller_dev = new VirtualController(m_selected_device, m_events, m_input_listener_exists, this);

	m_virtual_controller_dev->start();
	m_start_button->setDisabled(true);
	m_stop_button->setDisabled(false);
}

void Home::stop() {

	if (m_virtual_controller_dev) {
		m_virtual_controller_dev->stop();
		delete m_virtual_controller_dev;
		m_virtual_controller_dev = nullptr;

		m_start_button->setDisabled(false);
		m_stop_button->setDisabled(true);
	}
}

void Home::closeEvent(QCloseEvent* event) {
	hide();
	event->ignore();
}