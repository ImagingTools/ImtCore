#pragma once


// Qt includes
#include <QtCore/QMap>

// ACF includes
#include <istd/TSmartPtr.h>
#include <ibase/TFactorisableContainer.h>
#include <i2d/IObject2d.h>

// ImtCore includes
#include <imtreport/IReportPage.h>
#include <imtreport/CGraphicsElementFactory.h>

namespace imtreport
{


class CReportPage:
			virtual public IReportPage, 
			protected ibase::TFactorisableContainer<i2d::IObject2d>
{
public:
	typedef ibase::TFactorisableContainer<i2d::IObject2d> BaseClass;

	CReportPage();

	// reimplemented (IReportPage)
	ElementIds GetPageElements() const override;
   i2d::IObject2d* GetPageElement(const QByteArray& elementId) const override;
	QByteArray AddText(const QString& text, const i2d::CVector2d& position, const double textWidth = -1.0, const Qt::Alignment alignment = Qt::AlignLeft) override;
	QByteArray AddImage(const QString& imagePath, const i2d::CRectangle& rect) override;
	QByteArray AddLine(const i2d::CLine2d& line) override;
	QByteArray AddRectangle(const i2d::CRectangle& rect, const QColor& fillColor = Qt::transparent) override;
	QByteArray AddPolygone(const QVector<i2d::CVector2d>& points, const QColor& fillColor = Qt::transparent) override;
	QByteArray AddTextTable(const i2d::CRectangle& rect, int rowCount, int columnCount) override;

	bool RemovePageElement(const QByteArray& elementId) override;

	// reimplemented (istd::IChangeable)
	virtual int GetSupportedOperations() const override;
	virtual bool CopyFrom(const IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
	virtual bool IsEqual(const IChangeable& object) const override;
	virtual IChangeable* CloneMe(CompatibilityMode mode = CM_WITHOUT_REFS) const override;
	virtual bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;

protected:
	// reimplemented (ibase::TFactorisableContainer)
	bool SerializeItem(ItemClass& item, iser::IArchive& archive, iser::CArchiveTag* parentTagPtr = NULL) override;

private:
	int FindItemIndex(const i2d::IObject2d& element) const;

	template <typename TObject> TObject* AddObject(QByteArray& uuid);

	typedef QMap<QByteArray, int> ElementsIndiciesMap; // key - element uuid, value - element index in TContainer
	ElementsIndiciesMap m_elementsIndicies;

	CGraphicsElementFactory m_elementsFactory;
};


} // namespace imtreport


