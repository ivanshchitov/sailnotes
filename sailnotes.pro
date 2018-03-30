TARGET = harbour-sailnotes

CONFIG += sailfishapp c++11
PKGCONFIG += nemonotifications-qt5

include(lib/QEverCloud/QEverCloud/QEverCloud.pro)

SOURCES += src/sailnotes.cpp \
    src/file_helper.cpp \
    src/evernote_synchronizer.cpp \
    src/settings.cpp \
    src/database_manager.cpp \
    src/note_list_model.cpp \
    src/local_note.cpp \
    src/notification_manager.cpp \
    src/audiorecorder.cpp

OTHER_FILES += qml/sailnotes.qml \
    qml/cover/CoverPage.qml \
    qml/pages/FirstPage.qml \
    qml/pages/SecondPage.qml \
    rpm/harbour-sailnotes.changes.in \
    rpm/harbour-sailnotes.yaml \
    translations/*.ts

SAILFISHAPP_ICONS = 86x86 108x108 128x128 256x256

CONFIG += sailfishapp_i18n

localization.files = localization
localization.path = /usr/share/harbour-sailnotes/localization

INSTALLS += localization

TRANSLATIONS += translations/harbour-sailnotes-ru.ts

HEADERS += \
    src/file_helper.h \
    src/evernote_synchronizer.h \
    src/settings.h \
    src/database_manager.h \
    src/note_list_model.h \
    src/local_note.h \
    src/notification_manager.h \
    src/audiorecorder.h

DISTFILES += \
    qml/pages/CameraPage.qml \
    qml/pages/EditTextNoteDialog.qml \
    qml/pages/MainPage.qml \
    qml/menu/PullDownMenu.qml \
    qml/pages/EditImagePhotoNoteDialog.qml \
    qml/pages/AddPhotoNoteDialog.qml \
    qml/pages/AddPictureNoteDialog.qml \
    qml/dialogs/AddPictureDialog.qml \
    qml/dialogs/EditNoteDialog.qml \
    qml/pages/NoteDetailsPage.qml \
    qml/menu/PushUpMenu.qml \
    qml/pages/EvernoteAuthPage.qml \
    qml/pages/SettingsPage.qml \
    qml/pages/TagsPage.qml \
    qml/dialogs/EditReminderDialog.qml \
    harbour-sailnotes.desktop \
    qml/components/AudioPlayer.qml

QT += core network sql xml gui dbus multimedia
