#pragma once

#include <QtCore>

namespace nmcli
{

bool available();
QString exec(const QStringList &parameters);
QStringList getFields(QString line);

}
