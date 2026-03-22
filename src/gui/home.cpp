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

	
	LabelButton* btn_a = new LabelButton("A", "No key assigned", "south");
	top_layout->addRow(btn_a);
	connect(btn_a->button, &QPushButton::clicked, this, [this, btn_a](){start_key_listener(btn_a);});
	
	LabelButton* btn_b = new LabelButton("B", "No key assigned", "east");
	top_layout->addRow(btn_b);
	connect(btn_b->button, &QPushButton::clicked, this, [this, btn_b](){start_key_listener(btn_b);});

	LabelButton* btn_x = new LabelButton("X", "No key assigned", "north");
	top_layout->addRow(btn_x);
	connect(btn_x->button, &QPushButton::clicked, this, [this, btn_x](){start_key_listener(btn_x);});

	LabelButton* btn_y = new LabelButton("Y", "No key assigned", "west");
	top_layout->addRow(btn_y);
	connect(btn_y->button, &QPushButton::clicked, this, [this, btn_y](){start_key_listener(btn_y);});

	LabelButton* btn_dpad_up = new LabelButton("Dpad Up", "No key assigned", "dpad_up");
	top_layout->addRow(btn_dpad_up);
	connect(btn_dpad_up->button, &QPushButton::clicked, this, [this, btn_dpad_up](){start_key_listener(btn_dpad_up);});

	LabelButton* btn_dpad_right = new LabelButton("Dpad Right", "No key assigned", "dpad_right");
	top_layout->addRow(btn_dpad_right);
	connect(btn_dpad_right->button, &QPushButton::clicked, this, [this, btn_dpad_right](){start_key_listener(btn_dpad_right);});

	LabelButton* btn_dpad_down= new LabelButton("Dpad Down", "No key assigned", "dpad_down");
	top_layout->addRow(btn_dpad_down);
	connect(btn_dpad_down->button, &QPushButton::clicked, this, [this, btn_dpad_down](){start_key_listener(btn_dpad_down);});

	LabelButton* btn_dpad_left = new LabelButton("Dpad Left", "No key assigned", "dpad_left");
	top_layout->addRow(btn_dpad_left);
	connect(btn_dpad_left->button, &QPushButton::clicked, this, [this, btn_dpad_left](){start_key_listener(btn_dpad_left);});

	LabelButton* btn_start = new LabelButton("Start", "No key assigned", "start");
	top_layout->addRow(btn_start);
	connect(btn_start->button, &QPushButton::clicked, this, [this, btn_start](){start_key_listener(btn_start);});

	LabelButton* btn_mode = new LabelButton("Brand Button", "No key assigned", "mode");
	top_layout->addRow(btn_mode);
	connect(btn_mode->button, &QPushButton::clicked, this, [this, btn_mode](){start_key_listener(btn_mode);});

	LabelButton* btn_select = new LabelButton("Select", "No key assigned", "select");
	top_layout->addRow(btn_select);
	connect(btn_select->button, &QPushButton::clicked, this, [this, btn_select](){start_key_listener(btn_select);});

	LabelButton* btn_tr = new LabelButton("RB", "No key assigned", "tr");
	top_layout->addRow(btn_tr);
	connect(btn_tr->button, &QPushButton::clicked, this, [this, btn_tr](){start_key_listener(btn_tr);});

	LabelButton* btn_tl = new LabelButton("LB", "No key assigned", "tl");
	top_layout->addRow(btn_tl);
	connect(btn_tl->button, &QPushButton::clicked, this, [this, btn_tl](){start_key_listener(btn_tl);});

	LabelButton* btn_tr2 = new LabelButton("RT", "No key assigned", "tr2");
	top_layout->addRow(btn_tr2);
	connect(btn_tr2->button, &QPushButton::clicked, this, [this, btn_tr2](){start_key_listener(btn_tr2);});

	LabelButton* btn_tl2 = new LabelButton("LT", "No key assigned", "tl2");
	top_layout->addRow(btn_tl2);
	connect(btn_tl2->button, &QPushButton::clicked, this, [this, btn_tl2](){start_key_listener(btn_tl2);});

	LabelButton* btn_ljoystick_up = new LabelButton("L-Joystick Up", "No key assigned", "abs:y:-32768");
	top_layout->addRow(btn_ljoystick_up);
	connect(btn_ljoystick_up->button, &QPushButton::clicked, this, [this, btn_ljoystick_up](){start_key_listener(btn_ljoystick_up);});

	LabelButton* btn_ljoystick_right = new LabelButton("L-Joystick Right", "No key assigned", "abs:x:32767");
	top_layout->addRow(btn_ljoystick_right);
	connect(btn_ljoystick_right->button, &QPushButton::clicked, this, [this, btn_ljoystick_right](){start_key_listener(btn_ljoystick_right);});

	LabelButton* btn_ljoystick_down = new LabelButton("L-Joystick Down", "No key assigned", "abs:y:32767");
	top_layout->addRow(btn_ljoystick_down);
	connect(btn_ljoystick_down->button, &QPushButton::clicked, this, [this, btn_ljoystick_down](){start_key_listener(btn_ljoystick_down);});

	LabelButton* btn_ljoystick_left = new LabelButton("L-Joystick Left", "No key assigned", "abs:x:-32768");
	top_layout->addRow(btn_ljoystick_left);
	connect(btn_ljoystick_left->button, &QPushButton::clicked, this, [this, btn_ljoystick_left](){start_key_listener(btn_ljoystick_left);});

	LabelButton* btn_ljoystick_button = new LabelButton("L-Joystick Button", "No key assigned", "thumbl");
	top_layout->addRow(btn_ljoystick_button);
	connect(btn_ljoystick_button->button, &QPushButton::clicked, this, [this, btn_ljoystick_button](){start_key_listener(btn_ljoystick_button);});

	LabelButton* btn_rjoystick_up = new LabelButton("R-Joystick Up", "No key assigned", "abs:ry:-32768");
	top_layout->addRow(btn_rjoystick_up);
	connect(btn_rjoystick_up->button, &QPushButton::clicked, this, [this, btn_rjoystick_up](){start_key_listener(btn_rjoystick_up);});

	LabelButton* btn_rjoystick_right = new LabelButton("R-Joystick Right", "No key assigned", "abs:rx:32767");
	top_layout->addRow(btn_rjoystick_right);
	connect(btn_rjoystick_right->button, &QPushButton::clicked, this, [this, btn_rjoystick_right](){start_key_listener(btn_rjoystick_right);});

	LabelButton* btn_rjoystick_down = new LabelButton("R-Joystick Down", "No key assigned", "abs:ry:32767");
	top_layout->addRow(btn_rjoystick_down);
	connect(btn_rjoystick_down->button, &QPushButton::clicked, this, [this, btn_rjoystick_down](){start_key_listener(btn_rjoystick_down);});

	LabelButton* btn_rjoystick_left = new LabelButton("R-Joystick Left", "No key assigned", "abs:rx:-32768");
	top_layout->addRow(btn_rjoystick_left);
	connect(btn_rjoystick_left->button, &QPushButton::clicked, this, [this, btn_rjoystick_left](){start_key_listener(btn_rjoystick_left);});

	LabelButton* btn_rjoystick_button = new LabelButton("R-Joystick Button", "No key assigned", "thumbr");
	top_layout->addRow(btn_rjoystick_button);
	connect(btn_rjoystick_button->button, &QPushButton::clicked, this, [this, btn_rjoystick_button](){start_key_listener(btn_rjoystick_button);});

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
	if (!m_used_label_buttons.contains(lb)) m_used_label_buttons.append(lb);

	m_input_listener = new InputListener(m_selected_device, this);

	connect(m_input_listener, &InputListener::send_detected_keys, this, &Home::recieve_detected_keys);
	connect(m_input_listener, &QThread::finished, m_input_listener, &QThread::deleteLater);

	m_input_listener->start();
}

