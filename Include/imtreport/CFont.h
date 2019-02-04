#pragma once

// ACF includes
#include <iser/ISerializable.h>


namespace imtreport
{


class CFont: public iser::ISerializable
{
public:
	typedef iser::ISerializable BaseClass;

	enum FontFlags
	{
		FF_NORMAL = 0x0,
		FF_BOLD = 0x1,
		FF_ITALIC = 0x2,
		FF_UNDERLINE = 0x4
	};

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
	virtual int GetSupportedOperations() const;
	virtual bool CopyFrom(const IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS);
	virtual bool IsEqual(const IChangeable& object) const;
	virtual istd::IChangeable* CloneMe(CompatibilityMode mode = CM_WITHOUT_REFS) const;

private:
	QString m_name;
	double m_size;
	int m_fontFlags;
};


} // namespace imtreport


