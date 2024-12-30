#ifndef BARSVISUALIZER_H
#define BARSVISUALIZER_H

#include "visualizer.h"
#include <QTimer>

class BarsVisualizer : public Visualizer
{
    Q_OBJECT
public:
    BarsVisualizer();


    // QQuickPaintedItem interface
public:
    Q_INVOKABLE void pause();
    void paint(QPainter *painter) override;

    // Visualizer interface
protected:
    void visualize(const QAudioBuffer &) override;
private:
    QAudioBuffer buffer;
    const static int BarsNumber = 32;
    int h_values[BarsNumber];
    int h_drop[BarsNumber];

    //用于暂停时让所有bar归零
    QTimer *timer;
};

#endif // BARSVISUALIZER_H
