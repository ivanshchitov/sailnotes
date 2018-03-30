#include <QtQuick>
#include <QGuiApplication>
#include <QQmlEngine>
#include <QQuickView>
#include <QQmlContext>
#include <QTranslator>
#include <sailfishapp.h>
#include "file_helper.h"
#include "evernote_synchronizer.h"
#include "settings.h"
#include "database_manager.h"
#include "note_list_model.h"
#include "notification_manager.h"

#include "audiorecorder.h"

int main(int argc, char *argv[])
{
    QGuiApplication* app =  SailfishApp::application(argc, argv);
    QQuickView* view = SailfishApp::createView();
    qmlRegisterType<AudioRecorder>( "harbour.sailnotes", 1, 0, "AudioRecorder" );
    qmlRegisterType<Settings>("harbour.sailnotes.settings", 1, 0, "Settings");
    qmlRegisterType<LocalNote>("harbour.sailnotes.localNote", 1, 0, "LocalNote");
    qmlRegisterType<FileHelper>("harbour.sailnotes.fileHelper", 1, 0, "FileHelper");
    qmlRegisterType<NoteListModel>("harbour.sailnotes.noteListModel", 1, 0, "NoteListModel");
    qmlRegisterType<DatabaseManager>("harbour.sailnotes.databaseManager", 1, 0, "DatabaseManager");
    qmlRegisterType<NotificationManager>("harbour.sailnotes.notificationManager", 1, 0,
                                         "NotificationManager");
    qmlRegisterType<EvernoteSynchronizer>("harbour.sailnotes.evernoteSynchronizer", 1, 0,
                                          "EvernoteSynchronizer");
    QSettings settings("FRUCT", "SailNotes");
    settings.setPath(QSettings::NativeFormat, QSettings::UserScope,
                     QDir(QStandardPaths::writableLocation(QStandardPaths::ConfigLocation))
                     .filePath(QCoreApplication::applicationName()));
    QQmlEngine* qmlEngine = view->engine();
    qmlEngine->rootContext()->setContextProperty("settings", &settings);
    QTranslator translator;
    translator.load("sailnotes-" + QLocale().name(), SailfishApp::pathTo(QString("localization")).toLocalFile());
    app->installTranslator(&translator);
    view->setSource(SailfishApp::pathTo("qml/sailnotes.qml"));
    view->showFullScreen();
    QObject::connect(view->engine(), &QQmlEngine::quit, app, &QGuiApplication::quit);
    return app->exec();
}
