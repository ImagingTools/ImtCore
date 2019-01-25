#pragma once


// Qt includes
//#include 

// ACF includes
//#include 

// ImtCore includes
//#include 


namespace imtreport
{


class CTableItem: public ITableItem
{
public:
	typedef ITableItem BaseClass;

	CTableItem();
	explicit CTableItem(const QString& text);

	// reimplemented (ITableItem)
	int GetRow() const override;
	int GetColumn() const override;

	QString GetText() const override;
	void SetText(const QString& text) override;

	Qt::Alignment GetAlignment() const override;
	void SetAlignment(const Qt::Alignment alignment) override;

	double GetFontSize() const override;
	void SetFontSize(const double fontSize) override;

	QString GetFontName() const override;
	void SetFontName(const QString& fontName) override;

	QColor GetForegroundColor() const override;
	void SetForegroundColor(const QColor& color) override;

	QColor GetBackgroundColor() const override;
	void SetBackgroundColor(const QColor& color) override;

	// reimplemented (istd::IChangeable)
	int GetSupportedOperations() const override;

protected:
	// reimplemented (iser::ISerializeable)
	// ................

private:
	int m_row = -1;
	int m_column = -1;
	QString m_text;
	Qt::Alignment m_alignment = Qt::LeftAlignment;
	double m_fontSize = 8.0;
	QString m_fontName;
	QColor m_foregroundColor = Qt::black;
};


} // namespace imtreport


