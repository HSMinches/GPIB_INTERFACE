#include "mainwindow.h"
#include "ui/appstyle.h"

#include <QApplication>
#include <QGuiApplication>
#include <QRect>
#include <QScreen>

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    AppStyle::apply(app);

    MainWindow window;

    const QScreen* screen = QGuiApplication::primaryScreen();
    if (screen) {
        const QRect available = screen->availableGeometry();
        const int width = static_cast<int>(available.width() * 0.90);
        const int height = static_cast<int>(available.height() * 0.90);
        window.resize(width, height);
        window.move(
            available.x() + (available.width() - width) / 2,
            available.y() + (available.height() - height) / 2
            );
    } else {
        window.resize(1400, 900);
    }

    window.show();
    return app.exec();
}
