#pragma once

#include <QString>

#include <utility>

class Equipment {
public:
    Equipment() = default;

    Equipment(int address, QString code, QString type)
        : address_(address),
          code_(std::move(code)),
          type_(std::move(type)) {
    }

    int address() const {
        return address_;
    }

    const QString& code() const {
        return code_;
    }

    const QString& type() const {
        return type_;
    }

    void setAddress(int address) {
        address_ = address;
    }

    void setCode(QString code) {
        code_ = std::move(code);
    }

    void setType(QString type) {
        type_ = std::move(type);
    }

    QString label() const {
        return QString("%1 [%2] - %3").arg(code_).arg(address_).arg(type_);
    }

    bool isValid() const {
        return !code_.trimmed().isEmpty() && !type_.trimmed().isEmpty();
    }

private:
    int address_ = 10;
    QString code_;
    QString type_;
};
