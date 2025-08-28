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
	template<typename RequestInfo, typename Representation>
	static bool FillId(
		const imtbase::IObjectCollectionIterator& objectCollectionIterator,
		const RequestInfo& requestInfo,
		Representation& representationObject,
		QString& errorMessage)
	{
		if (requestInfo.items.isIdRequested) {
			representationObject.id = objectCollectionIterator.GetObjectId();
		}

		return true;
	}


	template<typename RequestInfo, typename Representation>
	static bool FillName(
		const imtbase::IObjectCollectionIterator& objectCollectionIterator,
		const RequestInfo& requestInfo,
		Representation& representationObject,
		QString& errorMessage)
	{
		if (requestInfo.items.isNameRequested) {
			representationObject.name = objectCollectionIterator.GetElementInfo("Name").toString();
		}

		return true;
	}


	template<typename RequestInfo, typename Representation>
	static bool FillAddedTime(
		const imtbase::IObjectCollectionIterator& objectCollectionIterator,
		const RequestInfo& requestInfo,
		Representation& representationObject,
		QString& errorMessage)
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
	static bool FillLastModifiedTime(
		const imtbase::IObjectCollectionIterator& objectCollectionIterator,
		const RequestInfo& requestInfo,
		Representation& representationObject,
		QString& errorMessage)
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
	static bool GetDocument(const Interface* document, const istd::IChangeable& data, QString& errorMessage)
	{
		document = dynamic_cast<const Interface*>(&data);

		if (document == nullptr) {
			errorMessage = QObject::tr("Invalid document");

			return false;
		}

		return true;
	}

	template<typename Interface>
	static bool CheckArgumentsId(const Interface& arguments, QString& errorMessage)
	{
		if (!arguments.input.Version_1_0 || !arguments.input.Version_1_0->id) {
			errorMessage = QObject::tr("Version 1.0 of the GQL request arguments is missing");

			return false;
		}

		return true;
	}
};


} // namespace imtcol


