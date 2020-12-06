#ifndef ENTITYTREEMODEL_H
#define ENTITYTREEMODEL_H

#include <QAbstractItemModel>
#include <QModelIndex>
#include <QVariant>
#include <QPixmap>

#include "types.h"

class EntityTreeItem;
struct AMDModel;

class EntityTreeModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    explicit EntityTreeModel(QObject* parent = nullptr);
    ~EntityTreeModel();

    QVariant data(const QModelIndex& index, int role) const override;
    Qt::ItemFlags flags(const QModelIndex& index) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex& index) const override;
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;

    //bool setData(const QModelIndex& index, const QVariant& value, int role) override;

    EntityTreeItem* getItem(const QModelIndex &index) const;
    void setEntityTree(const AMDModel& model);

private:
    void setupModelData(const AMDModel& model, u16 nodeID, EntityTreeItem* parent);

    EntityTreeItem* root;

    QPixmap entityImages[6];

};

#endif // ENTITYTREEMODEL_H
