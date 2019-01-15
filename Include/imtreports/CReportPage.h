#pragma once


// Qt includes
#include <QtCore/QMap>

// ACF includes
#include <istd/TSmartPtr.h>

// imtreports includes
#include <imtreports/IReportPage.h>


namespace imtreports
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
		QByteArray uuid;
	};

	typedef QMap<QByteArray, PageElement> PageElementsMap;
	PageElementsMap m_pageElementsMap;
};


} // namespace imtreports


