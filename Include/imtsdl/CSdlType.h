#pragma once


// Qt includes
#include <QtCore/QString>

//Acf includes
#include <iser/ISerializable.h>

// ImtCore includes
#include <imtsdl/CSdlField.h>
#include <imtsdl/CSdlEntryBase.h>


namespace imtsdl
{


class CSdlType;
typedef QList<CSdlType> SdlTypeList;


class CSdlType: virtual public CSdlEntryBase
{

public:
	typedef CSdlEntryBase BaseClass;

	[[nodiscard]] SdlFieldList GetFields() const;
	void SetFields(const SdlFieldList& fields);

	// reimplemented(iser::ISerializable)
	virtual bool Serialize(iser::IArchive& archive) override;

	// operators
	[[nodiscard]] bool operator==(const CSdlType& other) const;
	[[nodiscard]] bool operator!=(const CSdlType& other) const { return !(operator==(other)); }

private:
	SdlFieldList m_fields;
};


}
