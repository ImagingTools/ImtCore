#include <imtreport/CReportPage.h>


// Qt includes
#include <QtCore/QUuid>

// ACF includes
#include <istd/CChangeNotifier.h>
#include <istd/TDelPtr.h>
#include <iser/IArchive.h>
#include <iser/CArchiveTag.h>

// ImtCore includes
#include <imtreport/TGraphicsElement.h>
#include <imtreport/CTextTable.h>


namespace imtreport
{


// public methods

CReportPage::CReportPage()
{
	RegisterItemFactory(&m_elementsFactory);
}


// reimplemented (IReportPage)

IReportPage::ElementIds CReportPage::GetPageElements() const
{
	return m_elementsIndicies.keys();
}


i2d::IObject2d* CReportPage::GetPageElement(const QByteArray& elementId) const
{
	ElementsIndiciesMap::ConstIterator i = m_elementsIndicies.find(elementId);
	if (i != m_elementsIndicies.cend()){
		return BaseClass::GetElement(i.value());
	}

	return NULL;
}


QByteArray CReportPage::AddText(const QString& text, const i2d::CVector2d& position, const double textWidth /*= -1.0*/, const Qt::Alignment alignment /*= Qt::AlignLeft*/)
{
	QByteArray uuid;

	CTextLabelElement* element = AddObject<CTextLabelElement>(uuid);
	Q_ASSERT(element);

	element->SetFont(CFont("Arial", 5.0));
	element->SetText(text);
	element->SetPosition(position);
	element->SetRectangle(i2d::CRectangle(position.GetX(), position.GetY(), textWidth, 0.0));
	element->SetAlignment(alignment);

	return uuid;
}


QByteArray CReportPage::AddImage(const QString& imagePath, const i2d::CRectangle& rect)
{
	QByteArray uuid;

	CImageRectangleElement* element = AddObject<CImageRectangleElement>(uuid);
	Q_ASSERT(element);

	element->SetImagePath(imagePath);
	element->SetLeft(rect.GetLeft());
	element->SetRight(rect.GetRight());
	element->SetTop(rect.GetTop());
	element->SetBottom(rect.GetBottom());

	return uuid;
}


QByteArray CReportPage::AddLine(const i2d::CLine2d& line)
{
	QByteArray uuid;

	CLineElement* element = AddObject<CLineElement>(uuid);
	Q_ASSERT(element);

	element->SetPoint1(line.GetPoint1());
	element->SetPoint2(line.GetPoint2());

	return uuid;
}

QByteArray CReportPage::AddRectangle(const i2d::CRectangle& rect, const QColor& fillColor /*= Qt::transparent*/)
{
	QByteArray uuid;

	CRectangleElement* element = AddObject<CRectangleElement>(uuid);
	Q_ASSERT(element);

	element->SetLeft(rect.GetLeft());
	element->SetRight(rect.GetRight());
	element->SetTop(rect.GetTop());
	element->SetBottom(rect.GetBottom());
	element->SetFillColor(fillColor);

	return uuid;
}


QByteArray CReportPage::AddPolygone(const QVector<i2d::CVector2d>& points, const QColor& fillColor /*= Qt::transparent*/)
{
	QByteArray uuid;

	CPolygonElement* element = AddObject<CPolygonElement>(uuid);
	Q_ASSERT(element);

	for (const i2d::CVector2d& point : points){
		element->InsertNode(point);
	}

	element->SetFillColor(fillColor);

	return uuid;
}


QByteArray CReportPage::AddTextTable(const i2d::CRectangle& rect, int rowCount, int columnCount)
{
	QByteArray uuid;

	CTextTable* element = AddObject<CTextTable>(uuid);
	Q_ASSERT(element);

	element->SetDimensions(rowCount, columnCount);
	element->SetLeft(rect.GetLeft());
	element->SetRight(rect.GetRight());
	element->SetTop(rect.GetTop());
	element->SetBottom(rect.GetBottom());

	return uuid;
}


bool CReportPage::RemovePageElement(const QByteArray& elementId)
{
	if (m_elementsIndicies.contains(elementId)){
		istd::CChangeNotifier changeNotifier(this);

		int elementIndex = m_elementsIndicies.take(elementId);

		m_elementsIndicies.remove(elementId);
		BaseClass::RemoveAt(elementIndex);

		return true;
	}

	return false;
}


// reimplemented (istd::IChangeable)

int CReportPage::GetSupportedOperations() const
{
	return (SO_COPY | SO_CLONE | SO_RESET | SO_COMPARE);
}


bool CReportPage::CopyFrom(const IChangeable & object, CompatibilityMode mode)
{
	const CReportPage* sourcePtr = dynamic_cast<const CReportPage*>(&object);

	if (sourcePtr != nullptr){
		istd::CChangeNotifier changeNotifier(this);

		if (BaseClass::GetSupportedOperations() & SO_COPY){
			if (!BaseClass::CopyFrom(object, mode)){
				return false;
			}
		}

		m_elementsIndicies = sourcePtr->m_elementsIndicies;

		return true;
	}

	return false;
}


bool CReportPage::IsEqual(const IChangeable & object) const
{
	const CReportPage* sourcePtr = dynamic_cast<const CReportPage*>(&object);

	if (sourcePtr != nullptr){
		if (BaseClass::GetSupportedOperations() & SO_COMPARE){
			if (!BaseClass::IsEqual(object)){
				return false;
			}
		}

		return (m_elementsIndicies == sourcePtr->m_elementsIndicies);
	}

	return false;
}


istd::IChangeable* CReportPage::CloneMe(CompatibilityMode mode) const
{
	istd::TDelPtr<CReportPage> clonePtr(new CReportPage());

	if (clonePtr->CopyFrom(*this, mode)){
		return clonePtr.PopPtr();
	}

	return nullptr;
}


bool CReportPage::ResetData(CompatibilityMode mode)
{
	istd::CChangeNotifier changeNotifier(this);

	if (BaseClass::GetSupportedOperations() & SO_RESET){
		if (!BaseClass::ResetData(mode)){
			return false;
		}
	}

	m_elementsIndicies.clear();

	return true;
}


// reimplemented (ibase::TFactorisableContainer)

bool CReportPage::SerializeItem(ItemClass& item, iser::IArchive& archive, iser::CArchiveTag* /*parentTagPtr*/)
{
	bool retVal = true;

   i2d::IObject2d* elementPtr = item.first.GetPtr();
	Q_ASSERT(elementPtr != nullptr);

	QByteArray uuid;
	if (archive.IsStoring()){
		int index = FindItemIndex(*elementPtr);
		Q_ASSERT(index >= 0);
		Q_ASSERT(m_elementsIndicies.values().contains(index));

		for (ElementsIndiciesMap::ConstIterator iter = m_elementsIndicies.constBegin(); iter != m_elementsIndicies.constEnd(); ++iter){
			if (iter.value() == index){
				uuid = iter.key();
				break;
			}
		}
	}

	static iser::CArchiveTag uuidTag("Uuid", "UUID of the page item", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(uuidTag);
	retVal = retVal && archive.Process(uuid);
	retVal = retVal && archive.EndTag(uuidTag);

	static iser::CArchiveTag elementTag("PageElement", "Page element", iser::CArchiveTag::TT_GROUP);
	retVal = retVal && archive.BeginTag(elementTag);
	retVal = retVal && BaseClass::SerializeItem(item, archive, &elementTag);
	retVal = retVal && archive.EndTag(elementTag);

	if (retVal && !archive.IsStoring()){
		m_elementsIndicies.insert(uuid, BaseClass::GetItemsCount() - 1);
	}

	return retVal;
}


// private methods

int CReportPage::FindItemIndex(const i2d::IObject2d& element) const
{
	for (ElementsIndiciesMap::ConstIterator iter = m_elementsIndicies.constBegin(); iter != m_elementsIndicies.constEnd(); ++iter){
		int index = iter.value();

		const i2d::IObject2d* elementPtr = GetAt(index).first.GetPtr();
		if (elementPtr == &element){
			return index;
		}
	}

	return -1;
}


template <typename TObject> TObject* CReportPage::AddObject(QByteArray& uuid)
{
	uuid.clear();

	istd::CChangeNotifier changeNotifier(this);

	TObject* element = dynamic_cast<TObject*>(BaseClass::AddElement(TObject::GetTypeName()));
	if (!element){
		return nullptr;
	}

	uuid = QUuid::createUuid().toByteArray();

	m_elementsIndicies.insert(uuid, BaseClass::GetItemsCount() - 1);

	return element;
}


} // namespace imtreport


