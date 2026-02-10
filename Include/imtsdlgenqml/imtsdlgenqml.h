// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


namespace imtsdlgenqml
{


/**
	\brief The QmldirModelParamIds struct defines param IDs of a QT QML module (qmldir) model
*/
struct QmldirModelParamIds
{

	// common param IDs
	/// defines an ID of the "module name" parameter, represented as \c iprm::INameParam
	static const inline QByteArray ModuleName = QByteArrayLiteral("ModuleName");

	/// defines an ID of the "Objects" parameter, represented as ilst of parameters i.e. \c iprm::IParamsManager of \c iprm::IParamsSet
	static const inline QByteArray Objects = QByteArrayLiteral("Objects");

	// object param IDs
	/// defines an ID of the "singleton" parameter, represented as \c ipem::IEnableableParam
	static const inline QByteArray ObjectIsSingleton = QByteArrayLiteral("IsSingleton");

	/// defines an ID of the "type name" parameter, represented as \c iprm::INameParam
	static const inline QByteArray ObjectTypeName = QByteArrayLiteral("TypeName");

	/// defines an ID of the "object version name" parameter, represented as \c iprm::INameParam
	static const inline QByteArray ObjectVersionName = QByteArrayLiteral("VersionName");

	/// defines an ID of the "object file name" parameter - is a relative file path, represented as \c iprm::INameParam
	static const inline QByteArray ObjectFileName = QByteArrayLiteral("FileName");
};


} // namespace imtsdlgenqml
