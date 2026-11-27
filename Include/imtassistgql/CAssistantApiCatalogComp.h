// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <ilog/TLoggerCompWrap.h>

// ImtCore includes
#include <imtassistgql/IAssistantApiCatalog.h>


namespace imtassistgql
{


/**
	API catalog from the SDL files of the application (attribute \c SchemaPaths: files or
	directories, also in the resources), with the policy of the product:
	- queries: only \c AllowedQueries (none by default): some queries have effects, e.g. sending an e-mail;
	- mutations: only \c AllowedMutations (none by default);
	- never: \c ForbiddenCommands and commands whose names contain a word of authentication or secrets
	  (Password, Token, Session, Login, Logout, Authoriz, Superuser, Secret, Credential, Register);
	- fields whose names contain password, token, secret, apikey, privatekey, credential, salt or
	  hash are removed from results and refused in inputs (plus \c ForbiddenFields).

	\ingroup imtassistgql
*/
class CAssistantApiCatalogComp:
			public ilog::CLoggerComponentBase,
			virtual public IAssistantApiCatalog
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CAssistantApiCatalogComp);
		I_REGISTER_INTERFACE(IAssistantApiCatalog);
		I_ASSIGN_MULTI_0(m_schemaPathsAttrPtr, "SchemaPaths", "SDL files or directories with SDL files, e.g. :/AssistantApi/imtauth", false);
		I_ASSIGN_MULTI_0(m_allowedQueriesAttrPtr, "AllowedQueries", "Queries the assistant may use; none when empty", false);
		I_ASSIGN_MULTI_0(m_allowedMutationsAttrPtr, "AllowedMutations", "Mutations the assistant may plan; none when empty", false);
		I_ASSIGN_MULTI_0(m_forbiddenCommandsAttrPtr, "ForbiddenCommands", "Commands the assistant never uses", false);
		I_ASSIGN_MULTI_0(m_forbiddenFieldsAttrPtr, "ForbiddenFields", "Additional field names that are removed from results and refused in inputs", false);
	I_END_COMPONENT;

	/**
		Load the SDL files of a file or directory path.
		\return Number of loaded files.
	*/
	int LoadPath(const QString& path);

	// reimplemented (imtassistgql::IAssistantApiCatalog)
	virtual const CSdlApiCatalog& GetCatalog() const override;
	virtual bool IsCommandAllowed(const QString& commandName) const override;
	virtual bool IsFieldForbidden(const QString& fieldName) const override;

protected:
	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;

private:
	static bool Contains(const QByteArrayList& list, const QString& value);

private:
	I_MULTIATTR(QString, m_schemaPathsAttrPtr);
	I_MULTIATTR(QByteArray, m_allowedQueriesAttrPtr);
	I_MULTIATTR(QByteArray, m_allowedMutationsAttrPtr);
	I_MULTIATTR(QByteArray, m_forbiddenCommandsAttrPtr);
	I_MULTIATTR(QByteArray, m_forbiddenFieldsAttrPtr);

	CSdlApiCatalog m_catalog;
	QByteArrayList m_allowedQueries;
	QByteArrayList m_allowedMutations;
	QByteArrayList m_forbiddenCommands;
	QStringList m_forbiddenFields;
};


} // namespace imtassistgql


