#include "visualizer.h"
#include "songhelper.h"

Visualizer::Visualizer() {
    SongHelper* songHelper = SongHelper::getSongHelper();
    connToAudioBuffer = QObject::connect(songHelper->getAudioBufferOutput(),&QAudioBufferOutput::audioBufferReceived,
                     this,[&](const QAudioBuffer& buffer){
        visualize(buffer);
    });
    connToMainColor = QObject::connect(songHelper,&SongHelper::mainColorChanged,this,[=](QColor color){
        this->mainColor_ = color;
    });
}

Visualizer::~Visualizer()
{
    QObject::disconnect(connToAudioBuffer);
    QObject::disconnect(connToMainColor);
}

QColor Visualizer::mainColor() const
{
    return mainColor_;
}
