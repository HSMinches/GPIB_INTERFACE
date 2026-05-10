#pragma once

#include <QString>

struct Equipment {
    int address = 10;
    QString code;
    QString type;

    QString label() const {
        return QString("%1 [%2] - %3").arg(code).arg(address).arg(type);
    }

    bool isValid() const {
        return !code.trimmed().isEmpty() && !type.trimmed().isEmpty();
    }
};
