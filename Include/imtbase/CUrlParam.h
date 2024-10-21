#pragma once


// ACF includes
#include <ifile/IFileNameParam.h>

// ImtCore includes
#include <imtbase/IUrlParam.h>


namespace imtbase
{


class CUrlParam: virtual public IUrlParam, virtual public ifile::IFileNameParam
{
public:
	// reimplemented (IUrlParam)
	virtual const QUrl& GetUrl() const override;
	virtual bool SetUrl(const QUrl& url) override;
	virtual bool IsReadOnly() const override;

	// reimplemented (ifile::IFileNameParam)
	virtual int GetPathType() const override;
	virtual const QString& GetPath() const  override;
	virtual void SetPath(const QString& path)  override;

	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive& archive);

	// reimplemented (istd::IChangeable)
	virtual int GetSupportedOperations() const override;
	virtual bool CopyFrom(const IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
	virtual bool IsEqual(const IChangeable& object) const override;
	virtual IChangeable* CloneMe(CompatibilityMode mode = CM_WITHOUT_REFS) const override;
	virtual bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;

protected:
	QUrl m_url;
	mutable QString m_textRep;
};


} // namespace imtbase


