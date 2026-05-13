// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once

// Qt includes
#include <QtCore/QAbstractListModel>

// ACF includes
#include <istd/TSharedNullable.h>


namespace imtbase
{


template <class ModelDataType, class ModelObjectDataType>
class TListModelBase: public QAbstractListModel
{
public:
	explicit TListModelBase(QObject* parent = nullptr);
protected:
	// reimplemented (QAbstractListModel)
	virtual int rowCount(const QModelIndex&  parent = QModelIndex()) const override;
	virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
	virtual bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole) override;
	virtual QHash<int, QByteArray> roleNames() const override;

	void ClearCache();
	ModelObjectDataType* GetOrCreateCachedObject(int index) const;

public:
	// std::optional<QList<ModelDataType>>& m_modelData;
	// available version members
	istd::TSharedNullable<QList<ModelDataType>> Version_1_0;
	mutable QMap<int, ModelObjectDataType*> m_objectDataTypeMap;
};


template <class ModelDataType, class ModelObjectDataType>
void TListModelBase<ModelDataType, ModelObjectDataType>::ClearCache()
{
	for (auto it = m_objectDataTypeMap.begin(); it != m_objectDataTypeMap.end(); ++it){
		it.value()->deleteLater();
	}

	m_objectDataTypeMap.clear();
}


template <class ModelDataType, class ModelObjectDataType>
ModelObjectDataType* TListModelBase<ModelDataType, ModelObjectDataType>::GetOrCreateCachedObject(int index) const
{
	ModelObjectDataType* retVal = nullptr;
	if (m_objectDataTypeMap.contains(index)){
		retVal = m_objectDataTypeMap[index];
	}
	else{
		retVal = new ModelObjectDataType();
		retVal->Version_1_0 = Version_1_0->at(index);
		m_objectDataTypeMap.insert(index, retVal);
	}
	
	return retVal;
}


// reimplemented (TListModelBase)

template <class ModelDataType, class ModelObjectDataType>
TListModelBase<ModelDataType, ModelObjectDataType>::
			TListModelBase(QObject *parent): QAbstractListModel(parent)
{
	Version_1_0.emplace();
}


// reimplemented (QAbstractListModel)

template <class ModelDataType, class ModelObjectDataType>
int TListModelBase<ModelDataType, ModelObjectDataType>::
			rowCount(const QModelIndex& /*parent*/) const
{
	if (Version_1_0.has_value()){
		return Version_1_0->count();
	}

	return 0;
}


template <class ModelDataType, class ModelObjectDataType>
QVariant TListModelBase<ModelDataType, ModelObjectDataType>::
			data(const QModelIndex& index, int role) const
{
	Q_UNUSED(role);

	int row = index.row();
	if (!Version_1_0.has_value() || row < 0 || row > Version_1_0->count() - 1){
		return QVariant();
	}

	ModelObjectDataType* retVal = GetOrCreateCachedObject(row);
	return QVariant::fromValue(retVal);
}


template <class ModelDataType, class ModelObjectDataType>
bool TListModelBase<ModelDataType, ModelObjectDataType>::
			setData(const QModelIndex& index, const QVariant& value, int role)
{
	Q_UNUSED(role);

	int row = index.row();

	if (!Version_1_0.has_value() || row < 0 || row > Version_1_0->count() - 1){
		return false;
	}

	Version_1_0->insert(row, value.value<ModelDataType>());

	return true;
}


template <class ModelDataType, class ModelObjectDataType>
QHash<int, QByteArray> TListModelBase<ModelDataType, ModelObjectDataType>::roleNames() const
{
	QHash<int, QByteArray> hash;
	hash.insert(0,"item");

	return hash;
}


} // namespace imtbase


