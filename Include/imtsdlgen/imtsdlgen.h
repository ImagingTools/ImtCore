#pragma once

// Qt includes
#include <QtCore/QList>


namespace imtsdlgen
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


/**
	\brief The QtResourceModelParamIds struct defines param IDs of a QT resource's (QRC) model
*/
struct QtResourceModelParamIds
{
	// prefix properties
	/// defines an ID of the "prefix name" parameter, represented as \c iprm::INameParam
	static const inline QByteArray PrefixName = QByteArrayLiteral("Name");

	/// defines an ID of the "Objects" parameter, represented as ilst of parameters i.e. \c iprm::IParamsManager of \c iprm::IParamsSet
	static const inline QByteArray QrcObjects = QByteArrayLiteral("Objects");

	// QRC-object properties
	/// defines an ID of the "QRC object alias" parameter, represented as \c iprm::INameParam
	static const inline QByteArray QrcAlias = QByteArrayLiteral("Alias");

	/// defines an ID of the "QRC object name" parameter - is a relative file path, represented as \c iprm::INameParam
	static const inline QByteArray QrcFileName = QByteArrayLiteral("FileName");
};


} // namespace imtsdlgen
