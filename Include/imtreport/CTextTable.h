#pragma once


// ACF includes
#include <i2d/CRectangle.h>

// ImtCore includes
#include <imtreport/CTextTableItem.h>


namespace imtreport
{


class CTextTable: public i2d::CRectangle
{
public:
	typedef i2d::CRectangle BaseClass;

	CTextTable();

	void SetDimensions(int rowCount, int columnCount);

	int GetRowCount() const;
	int GetColumnCount() const;

	bool IsHorizontalHeaderVisible() const;
	void ShowHorizontalHeader(bool show);

	bool IsVerticalHeaderVisible() const;
	void ShowVerticalHeader(bool show);

	void SetHorizontalHeaderLabels(const QStringList& labels);
	void SetVerticalHeaderLabels(const QStringList& labels);

	double GetColumnWidth(int column) const;
	void SetColumnWidths(const QVector<double>& columnWidths);

	const CTextTableItem& GetHorizontalHeaderItem(int column) const;
	void SetHorizontalHeaderItem(int column, const CTextTableItem& item);

	const CTextTableItem& GetVerticalHeaderItem(int row) const;
	void SetVerticalHeaderItem(int row, const CTextTableItem& item);

	const CTextTableItem& GetItem(int row, int column) const;
	void SetItem(int row, int column, const CTextTableItem& item);

	static QByteArray GetTypeName(){ return "TextTable"; }

	// reimplemented (iser::ISerializable)
	bool Serialize(iser::IArchive& archive) override;

	// reimplemented istd::IChangeable
	virtual int GetSupportedOperations() const override;
	virtual bool CopyFrom(const IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
	virtual bool IsEqual(const IChangeable& object) const override;
	virtual IChangeable* CloneMe(CompatibilityMode mode = CM_WITHOUT_REFS) const override;
	virtual bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;

private:
	void Resize(int rowCount, int columnCount);
	int GetInternalColumnCount() const;

	const CTextTableItem& GetInternalItem(int row, int column) const;
	void SetInternalItem(int row, int column, const CTextTableItem& item);

private:
	bool m_showHorizontalHeader;
	bool m_showVerticalHeader;
	QVector<double> m_columnWidths;
	QVector<QVector<CTextTableItem>> m_items;
	
	static const double s_defaultColumnWidth;
};


} // namespace imtreport

