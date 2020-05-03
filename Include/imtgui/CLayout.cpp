#include <imtgui/CLayout.h>


// Qt includes
#include <QtCore/QDataStream>
#include <QtCore/QUuid>

// Acf includes
#include <iser/IArchive.h>
#include <iser/CPrimitiveTypesSerializer.h>
#include <iser/CArchiveTag.h>
#include <istd/CChangeNotifier.h>
#include <istd/CChangeGroup.h>


namespace imtgui
{


// public methods

CLayout::CLayout(CLayout* root) :
	m_parent(NULL),
	m_layoutType(LT_NONE),
	m_alignType(AT_LEFT),
	m_title("No name")
{
	m_id = QUuid::createUuid().toByteArray();
	if (root == NULL){
		m_root = this;
	}
	else{
		m_root = root;
	}
}

CLayout::~CLayout()
{
	while (m_childs.count() > 0){
		delete TakeFirst();
	}
}


void CLayout::SetSplitterLayout(ILayout::LayoutType type)
{
	istd::CChangeGroup changeGroup(m_root);
	//CLayout* layout = dynamic_cast<CLayout*>(m_root->FindChild(id));
	//layout = m_rootLayout;
	CLayout* parent = dynamic_cast<CLayout*>(GetParent());
	if (parent != NULL && parent->GetType() == ILayout::LT_NONE) {
		parent->AppendChild(new CLayout(m_root));
	}
	else{
		CLayout* newLayout = new CLayout(m_root);
		newLayout->SetLayoutId(GetLayoutId());
		newLayout->SetType(GetType());
		newLayout->SetTitleAlign(GetTitleAlign());
		newLayout->SetTitle(GetTitle());
		newLayout->SetIcon(GetIcon());
		newLayout->SetViewId(GetViewId());
		newLayout->SetSizes(GetSizes());

		SetLayoutId(QUuid::createUuid().toByteArray());
		SetType(type);
		SetTitleAlign(ILayout::AT_LEFT);
		SetTitle("");
		SetIcon(QPixmap());
		AppendChild(newLayout);
		newLayout = new CLayout(m_root);
		AppendChild(newLayout);
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
	istd::CChangeNotifier changeNotifier(m_root);
	m_title = title;
}


QString CLayout::GetTitle() const
{
	return m_title;
}


void CLayout::SetTitleAlign(const AlignType& align)
{
	istd::CChangeNotifier changeNotifier(m_root);
	m_alignType = align;
}


ILayout::AlignType CLayout::GetTitleAlign() const
{
	return m_alignType;
}


void CLayout::SetIcon(const QPixmap& icon)
{
	istd::CChangeNotifier changeNotifier(m_root);
	m_icon = icon;
}


QPixmap CLayout::GetIcon() const
{
	return m_icon;
}


void CLayout::SetViewId(const QByteArray& viewId)
{
	//istd::CChangeNotifier changeNotifier(m_root);
	m_viewId = viewId;
}


QByteArray CLayout::GetViewId() const
{
	return m_viewId;
}


void CLayout::SetSizes(const SizeList& sizes)
{
	istd::CChangeNotifier changeNotifier(m_root);
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
	//if (m_parent == NULL){
	//	return this;
	//}
	//return m_parent->GetRoot();
	return m_root;
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
	istd::CChangeNotifier changeNotifier(m_root);
	if (index < 0){
		m_childs.append(layout);
	}
	else{
		m_childs.insert(index, layout);
	}
	CLayout* cLayout = dynamic_cast<CLayout*>(layout);
	if (cLayout != NULL){
		cLayout->m_parent = this;
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
		istd::CChangeNotifier changeNotifier(m_root);
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
	istd::CChangeNotifier changeNotifier(m_root);
	while (m_childs.count() > 0){
		delete TakeFirst();
	}
	SetTitle("No name");
	SetIcon(QPixmap());
	SetType(LT_NONE);
	SetTitleAlign(AT_LEFT);
	SetViewId(QByteArray());
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


ILayout* CLayout::RemoveChild(const QByteArray& id)
{
	istd::CChangeGroup changeGroup(m_root);
	istd::CChangeNotifier changeNotifier(m_root);
	ILayout* retVal = NULL;
	CLayout* layout = dynamic_cast<CLayout*>(FindChild(id));
	if (layout != NULL){
		CLayout* parent = dynamic_cast<CLayout*>(layout->GetParent());
		if (parent != NULL){
			int index = parent->m_childs.indexOf(layout);
			if (index > -1){
				retVal = parent->m_childs.takeAt(index);
				if (parent->m_childs.count() == 1){
					layout = dynamic_cast<CLayout*>(parent->m_childs.takeFirst());
					CLayout* parentParent = dynamic_cast<CLayout*>(parent->GetParent());
					if (parentParent == NULL){
						parent->SetLayoutId(layout->GetLayoutId());
						parent->SetType(layout->GetType());
						parent->SetTitle(layout->GetTitle());
						parent->SetTitleAlign(layout->GetTitleAlign());
						parent->SetIcon(layout->GetIcon());
						parent->SetSizes(layout->GetSizes());
						parent->m_childs = layout->m_childs;
						while (layout->GetChildsCount() > 0){
							parent->InsertChild(0, layout->TakeLast());
						}
						parent->m_parent = NULL;
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


// reimplemented (iser::ISerializable)

bool CLayout::Serialize(iser::IArchive& archive)
{
	istd::CChangeGroup changeGroup(m_root);
	if (!archive.IsStoring()){
		Clear();
	}
	return InternalSerializeItemRecursive(archive);
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
	if ((m_layoutType == LayoutType::LT_HORIZONTAL_SPLITTER) || (m_layoutType == LayoutType::LT_VERTICAL_SPLITTER)) {
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
			while (m_childs.count() > 0) {
				delete TakeFirst();
			}
		}

		for (int i = 0; i < childCount; i++) {
			CLayout *layout = NULL;
			if (archive.IsStoring()) {
				layout = dynamic_cast<CLayout*>(GetChild(i));
			}
			else {
				layout = new CLayout(m_root);
			}
			retVal = retVal && layout->InternalSerializeItemRecursive(archive);
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


