// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QList>
#include <QtCore/QJsonObject>

// forward declaration
class QJsonObject;


namespace imtoas
{


class COasTools
{
public:
	/**
		\brief Finds onject in \c lookUpjsonObject if \c jsonObject is \c CReferenceObject and put it in \c derefed
		\param jsonObject		 - source object
		\param lookUpjsonObject	 - object, used to lookup required object
		\param derefed			 - output value for found object
		\param isReferencePtr	 - (optional) could be used to get info if \c jsonObject - is reference object
		\returns true - if \c jsonObject if NOT a \c CReferenceObject or if reference is exists and false otherwise
		\note if \c jsonObject if NOT a \c CReferenceObject \c derefed will not be changed
	*/
	static bool DeReferenceObject(
				const QJsonObject& jsonObject,
				const QJsonObject& lookUpjsonObject,
				QJsonObject& derefed,
				bool* isReferencePtr = nullptr);

	/**
		\brief Finds JSON object with scheme by name
		\param schemeName		 - 	name of scheme
		\param lookUpjsonObject	 - 	object, used to lookup required scheme object
		\return JSON object with required scheme
		\returns empty JSON object - if scheme is not founs
	*/
	static QJsonObject FindSecuritySchemeByName(
				const QString& schemeName,
				const QJsonObject& lookUpjsonObject);

	/**
		\brief Extracts elements from \c objectsArray as if it were an array and sets the ID value equal to the key of the object and places result in \c container
		\param container		- List where results will be written
		\param objectsArray		- Json object to read items
		\param lookupObject		- Used to find references
		\returns true - if object could be read and false otherwise
	 */
	template <class _T>
	static bool ExtractItemsFromObject(QList<_T>& container, const QJsonObject& objectsArray, const QJsonObject& lookupObject);

};



template<class _T>
bool COasTools::ExtractItemsFromObject(QList<_T>& container, const QJsonObject& objectsArray, const QJsonObject& lookupObject)
{
	for (QJsonObject::const_iterator arrayIter = objectsArray.constBegin(); arrayIter != objectsArray.constEnd(); ++arrayIter){
		_T item;
		QJsonObject arrayItemObject = arrayIter->toObject();
		if (!DeReferenceObject(arrayItemObject, lookupObject, arrayItemObject)){
			return false;
		}
		if (!_T::ReadFromJsonObject(item, arrayItemObject, lookupObject)){
			return false;
		}
		item.SetId(arrayIter.key());
		container << item;
	}

	return true;
}


} // namespace imtoas
