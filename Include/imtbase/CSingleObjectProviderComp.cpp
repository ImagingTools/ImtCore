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


// reimplemented (IElementList)

IElementList::Ids CSingleObjectProviderComp::GetElementIds() const
{
	Ids retVal;
	retVal.insert(m_info.GetOptionId(0));

	return retVal;
}


QString CSingleObjectProviderComp::GetElementName(const QByteArray & /*elementId*/) const
{
	return m_info.GetOptionName(0);
}


QString CSingleObjectProviderComp::GetElementDescription(const QByteArray & /*elementId*/) const
{
	return m_info.GetOptionDescription(0);
}


bool CSingleObjectProviderComp::IsElementEnabled(const QByteArray & /*elementId*/) const
{
	return m_info.IsOptionEnabled(0);
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


