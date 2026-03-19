#include "label-button.hpp"

LabelButton::LabelButton(QString label_text, QString button_text, QWidget* parent) : QWidget(parent) {
	label = new QLabel(label_text);
	label->setAlignment(Qt::AlignCenter);
	label->setMinimumWidth(100);

	button = new QPushButton(button_text);
	button->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
	
	layout = new QHBoxLayout(this);
	layout->setContentsMargins(0, 0, 0, 0);
	layout->addWidget(label);
	layout->addWidget(button);
}