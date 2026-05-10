#pragma once

class QApplication;

class AppStyle final {
public:
    AppStyle() = delete;

    static void apply(QApplication& app);
};
