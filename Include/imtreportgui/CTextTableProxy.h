// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
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

private:
	const imtreport::CTextTable& m_textTable;
};


} // namespace imtreportgui

