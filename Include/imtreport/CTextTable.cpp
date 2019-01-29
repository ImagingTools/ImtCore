#include <imtreport/CTextTable.h>


// STL includes
#include <numeric>

// ACF includes
#include <istd/CChangeNotifier.h>
#include <istd/TDelPtr.h>
#include <iser/IArchive.h>
#include <iser/CArchiveTag.h>
#include <iser/CPrimitiveTypesSerializer.h>


namespace imtreport
{


// private static members

const double CTextTable::s_defaultColumnWidth = 30.0;


// public methods

CTextTable::CTextTable()
{
}


void CTextTable::Initialize(int rowCount, int columnCount)
{
	istd::CChangeNotifier notifier(this, &istd::IChangeable::GetAllChanges());
	
	Resize(rowCount, columnCount);

	UpdateRect();
}


void CTextTable::SetTopLeft(const i2d::CVector2d& topLeft)
{
	istd::CChangeNotifier notifier(this, &istd::IChangeable::GetAllChanges());

	BaseClass::SetTopLeft(topLeft);
}


int CTextTable::GetRowCount() const
{
	return m_items.isEmpty() ? 0 : m_items[0].size();
}


int CTextTable::GetColumnCount() const
{
	return m_items.size();
}


void CTextTable::SetColumnHeaderLabels(const QStringList& labels)
{
	Q_ASSERT(GetRowCount() > 0);

	istd::CChangeNotifier notifier(this, &istd::IChangeable::GetAllChanges());

	for (int col = 0; col < m_items.size(); col++){
		m_items[col][0].SetText(labels[col]);
		m_items[col][0].SetAlignment(Qt::AlignCenter);
	}
}


const QVector<double>& CTextTable::GetColumnHeaderWidths() const
{
	return m_columnWidths;
}


void CTextTable::SetColumnHeaderWidths(const QVector<double>& widths)
{
	Q_ASSERT(widths.size() == m_items.size());

	istd::CChangeNotifier notifier(this, &istd::IChangeable::GetAllChanges());

	m_columnWidths = widths;

	UpdateRect();
}


const CTextTableItem& CTextTable::GetItem(int row, int column) const
{
	Q_ASSERT(column > 0 && column < m_items.size());
	Q_ASSERT(row > 0 && row < m_items[column].size());

	return m_items[column][row];
}


void CTextTable::SetItem(int row, int column, const CTextTableItem& item)
{
	Q_ASSERT(column > 0 && column < m_items.size());
	Q_ASSERT(row > 0 && row < m_items[column].size());

	istd::CChangeNotifier notifier(this, &istd::IChangeable::GetAllChanges());

	m_items[column][row] = item;
	
	UpdateRect();
}


// reimplemented (iser::ISerializable)

bool CTextTable::Serialize(iser::IArchive& archive)
{
	istd::CChangeNotifier notifier(archive.IsStoring() ? NULL: this, &istd::IChangeable::GetAllChanges());
	Q_UNUSED(notifier);

	// base
	bool retVal = BaseClass::Serialize(archive);

	// column widths
//	retVal = retVal && iser::CPrimitiveTypesSerializer::SerializeContainer<QVector>(archive, m_columnWidths, "ColumnWidths", "ColumnWidth");

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

void CTextTable::Resize(const int rows, const int columns)
{
	Q_ASSERT(rows > 0);
	Q_ASSERT(columns > 0);

	m_items.resize(columns);
	m_columnWidths.resize(columns);

	for (int col = 0; col < m_items.size(); col++){
		m_items[col].resize(rows);
		m_columnWidths[col] = s_defaultColumnWidth;
	}
}


void CTextTable::UpdateRect()
{
	SetRight(GetLeft() + GetTableWidth());

	SetBottom(GetTop() + GetTableHeight());
}


double CTextTable::GetTableWidth() const
{
	return std::accumulate(m_columnWidths.begin(), m_columnWidths.end(), 0.0);
}


double CTextTable::GetTableHeight() const
{
	// calculate table height as sum of all rows heights,
	// row height is height of the highest cell in a row
	double retVal = 0.0;

	for (int col = 0; col < m_items.size(); col++){
		double cellHeightMax = 0.0;

		for (int row = 0; row < m_items.size(); row++){
			double cellHeight = m_items[col][row].GetHeight();
			if (cellHeight > cellHeightMax){
				cellHeightMax = cellHeight;
			}
		}

		retVal += cellHeightMax;
	}

	return retVal;
}


} // namespace imtreport

