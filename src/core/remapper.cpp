#include "remapper.hpp"
#include <QDebug>

Remapper::Remapper() {}

QString Remapper::generate_arguments(QList<LabelButton*> lbs) {

	QString command = "";

	for (LabelButton* lb : lbs) {

		if (lb->target.left(3) == "abs") {

			QString joystick = lb->target.split(':').at(1);

			command += "--map key:" + lb->key_code.at(0).mid(4).toLower() + ":1" + ' ' + lb->target + ' ';
			command += "--map key:" + lb->key_code.at(0).mid(4).toLower() + ":0 abs:" + joystick + ":0 ";

		} else {

			if (lb->key_code.size() > 1) {
				command += "--hook ";

				for (QString k : lb->key_code) {
					command += "key:" + k.mid(4).toLower() + ' ';
				}

				command += "send-key=btn:" + lb->target + ' ';

			} else {

				command += "--map key:" + lb->key_code.at(0).mid(4).toLower() + ' ' + "btn:" + lb->target + ' ';

			}
		}
	}

	return command;
}