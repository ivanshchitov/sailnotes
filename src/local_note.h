#ifndef LOCALNOTE_H
#define LOCALNOTE_H

#include <QObject>
#include <QHash>

class LocalNote : public QObject
{
    Q_OBJECT

    Q_PROPERTY(int id READ getId WRITE setId)
    Q_PROPERTY(QString title READ getTitle WRITE setTitle NOTIFY onTitleChanged)
    Q_PROPERTY(QString description READ getDescription WRITE setDescription NOTIFY onDescriptionChanged)
    Q_PROPERTY(QString picturePaths READ getPicturePathsAsString WRITE addPicturePaths NOTIFY onPicturePathsChanged)
    Q_PROPERTY(QString hashes READ getHashesAsString WRITE addHashes NOTIFY onHashesChanged)
    Q_PROPERTY(QString tags READ getTagNamesAsString WRITE setTagNamesFromString NOTIFY onTagNamesChanged)
    Q_PROPERTY(qint64 reminderTimestamp READ getReminderTimestamp WRITE setReminderTimestamp NOTIFY onReminderTimestampChanged)
    Q_PROPERTY(QString externalId READ getExternalId WRITE setExternalId)
    Q_PROPERTY(bool deleted READ isDeleted WRITE setDeleted)
    Q_PROPERTY(bool synchronized READ isSynchronized WRITE setSynchronized)

public:
    explicit LocalNote();
    explicit LocalNote(const QString &title, const QString &description, QList<QString> picturePaths, QList<QString> hashes,
                       const QString &externalId = QString(), const bool deleted = false, const bool synchronized = false);
    explicit LocalNote(const int id, const QString &title, const QString &description, QList<QString> picturePaths, QList<QString> hashes,
                       const QString &externalId = QString(), const bool deleted = false, const bool synchronized = false);
    explicit LocalNote(const int id, const QString &title, const QString &description, const QString &picturePaths, const QString &hashes,
                       const QString &tagExternalIds, const QString &tagNames, const qint64 reminderTimestamp,
                       const QString &externalId = QString(), const bool deleted = false, const bool synchronized = false);

    int getId() const;
    void setId(const int id);

    QString getTitle() const;
    void setTitle(const QString &title);

    QString getDescription() const;
    void setDescription(const QString &description);

    QString getPicturePathsAsString() const;
    Q_INVOKABLE QList<QString> getPicturePaths() const;
    Q_INVOKABLE void addPicturePaths(const QString &picturePaths);
    void addPicturePaths(QList<QString> picturePaths);
    Q_INVOKABLE void removePicturePath(const QString &picturePath);

    QString getHashesAsString() const;
    Q_INVOKABLE QList<QString> getHashes() const;
    Q_INVOKABLE void addHashes(const QString &hashes);
    void addHashes(QList<QString> hashes);
    Q_INVOKABLE void removeHash(const QString &hash);

    QString getTagExternalIdsAsString() const;
    QList<QString> getTagExternalIds() const;

    QString getTagNamesAsString() const;
    QList<QString> getTagNames() const;
    void setTagNamesFromString(const QString &tagNames);

    QHash<QString, QString> getTags();
    void addTag(const QString &tagName, const QString &tagExternalId);
    void removeTagByExternalId(const QString &tagExternalId);
    Q_INVOKABLE void setTagsFromStrings(const QString &tagNames, const QString &tagExternalIds);

    qint64 getReminderTimestamp() const;
    void setReminderTimestamp(qint64 timestamp);

    QString getExternalId() const;
    void setExternalId(const QString &externalId);

    bool isDeleted() const;
    void setDeleted(const bool deleted);

    bool isSynchronized() const;
    void setSynchronized(const bool synchronized);

private:
    int id;
    QString title;
    QString description;
    QList<QString> picturePaths;
    QList<QString> hashes;
    QHash<QString, QString> tags;
    qint64 reminderTimestamp;
    QString externalId;
    bool deleted;
    bool synchronized;

signals:
    void onTitleChanged();
    void onDescriptionChanged();
    void onPicturePathsChanged();
    void onHashesChanged();
    void onTagNamesChanged();
    void onReminderTimestampChanged();
public slots:
};

#endif // LOCALNOTE_H
