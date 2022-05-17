///////////////////////////////////////////////////////////////////////////////
//                                                                           //
//    This file is part of QtJack.                                           //
//    Copyright (C) 2014-2015 Jacob Dawid <jacob@omg-it.works>               //
//                  2022 <majorx234@googlemail.com>                          //
//    QtJack is free software: you can redistribute it and/or modify         //
//    it under the terms of the GNU General Public License as published by   //
//    the Free Software Foundation, either version 3 of the License, or      //
//    (at your option) any later version.                                    //
//                                                                           //
//    QtJack is distributed in the hope that it will be useful,              //
//    but WITHOUT ANY WARRANTY; without even the implied warranty of         //
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          //
//    GNU General Public License for more details.                           //
//                                                                           //
//    You should have received a copy of the GNU General Public License      //
//    along with QtJack. If not, see <http://www.gnu.org/licenses/>.         //
//                                                                           //
//    It is possible to obtain a closed-source license of QtJack.            //
//    If you're interested, contact me at: jacob@omg-it.works                //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

#include <QtWidgets/QApplication>
#include <QMainWindow>
#include <QLayout>
#include "bpm_tab.hpp"

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
    bpm_window.resize(420,438);
    bpm_window.show();
    return app.exec();
}
