#include "audiorecorder.h"

#include <QDir>
#include <QUrl>
#include <QUuid>
#include <QStandardPaths>

/*!
 * \brief Constructor initializes the recorder field.
 * \param parent
 */
AudioRecorder::AudioRecorder(QObject* parent) : QObject(parent) {
    recorder = new QAudioRecorder(this);
    recording = false;
}

/*!
 * \brief Destructors deletes the recorder ponter.
 */
AudioRecorder::~AudioRecorder() {
    delete recorder;
}

/*!
 * \return True if recording is runnig right now, false - otherwise.
 */
bool AudioRecorder::isRecording() const {
    return recording;
}

/*!
 * \return The full path to the audio file.
 */
QString AudioRecorder::createAudioFileName() {
    QDir dir(QStandardPaths::writableLocation(QStandardPaths::DataLocation));
    if (!dir.exists()) dir.mkpath(".");
    return QString("%1/voice%2.wav").arg(dir.path()).arg(QUuid::createUuid().toString());
}

/*!
 * \brief Checks that the audio file is exist by the given path.
 * \param path The path to the audio file.
 * \return True if the audio file is exist by the given path, false - otherwise.
 */
bool AudioRecorder::isAudioFileExist(const QString& path) {
    return QFile::exists(path);
}

/*!
 * \brief Removes the audio file by the given path.
 * \param path The path to the audio file to remove.
 */
void AudioRecorder::removeAudioFile(const QString& path) {
    if (QFile::exists(path)) QFile(path).remove();
    emit audioFileChanged();
}

/*!
 * \brief Records and saves the audio file.
 * \param path The path for recording an audio file.
 */
void AudioRecorder::record(const QString& path) {
    recorder->setOutputLocation(QUrl(path));
    if (recorder->state() == QMediaRecorder::StoppedState) {
        recorder->record();
        recording = true;
        emit recordingChanged();
    }
}

/*!
 * \brief Stops the recording of the audio file.
 */
void AudioRecorder::stop() {
    if (recorder->state() == QMediaRecorder::RecordingState) {
        recorder->stop();
        recording = false;
        emit recordingChanged();
        emit audioFileChanged();
    }
}
