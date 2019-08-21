#pragma once


// ACF includes
#include <istd/IChangeable.h>
#include <icomp/CComponentBase.h>
#include <imod/CModelUpdateBridge.h>
#include <iprm/COptionsManager.h>

// ImtCore includes
#include <imtbase/IObjectProvider.h>


namespace imtbase
{


/**
	Implementation of the sinle data-object provider.
*/
class CSingleObjectProviderComp:
			public icomp::CComponentBase,
			virtual public IObjectProvider
{
public:
	typedef icomp::CComponentBase BaseClass;
		
	I_BEGIN_COMPONENT(CSingleObjectProviderComp);
		I_REGISTER_INTERFACE(IObjectProvider);
		I_ASSIGN(m_dataObjectCompPtr, "Object", "Object", false, "Object");
		I_ASSIGN_TO(m_dataObjectModelCompPtr, m_dataObjectCompPtr, false);
		I_ASSIGN(m_typeIdAttrPtr, "TypeId", "Type-ID of the object", true, "Default");
		I_ASSIGN(m_objectNameAttrPtr, "ObjectName", "Name of the object", true, "");
		I_ASSIGN(m_objectDescriptionAttrPtr, "ObjectDescription", "Description of the object", true, "");
	I_END_COMPONENT;

	CSingleObjectProviderComp();

	// reimplemented (IObjectProvider)
	virtual const iprm::IOptionsList* GetObjectTypesInfo() const override;
	virtual QByteArray GetObjectTypeId(const QByteArray& objectId) const override;
	virtual const istd::IChangeable* GetDataObject(const QByteArray& objectId) const override;

	// reimplemented (ICollectionInfo)
	virtual Ids GetElementIds() const override;
	virtual QString GetElementName(const QByteArray & elementId) const override;
	virtual QString GetElementDescription(const QByteArray & elementId) const override;
	virtual bool IsElementEnabled(const QByteArray & elementId) const override;

protected:
	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;
	virtual void OnComponentDestroyed() override;

protected:
	I_REF(istd::IChangeable, m_dataObjectCompPtr);
	I_REF(imod::IModel, m_dataObjectModelCompPtr);
	I_ATTR(QByteArray, m_typeIdAttrPtr);
	I_ATTR(QString, m_objectNameAttrPtr);
	I_ATTR(QString, m_objectDescriptionAttrPtr);

private:
	imod::CModelUpdateBridge m_modelUpdateBridge;
	iprm::COptionsManager m_info;

	// Attribute shadows
	QByteArray m_typeId;
};


} // namespace imtbase


