#pragma once


// Qt includes
#include <QtCore/QString>

//Acf includes
#include <iser/ISerializable.h>
#include <imod/CModelUpdateBridge.h>
#include <iprm/IParamsSet.h>

// ImtCore includes
#include <imtsdl/imtsdl.h>
#include <imtsdl/CSdlEntryBase.h>
#include <imtsdl/CSdlField.h>



namespace imtsdl
{


class CSdlRequest: virtual public CSdlEntryBase
{

public:
	typedef CSdlEntryBase BaseClass;
	enum Type
	{
		T_QUERY,
		T_MUTATION
	};
	I_DECLARE_ENUM(Type, T_QUERY, T_MUTATION)

	Type GetType() const;
	void SetType(Type type);

	SdlFieldList GetInputArguments() const;
	void SetInputArguments(const SdlFieldList& inputArguments);

	CSdlField GetOutputArgument() const;
	void SetOutputArgument(const CSdlField& outputArgument);

	// operators
	bool operator==(const CSdlRequest& other) const;
	bool operator!=(const CSdlRequest& other) const {return !(operator==(other));}

	// reimplemented(iser::ISerializable)
	virtual bool Serialize(iser::IArchive& archive) override;

private:
	Type m_type;
	SdlFieldList m_inputArguments;
	CSdlField m_outputArgument;
};


} // namespace imtsdl
