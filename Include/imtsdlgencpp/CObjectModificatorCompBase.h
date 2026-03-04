// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtsdl/CSdlEnumConverter.h>
#include <imtsdlgencpp/CSdlClassModificatorBaseComp.h>
#include <imtsdlgencpp/CSdlGenTools.h>
#include <imtsdlgencpp/CSdlUnionConverter.h>


namespace imtsdlgencpp
{


/**
	The base C++ class modificator, adds saving and loading generated SDL types to/from 'object'
	\warning ALL modificators MUST inherit from this class!
 */
class CObjectModificatorCompBase:
			public CSdlClassModificatorBaseComp,
			protected CSdlGenTools,
			protected imtsdl::CSdlTools,
			protected imtsdl::CSdlEnumConverter,
			protected CSdlUnionConverter
{
	using CSdlGenTools::AddArrayInternalChecksFail;

public:
	typedef CSdlClassModificatorBaseComp BaseClass;

	I_BEGIN_BASE_COMPONENT(CObjectModificatorCompBase)
	I_END_COMPONENT;

protected:
	enum MetdodType
	{
		MT_READ,
		MT_WRITE,
		MT_OPT_READ,
		MT_OPT_WRITE
	};

	/*!
		\brief The ListAccessResult used as a storage for list methods. See methods for details, uses it.
		\sa AddContainerListAccessCode
		\c listVariableName				- a name of variable, that defines a storage list
		\c listCountVariableName		- a name of variable, that defines a  number of elements
		\c listCountVariableType		- a type of \c listCountVariableName
		\c customListAccessCode			- a code, used to access items, by index it can be a NULL; \warning MUST contain placeholder $(index) - will be replaced to correct index
		\c customAccessedElementName	- a name of element, extracted, by \c customListAccessCode \warning MUST be non-empty if \c customAccessedElementName is non-empty
		\c toObjectTransformMethod		- a method, used to convert a 'object_value' to 'object_type'. Can be NULL if no convertation is required (if container a \c QList). The returned type of a method MUST be same as in \c GetContainerObjectClassName()
		\warning \c customListAccessMethodName MUST NOT contain endian brackets! \example "at"; NOT ".at"; NOT "at()"; NOT ".at()"
		\warning \c toObjectTransformMethod MUST be complete! WITH endian brackets! \example ".toJsonObject()" - for \c QJsonArray \c QJsonObject \sa QJsonValue
	 */
	struct ListAccessResult
	{
		QString listVariableName;
		QString listCountVariableName;
		QString listCountVariableType;
		QString customListAccessCode;
		QString customAccessedElementName;
		bool isCustomAccessedElementPointer = false;
		QString toObjectTransformMethod;
	};

protected:
	// abstract methods
	/*!
		\brief Method, defines an object container class
		\warning a container class MUST have a default constructor!
		\example QJsonObject
		\example ::imtbase::CTreeItemModel
		\returns FULL object container class name with namespace (if exists)
	 */
	virtual QString GetContainerObjectClassName() const = 0;

	/*!
		\brief Method, defines an object container variable name, to be used for naming variables for methods, temp variables
		\warning value MUST be a valid C variable name i.e. single word
		\example jsonObject
		\returns an object container class variable name
	 */
	virtual QString GetContainerObjectVariableName() const = 0;

	/*!
		\brief Same as \c GetContainerObjectClassName, but only for arrays/lists (sclar types only)
		\example QJsonArray
		\note it could be the same as \c GetContainerObjectClassName
		\sa GetContainerObjectClassName
	 */
	virtual QString GetScalarArrayContainerObjectClassName() const = 0;

	/*!
		\brief Same as \c GetScalarArrayContainerObjectClassName, but for custom types only
		\example QJsonArray
		\note it could be the same as \c GetScalarArrayContainerObjectClassName
		\sa GetScalarArrayContainerObjectClassName
	 */
	virtual QString GetCustomArrayContainerObjectClassName() const = 0;

	/*!
		\brief Same as \c GetContainerObjectVariableName, but only for arrays/lists
		\warning it MUST NOT be identical to \c GetContainerObjectVariableName!
		\sa GetContainerObjectVariableName
	 */
	virtual QString GetArrayContainerObjectVariableName() const = 0;

	/*!
		\brief Writes code, that sets a scalar value to an object
		\param field - field to set
		\param variableName - variable name, that contains a value to be set, conforms to a \c field
		\param horizontalIndents - for multiline generation, defines, how much tabs SHOULD be inserted after at each new line (except for the first one)

		\code // example of C++ generated code
			jsonObject["value"] = *m_value; ///< 'jsonObject' - MUST be same as \c GetContainerObjectVariableName() 'value' - could be got from \c field (field.GetName()); '*m_value' - \c variableName
		\endcode

		\warning this mthod SHOULD NOT add leading and ending horizontal/vertical indents. The stream is already at the correct position.
		\returns true - if write to \c stream is succesfull and false otherwise
	 */
	virtual bool AddFieldValueWriteToObject(QTextStream& stream, const imtsdl::CSdlField& field, const QString& variableName, uint horizontalIndents) const = 0;

	/*!
		\brief Same as \c AddFieldValueWriteToObject, but only for object's container as object i.e. for custom user's types
		\sa AddFieldValueWriteToObject

		\code
			// C++ code before
			QJsonObject newValueJsonObject;
			// somehow create a valid JSON...
			// example of C++ generated code
			jsonObject["value"] = newValueJsonObject; ///< 'jsonObject' - MUST be same as \c GetContainerObjectVariableName() 'value' - can be obtained from \c field (field.GetName()); 'newValueJsonObject' - \c variableName
		\endcode
	 */
	virtual bool AddObjectValueWriteToObject(QTextStream& stream, const imtsdl::CSdlField& field, const QString& variableName, uint horizontalIndents) const = 0;

	/*!
		\brief This method is used to create an array to store objects of the same type as the \c field.

		\code //example of C++ generated code
			QJsonArray newValuesArray; ///< in this case this method MUST return "newValuesArray"!
		\endcode

		\code //another example of C++ generated code for mote complex object
			/// This is a specific example of context-dependent creation.
			::imtbase::CTreeItemModel* newValuesModelPtr = model.AddTreeModel("values", modelIndex); ///< 'values' - can be obtained from \c field (field.GetName())
			///	In this case this method MUST return "newValuesModelPtr"
		\endcode

		\returns a name of created container if operation is successful and a NULL string otherwise
		\warning returned value MUST be uniquie for each fieldId \sa CSdlField::GetId()
		\sa QString::isNull()
	 */
	virtual QString AddCreationOfTemporaryArray(QTextStream& stream, const imtsdl::CSdlField& field) const = 0;

	/*!
		\brief Writes code, that appends a scalar value to an object
		\param field - field to set
		\param arrayContainerVariableName - name of an array, to store elements, types of a \c field
		\param variableName - variable name, that contains a value to be added to an array \c arrayContainerVariableName, conforms to a \c field
		\param horizontalIndents - for multiline generation, defines, how much tabs SHOULD be inserted after at each new line (except for the first one)

		\code // example of C++ generated code
			valuesJsonArray <<  aValue; ///< 'valuesJsonArray' - \c arrayContainerVariableName 'aValue' - \c variableName;
		\endcode

		\warning this mthod SHOULD NOT add leading and ending horizontal/vertical indents. The stream is already at the correct position.
		\returns true - if write to \c stream is succesfull and false otherwise
	 */
	virtual bool AddFieldValueAppendToObjectArray(QTextStream& stream, const imtsdl::CSdlField& field, const QString& arrayContainerVariableName, const QString& variableName, uint horizontalIndents) const = 0;

	/*!
		\brief writes code, that sets an array \c variableName to an object \c targetObjectVariableName
		\code
			// C++ code before
			QJsonArray newValuesArray;
			// somehow fill an array
			// example of C++ generated code
			jsonObject["values"] = newValuesArray; ///< 'jsonObject' - \c targetObjectVariableName (if is empty, MUST be same as \c GetContainerObjectVariableName()); 'values' - can be obtained from \c field (field.GetName()); 'newValuesArray' - \c variableName
		\endcode
	 */
	virtual bool AddArrayWriteToObject(QTextStream& stream, const imtsdl::CSdlField& field, const QString& variableName, const QString& targetObjectVariableName, uint horizontalIndents) const = 0;

	/*!
		\brief Writes a begin of code, that checks whether the object contains a value
		\param expected - if true, that mens, the value MUST exists in the object
		\note this method MUST Depending on the type, generate a different result, even if it is complex type

		\code // example of C++ generated code if \c expected == true
			if (jsonObject.contains("value") && jsonObject["value"].isString()){
		\endcode

		\code // example of C++ generated code if \c expected == false
			if (!jsonObject.contains("value") || !jsonObject["value"].isString()){
		\endcode
	 */
	virtual bool AddContainerValueCheckConditionBegin(QTextStream& stream, const imtsdl::CSdlField& field, bool expected, quint16 horizontalIndents) const = 0;

	/*!
		\brief Writes a begin of code, that reads a value from an object
		\warning this method MUST NOT write endian semicolon(;)!

		\code // example of C++ generated code
			jsonObject["value"].toString() ///< 'jsonObject' - \c variableName (if is empty, MUST be same as \c GetContainerObjectVariableName()); 'value' - can be obtained from \c field (field.GetName());
		\endcode
	 */
	virtual bool AddContainerValueReadFromObject(QTextStream& stream, const imtsdl::CSdlField& field, const QString& variableName, quint16 horizontalIndents) const = 0;

	/*!
		\brief Writes code, that obtains a value from an object as list

		\code // example of C++ generated code
			const QJsonArray vsluesArray = jsonObject["values"].toArray();
			const qsizetype valuesArrayCount = valuesArray.size();
		\endcode

		\details this method MUST fill \c result with correct values
					\c ListAccessResult::listVariableName				= "vsluesArray", because it is a list storage
					\c ListAccessResult::listCountVariableName			= "valuesArrayCount", because it indicates a  number of elements
					\c ListAccessResult::listCountVariableType			= "qsizetype", because a type of count variable
					\c ListAccessResult::customListAccessMethodName		= {NULL|"at"}, because the \c QJsonArray (a container of objects) uses operator[] or method at() to access items by index
					\c ListAccessResult::toObjectTransformMethod		= ".toJsonObject()", bacause access by index returns \c QJsonValue, but we need a \c QJsonObject in case JSON for custom types
					\c ListAccessResult::toObjectTransformMethod		= ".toString()", bacause access by index returns \c QJsonValue, but we need a \c QString in case JSON if a \c field is string
	 */
	virtual bool AddContainerListAccessCode(QTextStream& stream, const imtsdl::CSdlField& field, const QString& variableName, quint16 horizontalIndents, ListAccessResult& result) const = 0;

	/*!
		\brief Writes code that writes the typename to an object
		\code // example of C++ generated code
			jsonObject["__typename"] = QStringLiteral("TypeName");
		\endcode
	*/
	virtual void WriteTypenameToObjectCode(QTextStream& stream, const imtsdl::CSdlType& sdlType) const = 0;

	virtual CSdlUnionConverter::ConversionType GetUnionScalarConversionType() const = 0;
	virtual CSdlUnionConverter::ConversionType GetUnionArrayConversionType() const = 0;

	// additional methods
	/// \todo refactor it and describe
	virtual QString GetUnionListElementType(bool forScalar) const ;
	virtual void AddUnionFieldValueReadFromObject(QTextStream& stream, const imtsdl::CSdlField& field, bool optional, quint16 hIndents) const = 0 ;
	virtual void AddUnionFieldValueWriteToObject(QTextStream& stream, const imtsdl::CSdlField& field, bool optional, quint16 hIndents) const = 0 ;


	// comfort methods
	void WriteMethodCall(QTextStream& stream, MetdodType type, const QString& objectName = QString(), bool isPointer = false) const;
	std::shared_ptr<imtsdl::CSdlEntryBase> FindEntryByName(const QString& entryName, bool onlyLocal = false) const;

	/// Simply generates a start of code, that sets a value to struct like \code *object.value = \endcode
	void WriteSetValueToStruct(QTextStream& stream, const imtsdl::CSdlField& field, const QString& objectName = QStringLiteral("object")) const;

	// reimplemented (imtsdlgencpp::CSdlClassModificatorBaseComp)
	virtual bool ProcessSourceClassFile(const imtsdl::CSdlType& sdlType, QIODevice* headerDevicePtr, const iprm::IParamsSet* paramsPtr) const override;
	virtual bool ProcessHeaderClassFile(const imtsdl::CSdlType& sdlType, QIODevice* sourceDevicePtr, const iprm::IParamsSet* paramsPtr) const override;

private:
	// basic helper methods
	void AddFieldWriteToObjectCode(QTextStream& stream, const imtsdl::CSdlField& field, bool optional) const;
	void AddFieldReadFromObjectCode(QTextStream& stream, const imtsdl::CSdlField& field, bool optional) const;

	// write helpers
	void AddScalarFieldWriteToObjectCode(QTextStream& stream, const imtsdl::CSdlField& field, bool optional, bool isEnum, bool isUnion) const;
	void AddCustomFieldWriteToObjectCode(QTextStream& stream, const imtsdl::CSdlField& field, bool optional) const;
	void AddCustomFieldWriteToObjectImplCode(QTextStream& stream, const imtsdl::CSdlField& field, bool optional, quint16 hIndents = 1) const;
	void AddArrayFieldWriteToObjectCode(QTextStream& stream, const imtsdl::CSdlField& field, bool optional) const;
	void AddArrayFieldWriteToObjectImplCode(QTextStream& stream, const imtsdl::CSdlField& field, bool optional, quint16 hIndents = 1) const;
	void AddCustomArrayFieldWriteToObjectCode(QTextStream& stream, const imtsdl::CSdlField& field, bool optional) const;
	void AddCustomArrayFieldWriteToObjectImplCode(QTextStream& stream, const imtsdl::CSdlField& field, bool optional, quint16 hIndents = 1) const;

	// read helpers
	void AddFieldValueReadFromObject(QTextStream& stream, const imtsdl::CSdlField& field, bool optional, bool isEnum, bool isUnion, quint16 hIndents = 1) const;
	void AddCustomFieldReadFromObjectCode(QTextStream& stream, const imtsdl::CSdlField& field, bool optional) const;
	void AddCustomFieldReadFromObjectImplCode(QTextStream& stream, const imtsdl::CSdlField& field, bool optional, quint16 hIndents = 1) const;
	void AddArrayFieldReadFromObjectCode(QTextStream& stream, const imtsdl::CSdlField& field, bool optional, bool isEnum, bool isUnion) const;
	void AddArrayFieldReadFromObjectImplCode(QTextStream& stream, const imtsdl::CSdlField& field, bool optional, bool isEnum, bool isUnion, quint16 hIndents = 1) const;
	void AddCustomArrayFieldReadFromObjectCode(QTextStream& stream, const imtsdl::CSdlField& field, bool optional) const;
	void AddCustomArrayFieldReadToObjectImplCode(QTextStream& stream, const imtsdl::CSdlField& field, bool optional, quint16 hIndents = 1) const;
};


} // namespace imtsdlgencpp

