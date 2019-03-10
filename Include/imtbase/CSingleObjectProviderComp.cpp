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

const iprm::IOptionsList& CSingleObjectProviderComp::GetObjectInfoList() const
{
	return m_info;
}


const istd::IChangeable* CSingleObjectProviderComp::GetObject(const QByteArray& objectId) const
{
	int index = m_info.GetOptionIndexById(objectId);
	if ((index >= 0) && m_dataObjectCompPtr.IsValid()){
		return m_dataObjectCompPtr.GetPtr();
	}

	return nullptr;
}


QByteArray CSingleObjectProviderComp::GetObjectTypeId(const QByteArray& objectId) const
{
	int index = m_info.GetOptionIndexById(objectId);
	if (index >= 0){
		return m_typeId;
	}
	
	return QByteArray();
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


