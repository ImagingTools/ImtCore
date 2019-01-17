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


QByteArray CReportPage::AddText(const QString& text, const i2d::CVector2d& position)
{
	istd::CChangeNotifier changeNotifier(this);

	IGraphicsElement* newElement = BaseClass::AddElement(CTextLabelElement::GetTypeName());
	if (!newElement){
		return QByteArray();
	}

	CTextLabelElement* newTextLabel = dynamic_cast<CTextLabelElement*>(newElement);
	Q_ASSERT(newTextLabel);

	newTextLabel->SetFontSize(80);
	newTextLabel->SetText(text);
	newTextLabel->SetPosition(position);
	newTextLabel->SetFillColor(qRgb(255, 0, 0));
	newTextLabel->SetStrokeColor(qRgb(0, 255, 255));

	QByteArray uuid = QUuid::createUuid().toByteArray();

	m_elementsIndicies.insert(uuid, BaseClass::GetItemsCount() - 1);

	return uuid;
}


QByteArray CReportPage::AddImage(const QString& imagePath, const i2d::CVector2d& position)
{
	istd::CChangeNotifier changeNotifier(this);

	IGraphicsElement* newElement = BaseClass::AddElement(CImageRectangleElement::GetTypeName());
	if (!newElement){
		return QByteArray();
	}

	CImageRectangleElement* newTextLabel = dynamic_cast<CImageRectangleElement*>(newElement);
	Q_ASSERT(newTextLabel);

	newTextLabel->SetImagePath(imagePath);
	newTextLabel->SetLeft(0);
	newTextLabel->SetRight(500);
	newTextLabel->SetTop(400);
	newTextLabel->SetBottom(500);

	QByteArray uuid = QUuid::createUuid().toByteArray();

	m_elementsIndicies.insert(uuid, BaseClass::GetItemsCount() - 1);

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


// reimplemented (iser::ISerializable)

//bool CReportPage::Serialize(iser::IArchive& archive)
//{
//	static iser::CArchiveTag elementsTag("Elements", "Container of the page elements", iser::CArchiveTag::TT_MULTIPLE);
//	static iser::CArchiveTag elementTag("Elements", "Single page element", iser::CArchiveTag::TT_GROUP, &elementsTag);
//
//	int elementsCount = m_pageElementsMap.count();
//
//	bool retVal = archive.BeginMultiTag(elementsTag, elementTag, elementsCount);
//
//	if (archive.IsStoring()){
//		for (PageElementsMap::Iterator i = m_pageElementsMap.begin(); i != m_pageElementsMap.end(); ++i){
//			retVal = retVal && archive.BeginTag(elementTag);
//			retVal = retVal && i->IsValid() && i->GetPtr()->Serialize(archive);
//			retVal = retVal && archive.EndTag(elementTag);
//		}
//	}
//	else{
//		istd::CChangeNotifier changePtr(this);
//
//		ResetData();
//
//		for (PageElementsMap::ConstIterator i = m_pageElementsMap.cbegin(); i != m_pageElementsMap.cend(); ++i){
//			AddPageElement(NULL, NULL, elementIndex);
//
//			retVal = retVal && archive.BeginTag(elementTag);
//			retVal = retVal && Serializeelement(archive, m_pageElementsMap[elementIndex]);
//			retVal = retVal && archive.EndTag(elementTag);
//		}
//	}
//
//	retVal = retVal && archive.EndTag(elementsTag);
//
//	return retVal;
//}


// reimplemented (istd::IChangeable)

int CReportPage::GetSupportedOperations() const
{
	return (SO_COPY | SO_CLONE | SO_RESET);
}


//bool CReportPage::CopyFrom(const IChangeable& object, CompatibilityMode mode/* = CM_WITHOUT_REFS*/)
//{
//	const CReportPage* sourcePtr = dynamic_cast<const CReportPage*>(&object);
//	if (sourcePtr){
//		istd::CChangeNotifier changeNotifier(this);
//
//		m_pageElementsMap.clear();
//
//		for (PageElementsMap::ConstIterator i = sourcePtr->m_pageElementsMap.cbegin(); i != sourcePtr->m_pageElementsMap.cend(); ++i){
//			IGraphicsElement* pageElement = dynamic_cast<IGraphicsElement*>(i->GetPtr()->CloneMe());
//
//			if (AddPageElement(pageElement).isNull())
//				return false;
//		}
//
//		return BaseClass::CopyFrom(object, mode);
//	}
//
//	return false;
//}
//
//
//bool CReportPage::IsEqual(const IChangeable& object) const
//{
//	const CReportPage* otherPage = dynamic_cast<const CReportPage*>(&object);
//	if (!otherPage)
//		return false;
//
//	if (m_pageElementsMap.size() != otherPage->m_pageElementsMap.size())
//		return false;
//
//	PageElementsMap::ConstIterator i = m_pageElementsMap.cbegin();
//	PageElementsMap::ConstIterator j = otherPage->m_pageElementsMap.cbegin();
//
//	for (; i != m_pageElementsMap.cend(); ++i, ++j){
//		const IGraphicsElement* pageElement      = i->GetPtr();
//		const IGraphicsElement* otherPageElement = j->GetPtr();
//		if (!pageElement || !otherPageElement || !pageElement->IsEqual(*otherPageElement))
//			return false;
//	}
//
//	return BaseClass::IsEqual(object);
//}
//
//
//istd::IChangeable* CReportPage::CloneMe(CompatibilityMode mode/* = CM_WITHOUT_REFS*/) const
//{
//	istd::TDelPtr<CReportPage> clonePtr(new CReportPage);
//
//	if (clonePtr->CopyFrom(*this, mode))
//		return clonePtr.PopPtr();
//
//	return NULL;
//}
//
//
//bool CReportPage::ResetData(CompatibilityMode mode /*= CM_WITHOUT_REFS*/)
//{
//	Q_UNUSED(mode);
//
//	m_pageElementsMap.clear();
//
//	return true;
//}


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


} // namespace imtreport


