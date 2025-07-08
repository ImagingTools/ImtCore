#pragma once

// Qt includes
#include <QtCore/QAbstractListModel>

// Acf includes
#include <istd/TSharedNullable.h>



namespace imtbase
{


template <class ModelDataType, class ModelObjectDataType>
class TListModelBase: public QAbstractListModel
{
public:
	TListModelBase(QObject* parent = nullptr);
protected:
	// reimplemented (QAbstractListModel)
	virtual int rowCount(const QModelIndex&  parent = QModelIndex()) const override;
	virtual QVariant data(const QModelIndex&  index, int role = Qt::DisplayRole) const override;
	virtual bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole) override;
	virtual QHash<int, QByteArray> roleNames() const override;
public:
	// std::optional<QList<ModelDataType>>& m_modelData;
	// available version members
	istd::TSharedNullable<QList<ModelDataType>> Version_1_0;
};


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
	int row = index.row();

	qDebug() << "TListModelBase data" << index << role << Version_1_0.has_value();


	if (!Version_1_0.has_value() || row < 0 || row > Version_1_0->count() - 1){
		return QVariant();
	}
	ModelObjectDataType* retVal = new ModelObjectDataType();
	retVal->Version_1_0 = Version_1_0->at(row);

	return QVariant::fromValue(retVal);
}

template <class ModelDataType, class ModelObjectDataType>
bool TListModelBase<ModelDataType, ModelObjectDataType>::
			setData(const QModelIndex& index, const QVariant& value, int role)
{
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
