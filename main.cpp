#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include "songattachedpicprovider.h"
#include "songhelper.h"
#include "barsvisualizer.h"


int main(int argc, char *argv[])
{

    SongAttachedPicProvider *imgProvider = new SongAttachedPicProvider();
    QGuiApplication app(argc, argv);
    QQmlApplicationEngine engine;

    qmlRegisterSingletonType<SongHelper>("My.SongHelper",1,0,"SongHelper",SongHelper::singletonProvider);
    qmlRegisterType<BarsVisualizer>("My.BarsVisualizer",1,0,"BarsVisualizer");
    engine.addImageProvider(QLatin1String("songpic"),imgProvider);

    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &app,
        []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);

    engine.loadFromModule("MusicPlayer", "Main");

    return app.exec();
}
