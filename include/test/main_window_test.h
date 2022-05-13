#ifndef MAIN_WINDOW_TEST_H_
#define MAIN_WINDOW_TEST_H_

#include <QMainWindow>
#include "wave_widget.h"

class MainWindowTest : public QMainWindow
{
  Q_OBJECT
public:
  explicit MainWindowTest(QWidget *parent = 0);
  ~MainWindowTest();

private:
    int timerId;
    WaveWidget wave_widget;
protected:
  void timerEvent(QTimerEvent *event);
};

#endif // MAIN_WINDOW_TEST_H_
