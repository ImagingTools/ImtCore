#pragma once


// ImtCore includes
#include <imtreport/CTextTable.h>


namespace imtreportgui
{


class CTextTableProxy
{
public:
	CTextTableProxy(const imtreport::CTextTable& textTable);

	int GetRowCount() const;
	int GetColumnCount() const;
	const imtreport::CTextTableItem* GetItem(int row, int col) const;

	double GetRowHeight(int row) const;
	double GetColumnWidth(int col) const;

private:
	const imtreport::CTextTable& m_textTable;
};


} // namespace imtreportgui

