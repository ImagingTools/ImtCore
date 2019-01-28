#pragma once


// ACF includes
#include <i2d/CObject2dBase.h>

// ImtCore includes
#include <CTextTableItem.h>


namespace imtreport
{


class CTextTable: protected virtual CRectangle
{
public:
	typedef CRectangle BaseClass;

	explicit CTextTable(int rowCount, int columnCount); // including header

	void SetTopLeft(const CVector2d& topLeft);

	int GetRowCount() const;
	int GetColumnCount() const;

	void SetColumnHeaderLabels(const QStringList& labels);
	void SetColumnHeaderWidths(const QVector<double>& widths);

	const CTextTableItem& GetItem(int row, int column) const;
	void SetItem(int row, int column, const CTextTableItem& item);

	// reimplemented (iser::ISerializable)
	bool Serialize(iser::IArchive& archive) override;

	// reimplemented istd::IChangeable
	int GetSupportedOperations() const override;
	bool CopyFrom(const IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
	istd::IChangeable* CloneMe(CompatibilityMode mode = CM_WITHOUT_REFS) const override;

private:
	void Resize(int rowCount, int columnCount);
	void UpdateRect();

	double GetTableWidth() const;
	double GetTableHeight() const;

private:
	QVector<double> m_columnWidths;
	QVector<QVector<CTextTableItem>> m_items;
	
	static const double s_defaultColumnWidth;
};


} // namespace imtreport

