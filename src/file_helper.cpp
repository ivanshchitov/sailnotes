#include "file_helper.h"
#include <QStandardPaths>
#include <QFile>
#include <QDir>
#include <QUuid>
#include <QCryptographicHash>
#include <QImage>
#include <QTransform>
#include <QDebug>

/**
 * @brief The constructor.
 * @param parent The QObject instance.
 */
FileHelper::FileHelper(QObject *parent) : QObject(parent)
{

}

/**
 * @brief Generates the full name (with path) for saving a picture.
 * @param extension  The picture file extension.
 * @return The full picture name.
 */
QString FileHelper::generatePictureFullName(const QString &extension)
{
    return getPictureWritablePath() + QUuid::createUuid().toString() + "." + extension;
}

/**
 * @brief Removes a file by the given path.
 * @param path The file path.
 */
void FileHelper::removeFile(QString path)
{
    QFile::remove(path);
}

/**
 * @brief Generates the MD5 hash by the given file path.
 * @param pathToFile The path to file.
 * @return The MD5 hash of the file with the given path.
 */
QString FileHelper::generateFileMD5Hash(const QString &pathToFile)
{
    QFile file(pathToFile);
    file.open(QIODevice::ReadOnly);
    QByteArray fileBody = file.readAll();
    file.close();
    return QCryptographicHash::hash(fileBody, QCryptographicHash::Md5).toHex();
}

/**
 * @brief Rotates a photo by the given path by 90 degrees.
 * @param photoPath The path of photo to rotate.
 */
void FileHelper::rotatePhoto(const QString &photoPath)
{
    QImage image(photoPath);
    QTransform rotation;
    rotation.rotate(90);
    QImage rotatedImage = image.transformed(rotation);
    rotatedImage.save(photoPath);
}

/**
 * @brief Removes the local application cookies.
 */
void FileHelper::clearCookies()
{
    QStringList dataPaths = QStandardPaths::standardLocations(QStandardPaths::CacheLocation);
    QDir webData(QDir(dataPaths.at(0)).filePath(".QtWebKit"));
    if (webData.exists()) webData.removeRecursively();
}

/**
 * @return The writable path for pictures.
 */
QString FileHelper::getPictureWritablePath()
{
    QString path = QStandardPaths::writableLocation(QStandardPaths::PicturesLocation) + "/.sailnotes/";
    if (!QDir(path).exists()) QDir().mkdir(path);
    return path;
}
