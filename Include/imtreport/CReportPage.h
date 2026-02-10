// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QMap>

// ACF includes
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
	virtual ElementIds GetPageElements() const override;
	virtual i2d::IObject2d* GetPageElement(const QByteArray& elementId) const override;
	virtual QByteArray AddText(const QString& text, const i2d::CVector2d& position, const double textWidth = -1.0, const Qt::Alignment alignment = Qt::AlignLeft) override;
	virtual QByteArray AddImage(const QString& imagePath, const i2d::CRectangle& rect) override;
	virtual QByteArray AddLine(const i2d::CLine2d& line) override;
	virtual QByteArray AddRectangle(const i2d::CRectangle& rect, const QColor& fillColor = Qt::transparent) override;
	virtual QByteArray AddPolygone(const QVector<i2d::CVector2d>& points, const QColor& fillColor = Qt::transparent) override;
	virtual QByteArray AddTextTable(const i2d::CRectangle& rect, int rowCount, int columnCount) override;

	virtual bool RemovePageElement(const QByteArray& elementId) override;

	// reimplemented (istd::IChangeable)
	virtual int GetSupportedOperations() const override;
	virtual bool CopyFrom(const IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
	virtual bool IsEqual(const IChangeable& object) const override;
	virtual istd::IChangeableUniquePtr CloneMe(CompatibilityMode mode = CM_WITHOUT_REFS) const override;
	virtual bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;

protected:
	// reimplemented (ibase::TFactorisableContainer)
	virtual bool SerializeItem(ItemClass& item, iser::IArchive& archive, iser::CArchiveTag* parentTagPtr = NULL) override;

private:
	int FindItemIndex(const i2d::IObject2d& element) const;

	template <typename TObject>
	TObject* AddObject(QByteArray& uuid);

private:
	typedef QMap<QByteArray, int> ElementsIndiciesMap; // key - element uuid, value - element index in TContainer
	ElementsIndiciesMap m_elementsIndicies;

	CGraphicsElementFactory m_elementsFactory;
};


} // namespace imtreport


