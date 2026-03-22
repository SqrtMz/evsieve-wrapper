#pragma once

#include "gui/label-button.hpp"
#include <iostream>
#include <string>
#include <QString>
#include <QList>

class Remapper {
	public:
		Remapper();
		QString generate_arguments(QList<LabelButton*> lbs);
};