#pragma once

struct controller_events {
	QList<int>* btn_west = nullptr;
	QList<int>* btn_east = nullptr;
	QList<int>* btn_south = nullptr;
	QList<int>* btn_north = nullptr;
	QList<int>* btn_dpad_up = nullptr;
	QList<int>* btn_dpad_right = nullptr;
	QList<int>* btn_dpad_down = nullptr;
	QList<int>* btn_dpad_left = nullptr;
	QList<int>* btn_start = nullptr;
	QList<int>* btn_mode = nullptr;
	QList<int>* btn_select = nullptr;
	QList<int>* btn_tl = nullptr;
	QList<int>* btn_tr = nullptr;
	QList<int>* btn_tl2 = nullptr;
	QList<int>* btn_tr2 = nullptr;
	QList<int>* abs_ljoy_up = nullptr;
	QList<int>* abs_ljoy_right = nullptr;
	QList<int>* abs_ljoy_down = nullptr;
	QList<int>* abs_ljoy_left = nullptr;
	QList<int>* btn_thumbl = nullptr;
	QList<int>* abs_rjoy_up = nullptr;
	QList<int>* abs_rjoy_right = nullptr;
	QList<int>* abs_rjoy_down = nullptr;
	QList<int>* abs_rjoy_left = nullptr;
	QList<int>* btn_thumbr = nullptr;
};