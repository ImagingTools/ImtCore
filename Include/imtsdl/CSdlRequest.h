// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
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


class CSdlRequest;
typedef QList<CSdlRequest> SdlRequestList;


class CSdlRequest: virtual public CSdlEntryBase
{

public:
	typedef CSdlEntryBase BaseClass;
	enum Type
	{
		T_QUERY,
		T_MUTATION,
		T_SUBSCRIPTION
	};
	I_DECLARE_ENUM(Type, T_QUERY, T_MUTATION, T_SUBSCRIPTION)

	CSdlRequest();
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
