#pragma once


// Qt includes
#include <QtCore/QMap>

// ACF includes
#include <istd/TSmartPtr.h>
#include <ibase/TFactorisableContainer.h>

// imtreport includes
#include <imtreport/IReportPage.h>
#include <imtreport/IGraphicsElement.h>
#include <imtreport/CGraphicsElementFactory.h>

namespace imtreport
{


class CReportPage:
			virtual public IReportPage, 
			protected ibase::TFactorisableContainer<IGraphicsElement>
{
public:
	typedef ibase::TFactorisableContainer<IGraphicsElement> BaseClass;

	CReportPage();

	// reimplemented (IReportPage)
	ElementIds GetPageElements() const override;
	const IGraphicsElement* GetPageElement(const QByteArray& elementId) const override;
	QByteArray AddText(const QString& text, const i2d::CVector2d& position) override;
	QByteArray AddImage(const QString& imagePath, const i2d::CVector2d& position) override;
	bool RemovePageElement(const QByteArray& elementId) override;

	// reimplemented (istd::IChangeable)
	int GetSupportedOperations() const override;
	//bool CopyFrom(const IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
	//IChangeable* CloneMe(CompatibilityMode mode = CM_WITHOUT_REFS) const;
	//bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;

protected:
	// reimplemented (ibase::TFactorisableContainer)
	bool SerializeItem(ItemClass& item, iser::IArchive& archive, iser::CArchiveTag* parentTagPtr = NULL) override;

private:
	int FindItemIndex(const IGraphicsElement& element) const;

private:
	typedef QMap<QByteArray, int> ElementsIndiciesMap; // key - element uuid, value - element index in TContainer
	ElementsIndiciesMap m_elementsIndicies;

	CGraphicsElementFactory m_elementsFactory;
};


} // namespace imtreport


