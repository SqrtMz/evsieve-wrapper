#pragma once

#include <QWidget>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>

class LabelButton : public QWidget {
	Q_OBJECT

	public:
		LabelButton(QString label_text, QString button_text, QWidget* parent = nullptr);

		QLabel* label;
		QPushButton* button;
		QHBoxLayout* layout;
		QList<int> key_code = QList<int>() << 0;
};