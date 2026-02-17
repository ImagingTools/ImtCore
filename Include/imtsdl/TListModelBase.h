// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtsdl/TSdlAbstractListModel.h>


namespace imtsdl
{


template <class ModelDataType, class ModelObjectDataType>
class TListModelBase: public TSdlAbstractListModel<ModelDataType, ModelObjectDataType>
{
public:
	TListModelBase(QObject* parent = nullptr);
protected:
	void append(ModelObjectDataType* item);
	void fromMe(TListModelBase<ModelDataType, ModelObjectDataType>* objectListPtr);
	QString toJson();
	QString toGraphQL();
	bool isEqualWithModel(TListModelBase<ModelDataType, ModelObjectDataType>* otherModelPtr);
	void insert(int index, ModelObjectDataType* item);

	// reimplemented (TSdlAbstractListModel)
	virtual QVariant GetOrCreateCachedObject(int index) const override;
};


template <class ModelDataType, class ModelObjectDataType>
TListModelBase<ModelDataType, ModelObjectDataType>::TListModelBase(QObject* parent)
	: TSdlAbstractListModel<ModelDataType, ModelObjectDataType>(parent)
{

}


template <class ModelDataType, class ModelObjectDataType>
void TListModelBase<ModelDataType, ModelObjectDataType>::append(ModelObjectDataType* item){
	this->beginInsertRows(QModelIndex(), this->rowCount(), this->rowCount());
	this->Version_1_0->append(*item->Version_1_0);
	this->ClearCache();
	this->endInsertRows();
}


template <class ModelDataType, class ModelObjectDataType>
void TListModelBase<ModelDataType, ModelObjectDataType>::fromMe(TListModelBase<ModelDataType, ModelObjectDataType>* objectListPtr)
{
	for (int i = 0; i < this->rowCount(); i++){
		QVariant item = this->getData("item", i);
		if (!item.canConvert<ModelObjectDataType>()){
			return;
		}

		ModelObjectDataType* itemObjectPtr = item.value<ModelObjectDataType*>();
		if (itemObjectPtr == nullptr){
			return;
		}

		ModelObjectDataType* copyItem(dynamic_cast<ModelObjectDataType*>(itemObjectPtr->copyMe()));
		objectListPtr->append(copyItem);
	}
}


template <class ModelDataType, class ModelObjectDataType>
QString TListModelBase<ModelDataType, ModelObjectDataType>::toJson()
{
	QString retVal = QStringLiteral("[");

	for (int i = 0; i < this->rowCount(); i++){
		if (i > 0 && i < this->rowCount() - 1){
			retVal += QStringLiteral(", ");
		}

		QVariant item = this->getData("item", i);
		if (!item.canConvert<ModelObjectDataType*>()){
			return nullptr;
		}

		ModelObjectDataType* itemObjectPtr = item.value<ModelObjectDataType*>();
		if (itemObjectPtr == nullptr){
			return QString();
		}

		retVal += itemObjectPtr->toJson();
	}

	retVal += QStringLiteral("]");

	return retVal;
}


template <class ModelDataType, class ModelObjectDataType>
QString TListModelBase<ModelDataType, ModelObjectDataType>::toGraphQL()
{
	QString retVal = QStringLiteral("[");

	for (int i = 0; i < this->rowCount(); i++){
		if (i > 0 && i < this->rowCount() - 1){
			retVal += QStringLiteral(", ");
		}

		QVariant item = this->getData("item", i);
		if (!item.canConvert<ModelObjectDataType*>()){
			return nullptr;
		}

		ModelObjectDataType* itemObjectPtr = item.value<ModelObjectDataType*>();
		if (itemObjectPtr == nullptr){
			return QString();
		}

		retVal += itemObjectPtr->toGraphQL();
	}

	retVal += QStringLiteral("]");

	return retVal;
}


template <class ModelDataType, class ModelObjectDataType>
bool TListModelBase<ModelDataType, ModelObjectDataType>::isEqualWithModel(TListModelBase<ModelDataType, ModelObjectDataType>* otherModelPtr)
{
	if (otherModelPtr == nullptr){
		return false;
	}

	if (this == otherModelPtr){
		return false;
	}

	if (this->rowCount() != otherModelPtr->rowCount()){
		return false;
	}

	for (int i = 0; i < this->rowCount(); i++){
		QVariant selfItem = this->getData("item", i);
		QVariant otherItem = otherModelPtr->getData("item", i);
		if (!selfItem.canConvert<ModelObjectDataType>() || !otherItem.canConvert<ModelObjectDataType>()){
			return false;
		}

		ModelObjectDataType* selfItemObjectPtr = selfItem.value<ModelObjectDataType*>();
		if (selfItemObjectPtr == nullptr){
			return false;
		}

		ModelObjectDataType* otherItemObjectPtr = otherItem.value<ModelObjectDataType*>();
		if (otherItemObjectPtr == nullptr){
			return false;
		}

		if (!selfItemObjectPtr->isEqualWithModel(otherItemObjectPtr)){
			return false;
		}
	}

	return true;
}


template <class ModelDataType, class ModelObjectDataType>
void TListModelBase<ModelDataType, ModelObjectDataType>::insert(int index, ModelObjectDataType* item)
{
	if (index < 0 || index > this->Version_1_0->size()) {
		return;
	}
	this->beginInsertRows(QModelIndex(), index, index);
	this->Version_1_0->insert(index, *item->Version_1_0);
	this->ClearCache();
	this->endInsertRows();
}


// reimplemented (TSdlAbstractListModel)
template <class ModelDataType, class ModelObjectDataType>
QVariant TListModelBase<ModelDataType, ModelObjectDataType>::GetOrCreateCachedObject(int index) const
{
	if (!this->Version_1_0){
		this->Version_1_0.emplace();
	}
	QVariant retVal;
	if (this->m_objectDataTypeMap.contains(index)){
		retVal = QVariant::fromValue(this->m_objectDataTypeMap[index]);
	}
	else{
		auto* newItem = new ModelObjectDataType();
		newItem->Version_1_0 = this->Version_1_0->at(index);
		this->m_objectDataTypeMap.insert(index, QVariant::fromValue(newItem));
		retVal = QVariant::fromValue(newItem);
	}
	
	return retVal;
}


} // namespace imtsdl


