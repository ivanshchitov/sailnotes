#ifndef IMAGESAVER_H
#define IMAGESAVER_H

#include <QObject>

class FileHelper : public QObject
{
    Q_OBJECT
public:
    explicit FileHelper(QObject *parent = 0);
    Q_INVOKABLE QString generatePictureFullName(const QString &extension = QString("png"));
    Q_INVOKABLE void removeFile(QString path);
    Q_INVOKABLE QString generateFileMD5Hash(const QString &pathToFile);
    Q_INVOKABLE void rotatePhoto(const QString &photoPath);
    Q_INVOKABLE void clearCookies();
private:
    Q_INVOKABLE QString getPictureWritablePath();
};

#endif