void Home::recieve_detected_keys(QList<QString> keys) {
	
	if (m_selected_label_button) {

		m_selected_label_button->key_code.clear();

		if (keys.size() < 1) {
			m_selected_label_button->button->setText("No key assigned");
			return;
		}

		QString text;

		for (QString k : keys) {
			text.append(k);
			text.append(" + ");
		}
		text.chop(3);

		if (m_selected_label_button->target.left(3) == "abs") m_selected_label_button->button->setText(keys.at(0));
		else m_selected_label_button->button->setText(text);

		m_selected_label_button->key_code = keys;

		m_selected_label_button = nullptr;
		m_input_listener = nullptr;
		text.clear();
	}
}

void Home::start() {

	// Remapper r;
	// QStringList arguments = QString("--input %1 %2 --output create-link=%3/EVSIEVE-WRAPPER-DEVICE name=EVSIEVE-WRAPPER-DEVICE repeat=disable btn:south btn:east btn:west btn:north btn:tl btn:tr btn:tl2 btn:tr2 btn:start btn:select btn:dpad_up btn:dpad_down btn:dpad_left btn:dpad_right abs:y abs:x abs:ry abs:rx btn:thumbl btn:thumbr btn:mode --output").arg(m_selected_device).arg(r.generate_arguments(m_used_label_buttons)).arg((getenv("HOME"))).split(' ');

	// m_proc = new QProcess(this);
	// m_proc->start("evsieve", arguments);

	m_vc_dev = new VirtualController(m_selected_device, this);

	m_vc_dev->start();
	m_start_button->setDisabled(true);
	m_stop_button->setDisabled(false);
}

void Home::stop() {
	// m_proc->terminate();

	if (m_vc_dev) {
		m_vc_dev->stop();
		delete m_vc_dev;
		m_vc_dev = nullptr;

		m_start_button->setDisabled(false);
		m_stop_button->setDisabled(true);
	}
}

void Home::closeEvent(QCloseEvent* event) {
	hide();
	event->ignore();
}