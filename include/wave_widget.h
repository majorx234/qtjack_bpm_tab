#ifndef WAVE_WIDGET_H_
#define WAVE_WIDGET_H_

#include <QWidget>

class WaveWidget : public QWidget
{
  Q_OBJECT
private:
  double *wave_[4];
  int wave_ofs_;
  int wavecounter_;
public:
  WaveWidget(QWidget* parent);
  ~WaveWidget();

protected:
  virtual void paintEvent(QPaintEvent*);
  virtual void resizeEvent(QResizeEvent *ev);

public slots:
    void getChunk();      
};


#endif // WAVE_WIDGET_H_
