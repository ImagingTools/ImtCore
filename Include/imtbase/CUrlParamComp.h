#pragma once


// ACF includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtbase/CUrlParam.h>


namespace imtbase
{


/**
	Component for automatic creation of the Universally Unique Identifier (UUID)
*/
class CUrlParamComp:
			public icomp::CComponentBase,
			public imtbase::CUrlParam
{
public:
	typedef icomp::CComponentBase BaseClass;
	typedef imtbase::CUrlParam BaseClass2;

	I_BEGIN_COMPONENT(CUrlParamComp);
		I_REGISTER_INTERFACE(IUrlParam);
		I_REGISTER_INTERFACE(iser::ISerializable);
		I_REGISTER_INTERFACE(istd::IChangeable);
		I_ASSIGN(m_defaultUrlAttrPtr, "DefaultUrl", "Default URL", false, "");
		I_ASSIGN(m_isReadOnlyAttrPtr, "IsReadOnly", "When enabled, the URL is fixed and cannot be changed during run tume", true, false);
	I_END_COMPONENT;

	// reimplemented (IUrlParam)
	virtual bool SetUrl(const QUrl& url) override;
	virtual bool IsReadOnly() const override;

protected:
	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated();

private:
	I_ATTR(QString, m_defaultUrlAttrPtr);
	I_ATTR(bool, m_isReadOnlyAttrPtr);
};


} // namespace imtbase


