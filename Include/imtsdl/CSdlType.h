#pragma once


// Qt includes
#include <QtCore/QString>

//Acf includes
#include <iser/ISerializable.h>
#include <imod/CModelUpdateBridge.h>

// imtsdl includes
#include <imtsdl/imtsdl.h>
#include <imtsdl/CSdlField.h>



namespace imtsdl
{


class CSdlType: public iser::ISerializable
{


public:
	QString GetName() const;
	void SetName(const QString& name);

	SdlFieldList GetFields() const;
	void SetFields(const SdlFieldList& fields);

	// reimplemented(iser::ISerializable)
	virtual bool Serialize(iser::IArchive& archive) override;

private:
	QString m_name;
	SdlFieldList m_fields;
};


}
