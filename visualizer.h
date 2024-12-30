#ifndef VISUALIZER_H
#define VISUALIZER_H

#include <QQuickPaintedItem>
#include <QAudioBuffer>

class Visualizer : public QQuickPaintedItem
{
    Q_OBJECT
public:
    Visualizer();
    virtual ~Visualizer();
protected:
    virtual void visualize(const QAudioBuffer&) = 0;
    QColor mainColor()const;
private:
    QMetaObject::Connection connToAudioBuffer;
    QMetaObject::Connection connToMainColor;
private:
    QColor mainColor_;
};

#endif // VISUALIZER_H
