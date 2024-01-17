#pragma once


// Qt includes
#include <QtCore/QString>

//Acf includes
#include <iser/ISerializable.h>

// imtsdl includes
#include <imtsdl/imtsdl.h>
#include <imtsdl/CSdlField.h>


namespace imtsdl
{


class CSdlMethod: public iser::ISerializable
{
public:
	QString GetName() const;
	void SetName(const QString& name);

	SdlFieldList GetArguments() const;
	void SetArguments(const SdlFieldList& arguments);

	QString GetOutputTypeId() const;
	void SetOutputTypeId(const QString& outputTypeId);

	// reimplemented(iser::ISerializable)
	virtual bool Serialize(iser::IArchive& archive) override;

private:
	QString m_name;
	SdlFieldList m_arguments;
	QString m_outputTypeId;
};


}
