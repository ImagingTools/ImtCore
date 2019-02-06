#include <imtreport/CTextTable.h>


// ACF includes
#include <istd/CChangeNotifier.h>
#include <istd/TDelPtr.h>
#include <iser/IArchive.h>
#include <iser/CArchiveTag.h>
#include <iser/CPrimitiveTypesSerializer.h>


namespace imtreport
{


// private static members

const double CTextTable::s_defaultColumnWidth = 10.0;


// public methods

CTextTable::CTextTable() :
	m_showHorizontalHeader(true),
	m_showVerticalHeader(true)
{
}


void CTextTable::SetDimensions(int rowCount, int columnCount)
{
	istd::CChangeNotifier notifier(this, &istd::IChangeable::GetAllChanges());
	
	Resize(rowCount, columnCount);
}


int CTextTable::GetRowCount() const
{
	return m_items.isEmpty() ? 0 : m_items[0].size() - 1;
}


int CTextTable::GetColumnCount() const
{
	return m_items.isEmpty() ? 0 : m_items.size() - 1;
}


bool CTextTable::IsHorizontalHeaderVisible() const
{
	return m_showHorizontalHeader;
}


void CTextTable::ShowHorizontalHeader(bool show)
{
	istd::CChangeNotifier notifier(this, &istd::IChangeable::GetAllChanges());

	m_showHorizontalHeader = show;
}


bool CTextTable::IsVerticalHeaderVisible() const
{
	return m_showVerticalHeader;
}


void CTextTable::ShowVerticalHeader(bool show)
{
	istd::CChangeNotifier notifier(this, &istd::IChangeable::GetAllChanges());

	m_showVerticalHeader = show;
}


void CTextTable::SetHorizontalHeaderLabels(const QStringList& labels)
{
	Q_ASSERT(!labels.isEmpty());
	Q_ASSERT(labels.size() == GetColumnCount());

	istd::CChangeNotifier notifier(this, &istd::IChangeable::GetAllChanges());

	for (int col = 0; col < GetColumnCount(); col++){
		SetInternalItem(0, col + 1, CTextTableItem(labels[col], Qt::AlignCenter));
	}
}


void CTextTable::SetVerticalHeaderLabels(const QStringList& labels)
{
	Q_ASSERT(!labels.isEmpty());
	Q_ASSERT(labels.size() == GetRowCount());

	istd::CChangeNotifier notifier(this, &istd::IChangeable::GetAllChanges());

	for (int row = 0; row < GetRowCount(); row++){
		SetInternalItem(row + 1, 0, CTextTableItem(labels[row], Qt::AlignCenter));
	}
}


double CTextTable::GetColumnWidth(int column) const
{
	Q_ASSERT(column >= 0 && column < m_columnWidths.size());
	return m_columnWidths[column];
}


void CTextTable::SetColumnWidths(const QVector<double>& columnWidths)
{
	Q_ASSERT(columnWidths.size() == GetInternalColumnCount());

	istd::CChangeNotifier notifier(this, &istd::IChangeable::GetAllChanges());

	m_columnWidths = columnWidths;
}


const CTextTableItem& CTextTable::GetHorizontalHeaderItem(int column) const
{
	return GetInternalItem(0, column + 1);
}


void CTextTable::SetHorizontalHeaderItem(int column, const CTextTableItem& item)
{
	SetInternalItem(0, column + 1, item);
}


const CTextTableItem& CTextTable::GetVerticalHeaderItem(int row) const
{
	return GetInternalItem(row + 1, 0);
}


void CTextTable::SetVerticalHeaderItem(int row, const CTextTableItem& item)
{
	SetInternalItem(row + 1, 0, item);
}


const CTextTableItem& CTextTable::GetItem(int row, int column) const
{
	return GetInternalItem(row + 1, column + 1);
}


void CTextTable::SetItem(int row, int column, const CTextTableItem& item)
{
	SetInternalItem(row + 1, column + 1, item);
}


// reimplemented (iser::ISerializable)

bool CTextTable::Serialize(iser::IArchive& archive)
{
	istd::CChangeNotifier notifier(archive.IsStoring() ? NULL: this, &istd::IChangeable::GetAllChanges());
	Q_UNUSED(notifier);

	// base
	bool retVal = BaseClass::Serialize(archive);

	// header visibility
	static iser::CArchiveTag isHorizontalHeaderVisibleTag("IsHorizontalHeaderVisible", "Table horizontal header visibility", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(isHorizontalHeaderVisibleTag);
	retVal = retVal && archive.Process(m_showHorizontalHeader);
	retVal = retVal && archive.EndTag(isHorizontalHeaderVisibleTag);

	static iser::CArchiveTag isVerticalHeaderVisibleTag("IsVerticalHeaderVisible", "Table vertical header visibility", iser::CArchiveTag::TT_LEAF);
	retVal = archive.BeginTag(isVerticalHeaderVisibleTag);
	retVal = retVal && archive.Process(m_showVerticalHeader);
	retVal = retVal && archive.EndTag(isVerticalHeaderVisibleTag);

	// column widths
	retVal = retVal && iser::CPrimitiveTypesSerializer::SerializeContainer<QVector<double>>(archive, m_columnWidths, "ColumnWidths", "ColumnWidth");

	// items
	int rowCount = GetRowCount();
	int columnCount = GetColumnCount();

	static iser::CArchiveTag columnCountTag("TableColumnCount", "Table columns count", iser::CArchiveTag::TT_LEAF);
	retVal = archive.BeginTag(columnCountTag);
	retVal = retVal && archive.Process(columnCount);
	retVal = retVal && archive.EndTag(columnCountTag);

	static iser::CArchiveTag rowCountTag("TableRowCount", "Table rows count", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(rowCountTag);
	retVal = retVal && archive.Process(rowCount);
	retVal = retVal && archive.EndTag(rowCountTag);

	if (!archive.IsStoring()){
		m_items.clear();
		Resize(rowCount, columnCount);
	}

	static iser::CArchiveTag itemsTag("TableItems", "Table items", iser::CArchiveTag::TT_MULTIPLE);
	static iser::CArchiveTag itemTag("TableItem", "Table item", iser::CArchiveTag::TT_GROUP, &itemsTag);

	int count = rowCount * columnCount;
	retVal = retVal  && archive.BeginMultiTag(itemsTag, itemTag, count);

	for (int col = 0; col < columnCount && retVal; col++){
		for (int row = 0; row < rowCount && retVal; row++){
			retVal = retVal && archive.BeginTag(itemTag);
			retVal = retVal && m_items[col][row].Serialize(archive);
			retVal = retVal && archive.EndTag(itemTag);
		}
	}

	retVal = retVal && archive.EndTag(itemsTag);

	return retVal;
}


// reimplemented istd::IChangeable

int CTextTable::GetSupportedOperations() const
{
	return SO_COPY | SO_CLONE;
}


bool CTextTable::CopyFrom(const IChangeable& object, CompatibilityMode mode)
{
	const CTextTable* sourcePtr = dynamic_cast<const CTextTable*>(&object);

	if (sourcePtr != NULL){
		istd::CChangeNotifier changeNotifier(this);

		m_items = sourcePtr->m_items;
		m_showHorizontalHeader = sourcePtr->m_showHorizontalHeader;
		m_showVerticalHeader = sourcePtr->m_showVerticalHeader;
		m_columnWidths = sourcePtr->m_columnWidths;

		return BaseClass::CopyFrom(object, mode);
	}

	return false;
}


istd::IChangeable* CTextTable::CloneMe(CompatibilityMode mode) const
{
	istd::TDelPtr<CTextTable> clonePtr(new CTextTable());
	if (clonePtr->CopyFrom(*this, mode)){
		return clonePtr.PopPtr();
	}

	return nullptr;
}


// private methods

void CTextTable::Resize(int rowCount, int columnCount)
{
	Q_ASSERT(rowCount > 0);
	Q_ASSERT(columnCount > 0);

	m_items.resize(columnCount + 1);

	for (int col = 0; col < m_items.size(); col++){
		m_items[col].resize(rowCount + 1);
	}

	m_columnWidths.resize(GetInternalColumnCount());
	for (int col = 0; col < m_columnWidths.size(); col++){
		m_columnWidths[col] = s_defaultColumnWidth;
	}
}


int CTextTable::GetInternalColumnCount() const
{
	if (m_items.isEmpty()){
		return 0;
	}

	if (m_showVerticalHeader){
		return m_items.size();
	}
	else{
		return m_items.size() - 1;
	}
}


const CTextTableItem& CTextTable::GetInternalItem(int row, int column) const
{
	Q_ASSERT(column >= 0 && column < m_items.size());
	Q_ASSERT(row >= 0 && row < m_items[column].size());

	return m_items[column][row];
}


void CTextTable::SetInternalItem(int row, int column, const CTextTableItem& item)
{
	Q_ASSERT(column >= 0 && column < m_items.size());
	Q_ASSERT(row >= 0 && row < m_items[column].size());

	istd::CChangeNotifier notifier(this, &istd::IChangeable::GetAllChanges());

	m_items[column][row] = item;
}


} // namespace imtreport


