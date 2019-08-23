#include <imtbase/CSingleObjectProviderComp.h>


// Qt includes
#include <QtCore/QUuid>


namespace imtbase
{


// public methods

CSingleObjectProviderComp::CSingleObjectProviderComp()
	:m_modelUpdateBridge(this)
{
}


// reimplemented (IObjectProvider)

const iprm::IOptionsList * CSingleObjectProviderComp::GetObjectTypesInfo() const
{
	return nullptr;
}


QByteArray CSingleObjectProviderComp::GetObjectTypeId(const QByteArray& /*objectId*/) const
{
	return *m_typeIdAttrPtr;
}


const istd::IChangeable* CSingleObjectProviderComp::GetDataObject(const QByteArray& objectId) const
{
	int index = m_info.GetOptionIndexById(objectId);
	if ((index >= 0) && m_dataObjectCompPtr.IsValid()){
		return m_dataObjectCompPtr.GetPtr();
	}

	return nullptr;
}


// reimplemented (ICollectionInfo)

ICollectionInfo::Ids CSingleObjectProviderComp::GetElementIds() const
{
	Ids retVal;
	retVal.push_back(m_info.GetOptionId(0));

	return retVal;
}


QVariant CSingleObjectProviderComp::GetElementInfo(const QByteArray& /*elementId*/, int infoType) const
{
	switch (infoType){
	case EIT_DESCRIPTION:
		return m_info.GetOptionName(0);

	case EIT_NAME:
		return m_info.GetOptionDescription(0);

	case EIT_ENABLED:
		return m_info.IsOptionEnabled(0);
	}

	return QVariant();
}


// protected methods

// reimplemented (icomp::CComponentBase)

void CSingleObjectProviderComp::OnComponentCreated()
{
	if (m_dataObjectModelCompPtr.IsValid()){
		m_dataObjectModelCompPtr->AttachObserver(&m_modelUpdateBridge);
	}

	m_info.InsertOption(*m_objectNameAttrPtr, QUuid::createUuid().toByteArray(), *m_objectDescriptionAttrPtr);

	m_typeId = *m_typeIdAttrPtr;
}


void CSingleObjectProviderComp::OnComponentDestroyed()
{
	m_modelUpdateBridge.EnsureModelsDetached();

	BaseClass::OnComponentDestroyed();
}


} // namespace imtbase


