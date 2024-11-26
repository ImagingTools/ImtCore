#pragma once


// Qt includes
#include <QtCore/QString>

//Acf includes
#include <iser/ISerializable.h>
#include <imod/CModelUpdateBridge.h>
#include <iprm/IParamsSet.h>

// ImtCore includes
#include <imtsdl/imtsdl.h>
#include <imtsdl/CSdlField.h>



namespace imtsdl
{


class CSdlRequest: public iser::ISerializable
{


public:
	enum Type
	{
		T_QUERY,
		T_MUTATION
	};
	I_DECLARE_ENUM(Type, T_QUERY, T_MUTATION)

	Type GetType() const;
	void SetType(Type type);

	QString GetName() const;
	void SetName(const QString& name);

	SdlFieldList GetInputArguments() const;
	void SetInputArguments(const SdlFieldList& inputArguments);

	[[nodiscard]] QString GetSchemaFile() const;
	void SetSchemaFile(const QString& schemaFile);

	CSdlField GetOutputArgument() const;
	void SetOutputArgument(const CSdlField& outputArgument);

	[[nodiscard]] const iprm::IParamsSet& GetSchemaParams() const;
	void SetSchemaParamsPtr(const std::shared_ptr<iprm::IParamsSet>& schemaParamsPtr);

	// operators
	bool operator==(const CSdlRequest& other) const;
	bool operator!=(const CSdlRequest& other) const {return !(operator==(other));}

	// reimplemented(iser::ISerializable)
	virtual bool Serialize(iser::IArchive& archive) override;

private:
	Type m_type;
	QString m_name;
	QString m_schemaFile;
	SdlFieldList m_inputArguments;
	CSdlField m_outputArgument;
	std::shared_ptr<iprm::IParamsSet> m_schemaParamsPtr;
};


} // namespace imtsdl
