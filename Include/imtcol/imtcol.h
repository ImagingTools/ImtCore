// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QList>

// ImtCore includes
#include <imtbase/IObjectCollectionIterator.h>


/**
	Basic functionality and interfaces
	This package is system independent.
*/
namespace imtcol
{

class CCollectionRepresentationConverter
{

public:
	/// extracts a modification date from standard objectCollectionIterator's element info
	[[nodiscard]] static QDateTime GetLastModiffcationDate(const imtbase::IObjectCollectionIterator& objectCollectionIterator);

	/// extracts a creation date from standard objectCollectionIterator's element info
	[[nodiscard]] static QDateTime GetCreationDate(const imtbase::IObjectCollectionIterator& objectCollectionIterator);

	template<typename RequestInfo, typename Representation>
	static bool FillId(
				const imtbase::IObjectCollectionIterator& objectCollectionIterator,
				const RequestInfo& requestInfo,
				Representation& representationObject,
				QString& errorMessage);

	template<typename RequestInfo, typename Representation>
	static bool FillTypeId(
				const imtbase::IObjectCollectionIterator& objectCollectionIterator,
				const RequestInfo& requestInfo,
				Representation& representationObject,
				QString& errorMessage);

	template<typename RequestInfo, typename Representation>
	static bool FillName(
				const imtbase::IObjectCollectionIterator& objectCollectionIterator,
				const RequestInfo& requestInfo,
				Representation& representationObject,
				QString& errorMessage);

	template<typename RequestInfo, typename Representation>
	static bool FillAddedTime(
				const imtbase::IObjectCollectionIterator& objectCollectionIterator,
				const RequestInfo& requestInfo,
				Representation& representationObject,
				QString& errorMessage);

	template<typename RequestInfo, typename Representation>
	static bool FillLastModifiedTime(
				const imtbase::IObjectCollectionIterator& objectCollectionIterator,
				const RequestInfo& requestInfo,
				Representation& representationObject,
				QString& errorMessage);

	template<typename Interface>
	static bool GetDocument(const Interface* document, const istd::IChangeable& data, QString& errorMessage);

	template<typename Interface>
	static bool CheckArgumentsId(const Interface& arguments, QString& errorMessage);
};


// inline methods

inline QDateTime CCollectionRepresentationConverter::GetLastModiffcationDate(const imtbase::IObjectCollectionIterator& objectCollectionIterator)
{
	QDateTime retVal = objectCollectionIterator.GetElementInfo(QByteArrayLiteral("TimeStamp")).toDateTime();
	if (retVal.isValid()) {
		/// \known-limitation date in standard collection stored without timezone, and in UTC => explicit set UTC for date
		retVal.setTimeSpec(Qt::UTC);
	}

	return retVal;
}


inline QDateTime CCollectionRepresentationConverter::GetCreationDate(const imtbase::IObjectCollectionIterator& objectCollectionIterator)
{
	QDateTime retVal = objectCollectionIterator.GetElementInfo(QByteArrayLiteral("Added")).toDateTime();
	if (retVal.isValid()) {
		/// \known-limitation date in standard collection stored without timezone, and in UTC => explicit set UTC for date
		retVal.setTimeSpec(Qt::UTC);
	}

	return retVal;
}


template<typename RequestInfo, typename Representation>
inline bool CCollectionRepresentationConverter::FillTypeId(
	const imtbase::IObjectCollectionIterator& objectCollectionIterator,
	const RequestInfo& requestInfo,
	Representation& representationObject,
	QString& /* errorMessage */)
{
	if (requestInfo.items.isTypeIdRequested){
		representationObject.typeId = objectCollectionIterator.GetObjectTypeId();
	}

	return true;
}


template<typename RequestInfo, typename Representation>
inline bool CCollectionRepresentationConverter::FillId(
			const imtbase::IObjectCollectionIterator& objectCollectionIterator,
			const RequestInfo& requestInfo,
			Representation& representationObject,
			QString& /* errorMessage */)
{
	if (requestInfo.items.isIdRequested) {
		representationObject.id = objectCollectionIterator.GetObjectId();
	}

	return true;
}


template<typename RequestInfo, typename Representation>
inline bool CCollectionRepresentationConverter::FillName(
			const imtbase::IObjectCollectionIterator& objectCollectionIterator,
			const RequestInfo& requestInfo,
			Representation& representationObject,
			QString& /* errorMessage */)
{
	if (requestInfo.items.isNameRequested) {
		representationObject.name = objectCollectionIterator.GetElementInfo("Name").toString();
	}

	return true;
}


template<typename RequestInfo, typename Representation>
inline bool CCollectionRepresentationConverter::FillAddedTime(
			const imtbase::IObjectCollectionIterator& objectCollectionIterator,
			const RequestInfo& requestInfo,
			Representation& representationObject,
			QString& /* errorMessage */)
{
	if (requestInfo.items.isAddedRequested) {
		QDateTime addedTime = objectCollectionIterator.GetElementInfo("Added").toDateTime();
		addedTime.setTimeSpec(Qt::UTC);

		QString added = addedTime.toLocalTime().toString("dd.MM.yyyy hh:mm:ss");
		representationObject.added = added;
	}

	return true;
}


template<typename RequestInfo, typename Representation>
inline bool CCollectionRepresentationConverter::FillLastModifiedTime(
			const imtbase::IObjectCollectionIterator& objectCollectionIterator, 
			const RequestInfo& requestInfo, 
			Representation& representationObject, 
			QString& /* errorMessage */)
{
	if (requestInfo.items.isLastModifiedRequested) {
		QDateTime lastModifiedTime = objectCollectionIterator.GetElementInfo("TimeStamp").toDateTime();
		lastModifiedTime.setTimeSpec(Qt::UTC);

		QString lastModified = lastModifiedTime.toLocalTime().toString("dd.MM.yyyy hh:mm:ss");
		representationObject.lastModified = lastModified;
	}

	return true;
}


template<typename Interface>
inline bool CCollectionRepresentationConverter::GetDocument(
			const Interface* document,
			const istd::IChangeable& data,
			QString& errorMessage)
{
	document = dynamic_cast<const Interface*>(&data);

	if (document == nullptr) {
		errorMessage = QObject::tr("Invalid document");

		return false;
	}

	return true;
}


template<typename Interface>
inline bool CCollectionRepresentationConverter::CheckArgumentsId(
			const Interface& arguments,
			QString& errorMessage)
{
	if (!arguments.input.Version_1_0 || !arguments.input.Version_1_0->id) {
		errorMessage = QObject::tr("Version 1.0 of the GQL request arguments is missing");

		return false;
	}

	return true;
}


} // namespace imtcol


