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
	if (item != nullptr){
		this->Version_1_0->append(static_cast<const ModelDataType&>(*item));
	}
	else {
		this->Version_1_0->AppendNull();
	}
	this->ClearCache();
	this->endInsertRows();
}


template <class ModelDataType, class ModelObjectDataType>
void TListModelBase<ModelDataType, ModelObjectDataType>::fromMe(TListModelBase<ModelDataType, ModelObjectDataType>* objectListPtr)
{
	for (int i = 0; i < this->rowCount(); i++){
		QVariant item = this->getData("item", i);
		if (!item.isValid() || item.isNull()){
			objectListPtr->Version_1_0->AppendNull();
			continue;
		}
		if (!item.canConvert<ModelObjectDataType*>()){
			return;
		}

		ModelObjectDataType* itemObjectPtr = item.value<ModelObjectDataType*>();
		if (itemObjectPtr == nullptr){
			objectListPtr->Version_1_0->AppendNull();
			continue;
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
		if (i > 0){
			retVal += QStringLiteral(", ");
		}

		QVariant item = this->getData("item", i);
		if (!item.isValid() || item.isNull()){
			retVal += QStringLiteral("null");
			continue;
		}
		if (!item.canConvert<ModelObjectDataType*>()){
			return nullptr;
		}

		ModelObjectDataType* itemObjectPtr = item.value<ModelObjectDataType*>();
		if (itemObjectPtr == nullptr){
			retVal += QStringLiteral("null");
			continue;
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
		if (i > 0){
			retVal += QStringLiteral(", ");
		}

		QVariant item = this->getData("item", i);
		if (!item.isValid() || item.isNull()){
			retVal += QStringLiteral("null");
			continue;
		}
		if (!item.canConvert<ModelObjectDataType*>()){
			return nullptr;
		}

		ModelObjectDataType* itemObjectPtr = item.value<ModelObjectDataType*>();
		if (itemObjectPtr == nullptr){
			retVal += QStringLiteral("null");
			continue;
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
		const bool selfItemIsNull = !selfItem.isValid() || selfItem.isNull();
		const bool otherItemIsNull = !otherItem.isValid() || otherItem.isNull();
		if (selfItemIsNull != otherItemIsNull){
			return false;
		}
		if (selfItemIsNull){
			continue;
		}
		if (!selfItem.canConvert<ModelObjectDataType*>() || !otherItem.canConvert<ModelObjectDataType*>()){
			return false;
		}

		ModelObjectDataType* selfItemObjectPtr = selfItem.value<ModelObjectDataType*>();
		ModelObjectDataType* otherItemObjectPtr = otherItem.value<ModelObjectDataType*>();
		if ((selfItemObjectPtr == nullptr) != (otherItemObjectPtr == nullptr)){
			return false;
		}
		if (selfItemObjectPtr == nullptr){
			continue;
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
	if (item != nullptr){
		this->Version_1_0->insert(index, static_cast<const ModelDataType&>(*item));
	}
	else {
		this->Version_1_0->InsertNull(index);
	}
	this->ClearCache();
	this->endInsertRows();
}


// reimplemented (TSdlAbstractListModel)
template <class ModelDataType, class ModelObjectDataType>
QVariant TListModelBase<ModelDataType, ModelObjectDataType>::GetOrCreateCachedObject(int index) const
{
	if (!this->Version_1_0 || index < 0 || index >= this->Version_1_0->size()){
		return QVariant();
	}

	QVariant retVal;
	if (this->m_objectDataTypeMap.contains(index)){
		retVal = QVariant::fromValue(this->m_objectDataTypeMap[index]);
	}
	else if (!this->Version_1_0->at(index)){
		return QVariant();
	}
	else{
		auto* newItem = new ModelObjectDataType();
		static_cast<ModelDataType&>(*newItem) = *(this->Version_1_0->at(index));
		this->m_objectDataTypeMap.insert(index, QVariant::fromValue(newItem));
		retVal = QVariant::fromValue(newItem);
	}

	return retVal;
}


} // namespace imtsdl


