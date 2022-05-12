#ifndef WAVE_WIDGET_H_
#define WAVE_WIDGET_H_

#include <QWidget>

class WaveWidget : public QWidget
{
  Q_OBJECT

public:
  WaveWidget(QWidget* parent);
  ~WaveWidget();

public slots:
  void getChunk();

private:
  double *wave_[4];
  int wave_ofs_;
  int wavecounter_;
  int frame_rate;

protected:
  virtual void paintEvent(QPaintEvent*);
  virtual void resizeEvent(QResizeEvent *ev);
};


#endif // WAVE_WIDGET_H_
