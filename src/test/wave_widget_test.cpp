#include <QtWidgets/QApplication>
#include <signal.h>
#include <unistd.h>
#include <QMainWindow>
#include "wave_widget.h"

void catchUnixSignal(int quitSignal) {
  auto handler = [](int sig) -> void {
    printf("quit application. received signal(%d)\n", sig);
    QCoreApplication::quit();
  };

  sigset_t blocking_mask;
  sigemptyset(&blocking_mask);

  sigaddset(&blocking_mask, quitSignal);
  struct sigaction sa;
  sa.sa_handler = handler;
  sa.sa_mask = blocking_mask;
  sa.sa_flags = 0;

  sigaction(quitSignal, &sa, nullptr);
  
}

int main(int argc, char *argv[]) {
  QApplication app(argc, argv);
  catchUnixSignal(SIGINT);

  QMainWindow main_window;
  WaveWidget wave_widget(&main_window);
  main_window.setCentralWidget(&wave_widget);
  main_window.show();
  
  return app.exec();
}
 
