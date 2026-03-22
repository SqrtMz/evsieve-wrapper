#pragma once

#include <QWidget>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>

class LabelButton : public QWidget {
	Q_OBJECT

	public:
		LabelButton(QString label_text, QString button_text, QString target, QWidget* parent = nullptr);

		QLabel* label;
		QPushButton* button;
		QHBoxLayout* layout;
		QString target;
		QList<QString> key_code;
};