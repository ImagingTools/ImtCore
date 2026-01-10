#pragma once


// ImtCore includes
#include <imtsdl/CSdlEntryBase.h>


namespace imtsdl
{


class CSdlEnum;
typedef QList<CSdlEnum> SdlEnumList;


class CSdlEnum: virtual public CSdlEntryBase
{

public:
	typedef CSdlEntryBase BaseClass;

	[[nodiscard]] QList<QPair<QString, QString>> GetValues() const;
	void SetValues(const QList<QPair<QString, QString>>& values);
	void AddValue(const QPair<QString, QString>& value);

	// reimplemented(iser::ISerializable)
	virtual bool Serialize(iser::IArchive& archive) override;

	// operators
	[[nodiscard]] bool operator==(const CSdlEnum& other) const;
	[[nodiscard]] bool operator!=(const CSdlEnum& other) const { return !(operator==(other)); }

private:
	QList<QPair<QString/*OriginalName*/, QString/*StringEquivalent*/>> m_values;
};



}
