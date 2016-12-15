#include "note_list_model.h"
#include <QSqlRecord>

/**
 * @brief The constructor.
 * @param The parent QObject instance.
 */
NoteListModel::NoteListModel(QObject *parent) : QSqlQueryModel(parent)
{
    rolesHash[idRole] = "id";
    rolesHash[titleRole] = "title";
    rolesHash[descriptionRole] = "description";
    rolesHash[picturePathsRole] = "picturePaths";
    rolesHash[hashesRole] = "hashes";
    rolesHash[tagNamesRole] = "tagNames";
    rolesHash[tagExternalIdsRole] = "tagExternalIds";
    rolesHash[reminderTimestampRole] = "reminderTimestamp";
    rolesHash[externalIdRole] = "externalId";
    rolesHash[deletedRole] = "deleted";
    rolesHash[synchronizedRole] = "synchronized";
    updateModel();
}

/**
 * @brief Overriden from @link QAbstractItemModel::data() @endlink
 */
QVariant NoteListModel::data(const QModelIndex &index, int role) const
{
    if (role < Qt::UserRole) return QSqlQueryModel::data(index, role);
    QSqlRecord sqlRecord = record(index.row());
    return sqlRecord.value(QString(rolesHash.value(role))).toString();
}

/**
 * @brief Sets the given tag to select notes for updating view.
 * @param tag The note tag name.
 */
void NoteListModel::setNotesTag(const QString &tag)
{
    this->tag = tag;
}

/**
 * @brief Updates the model view.
 */
void NoteListModel::updateModel()
{
    QString tagCondition = tag.length() == 0 ? "" : QString("AND tagNames LIKE '%%1%'").arg(tag);
    setQuery(QStringLiteral("SELECT * FROM notes WHERE deleted = 0 %1 ORDER BY id DESC;").arg(tagCondition));
}
