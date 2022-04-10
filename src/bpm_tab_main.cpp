#include <QtWidgets/QApplication>
#include <QMainWindow>
#include <QLayout>
#include "bpm_tab.h"

int main(int argc, char **argv) {
    QApplication app(argc, argv);
    QMainWindow bpm_window;

    BpmTab* bpm_tab_widget = new BpmTab(&bpm_window);
    QHBoxLayout *layout = new QHBoxLayout;
    layout->addWidget(bpm_tab_widget);

    // Set layout in QWidget
    QWidget *bpm_tab_container = new QWidget();
    bpm_tab_container->setLayout(layout);

    bpm_window.setCentralWidget(bpm_tab_container);
    bpm_window.resize(370,288);
    bpm_window.show();
    return app.exec();
}