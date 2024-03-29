#include "entitytreemodel.h"
#include "entitytreeitem.h"

#include "amdmodel.h"



EntityTreeModel::EntityTreeModel(QObject* parent) : QAbstractItemModel(parent), root(new EntityTreeItem({QString("Unnamed Root")})) {

	entityImages[0].load(":/images/entity_node.png");
	entityImages[0] = entityImages[0].scaled(16, 16);
	entityImages[1].load(":/images/entity_cube.png");
	entityImages[1] = entityImages[1].scaled(16, 16);
	entityImages[2].load(":/images/entity_material.png");
	entityImages[2] = entityImages[2].scaled(16, 16);
	entityImages[3].load(":/images/entity_texture.png");
	entityImages[3] = entityImages[3].scaled(16, 16);

}



EntityTreeModel::~EntityTreeModel() {
	delete root;
}



QVariant EntityTreeModel::data(const QModelIndex& index, int role) const {

	if (!index.isValid()) {
		return QVariant();
	}

	EntityTreeItem* item = static_cast<EntityTreeItem*>(index.internalPointer());

	switch(role){

		case Qt::DisplayRole:
			return item->data(1);

		case Qt::DecorationRole:

			if(index.column() == 0){
				return item->data(0);
			}

		default:
			return QVariant();

	}

}



Qt::ItemFlags EntityTreeModel::flags(const QModelIndex& index) const {

	if (!index.isValid()) {
		return Qt::NoItemFlags;
	}

	return QAbstractItemModel::flags(index);

}



QVariant EntityTreeModel::headerData(int section, Qt::Orientation orientation, int role) const {

	if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
		return root->data(section);
	}

	return QVariant();

}



QModelIndex EntityTreeModel::index(int row, int column, const QModelIndex& parent) const {

	if (!hasIndex(row, column, parent)) {
		return QModelIndex();
	}

	EntityTreeItem* parentItem;

	if (!parent.isValid()) {
		parentItem = root;
	} else {
		parentItem = static_cast<EntityTreeItem*>(parent.internalPointer());
	}

	EntityTreeItem* childItem = parentItem->child(row);

	if (childItem) {
		return createIndex(row, column, childItem);
	}

	return QModelIndex();

}



QModelIndex EntityTreeModel::parent(const QModelIndex& index) const {

	if (!index.isValid()) {
		return QModelIndex();
	}

	EntityTreeItem* childItem = static_cast<EntityTreeItem*>(index.internalPointer());
	EntityTreeItem* parentItem = childItem->parentItem();

	if (parentItem == root) {
		return QModelIndex();
	}

	return createIndex(parentItem->row(), 0, parentItem);

}



int EntityTreeModel::rowCount(const QModelIndex& parent) const {

	EntityTreeItem* parentItem = getItem(parent);
	return parentItem ? parentItem->childCount() : 0;

}



int EntityTreeModel::columnCount(const QModelIndex& parent) const {
	return getItem(parent)->columnCount();
}


/*
bool EntityTreeModel::setData(const QModelIndex& index, const QVariant& value, int role) {

	if (role != Qt::EditRole){
		return false;
	}

	EntityTreeItem* item = getItem(index);
	bool result = item->setData(index.column(), value);

	if (result){
		emit dataChanged(index, index, {Qt::DisplayRole, Qt::EditRole});
	}

	return result;

}
*/


EntityTreeItem* EntityTreeModel::getItem(const QModelIndex &index) const {

	if (index.isValid()) {

		EntityTreeItem *item = static_cast<EntityTreeItem*>(index.internalPointer());

		if (item) {
			return item;
		}

	}

	return root;

}



void EntityTreeModel::setEntityTree(const AMDModel& model){

	if(!model.nodes.size()){
		return;
	}

	beginResetModel();

	root->clearChildren();
	setupModelData(model, 0, root);

	endResetModel();

}



void EntityTreeModel::setupModelData(const AMDModel& model, u16 nodeID, EntityTreeItem* parent) {

	const AMDNode& node = model.nodes.at(nodeID);

	EntityTreeItem* currentTreeNode = new EntityTreeItem({entityImages[0], QString::fromStdString(node.name)}, parent);
	parent->appendChild(currentTreeNode);

	for(u32 i = 0; i < node.childIDs.size(); i++){
		setupModelData(model, node.childIDs[i], currentTreeNode);
	}

	for(u32 i = 0; i < node.meshIDs.size(); i++) {

		const AMDMesh& mesh = model.meshes[node.meshIDs[i]];
		const AMDMaterial& material = model.materials[mesh.materialID];

        EntityTreeItem* currentMeshNode = new EntityTreeItem({entityImages[1], QString::fromStdString(mesh.name)}, currentTreeNode);
		currentTreeNode->appendChild(currentMeshNode);

		EntityTreeItem* currentMaterialNode = new EntityTreeItem({entityImages[2], QString::fromStdString(material.name)}, currentMeshNode);
		currentMeshNode->appendChild(currentMaterialNode);

		for(u32 j = 0; j < material.textureIDs.size(); j++){

			EntityTreeItem* currentTextureNode = new EntityTreeItem({entityImages[3], QString::fromStdString(model.textures[material.textureIDs[j]].name)}, currentMaterialNode);
			currentMaterialNode->appendChild(currentTextureNode);

		}

	}

}
