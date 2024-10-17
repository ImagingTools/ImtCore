#pragma once


// ACF includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtbase/IUrlParam.h>


namespace imtbase
{


class CDelegatedUrlParamComp:
			public icomp::CComponentBase,
			virtual public imtbase::IUrlParam
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CDelegatedUrlParamComp);
		I_REGISTER_INTERFACE(IUrlParam);
		I_REGISTER_INTERFACE(iser::ISerializable);
		I_REGISTER_INTERFACE(istd::IChangeable);
		I_ASSIGN(m_urlParamCompPtr, "DelegateUrlParam", "Delegate URL param", true, "DelegateUrlParam");
	I_END_COMPONENT;

	// reimplemented (IUrlParam)
	virtual const QUrl& GetUrl() const override;
	virtual bool SetUrl(const QUrl& url) override;
	virtual bool IsReadOnly() const override;

	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive& archive) override;

private:
	I_REF(imtbase::IUrlParam, m_urlParamCompPtr);
};


} // namespace imtbase


