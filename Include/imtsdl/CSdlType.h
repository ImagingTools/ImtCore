#pragma once


// Qt includes
#include <QtCore/QString>

//Acf includes
#include <iser/ISerializable.h>
#include <imod/CModelUpdateBridge.h>

// ImtCore includes
#include <imtsdl/imtsdl.h>


namespace imtsdl
{


class CSdlType: public iser::ISerializable
{


public:
	QString GetName() const;
	void SetName(const QString& name);

	SdlFieldList GetFields() const;
	void SetFields(const SdlFieldList& fields);

	QString GetNamespace() const;
	void SetNamespace(const QString& aNamespace);

	// reimplemented(iser::ISerializable)
	virtual bool Serialize(iser::IArchive& archive) override;

	// operators
	bool operator==(const CSdlType& other) const;
	bool operator!=(const CSdlType& other) const { return !(operator==(other)); }


private:
	QString m_name;
	QString m_namespace;
	SdlFieldList m_fields;
};


}
