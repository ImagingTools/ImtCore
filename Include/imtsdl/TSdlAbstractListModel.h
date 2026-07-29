// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once

// Qt includes
#include <QtCore/QAbstractListModel>

// ACF includes
#include <istd/TNullableValue.h>

// ImtCore includes
#include <imtsdl/TElementList.h>
#include <imtsdl/CSdandardSdlListModelBase.h>


namespace imtsdl
{


template <class ModelDataType, class ModelObjectDataType>
class TSdlAbstractListModel: public CSdandardSdlListModelBase
{


public:
	using BaseClass = CSdandardSdlListModelBase;

	explicit TSdlAbstractListModel(QObject* parent = nullptr);

	// reimplemented (CSdandardSdlListModelBase)
	void ClearCache() override;
	QVariantMap get(int row) const override;
	void remove(int index) override;
	void clear() override;
	QVariant getData(const QString& nameId, int index) const override;

	// reimplemented (QAbstractListModel)
	int rowCount(const QModelIndex& parent = QModelIndex()) const override;
	QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
	bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole) override;
	QHash<int, QByteArray> roleNames() const override;

public:
	// available version members
	istd::TNullableValue<imtsdl::TElementList<ModelDataType>> Version_1_0;
	mutable QMap<int, QVariant> m_objectDataTypeMap;
};


// reimplemented (TSdlAbstractListModel)

template <class ModelDataType, class ModelObjectDataType>
TSdlAbstractListModel<ModelDataType, ModelObjectDataType>::
	TSdlAbstractListModel(QObject *parent): BaseClass(parent)
{
	Version_1_0.emplace();
}


template <class ModelDataType, class ModelObjectDataType>
void TSdlAbstractListModel<ModelDataType, ModelObjectDataType>::ClearCache()
{
	m_objectDataTypeMap.clear();
}


template <class ModelDataType, class ModelObjectDataType>
QVariantMap  TSdlAbstractListModel<ModelDataType, ModelObjectDataType>::get(int row) const
{
	QVariantMap data;
	QModelIndex idx = index(row, 0);
	if (!idx.isValid()){
		return data;
	}
	QHash<int, QByteArray> roles = roleNames();
	for (auto it = roles.begin(); it != roles.end(); ++it){
		data[it.value()] = idx.data(it.key());
	}
	return data;
}


template <class ModelDataType, class ModelObjectDataType>
void TSdlAbstractListModel<ModelDataType, ModelObjectDataType>::remove(int index)
{
	if (index < 0 || index >= Version_1_0->size()){
		return;
	}
	beginRemoveRows(QModelIndex(), index, index);
	Version_1_0->removeAt(index);
	ClearCache();
	endRemoveRows();
}


template <class ModelDataType, class ModelObjectDataType>
void TSdlAbstractListModel<ModelDataType, ModelObjectDataType>::clear()
{
	beginResetModel();
	ClearCache();
	Version_1_0->clear();
	endResetModel();
}


template <class ModelDataType, class ModelObjectDataType>
QVariant TSdlAbstractListModel<ModelDataType, ModelObjectDataType>::getData(const QString& nameId, int index) const
{
	if (nameId == "item" && Version_1_0.has_value() && index >= 0 && index < Version_1_0->count()){
		QVariant retVal = GetOrCreateCachedObject(index);
		return retVal;
	}
	return {};
}


// reimplemented (QAbstractListModel)

template <class ModelDataType, class ModelObjectDataType>
int TSdlAbstractListModel<ModelDataType, ModelObjectDataType>::
			rowCount(const QModelIndex& /*parent*/) const
{
	if (Version_1_0.has_value()){
		return Version_1_0->count();
	}

	return 0;
}


template <class ModelDataType, class ModelObjectDataType>
QVariant TSdlAbstractListModel<ModelDataType, ModelObjectDataType>::
			data(const QModelIndex& index, int role) const
{
	Q_UNUSED(role);

	int row = index.row();
	if (!Version_1_0.has_value() || row < 0 || row > Version_1_0->count() - 1){
		return {};
	}

	QVariant retVal = GetOrCreateCachedObject(row);
	return retVal;
}


template <class ModelDataType, class ModelObjectDataType>
bool TSdlAbstractListModel<ModelDataType, ModelObjectDataType>::
			setData(const QModelIndex& index, const QVariant& value, int role)
{
	Q_UNUSED(role);

	int row = index.row();

	if (!Version_1_0.has_value() || row < 0 || row > Version_1_0->count() - 1){
		return false;
	}

	ModelDataType* newItemPtr = value.value<ModelDataType*>();
	istd::TNullableValue<ModelDataType> newItem = *newItemPtr;
	Version_1_0->insert(row, newItem);

	return true;
}


template <class ModelDataType, class ModelObjectDataType>
QHash<int, QByteArray> TSdlAbstractListModel<ModelDataType, ModelObjectDataType>::roleNames() const
{
	QHash<int, QByteArray> hash;
	hash.insert(0,"item");

	return hash;
}


} // namespace imtsdl


