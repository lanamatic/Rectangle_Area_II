#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QPalette dark;
    dark.setColor(QPalette::Window,          QColor(25, 25, 35));
    dark.setColor(QPalette::WindowText,      QColor(200, 200, 220));
    dark.setColor(QPalette::Base,            QColor(35, 35, 50));
    dark.setColor(QPalette::AlternateBase,   QColor(45, 45, 60));
    dark.setColor(QPalette::Text,            QColor(200, 200, 220));
    dark.setColor(QPalette::Button,          QColor(50, 50, 70));
    dark.setColor(QPalette::ButtonText,      QColor(200, 200, 220));
    dark.setColor(QPalette::Highlight,       QColor(70, 130, 200));
    dark.setColor(QPalette::HighlightedText, Qt::white);
    a.setPalette(dark);

    // Stylesheet za kontrole
    a.setStyleSheet(
        "QPushButton {"
        "  background-color: #3a3a55; color: #dde; border: 1px solid #555;"
        "  border-radius: 4px; padding: 5px 14px; font-family: monospace;"
        "}"
        "QPushButton:hover   { background-color: #4a4a70; }"
        "QPushButton:pressed { background-color: #2a2a40; }"
        "QComboBox {"
        "  background-color: #3a3a55; color: #dde; border: 1px solid #555;"
        "  border-radius: 4px; padding: 4px 8px; min-width: 200px;"
        "}"
        "QComboBox QAbstractItemView {"
        "  background-color: #2a2a40; color: #dde;"
        "  selection-background-color: #4a4a70;"
        "}"
        "QComboBox::drop-down { border: none; }"
        "QSlider::groove:horizontal {"
        "  height: 6px; background: #3a3a55; border-radius: 3px;"
        "}"
        "QSlider::handle:horizontal {"
        "  width: 16px; height: 16px; background: #7090c0;"
        "  border-radius: 8px; margin: -5px 0;"
        "}"
        "QLabel { color: #aab; font-family: monospace; }"
        );

    MainWindow w;
    w.show();
    return a.exec();
}
