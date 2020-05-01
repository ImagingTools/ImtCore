#include <imtgui/CLayout.h>


// Qt includes
#include <QtCore/QDataStream>

// Acf includes
#include <iser/IArchive.h>
#include <iser/CPrimitiveTypesSerializer.h>
#include <iser/CArchiveTag.h>
#include <istd/CChangeNotifier.h>


namespace imtgui
{


// public methods

CLayout::CLayout() :
	m_parent(NULL),
	m_layoutType(LT_NONE)
{
}

CLayout::~CLayout()
{
	while (m_childs.count() > 0){
		delete TakeFirst();
	}
}

// reimplemented (icarmagui::ILayout)

void CLayout::SetLayoutId(const QByteArray& id)
{
	m_id = id;
}


QByteArray CLayout::GetLayoutId() const
{
	return m_id;
}


void CLayout::SetType(const LayoutType& type)
{
	m_layoutType = type;
}


ILayout::LayoutType CLayout::GetType() const
{
	return m_layoutType;
}


void CLayout::SetTitle(const QString& title)
{
	m_title = title;
}


QString CLayout::GetTitle() const
{
	return m_title;
}


void CLayout::SetTitleAlign(const AlignType& align)
{
	m_alignType = align;
}


ILayout::AlignType CLayout::GetTitleAlign() const
{
	return m_alignType;
}


void CLayout::SetIcon(const QIcon& icon)
{
	m_icon = icon;
}


QIcon CLayout::GetIcon() const
{
	return m_icon;
}


void CLayout::SetViewId(const QByteArray& viewId)
{
	m_viewId = viewId;
}


QByteArray CLayout::GetViewId() const
{
	return m_viewId;
}


void CLayout::SetSizes(const SizeList& sizes)
{
	m_sizes = sizes;
}


ILayout::SizeList CLayout::GetSizes() const
{
	return m_sizes;
}


ILayout* CLayout::GetParent()
{
	return m_parent;
}


ILayout* CLayout::GetRoot()
{
	if (m_parent == NULL){
		return this;
	}
	return m_parent->GetRoot();
}


int CLayout::GetChildsCount()
{
	return m_childs.count();
}


ILayout* CLayout::GetChild(int index)
{
	ILayout* retVal = NULL;
	if (index > -1 && index < m_childs.count()){
		retVal = m_childs.at(index);
	}
	return retVal;
}


void CLayout::InsertChild(int index, ILayout* layout)
{
	if (index < 0){
		m_childs.append(layout);
	}
	else{
		m_childs.insert(index, layout);
	}
}


void CLayout::AppendChild(ILayout* layout)
{
	InsertChild(-1, layout);
}


ILayout* CLayout::TakeChild(int index)
{
	ILayout* retVal = NULL;
	if (index > -1 && index < m_childs.count()){
		retVal = m_childs.takeAt(index);
	}
	return retVal;
}


ILayout* CLayout::TakeFirst()
{
	return TakeChild(0);
}


ILayout* CLayout::TakeLast()
{
	return TakeChild(m_childs.count() - 1);
}


void CLayout::Clear()
{
	while (TakeFirst() != NULL)
	{

	}
}


ILayout* CLayout::FindChild(const QByteArray& id)
{
	ILayout* retVal = NULL;
	if (m_id == id){
		return this;
	}

	for (ILayout* layout : m_childs)
	{
		retVal = layout->FindChild(id);
		if (retVal != NULL){
			break;
		}
	}

	return retVal;
}

// reimplemented (iser::ISerializable)

bool CLayout::Serialize(iser::IArchive& archive)
{
	bool retVal = true;
	LayoutType layoutType = m_layoutType;


	static iser::CArchiveTag layoutItemTag("LayoutItem", "Layout item");
	retVal = retVal && archive.BeginTag(layoutItemTag);

	static iser::CArchiveTag layoutItemTypeTag("LayoutItemType", "Layout item type");
	retVal = retVal && archive.BeginTag(layoutItemTypeTag);

	retVal = retVal && I_SERIALIZE_ENUM(LayoutType, archive, layoutType);
	retVal = retVal && archive.EndTag(layoutItemTypeTag);
	QByteArray sizeListAsByteArray;
	if ((layoutType == LayoutType::LT_HORIZONTAL_SPLITTER) || (layoutType == LayoutType::LT_VERTICAL_SPLITTER)) {
		static iser::CArchiveTag layoutSizeListTag("LayoutSizeList", "Layout item size list");
		retVal = retVal && archive.BeginTag(layoutSizeListTag);
		if (archive.IsStoring()) {
			QDataStream stream(&sizeListAsByteArray, QIODevice::WriteOnly);
			SizeList sizeList = GetSizes();
			stream << sizeList;
		}
		else {
			QDataStream stream(&sizeListAsByteArray, QIODevice::ReadWrite);
			SizeList sizeList;
			stream >> sizeList;
			SetSizes(sizeList);
		}

		retVal = retVal && archive.Process(sizeListAsByteArray);
		retVal = retVal && archive.EndTag(layoutSizeListTag);


		int childCount = GetChildsCount();

		static iser::CArchiveTag childItemGroupTag("ChildItems", "Child items");
		retVal = retVal && archive.BeginMultiTag(childItemGroupTag, layoutItemTag, childCount);

		if (!archive.IsStoring()) {
			while (m_childs.count() > 0){
				delete TakeFirst();
			}
		}

		for (int i = 0; i < childCount; i++) {
			ILayout *layout = NULL;
			if (archive.IsStoring()){
				layout = GetChild(i);
			}
			else{
				layout = new CLayout();
			}
			retVal = retVal && layout->Serialize(archive);
			if (!archive.IsStoring()) {
				AppendChild(layout);
			}
		}


		retVal = retVal && archive.EndTag(childItemGroupTag);

	}
	else {
		static iser::CArchiveTag herrachicalWidget("HerarchicalWidget", "Herarchical widget");
		retVal = retVal && archive.BeginTag(herrachicalWidget);

		static iser::CArchiveTag herrachicalId("Id", "Herarchical id");
		retVal = retVal && archive.BeginTag(herrachicalId);
		retVal = retVal && archive.Process(m_id);
		retVal = retVal && archive.EndTag(herrachicalId);

		static iser::CArchiveTag herrachicalTitle("Title", "Herarchical title");
		retVal = retVal && archive.BeginTag(herrachicalTitle);
		retVal = retVal && archive.Process(m_title);
		retVal = retVal && archive.EndTag(herrachicalTitle);

		static iser::CArchiveTag herrachicalViewId("ViewId", "Herarchical viewId");
		retVal = retVal && archive.BeginTag(herrachicalViewId);
		retVal = retVal && archive.Process(m_viewId);
		retVal = retVal && archive.EndTag(herrachicalViewId);

		static iser::CArchiveTag herrachicalTitleAlign("Title Align", "Herarchical title align");
		retVal = retVal && archive.BeginTag(herrachicalTitleAlign);
		retVal = retVal && I_SERIALIZE_ENUM(AlignType, archive, m_alignType);
		retVal = retVal && archive.EndTag(herrachicalTitleAlign);

		
		QByteArray iconAsByteArray; 
		if (archive.IsStoring()) {
			QDataStream stream(&iconAsByteArray, QIODevice::WriteOnly);
			stream << m_icon;
		}
		static iser::CArchiveTag herrachicalIcon("Icon", "Herarchical icon");
		retVal = retVal && archive.BeginTag(herrachicalIcon);
		retVal = retVal && archive.Process(iconAsByteArray);
		retVal = retVal && archive.EndTag(herrachicalIcon);

		retVal = retVal && archive.EndTag(herrachicalWidget);

		if (!archive.IsStoring()) {
			QDataStream stream(&iconAsByteArray, QIODevice::ReadWrite);
			stream >> m_icon;
		}

	}

	retVal = retVal && archive.EndTag(layoutItemTag);

	return retVal;

}


} // namespace icarmagui


