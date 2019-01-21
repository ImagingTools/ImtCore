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


const IGraphicsElement* CReportPage::GetPageElement(const QByteArray& elementId) const
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

	CTextLabelElement* element = AddGraphicsElement<CTextLabelElement>(uuid);
	Q_ASSERT(element);

	element->SetFontSize(5);
	element->SetText(text);

	element->SetPosition(position);
	element->SetRectangle(i2d::CRectangle(position.GetX(), position.GetY(), textWidth, 0.0));
	element->SetFillColor(qRgb(255, 0, 0));
	element->SetStrokeColor(qRgb(0, 255, 255));
	element->SetAlignment(alignment);

	return uuid;
}


QByteArray CReportPage::AddImage(const QString& imagePath, const i2d::CRectangle& rect)
{
	QByteArray uuid;

	CImageRectangleElement* element = AddGraphicsElement<CImageRectangleElement>(uuid);
	Q_ASSERT(element);

	element->SetImagePath(imagePath);
	element->SetLeft(rect.GetLeft());
	element->SetRight(rect.GetRight());
	element->SetTop(rect.GetTop());
	element->SetBottom(rect.GetBottom());

	return uuid;
}


QByteArray CReportPage::AddRectangle(const i2d::CRectangle& rect, const QColor& fillColor /*= Qt::transparent*/)
{
	QByteArray uuid;

	CRectangleElement* element = AddGraphicsElement<CRectangleElement>(uuid);
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

	CPolygonElement* element = AddGraphicsElement<CPolygonElement>(uuid);
	Q_ASSERT(element);

	for (const i2d::CVector2d& point : points){
		element->InsertNode(point);
	}

	element->SetFillColor(fillColor);

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
	return (SO_COPY | SO_CLONE | SO_RESET);
}


// reimplemented (ibase::TFactorisableContainer)

bool CReportPage::SerializeItem(ItemClass& item, iser::IArchive& archive, iser::CArchiveTag* /*parentTagPtr*/)
{
	bool retVal = true;

	IGraphicsElement* elementPtr = item.first.GetPtr();
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

int CReportPage::FindItemIndex(const IGraphicsElement& element) const
{
	for (ElementsIndiciesMap::ConstIterator iter = m_elementsIndicies.constBegin(); iter != m_elementsIndicies.constEnd(); ++iter){
		int index = iter.value();

		const IGraphicsElement* elementPtr = GetAt(index).first.GetPtr();
		if (elementPtr == &element){
			return index;
		}
	}

	return -1;
}


template <typename TGraphicsElement> TGraphicsElement* CReportPage::AddGraphicsElement(QByteArray& uuid)
{
	uuid.clear();

	istd::CChangeNotifier changeNotifier(this);

	TGraphicsElement* element = dynamic_cast<TGraphicsElement*>(BaseClass::AddElement(TGraphicsElement::GetTypeName()));
	if (!element) {
		return nullptr;
	}

	uuid = QUuid::createUuid().toByteArray();

	m_elementsIndicies.insert(uuid, BaseClass::GetItemsCount() - 1);

	return element;
}


} // namespace imtreport


