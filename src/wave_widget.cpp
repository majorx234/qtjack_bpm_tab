#include <QPainter>
#include <QColor>
#include <QResizeEvent>
#include "wave_widget.h"

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
}

WaveWidget::~WaveWidget() {
  

}

void WaveWidget::paintEvent(QPaintEvent *) {
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

void WaveWidget::getChunk() {
  for (int i = 0; i < 4; i++)
    wave_[i][wave_ofs_] = 0; //ToDo:data->limits[i];
  wave_ofs_++;
  if (wave_ofs_ >= width()) wave_ofs_ = 0;
  /* Todo
    if (waveCounter++ >= (double)data->getRate() / (FRAME_RATE * (double)data->getJackBufSize())) {
    waveCounter = 0;
    repaint();
    }  */
}


