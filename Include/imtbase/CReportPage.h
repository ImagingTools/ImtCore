#pragma once


// Qt includes
#include <QtCore/QMap>

// ACF includes
#include <istd/TSmartPtr.h>

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

	struct PageElement
	{
		istd::TSmartPtr<IGraphicsElement> elementPtr;
	};

	typedef QMap<QByteArray, PageElement> PageElementsMap;
	PageElementsMap m_pageElementsMap;
};


} // namespace imtbase


