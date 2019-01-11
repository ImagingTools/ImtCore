#pragma once


// ImtCore includes
#include <imtbase/IReportPage.h>


namespace imtbase
{


class CReportPage: virtual public IReportPage
{
public:
	// reimplemented (IReportPage)
	virtual ElementIds GetPageElements() const;
	virtual const IGraphicsElement* GetPageElement(const QByteArray& elementId) const;

	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive & archive) override;

protected:
};


} // namespace imtbase


