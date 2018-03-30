#ifndef NOTELISTMODEL_H
#define NOTELISTMODEL_H

#include <QObject>
#include <QSqlQueryModel>

class NoteListModel : public QSqlQueryModel
{
    Q_OBJECT
public:
    enum Roles {
        idRole = Qt::UserRole + 1,
        titleRole,
        descriptionRole,
        picturePathsRole,
        hashesRole,
        tagNamesRole,
        tagExternalIdsRole,
        reminderTimestampRole,
        externalIdRole,
        deletedRole,
        synchronizedRole,
        audioFilePath
    };

    explicit NoteListModel(QObject *parent = 0);
    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;
    Q_INVOKABLE void setNotesTag(const QString &tag);

protected:
    inline QHash<int, QByteArray> roleNames() const { return rolesHash; }

private:
    QHash<int, QByteArray> rolesHash;
    QString tag;

signals:

public slots:
    void updateModel();
};

#endif // NOTELISTMODEL_H
