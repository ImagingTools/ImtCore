#include <imtreportgui/CTextTableProxy.h>


namespace imtreportgui
{


// public methods

CTextTableProxy::CTextTableProxy(const imtreport::CTextTable& textTable)
	:m_textTable(textTable)
{
}


int CTextTableProxy::GetRowCount() const
{
	if (m_textTable.IsHorizontalHeaderVisible()){
		return m_textTable.GetRowCount() + 1;
	}
	else{
		return m_textTable.GetRowCount();
	}
}


int CTextTableProxy::GetColumnCount() const
{
	if (m_textTable.IsVerticalHeaderVisible()){
		return m_textTable.GetColumnCount() + 1;
	}
	else{
		return m_textTable.GetColumnCount();
	}
}


const imtreport::CTextTableItem* CTextTableProxy::GetItem(int row, int col) const
{
	Q_ASSERT(row >= 0 && row < GetRowCount());
	Q_ASSERT(col >= 0 && col < GetColumnCount());

	// both headers on
	if (m_textTable.IsHorizontalHeaderVisible() && m_textTable.IsVerticalHeaderVisible() && row == 0 && col == 0){
		return NULL;
	}
	else if (m_textTable.IsHorizontalHeaderVisible() && m_textTable.IsVerticalHeaderVisible() && row == 0 && col > 0){
		return &m_textTable.GetHorizontalHeaderItem(col - 1);
	}
	else if (m_textTable.IsHorizontalHeaderVisible() && m_textTable.IsVerticalHeaderVisible() && row > 0 && col == 0){
		return &m_textTable.GetVerticalHeaderItem(row - 1);
	}
	else if (m_textTable.IsHorizontalHeaderVisible() && m_textTable.IsVerticalHeaderVisible() && row > 0 && col > 0){
		return &m_textTable.GetItem(row - 1, col - 1);
	}
	// horizontal header on, vertical header off
	else if (m_textTable.IsHorizontalHeaderVisible() && !m_textTable.IsVerticalHeaderVisible() && row == 0){
		return &m_textTable.GetHorizontalHeaderItem(col);
	}
	else if (m_textTable.IsHorizontalHeaderVisible() && !m_textTable.IsVerticalHeaderVisible() && row > 0){
		return &m_textTable.GetItem(row - 1, col);
	}
	// horizontal header off, vertical header on
	else if (!m_textTable.IsHorizontalHeaderVisible() && m_textTable.IsVerticalHeaderVisible() && col == 0){
		return &m_textTable.GetVerticalHeaderItem(row);
	}
	else if (!m_textTable.IsHorizontalHeaderVisible() && m_textTable.IsVerticalHeaderVisible() && col > 0){
		return &m_textTable.GetItem(row, col - 1);
	}
	// both headers off
	else if (!m_textTable.IsHorizontalHeaderVisible() && !m_textTable.IsVerticalHeaderVisible()){
		return &m_textTable.GetItem(row, col);
	}
	else{
		return NULL;
	}
}


} // namespace imtreportgui


