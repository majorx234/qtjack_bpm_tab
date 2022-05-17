#include "test/main_window_test.hpp"
#include "wave_widget.hpp"

MainWindowTest::MainWindowTest(QWidget *parent)
  : QMainWindow(parent)
  , wave_widget(this)
{
    setCentralWidget(&wave_widget);
    timerId = startTimer(1000);
}

MainWindowTest::~MainWindowTest() {
    killTimer(timerId);
}

void MainWindowTest::timerEvent(QTimerEvent *event) {
    wave_widget.testChunk();
}
