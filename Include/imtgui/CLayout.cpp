#include <imtgui/CLayout.h>


// Qt includes
#include <QtCore/QDataStream>
#include <QtCore/QUuid>

// ACF includes
#include <iser/IArchive.h>
#include <iser/CPrimitiveTypesSerializer.h>
#include <iser/CArchiveTag.h>
#include <istd/CChangeNotifier.h>
#include <istd/CChangeGroup.h>


namespace imtgui
{


// public methods

CLayout::CLayout(CLayout* parentLayoutPtr)
	:m_parent(parentLayoutPtr),
	m_layoutType(LT_NONE),
	m_alignType(AT_LEFT),
	m_title("No name")
{
	// Delegate change notifications to the parent layout:
	SetSlavePtr(m_parent);

	m_id = QUuid::createUuid().toByteArray();
}


CLayout::~CLayout()
{
	while (m_childs.count() > 0){
		delete TakeFirst();
	}
}


// reimplemented (imtgui::ILayout)

QByteArray CLayout::GetLayoutId() const
{
	return m_id;
}


void CLayout::SetType(const LayoutType& type)
{
	if (m_layoutType != type){
		istd::CChangeNotifier changeNotifier(this);

		m_layoutType = type;
	}
}


ILayout::LayoutType CLayout::GetType() const
{
	return m_layoutType;
}


void CLayout::SetTitle(const QString& title)
{
	if (m_title != title){
		istd::CChangeNotifier changeNotifier(this);

		m_title = title;
	}
}


QString CLayout::GetTitle() const
{
	return m_title;
}


void CLayout::SetTitleAlign(const AlignType& align)
{
	if (m_alignType != align){
		istd::CChangeNotifier changeNotifier(this);
	
		m_alignType = align;
	}
}


ILayout::AlignType CLayout::GetTitleAlign() const
{
	return m_alignType;
}


void CLayout::SetIcon(const QPixmap& icon)
{
	istd::CChangeNotifier changeNotifier(this);

	m_icon = icon;
}


QPixmap CLayout::GetIcon() const
{
	return m_icon;
}


void CLayout::SetViewId(const QByteArray& viewId)
{
	if (m_viewId != viewId){
		istd::CChangeNotifier changeNotifier(this);

		m_viewId = viewId;
	}
}


QByteArray CLayout::GetViewId() const
{
	return m_viewId;
}


void CLayout::SetSizes(const SizeList& sizes)
{
	if (m_sizes != sizes){
		istd::CChangeNotifier changeNotifier(this);

		m_sizes = sizes;
	}
}


ILayout::SizeList CLayout::GetSizes() const
{
	return m_sizes;
}


ILayout* CLayout::GetParent() const
{
	return m_parent;
}


int CLayout::GetChildsCount() const
{
	return m_childs.count();
}


ILayout* CLayout::GetChild(int index) const
{
	ILayout* retVal = nullptr;

	if (index > -1 && index < m_childs.count()){
		retVal = m_childs.at(index);
	}
	return retVal;
}


void CLayout::InsertChild(int index, ILayout* layout)
{
	istd::CChangeNotifier changeNotifier(this);
	if (index < 0){
		m_childs.append(layout);
	}
	else{
		m_childs.insert(index, layout);
	}

	CLayout* cLayout = dynamic_cast<CLayout*>(layout);
	if (cLayout != nullptr){
		cLayout->m_parent = this;

		cLayout->SetSlavePtr(this);
	}
}


void CLayout::AppendChild(ILayout* layout)
{
	InsertChild(-1, layout);
}


ILayout* CLayout::TakeChild(int index)
{
	ILayout* retVal = nullptr;
	if (index > -1 && index < m_childs.count()){
		istd::CChangeNotifier changeNotifier(this);

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
	istd::CChangeNotifier changeNotifier(this);

	while (m_childs.count() > 0){
		delete m_childs.takeAt(0);
	}

	m_icon = QPixmap();
	m_title = "No name";
	m_layoutType = LT_NONE;
	m_alignType = AT_LEFT;
	m_viewId.clear();
	m_sizes.clear();
}


ILayout* CLayout::FindChild(const QByteArray& id) const
{
	ILayout* retVal = nullptr;
	if (m_id == id){
		return const_cast<CLayout*>(this);
	}

	for (ILayout* layout : m_childs){
		retVal = layout->FindChild(id);
		if (retVal != nullptr){
			break;
		}
	}

	return retVal;
}


ILayout* CLayout::RemoveChild(const QByteArray& id)
{
	istd::CChangeNotifier changeNotifier(this);

	ILayout* retVal = nullptr;
	CLayout* layout = dynamic_cast<CLayout*>(FindChild(id));
	if (layout != nullptr){
		CLayout* parent = dynamic_cast<CLayout*>(layout->GetParent());
		if (parent != nullptr){
			int index = parent->m_childs.indexOf(layout);
			if (index > -1){
				retVal = parent->m_childs.takeAt(index);
				if (parent->m_childs.count() == 1){
					layout = dynamic_cast<CLayout*>(parent->m_childs.takeFirst());
					CLayout* parentParent = dynamic_cast<CLayout*>(parent->GetParent());
					if (parentParent == nullptr){
						parent->CopyData(layout);
						parent->m_childs = layout->m_childs;
						while (layout->GetChildsCount() > 0){
							parent->InsertChild(0, layout->TakeLast());
						}
						parent->m_parent = nullptr;
						delete layout;
					}
					else{
						index = parentParent->m_childs.indexOf(parent);
						if (index > -1){
							delete parentParent->m_childs.takeAt(index);
							parentParent->InsertChild(index, layout);
						}
					}
				}
			}
		}
		else{
			Clear();
		}

	}
	return retVal;
}



void CLayout::SplitLayout(ILayout::LayoutType type)
{
	istd::CChangeGroup changeGroup(this);
	
	ILayout* parentLayoutPtr = GetParent();

	if ((parentLayoutPtr != nullptr) && parentLayoutPtr->GetType() == ILayout::LT_NONE){
		parentLayoutPtr->AppendChild(new CLayout(this));
	}
	else{
		// Create first sub-layout (copy from parent):
		CLayout* leftLayoutPtr = new CLayout(this);
		leftLayoutPtr->CopyData(this);

		// Reset parent layout:
		Clear();

		m_id = QUuid::createUuid().toByteArray();
		
		SetType(type);
		AppendChild(leftLayoutPtr);

		// Create second sub layout:
		CLayout* rightLayout = new CLayout(this);

		AppendChild(rightLayout);
	}
}


// reimplemented (iser::ISerializable)

bool CLayout::Serialize(iser::IArchive& archive)
{
	istd::CChangeNotifier changeNotifier(archive.IsStoring() ? nullptr : this);

	if (!archive.IsStoring()){
		Clear();
	}

	return InternalSerializeItemRecursive(archive);
}


// private methods

void CLayout::CopyData(ILayout* source)
{
	m_id = source->GetLayoutId();
	m_layoutType = source->GetType();
	m_alignType = source->GetTitleAlign();
	m_title = source->GetTitle();
	m_icon = source->GetIcon();
	m_viewId = source->GetViewId();
	m_sizes = source->GetSizes();
}


bool CLayout::InternalSerializeItemRecursive(iser::IArchive& archive)
{
	bool retVal = true;

	static iser::CArchiveTag layoutItemTag("LayoutItem", "Layout item");
	retVal = retVal && archive.BeginTag(layoutItemTag);

	static iser::CArchiveTag layoutItemTypeTag("LayoutItemType", "Layout item type");
	retVal = retVal && archive.BeginTag(layoutItemTypeTag);

	retVal = retVal && I_SERIALIZE_ENUM(LayoutType, archive, m_layoutType);
	retVal = retVal && archive.EndTag(layoutItemTypeTag);
	QByteArray sizeListAsByteArray;
	if ((m_layoutType == LayoutType::LT_HORIZONTAL_SPLITTER) || (m_layoutType == LayoutType::LT_VERTICAL_SPLITTER)){
		static iser::CArchiveTag layoutSizeListTag("LayoutSizeList", "Layout item size list");
		retVal = retVal && archive.BeginTag(layoutSizeListTag);
		if (archive.IsStoring()){
			QDataStream stream(&sizeListAsByteArray, QIODevice::WriteOnly);
			SizeList sizeList = GetSizes();
			stream << sizeList;
		}

		retVal = retVal && archive.Process(sizeListAsByteArray);

		if (!archive.IsStoring()) {
			QDataStream stream(&sizeListAsByteArray, QIODevice::ReadWrite);
			SizeList sizeList;
			stream >> sizeList;
			SetSizes(sizeList);
		}

		retVal = retVal && archive.EndTag(layoutSizeListTag);

		int childCount = GetChildsCount();

		static iser::CArchiveTag childItemGroupTag("ChildItems", "Child items");
		retVal = retVal && archive.BeginMultiTag(childItemGroupTag, layoutItemTag, childCount);

		if (!archive.IsStoring()){
			while (m_childs.count() > 0){
				delete TakeFirst();
			}
		}

		for (int i = 0; i < childCount; i++){
			CLayout *layout = nullptr;
			if (archive.IsStoring()){
				layout = dynamic_cast<CLayout*>(GetChild(i));
			}
			else{
				layout = new CLayout(this);
			}
			retVal = retVal && layout->InternalSerializeItemRecursive(archive);
			if (!archive.IsStoring()){
				AppendChild(layout);
			}
		}

		retVal = retVal && archive.EndTag(childItemGroupTag);
	}
	else{
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
		if (archive.IsStoring()){
			QDataStream stream(&iconAsByteArray, QIODevice::WriteOnly);
			stream << m_icon;
		}

		static iser::CArchiveTag herrachicalIcon("Icon", "Herarchical icon");
		retVal = retVal && archive.BeginTag(herrachicalIcon);
		retVal = retVal && archive.Process(iconAsByteArray);
		retVal = retVal && archive.EndTag(herrachicalIcon);

		retVal = retVal && archive.EndTag(herrachicalWidget);

		if (!archive.IsStoring()){
			QDataStream stream(&iconAsByteArray, QIODevice::ReadWrite);
			stream >> m_icon;
		}
	}

	retVal = retVal && archive.EndTag(layoutItemTag);

	return retVal;
}


} // namespace icarmagui


