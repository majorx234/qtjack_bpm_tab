#include <QPainter>
#include <QColor>
#include <QResizeEvent>
#include "wave_widget.h"
#include "sine_wave.h"

WaveWidget::WaveWidget(QWidget *parent) {
  setMinimumSize(400,100);
  setPalette(QPalette(QColor(200, 200, 200), QColor(20, 0, 70)));
  setAutoFillBackground(true);
  for (int i = 0; i < 4; i++) {

    wave_[i] = (double *)malloc(width() * sizeof(double));
    memset(wave_[i], 0, width() * sizeof(double));
  }
  wave_ofs_ = 0;
  wavecounter_ = 0;
  frame_rate = 25.0;
}

WaveWidget::~WaveWidget() {
  for (int i = 0; i < 4; i++)
    delete wave_[i];

}

void WaveWidget::paintEvent(QPaintEvent *event) {
  QPainter p;

  int w = width();
  int h = height();
  double yScale = h / 4.0;
  int l = h >> 1;
  p.begin(this);
  p.setPen(QPen(QBrush(QColor(170, 120, 0)), 1));
  p.drawLine(0, (int)(0.5 * (double)l), w - 1, (int)(0.5 * (double)l));
  p.drawLine(0, (int)(1.5 * (double)l), w - 1, (int)(1.5 * (double)l));
  for (int i = 0; i < w; i++) {

    p.drawLine(i, (int)(0.5 * (double)l - yScale * wave_[0][(wave_ofs_ + i) % w]), i, (int)(0.5 * (double)l - yScale * wave_[1][(wave_ofs_ + i) % w]));
    p.drawLine(i, (int)(1.5 * (double)l - yScale * wave_[2][(wave_ofs_ + i) % w]), i, (int)(1.5 * (double)l - yScale * wave_[3][(wave_ofs_ + i) % w]));
  }
  p.end();
}

void WaveWidget::resizeEvent(QResizeEvent *ev) {
  
}
void WaveWidget::setChunk(double limit_1high,double limit_1low,double limit_2high,double limit_2low) {
  wave_[0][wave_ofs_] = limit_1high;
  wave_[1][wave_ofs_] = limit_1low;
  wave_[2][wave_ofs_] = limit_2high;
  wave_[3][wave_ofs_] = limit_2low;
  wave_ofs_++;
  if (wave_ofs_ >= width())
    wave_ofs_ = 0;
  repaint();
}

void WaveWidget::testChunk() {
  // <test code>
  float data[1024];
  sine_wave(data, 1000, 1, 1024, 1014*wave_ofs_, 48000);

  float limits[4] = {0.0, 0.0, 0.0, 0.0};

  for (int i = 0; i < 2; i++) {
    for (int j = 0; j < 1024; j++) {
      if(data[j] < limits[2*i]) {
        limits[2*i] = data[j];
      } else if (data[j] > limits[2 * i + 1]) {
        limits[2*i+1] = data[j];
      }
    }
  }

  // </test code>
  for (int i = 0; i < 4; i++)
    wave_[i][wave_ofs_] = limits[i];
  wave_ofs_++;
  if (wave_ofs_ >= width())
    wave_ofs_ = 0;
  // Todo
  if (wavecounter_++ >= 48000.0 / (25.0 * (double)1024.0)) {
    wavecounter_ = 0;
    repaint();
    }
}


