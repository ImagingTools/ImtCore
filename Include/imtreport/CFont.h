#pragma once


// ACF includes
#include <iser/ISerializable.h>


namespace imtreport
{


class CFont: virtual public iser::ISerializable
{
public:
	enum FontFlags
	{
		FF_NORMAL = 0x0,
		FF_BOLD = 0x1,
		FF_ITALIC = 0x2,
		FF_UNDERLINE = 0x4
	};

	I_DECLARE_FLAGS(FontFlags, FF_NORMAL, FF_BOLD, FF_ITALIC, FF_UNDERLINE);

	CFont();
	CFont(const QString& name, double size, int fontFlags = FontFlags::FF_NORMAL);

	QString GetName() const;
	void SetName(const QString& name);

	double GetSize() const;
	void SetSize(double size);

	int GetFontFlags() const;
	void SetFontFlags(int fontFlags);

	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive& archive);

	// reimplemented istd::IChangeable
	virtual int GetSupportedOperations() const override;
	virtual bool CopyFrom(const IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
	virtual bool IsEqual(const IChangeable& object) const override;
	virtual IChangeable* CloneMe(CompatibilityMode mode = CM_WITHOUT_REFS) const override;
	virtual bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;

private:
	QString m_name;
	double m_size;
	int m_fontFlags;
};


} // namespace imtreport


