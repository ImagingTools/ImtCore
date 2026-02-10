// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <icomp/CComponentBase.h>
#include <ifile/IFileNameParam.h>

// ImtCore includes
#include <imtbase/IUrlParam.h>


namespace imtbase
{


class CDelegatedUrlParamComp:
			public icomp::CComponentBase,
			virtual public imtbase::IUrlParam,
			virtual public ifile::IFileNameParam

{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CDelegatedUrlParamComp);
		I_REGISTER_INTERFACE(IUrlParam);
		I_REGISTER_INTERFACE(ifile::IFileNameParam);
		I_REGISTER_INTERFACE(iser::ISerializable);
		I_REGISTER_INTERFACE(istd::IChangeable);
		I_ASSIGN(m_urlParamCompPtr, "DelegateUrlParam", "Delegate URL param", true, "DelegateUrlParam");
	I_END_COMPONENT;

	// reimplemented (IUrlParam)
	virtual const QUrl& GetUrl() const override;
	virtual bool SetUrl(const QUrl& url) override;
	virtual bool IsReadOnly() const override;

	// reimplemented (ifile::IFileNameParam)
	virtual int GetPathType() const override;
	virtual const QString& GetPath() const  override;
	virtual void SetPath(const QString& path)  override;

	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive& archive) override;

private:
	I_REF(imtbase::IUrlParam, m_urlParamCompPtr);
};


} // namespace imtbase


