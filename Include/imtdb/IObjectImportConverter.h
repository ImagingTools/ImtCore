// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <istd/IChangeable.h>


namespace imtdb
{


/**
	Turns an object read from the source collection of an import migration into the object stored in the target table.
	It decides which objects are imported at all, and may change their type, e.g. to move them into another kind of collection.
	\sa CObjectCollectionImportMigrationControllerComp
*/
class IObjectImportConverter: virtual public istd::IPolymorphic
{
public:
	enum ConversionResult
	{
		/**
			The object is imported: the target object if the converter made one, otherwise the source object unchanged.
		*/
		CR_CONVERTED,
		/**
			The object does not belong to the target, it is left out.
		*/
		CR_SKIPPED,
		/**
			The object belongs to the target but cannot be converted: the migration fails.
		*/
		CR_FAILED
	};

	/**
		The object written into the target table.
	*/
	struct TargetObject
	{
		QByteArray typeId;
		QString name;
		QString description;
		istd::IChangeableUniquePtr objectPtr;
	};

	/**
		Converts the source object \a sourceObject with the ID \a objectId.
		\a targetObject comes filled with the type, the name and the description of the source object; the converter may change them
		and set another object. The target object keeps the ID of the source object.
	*/
	virtual ConversionResult ConvertObject(
				const QByteArray& objectId,
				const istd::IChangeable& sourceObject,
				TargetObject& targetObject,
				QString& errorMessage) const = 0;
};


} // namespace imtdb


