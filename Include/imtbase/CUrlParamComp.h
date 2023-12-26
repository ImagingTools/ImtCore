#pragma once


// ACF includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtbase/CUrlParam.h>


namespace imtbase
{


/**
	Component-based implementation of the URL parameter.
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
		I_ASSIGN(m_defaultSchemeAttrPtr, "DefaultScheme", "Default scheme", true, "http");
		I_ASSIGN(m_defaultUsernameAttrPtr, "DefaultUsername", "Default user name", true, "");
		I_ASSIGN(m_defaultPasswordAttrPtr, "DefaultPassword", "Default password", true, "");
		I_ASSIGN(m_defaultHostAttrPtr, "DefaultHost", "Default host", true, "");
		I_ASSIGN(m_defaultPortAttrPtr, "DefaultPort", "Default port", true, 80);
		I_ASSIGN(m_defaultPathAttrPtr, "DefaultPath", "Default path", true, "");
		I_ASSIGN(m_defaultQueryAttrPtr, "DefaultQuery", "Default query", true, "");
		I_ASSIGN(m_defaultFragmentAttrPtr, "DefaultFragment", "Default fragment", true, "");
		I_ASSIGN(m_isReadOnlyAttrPtr, "IsReadOnly", "When enabled, the URL is fixed and cannot be changed during run tume", true, false);
	I_END_COMPONENT;

	// reimplemented (IUrlParam)
	virtual bool SetUrl(const QUrl& url) override;
	virtual bool IsReadOnly() const override;

protected:
	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;

private:
	I_ATTR(QString, m_defaultHostAttrPtr);
	I_ATTR(QString, m_defaultSchemeAttrPtr);
	I_ATTR(QString, m_defaultPathAttrPtr);
	I_ATTR(QString, m_defaultQueryAttrPtr);
	I_ATTR(QString, m_defaultFragmentAttrPtr);
	I_ATTR(QString, m_defaultUsernameAttrPtr);
	I_ATTR(QString, m_defaultPasswordAttrPtr);
	I_ATTR(int, m_defaultPortAttrPtr);
	I_ATTR(bool, m_isReadOnlyAttrPtr);
};


} // namespace imtbase


