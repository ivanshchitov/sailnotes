#ifndef AUDIORECORDER_H
#define AUDIORECORDER_H

#include <QObject>
#include <QAudioRecorder>

/*!
 * \brief The class-helper to record an audio using Qt Multimedia API.
 */
class AudioRecorder : public QObject {
    Q_OBJECT
    Q_PROPERTY(bool isRecording READ isRecording NOTIFY recordingChanged)

public:
    explicit AudioRecorder(QObject* parent = NULL);
    ~AudioRecorder();
    bool isRecording() const;

    Q_INVOKABLE QString createAudioFileName();
    Q_INVOKABLE bool isExistAudioFile (const QString& path);
    Q_INVOKABLE void removeAudioFile(const QString& path);
    Q_INVOKABLE void record(const QString& path);
    Q_INVOKABLE void stop();

private:
    QAudioRecorder recorder;
    bool recording;

signals:
    void recordingChanged();
};

#endif // AUDIORECORDER_H
