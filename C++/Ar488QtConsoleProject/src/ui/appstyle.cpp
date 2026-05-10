#include "appstyle.h"

#include <QApplication>
#include <QColor>
#include <QFont>
#include <QPalette>
#include <QString>

void AppStyle::apply(QApplication& app) {
    app.setStyle("Fusion");
    app.setFont(QFont("Segoe UI", 10));

    QPalette palette;
    palette.setColor(QPalette::Window, QColor(15, 17, 23));
    palette.setColor(QPalette::WindowText, QColor(230, 233, 239));
    palette.setColor(QPalette::Base, QColor(18, 22, 29));
    palette.setColor(QPalette::AlternateBase, QColor(24, 29, 38));
    palette.setColor(QPalette::Text, QColor(230, 233, 239));
    palette.setColor(QPalette::Button, QColor(28, 34, 44));
    palette.setColor(QPalette::ButtonText, QColor(230, 233, 239));
    palette.setColor(QPalette::Highlight, QColor(70, 138, 255));
    palette.setColor(QPalette::HighlightedText, QColor(255, 255, 255));
    palette.setColor(QPalette::ToolTipBase, QColor(24, 29, 38));
    palette.setColor(QPalette::ToolTipText, QColor(230, 233, 239));
    palette.setColor(QPalette::PlaceholderText, QColor(130, 140, 155));
    app.setPalette(palette);

    app.setStyleSheet(QString::fromUtf8(R"(
        QWidget {
            background: #0f1117;
            color: #e6e9ef;
        }

        QWidget#sidebarContainer {
            background: #11151d;
            border: 1px solid #242b36;
            border-radius: 18px;
        }

        QWidget#sidebarContent {
            background: transparent;
        }

        QWidget#activeGpibBar {
            background: #121821;
            border: 1px solid #242b36;
            border-radius: 16px;
        }

        QLabel#activeGpibLabel {
            color: #f3f7ff;
            font-weight: 700;
        }

        QGroupBox {
            background: #151a22;
            border: 1px solid #242b36;
            border-radius: 16px;
            margin-top: 14px;
            padding-top: 12px;
            font-weight: 600;
        }

        QGroupBox::title {
            subcontrol-origin: margin;
            left: 14px;
            padding: 0 6px;
            color: #cfd7e6;
        }

        QLabel {
            background: transparent;
            color: #e6e9ef;
        }

        QLabel#sectionHint,
        QLabel#pageSubtitle {
            color: #98a3b6;
        }

        QLabel#sidebarGroupHeader {
            background: #151d28;
            color: #f5f8ff;
            border: 1px solid #2c3949;
            border-radius: 12px;
            padding: 10px 12px;
            font-size: 12pt;
            font-weight: 700;
        }

        QLineEdit,
        QPlainTextEdit,
        QComboBox,
        QSpinBox {
            background: #0f141b;
            color: #edf2f8;
            border: 1px solid #2a3340;
            border-radius: 12px;
            padding: 8px 10px;
            selection-background-color: #468aff;
            selection-color: white;
        }

        QTableWidget,
        QAbstractScrollArea {
            background: #0f141b;
            color: #edf2f8;
            border: 1px solid #2a3340;
            border-radius: 12px;
            selection-background-color: #468aff;
            selection-color: white;
            padding: 0px;
        }

        QLineEdit:focus,
        QPlainTextEdit:focus,
        QComboBox:focus,
        QSpinBox:focus,
        QTableWidget:focus {
            border: 1px solid #468aff;
        }

        QPlainTextEdit#replyView,
        QPlainTextEdit#logView,
        QPlainTextEdit#batchEdit {
            font-family: "Cascadia Code", "Consolas", "Courier New", monospace;
            font-size: 10pt;
        }

        QComboBox::drop-down {
            border: none;
            width: 24px;
            background: transparent;
        }

        QSpinBox::up-button,
        QSpinBox::down-button {
            width: 18px;
            border: none;
            background: transparent;
        }

        QPushButton {
            background: #1f2630;
            color: #edf2f8;
            border: 1px solid #2b3441;
            border-radius: 12px;
            padding: 8px 14px;
            min-height: 20px;
            font-weight: 600;
        }

        QPushButton:hover {
            background: #283241;
            border: 1px solid #344154;
        }

        QPushButton:pressed {
            background: #1b222d;
        }

        QPushButton:disabled {
            background: #171c24;
            color: #6f7b8f;
            border: 1px solid #222935;
        }

        QPushButton[accent="true"] {
            background: #468aff;
            border: 1px solid #468aff;
            color: white;
        }

        QPushButton[accent="true"]:hover {
            background: #5b98ff;
            border: 1px solid #5b98ff;
        }

        QPushButton#sidebarToggle,
        QPushButton#sidebarNavButton {
            text-align: center;
            color: #f3f7ff;
            min-height: 34px;
            font-weight: 700;
            background: #171d27;
            border: 1px solid #283241;
            border-radius: 14px;
        }

        QPushButton#presetButton {
            text-align: left;
            padding-left: 12px;
            background: #171d27;
            color: #edf2f8;
        }

        QPushButton#presetButton:hover {
            background: #212938;
        }

        QToolBox {
            border: none;
            background: transparent;
            padding: 0;
        }

        QToolBox::tab {
            background: #171d27;
            color: #edf2f8;
            border: 1px solid #283241;
            border-radius: 12px;
            padding: 10px 12px;
            margin-top: 4px;
            font-weight: 700;
            text-align: left;
        }

        QToolBox::tab:selected {
            background: #202837;
            color: #ffffff;
            border: 1px solid #3a475a;
        }

        QTabWidget::pane {
            border: 1px solid #242b36;
            background: #121821;
            border-radius: 16px;
            top: -1px;
        }

        QTabBar::tab {
            background: #171d27;
            color: #cfd7e6;
            border: 1px solid #283241;
            padding: 10px 16px;
            margin-right: 6px;
            border-top-left-radius: 12px;
            border-top-right-radius: 12px;
            font-weight: 700;
            min-width: 110px;
        }

        QTabBar::tab:selected {
            background: #202837;
            color: #ffffff;
            border: 1px solid #3a475a;
        }

        QTabBar::tab:hover:!selected {
            background: #1b2330;
        }

        QHeaderView::section {
            background: #1a202a;
            color: #d9e1ee;
            border: none;
            border-bottom: 1px solid #2a3340;
            padding: 6px 10px;
            min-height: 34px;
            font-weight: 600;
        }

        QTableWidget {
            gridline-color: #242d39;
        }

        QTableCornerButton::section {
            background: #1a202a;
            border: none;
            border-bottom: 1px solid #2a3340;
        }

        QScrollBar:vertical {
            background: transparent;
            width: 12px;
            margin: 4px 0 4px 0;
        }

        QScrollBar::handle:vertical {
            background: #2a3442;
            min-height: 24px;
            border-radius: 6px;
        }

        QScrollBar::handle:vertical:hover {
            background: #384557;
        }

        QScrollBar::add-line:vertical,
        QScrollBar::sub-line:vertical,
        QScrollBar::add-page:vertical,
        QScrollBar::sub-page:vertical,
        QScrollBar:horizontal,
        QScrollBar::add-line:horizontal,
        QScrollBar::sub-line:horizontal,
        QScrollBar::add-page:horizontal,
        QScrollBar::sub-page:horizontal {
            background: transparent;
            border: none;
            width: 0px;
            height: 0px;
        }

        QStatusBar {
            background: #0f1117;
            color: #98a3b6;
            border-top: 1px solid #1f2630;
        }

        QRadioButton {
            spacing: 8px;
            color: #dce3ef;
        }

        QRadioButton::indicator {
            width: 16px;
            height: 16px;
            border-radius: 8px;
            border: 1px solid #445064;
            background: #0f141b;
        }

        QRadioButton::indicator:checked {
            background: #468aff;
            border: 1px solid #468aff;
        }

        QToolTip {
            background: #1a202a;
            color: #edf2f8;
            border: 1px solid #2a3340;
            padding: 6px;
        }
    )"));
}
