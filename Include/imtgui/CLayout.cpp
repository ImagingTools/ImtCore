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


void CLayout::SetLayoutProperties(const LayoutProperties &properties)
{
	if (m_properties != properties){
		istd::CChangeNotifier changeNotifier(this);

		m_properties = properties;
	}
}


ILayout::LayoutProperties CLayout::GetLayoutProperties() const
{
	return m_properties;
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
	m_properties = LayoutProperties();
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
						parent->m_childs.clear();
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



void CLayout::SplitLayout(ILayout::LayoutType type, int width, int height)
{
	istd::CChangeGroup changeGroup(this);
	
	ILayout* parentLayoutPtr = GetParent();
	if ((parentLayoutPtr != nullptr) && parentLayoutPtr->GetType() == type){
		parentLayoutPtr->AppendChild(new CLayout(this));
	}
	else{
		// Create first sub-layout (copy from parent):
		CLayout* leftLayoutPtr = new CLayout(this);
		leftLayoutPtr->CopyData(this);

		m_id = QUuid::createUuid().toByteArray();

		// Reset parent layout:
		Clear();

		SetType(type);
		AppendChild(leftLayoutPtr);

		// Create second sub layout:
		CLayout* rightLayout = new CLayout(this);

		AppendChild(rightLayout);

		parentLayoutPtr = this;
	}

	ILayout::SizeList sizes;
	int size = 0;
	int count = parentLayoutPtr->GetChildsCount();
	if (type == LT_VERTICAL_SPLITTER && count > 0 && height > 0){
		size = height / count;
	}
	if (type == LT_HORIZONTAL_SPLITTER && count > 0 && width > 0){
		size = width / count;
	}
	if (size > 0 && count > 0){
		for (int i = 0; i < count; i++){
			sizes << size;
		}
		parentLayoutPtr->SetSizes(sizes);
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
	m_properties = source->GetLayoutProperties();
}


bool CLayout::InternalSerializeItemRecursive(iser::IArchive& archive)
{
	bool retVal = true;

	static iser::CArchiveTag layoutItemTypeTag("LayoutItemType", "Layout item type", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(layoutItemTypeTag);
	retVal = retVal && I_SERIALIZE_ENUM(LayoutType, archive, m_layoutType);
	retVal = retVal && archive.EndTag(layoutItemTypeTag);

	retVal = retVal && SerializeProperties(archive, m_properties);

	if ((m_layoutType == LayoutType::LT_HORIZONTAL_SPLITTER) || (m_layoutType == LayoutType::LT_VERTICAL_SPLITTER)){
		static iser::CArchiveTag uuidTag("UUID", "UUID for the layout item", iser::CArchiveTag::TT_LEAF);
		retVal = retVal && archive.BeginTag(uuidTag);
		retVal = retVal && archive.Process(m_id);
		retVal = retVal && archive.EndTag(uuidTag);

		static iser::CArchiveTag sizesTag("LayoutSizes", "Layout item size list", iser::CArchiveTag::TT_MULTIPLE);
		static iser::CArchiveTag subSizeTag("Size", "Size for item", iser::CArchiveTag::TT_LEAF, &sizesTag);

		SizeList sizeList = GetSizes();
		if (!archive.IsStoring()){
			sizeList.clear();
		}

		int countSize = sizeList.count();
		retVal = retVal && archive.BeginMultiTag(sizesTag, subSizeTag, countSize);
		for (int i = 0; i < countSize; i++){
			int size = 0;
			if (archive.IsStoring()){
				size = sizeList.at(i);
			}

			retVal = retVal && archive.BeginTag(subSizeTag);
			retVal = retVal && archive.Process(size);
			retVal = retVal && archive.EndTag(subSizeTag);

			if (retVal && !archive.IsStoring()){
				sizeList.append(size);
			}
		}

		retVal = retVal && archive.EndTag(sizesTag);

		if (retVal && !archive.IsStoring()){
			m_sizes = sizeList;
		}

		int childCount = GetChildsCount();

		static iser::CArchiveTag childItemGroupTag("Sublayouts", "Child items");
		static iser::CArchiveTag sublayoutItemInfoTag("Sublayout", "Layout item");

		retVal = retVal && archive.BeginMultiTag(childItemGroupTag, sublayoutItemInfoTag, childCount);

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

			retVal = retVal && archive.BeginTag(sublayoutItemInfoTag);

			retVal = retVal && layout->InternalSerializeItemRecursive(archive);
			if (!archive.IsStoring()){
				AppendChild(layout);
			}

			retVal = retVal && archive.EndTag(sublayoutItemInfoTag);
		}

		retVal = retVal && archive.EndTag(childItemGroupTag);
	}
	else{
		static iser::CArchiveTag layoutItemInfoTag("LayoutItemInfo", "Layout item informations", iser::CArchiveTag::TT_GROUP);
		retVal = retVal && archive.BeginTag(layoutItemInfoTag);

		static iser::CArchiveTag uuidTag("UUID", "UUID for the layout item", iser::CArchiveTag::TT_LEAF, &layoutItemInfoTag);
		retVal = retVal && archive.BeginTag(uuidTag);
		retVal = retVal && archive.Process(m_id);
		retVal = retVal && archive.EndTag(uuidTag);

		static iser::CArchiveTag viewTitleTag("Title", "Title of the view", iser::CArchiveTag::TT_LEAF, &layoutItemInfoTag);
		retVal = retVal && archive.BeginTag(viewTitleTag);
		retVal = retVal && archive.Process(m_title);
		retVal = retVal && archive.EndTag(viewTitleTag);

		static iser::CArchiveTag viewIdTag("ViewTypeId", "ID of the view type", iser::CArchiveTag::TT_LEAF, &layoutItemInfoTag);
		retVal = retVal && archive.BeginTag(viewIdTag);
		retVal = retVal && archive.Process(m_viewId);
		retVal = retVal && archive.EndTag(viewIdTag);

		static iser::CArchiveTag titleAlignmentTag("TitleAlignment", "Alignment type of the view title", iser::CArchiveTag::TT_LEAF, &layoutItemInfoTag);
		retVal = retVal && archive.BeginTag(titleAlignmentTag);
		retVal = retVal && I_SERIALIZE_ENUM(AlignType, archive, m_alignType);
		retVal = retVal && archive.EndTag(titleAlignmentTag);

		QByteArray iconAsByteArray;
		if (archive.IsStoring()){
			QDataStream stream(&iconAsByteArray, QIODevice::WriteOnly);
			stream << m_icon;
		}

		QByteArray iconAsBase64 = iconAsByteArray.toBase64();
		static iser::CArchiveTag viewIconTag("Icon", "View's icon", iser::CArchiveTag::TT_LEAF, &layoutItemInfoTag);
		retVal = retVal && archive.BeginTag(viewIconTag);
		retVal = retVal && archive.Process(iconAsBase64);
		retVal = retVal && archive.EndTag(viewIconTag);

		retVal = retVal && archive.EndTag(layoutItemInfoTag);

		if (!archive.IsStoring()){
			iconAsByteArray = QByteArray::fromBase64(iconAsBase64);
			QDataStream stream(&iconAsByteArray, QIODevice::ReadWrite);
			stream >> m_icon;
		}
	}

	return retVal;
}


bool CLayout::SerializeProperties(iser::IArchive& archive, LayoutProperties &properties)
{
	static iser::CArchiveTag propertiesTag("Properties", "Layout item properties", iser::CArchiveTag::TT_GROUP);
	bool retVal = archive.BeginTag(propertiesTag);

	static iser::CArchiveTag fixedLayoutTag("IsFixedLayout", "Fixed Layout", iser::CArchiveTag::TT_LEAF, &propertiesTag);
	retVal = retVal && archive.BeginTag(fixedLayoutTag);
	retVal = retVal && archive.Process(properties.isFixedLayout);
	retVal = retVal && archive.EndTag(fixedLayoutTag);

	static iser::CArchiveTag showBoxTag("IsShowBox", "Show box", iser::CArchiveTag::TT_LEAF, &propertiesTag);
	retVal = retVal && archive.BeginTag(showBoxTag);
	retVal = retVal && archive.Process(properties.isBorderEnabled);
	retVal = retVal && archive.EndTag(showBoxTag);

	QString nameColor = properties.borderColor.name();
	static iser::CArchiveTag borderColorTag("BorderColor", "Border color", iser::CArchiveTag::TT_LEAF, &propertiesTag);
	retVal = retVal && archive.BeginTag(borderColorTag);
	retVal = retVal && archive.Process(nameColor);
	retVal = retVal && archive.EndTag(borderColorTag);
	if (!archive.IsStoring()){
		properties.borderColor.setNamedColor(nameColor);
	}

	static iser::CArchiveTag minWidthTag("MinWidth", "Minimum width", iser::CArchiveTag::TT_LEAF, &propertiesTag);
	retVal = retVal && archive.BeginTag(minWidthTag);
	retVal = retVal && archive.Process(properties.minWidth);
	retVal = retVal && archive.EndTag(minWidthTag);

	static iser::CArchiveTag maxWidthTag("MaxWidth", "Maximum width", iser::CArchiveTag::TT_LEAF, &propertiesTag);
	retVal = retVal && archive.BeginTag(maxWidthTag);
	retVal = retVal && archive.Process(properties.maxWidth);
	retVal = retVal && archive.EndTag(maxWidthTag);

	static iser::CArchiveTag minHeightTag("MinHeight", "Minimum height", iser::CArchiveTag::TT_LEAF, &propertiesTag);
	retVal = retVal && archive.BeginTag(minHeightTag);
	retVal = retVal && archive.Process(properties.minHeight);
	retVal = retVal && archive.EndTag(minHeightTag);

	static iser::CArchiveTag maxHeightTag("MaxHeight", "Maximum height", iser::CArchiveTag::TT_LEAF, &propertiesTag);
	retVal = retVal && archive.BeginTag(maxHeightTag);
	retVal = retVal && archive.Process(properties.maxHeight);
	retVal = retVal && archive.EndTag(maxHeightTag);

	static iser::CArchiveTag leftMarginTag("LeftMargin", "Left margin", iser::CArchiveTag::TT_LEAF, &propertiesTag);
	retVal = retVal && archive.BeginTag(leftMarginTag);
	retVal = retVal && archive.Process(properties.leftMargin);
	retVal = retVal && archive.EndTag(leftMarginTag);

	static iser::CArchiveTag rightMarginTag("RightMargin", "Right margin", iser::CArchiveTag::TT_LEAF, &propertiesTag);
	retVal = retVal && archive.BeginTag(rightMarginTag);
	retVal = retVal && archive.Process(properties.rightMargin);
	retVal = retVal && archive.EndTag(rightMarginTag);

	static iser::CArchiveTag topMarginTag("TopMargin", "Top margin", iser::CArchiveTag::TT_LEAF, &propertiesTag);
	retVal = retVal && archive.BeginTag(topMarginTag);
	retVal = retVal && archive.Process(properties.topMargin);
	retVal = retVal && archive.EndTag(topMarginTag);

	static iser::CArchiveTag bottomMarginTag("BottomMargin", "Bottom margin", iser::CArchiveTag::TT_LEAF, &propertiesTag);
	retVal = retVal && archive.BeginTag(bottomMarginTag);
	retVal = retVal && archive.Process(properties.bottomMargin);
	retVal = retVal && archive.EndTag(bottomMarginTag);

	retVal = retVal && archive.EndTag(propertiesTag);

	return retVal;
}



QDataStream &operator<<(QDataStream &dataStream, const ILayout::LayoutProperties &src)
{
	dataStream << src.isFixedLayout << src.isBorderEnabled << src.borderColor;
	dataStream << src.minWidth << src.maxWidth << src.minHeight << src.maxHeight;
	return dataStream;
}


QDataStream &operator>>(QDataStream &dataStream, ILayout::LayoutProperties &src)
{
	dataStream >> src.isFixedLayout >> src.isBorderEnabled >> src.borderColor;
	dataStream >> src.minWidth >> src.maxWidth >> src.minHeight >> src.maxHeight;
	return dataStream;
}


bool operator ==(const ILayout::LayoutProperties &left, const ILayout::LayoutProperties &right)
{
	bool retVal = (left.isFixedLayout == right.isFixedLayout) && (left.isBorderEnabled == right.isBorderEnabled);
	retVal = retVal && (left.borderColor == right.borderColor);
	retVal = retVal && (left.minWidth == right.minWidth) && (left.maxWidth == right.maxWidth);
	retVal = retVal && (left.minHeight == right.minHeight) && (left.maxHeight == right.maxHeight);
	retVal = retVal && (left.leftMargin == right.leftMargin) && (left.rightMargin == right.rightMargin);
	retVal = retVal && (left.topMargin == right.topMargin) && (left.bottomMargin == right.bottomMargin);
	return retVal;
}


bool operator !=(const ILayout::LayoutProperties &left, const ILayout::LayoutProperties &right)
{
	return !(left == right);
}


} // namespace icarmagui


