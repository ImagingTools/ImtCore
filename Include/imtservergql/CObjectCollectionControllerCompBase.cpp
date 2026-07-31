// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtservergql/CObjectCollectionControllerCompBase.h>

// std includes
#include <cmath>

// Qt includes
#include <QtCore/QFile>
#include <QtCore/QUuid>
#include <QtCore/QTemporaryDir>
#include <QtCore/QFileInfo>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonDocument>

// ACF includes
#include <iprm/CTextParam.h>
#include <iprm/CIdParam.h>
#include <iprm/CEnableableParam.h>
#include <istd/TDelPtr.h>
#include <istd/TSingleFactory.h>
#include <iser/CJsonMemWriteArchive.h>
#include <iser/CJsonMemReadArchive.h>
#include <iprm/TParamsPtr.h>
#include <iqt/iqt.h>

// ImtCore includes
#include <imtbase/imtbase.h>
#include <imtbase/CSearchResults.h>
#include <imtbase/IIdentifiable.h>
#include <imtbase/CCollectionFilter.h>
#include <imtbase/CComplexCollectionFilter.h>
#include <imtbase/IObjectCollectionIterator.h>
#include <imtbase/COperationContext.h>
#include <imtbase/CTimeFilterParam.h>
#include <imtbase/CObjectCollection.h>
#include <imtbase/COperationDescription.h>
#include <imtbase/IRevisionController.h>
#include <imtauth/CUserInfo.h>
#include <imtgql/imtgql.h>
#include <imtgql/IGqlRequestProvider.h>
#include <imtcol/CComplexCollectionFilterRepresentationController.h>
#include <imtcol/CDocumentCollectionFilterRepresentationController.h>
#include <imtcol/CDocumentCollectionFilter.h>
#include <imtauthgql/imtauthgql.h>
#include <GeneratedFiles/imtbasesdl/SDL/1.0/CPP/ComplexCollectionFilter.h>
#include <GeneratedFiles/imtbasesdl/SDL/1.0/CPP/DocumentCollectionFilter.h>
#include <GeneratedFiles/imtbasesdl/SDL/1.0/CPP/ImtCollection.h>


namespace imtservergql
{


// static helpers

/**
	Returns the storage type ID of the value stored in the \c var.
*/
[[nodiscard]] static QByteArray GetTypeIdOfVariant(const QVariant& var)
{
	static const QHash<int, QByteArray> typeIdMap = {
		{ QMetaType::Void,                   QByteArrayLiteral("Void")                },
		{ QMetaType::Bool,                   QByteArrayLiteral("Bool")                },
		{ QMetaType::Int,                    QByteArrayLiteral("Int")                 },
		{ QMetaType::UInt,                   QByteArrayLiteral("UInt")                },
		{ QMetaType::Double,                 QByteArrayLiteral("Double")              },
		{ QMetaType::QChar,                  QByteArrayLiteral("Char")                },
		{ QMetaType::Char,                   QByteArrayLiteral("Char")                },
		{ QMetaType::Char16,                 QByteArrayLiteral("Char16")              },
		{ QMetaType::Char32,                 QByteArrayLiteral("Char32")              },
		{ QMetaType::SChar,                  QByteArrayLiteral("SChar")               },
		{ QMetaType::QString,               QByteArrayLiteral("String")               },
		{ QMetaType::QByteArray,            QByteArrayLiteral("ByteArray")            },
		{ QMetaType::Nullptr,               QByteArrayLiteral("Null")                 },
		{ QMetaType::VoidStar,              QByteArrayLiteral("Void*")                },
		{ QMetaType::Long,                  QByteArrayLiteral("Long")                 },
		{ QMetaType::LongLong,              QByteArrayLiteral("LongLong")             },
		{ QMetaType::Short,                 QByteArrayLiteral("Short")                },
		{ QMetaType::ULong,                 QByteArrayLiteral("ULong")                },
		{ QMetaType::ULongLong,             QByteArrayLiteral("ULongLong")            },
		{ QMetaType::UShort,                QByteArrayLiteral("UShort")               },
		{ QMetaType::UChar,                 QByteArrayLiteral("UChar")                },
		{ QMetaType::Float,                 QByteArrayLiteral("Float")                },
		{ QMetaType::Float16,               QByteArrayLiteral("Float16")              },
		{ QMetaType::QDate,                 QByteArrayLiteral("Date")                 },
		{ QMetaType::QSize,                 QByteArrayLiteral("Size")                 },
		{ QMetaType::QSizeF,                QByteArrayLiteral("Size")                 },
		{ QMetaType::QTime,                 QByteArrayLiteral("Time")                 },
		{ QMetaType::QVariantList,          QByteArrayLiteral("VariantList")          },
		{ QMetaType::QPolygon,              QByteArrayLiteral("Polygon")              },
		{ QMetaType::QPolygonF,             QByteArrayLiteral("Polygon")              },
		{ QMetaType::QColor,                QByteArrayLiteral("Color")                },
		{ QMetaType::QColorSpace,           QByteArrayLiteral("ColorSpace")           },
		{ QMetaType::QRect,                 QByteArrayLiteral("Rect")                 },
		{ QMetaType::QRectF,                QByteArrayLiteral("Rect")                 },
		{ QMetaType::QLine,                 QByteArrayLiteral("Line")                 },
		{ QMetaType::QLineF,                QByteArrayLiteral("Line")                 },
		{ QMetaType::QStringList,           QByteArrayLiteral("StringList")           },
		{ QMetaType::QVariantMap,           QByteArrayLiteral("VariantMap")           },
		{ QMetaType::QVariantHash,          QByteArrayLiteral("VariantHash")          },
		{ QMetaType::QVariantPair,          QByteArrayLiteral("VariantPair")          },
		{ QMetaType::QIcon,                 QByteArrayLiteral("Icon")                 },
		{ QMetaType::QPen,                  QByteArrayLiteral("Pen")                  },
		{ QMetaType::QPoint,                QByteArrayLiteral("Point")                },
		{ QMetaType::QPointF,               QByteArrayLiteral("Point")                },
		{ QMetaType::QUrl,                  QByteArrayLiteral("Url")                  },
		{ QMetaType::QRegularExpression,    QByteArrayLiteral("RegExp")               },
		{ QMetaType::QDateTime,             QByteArrayLiteral("DateTime")             },
		{ QMetaType::QPalette,              QByteArrayLiteral("Palette")              },
		{ QMetaType::QFont,                 QByteArrayLiteral("Font")                 },
		{ QMetaType::QBrush,                QByteArrayLiteral("Brush")                },
		{ QMetaType::QRegion,               QByteArrayLiteral("Region")               },
		{ QMetaType::QBitArray,             QByteArrayLiteral("BitArray")             },
		{ QMetaType::QImage,                QByteArrayLiteral("Image")                },
		{ QMetaType::QKeySequence,          QByteArrayLiteral("KeySeq")               },
		{ QMetaType::QSizePolicy,           QByteArrayLiteral("SizePolicy")           },
		{ QMetaType::QPixmap,               QByteArrayLiteral("Pixmap")               },
		{ QMetaType::QLocale,               QByteArrayLiteral("Locale")               },
		{ QMetaType::QBitmap,               QByteArrayLiteral("Bitmap")               },
		{ QMetaType::QTransform,            QByteArrayLiteral("Transform")            },
		{ QMetaType::QMatrix4x4,            QByteArrayLiteral("Matrix4x4")            },
		{ QMetaType::QVector2D,             QByteArrayLiteral("Vector2D")             },
		{ QMetaType::QVector3D,             QByteArrayLiteral("Vector3D")             },
		{ QMetaType::QVector4D,             QByteArrayLiteral("Vector4D")             },
		{ QMetaType::QQuaternion,           QByteArrayLiteral("Quaternion")           },
		{ QMetaType::QEasingCurve,          QByteArrayLiteral("EasingCurve")          },
		{ QMetaType::QJsonValue,            QByteArrayLiteral("JsonValue")            },
		{ QMetaType::QJsonObject,           QByteArrayLiteral("JsonObject")           },
		{ QMetaType::QJsonArray,            QByteArrayLiteral("JsonArray")            },
		{ QMetaType::QJsonDocument,         QByteArrayLiteral("JsonDocument")         },
		{ QMetaType::QCborValue,            QByteArrayLiteral("CborValue")            },
		{ QMetaType::QCborArray,            QByteArrayLiteral("CborArray")            },
		{ QMetaType::QCborMap,              QByteArrayLiteral("CborMap")              },
		{ QMetaType::QCborSimpleType,       QByteArrayLiteral("CborSimpleType")       },
		{ QMetaType::QModelIndex,           QByteArrayLiteral("ModelIndex")           },
		{ QMetaType::QPersistentModelIndex, QByteArrayLiteral("PersistentModelIndex") },
		{ QMetaType::QUuid,                 QByteArrayLiteral("Uuid")                 },
		{ QMetaType::QByteArrayList,        QByteArrayLiteral("ByteArrayList")        },
		{ QMetaType::QVariant,              QByteArrayLiteral("Variant")              },
	};

	const auto it = typeIdMap.constFind(var.typeId());
	if (it != typeIdMap.constEnd()) {
		return it.value();
	}
	return var.typeName();
}


// public methods

QMap<int, QByteArray> CObjectCollectionControllerCompBase::GetSupportedCommandIds() const
{
	static QMap<int, QByteArray> retVal;

	return retVal;
}


// reimplemented (ISearchController)

QByteArray CObjectCollectionControllerCompBase::GetControllerId() const
{
	if (m_collectionIdAttrPtr.IsValid()){
		return *m_collectionIdAttrPtr;
	}

	return QByteArray();
}


QString CObjectCollectionControllerCompBase::GetControllerName() const
{
	if (m_collectionNameAttrPtr.IsValid()){
		return *m_collectionNameAttrPtr;
	}

	return QString();
}


void CObjectCollectionControllerCompBase::BuildSearchComplexFilter(const QString& text, sdl::V1_0::imtbase::CComplexCollectionFilter& outFilter) const
{
	if (!m_headersProviderCompPtr.IsValid()){
		return;
	}

	typename imtcol::ICollectionHeadersProvider::HeaderIds headerIds = m_headersProviderCompPtr->GetHeaderIds();

	sdl::V1_0::imtbase::CGroupFilter groupFilter;
	groupFilter.logicalOperation = sdl::V1_0::imtbase::LogicalOperation::Or;

	QList<sdl::V1_0::imtbase::FieldFilterUnion> fieldList;
	for (const QByteArray& headerId : headerIds){
		typename imtcol::ICollectionHeadersProvider::HeaderInfo headerInfo;
		if (m_headersProviderCompPtr->GetHeaderInfo(headerId, headerInfo)){
			if (headerInfo.filterable){
				sdl::V1_0::imtbase::CFieldFilter fieldFilter;
				fieldFilter.fieldId = headerInfo.headerId;
				fieldFilter.filterValue = text;
				fieldFilter.filterValueType = sdl::V1_0::imtbase::ValueType::String;

				imtsdl::TElementList<sdl::V1_0::imtbase::FilterOperation> filterOperations;
				filterOperations << sdl::V1_0::imtbase::FilterOperation::Contains;
				fieldFilter.filterOperations = filterOperations;

				fieldList << sdl::V1_0::imtbase::FieldFilterUnion(fieldFilter);
			}
		}
	}

	groupFilter.fieldFilters.Emplace().FromList(fieldList);
	outFilter.fieldsFilter = groupFilter;
}

int CObjectCollectionControllerCompBase::GetMatchCount(const QString& text) const
{
	if (!m_headersProviderCompPtr.IsValid()){
		return 0;
	}

	QMap<int, QByteArray> commandIds = GetSupportedCommandIds();
	QByteArray listCommandId = commandIds.value(OT_LIST);
	if (listCommandId.isEmpty()){
		return 0;
	}

	imtgql::CGqlRequest gqlRequest(imtgql::IGqlRequest::RT_QUERY, listCommandId);

	// copy minimal context if available
	imtgql::IGqlRequestProvider* gqlRequestProviderPtr = QueryInterface<imtgql::IGqlRequestProvider>(const_cast<CObjectCollectionControllerCompBase*>(this));
	if (gqlRequestProviderPtr != nullptr){
		const imtgql::IGqlRequest* gqlRequestPtr = gqlRequestProviderPtr->GetGqlRequest();
		if (gqlRequestPtr != nullptr){
			istd::TUniqueInterfacePtr<imtgql::IGqlContext> gqlContextPtr;
			gqlContextPtr.MoveCastedPtr(gqlRequestPtr->GetRequestContext()->CloneMe());
			if (gqlContextPtr.IsValid()){
				gqlRequest.SetGqlContext(gqlContextPtr.PopInterfacePtr());
			}
		}
	}

	QString errorMessage;
	bool ok = CheckPermissions(gqlRequest, errorMessage);
	if (!ok){
		return 0;
	}

	sdl::V1_0::imtbase::CComplexCollectionFilter complexFilter;
	BuildSearchComplexFilter(text, complexFilter);

	imtgql::CGqlParamObject input;
	imtgql::CGqlParamObject viewParams;
	viewParams.InsertParam("offset", 0);
	viewParams.InsertParam("count", 0);

	imtgql::CGqlParamObject complexFilterGqlOblect;
	if (complexFilter.WriteToGraphQlObject(complexFilterGqlOblect)){
		viewParams.InsertParam("filterModel", complexFilterGqlOblect);
	}

	input.InsertParam("viewParams", viewParams);
	gqlRequest.AddParam("input", input);

	QJsonObject resultObj = GetElementsCount(gqlRequest, errorMessage);
	if (resultObj.isEmpty()){
		return 0;
	}

	QJsonObject dataObj = resultObj.value(QStringLiteral("data")).toObject();
	int cnt = dataObj.value(QStringLiteral("itemsCount")).toInt();
	return cnt;
}

const imtbase::ISearchResults* CObjectCollectionControllerCompBase::Search(
	const QString& text, int offset, int count) const
{
	if (!m_headersProviderCompPtr.IsValid()){
		return nullptr;
	}

	QMap<int, QByteArray> commandIds = GetSupportedCommandIds();
	QByteArray listCommandId = commandIds.value(OT_LIST);
	if (listCommandId.isEmpty()){
		Q_ASSERT(false);
		return nullptr;
	}

	imtgql::CGqlRequest gqlRequest(imtgql::IGqlRequest::RT_QUERY, listCommandId);

	imtgql::IGqlRequestProvider* gqlRequestProviderPtr = QueryInterface<imtgql::IGqlRequestProvider>(const_cast<CObjectCollectionControllerCompBase*>(this));
	if (gqlRequestProviderPtr != nullptr){
		const imtgql::IGqlRequest* gqlRequestPtr = gqlRequestProviderPtr->GetGqlRequest();
		if (gqlRequestPtr != nullptr){
			istd::TUniqueInterfacePtr<imtgql::IGqlContext> gqlContextPtr;
			gqlContextPtr.MoveCastedPtr(gqlRequestPtr->GetRequestContext()->CloneMe());
			if (!gqlContextPtr.IsValid()){
				return nullptr;
			}

			gqlRequest.SetGqlContext(gqlContextPtr.PopInterfacePtr());
		}
	}

	QString errorMessage;
	bool ok = CheckPermissions(gqlRequest, errorMessage);
	if (!ok){
		return nullptr;
	}

	sdl::V1_0::imtbase::CComplexCollectionFilter complexFilter;
	BuildSearchComplexFilter(text, complexFilter);

	imtgql::CGqlParamObject input;
	imtgql::CGqlParamObject viewParams;
	viewParams.InsertParam("offset", offset);
	int effCount = (count > 0 ? count : 30);
	viewParams.InsertParam("count", effCount);

	imtgql::CGqlParamObject complexFilterGqlOblect;
	if (complexFilter.WriteToGraphQlObject(complexFilterGqlOblect)){
		viewParams.InsertParam("filterModel", complexFilterGqlOblect);
	}

	input.InsertParam("viewParams", viewParams);
	gqlRequest.AddParam("input", input);

	imtgql::CGqlFieldObject items;
	items.InsertField("id");
	items.InsertField("name");
	items.InsertField("typeId");
	items.InsertField("description");
	gqlRequest.AddField("items", items);

	QJsonObject resultObj = ListObjects(gqlRequest, errorMessage);
	if (resultObj.isEmpty()){
		return nullptr;
	}

	QJsonObject dataObj = resultObj.value(QStringLiteral("data")).toObject();
	if (dataObj.isEmpty()){
		return nullptr;
	}

	QJsonArray itemsArray = dataObj.value(QStringLiteral("items")).toArray();

	imtbase::CSearchResults* searchResultsPtr = new imtbase::CSearchResults();
	for (int i = 0; i < itemsArray.count(); i++){
		QJsonObject itemObj = itemsArray.at(i).toObject();
		imtbase::ISearchResults::SearchResult searchResult;

		searchResult.contextId = itemObj.value(QStringLiteral("id")).toString().toUtf8();
		searchResult.resultName = itemObj.value(QStringLiteral("name")).toString();
		searchResult.resultDescription = itemObj.value(QStringLiteral("description")).toString();
		searchResult.contextTypeId = itemObj.value(QStringLiteral("typeId")).toString().toUtf8();

		QUrl url;
		url.setScheme("applink");

		url.setPath(*m_collectionIdAttrPtr + "/" + searchResult.contextTypeId + "/" + searchResult.contextId);
		searchResult.url = url;

		searchResultsPtr->AddSearchResult(searchResult);
	}

	return searchResultsPtr;
}


// reimplemented (icomp::CComponentBase)

void CObjectCollectionControllerCompBase::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	// for object collection thread running
	if (!m_objectCollectionCompPtr.EnsureInitialized()){
		qDebug() << "Invalid object collection component";
	}

	int count = m_replaceableFieldsAttrPtr.GetCount();
	count = qMax(count, m_replacementFieldsAttrPtr.GetCount());

	for (int i = 0; i < count; i++){
		QByteArray replaceableField = m_replaceableFieldsAttrPtr[i];
		QByteArray replacementField = m_replacementFieldsAttrPtr[i];
		if (!replaceableField.isEmpty() && ! replacementField.isEmpty()){
			m_fieldReplacementMap[replaceableField] = replacementField;
		}
	}

	RegisterFilterToSelectionParams(m_documentCollectionFilter, m_documentFilterRepresentationController);
	RegisterFilterToSelectionParams(m_complexCollectionFilter, m_complexCollectionFilterRepresentationController);
	RegisterFilterToSelectionParams(m_documentIdFilter, m_documentIdFilterRepresentationController);
}


// reimplemented (sdl::V1_0::imtbase::CImtCollectionGqlHandlerCompBase)

sdl::V1_0::imtbase::CDuplicateElementsPayload CObjectCollectionControllerCompBase::OnDuplicateElements(
			const sdl::V1_0::imtbase::CDuplicateElementsGqlRequest& duplicateElementsRequest,
			const ::imtgql::CGqlRequest& /*gqlRequest*/,
			QString& errorMessage) const
{
	sdl::V1_0::imtbase::CDuplicateElementsPayload response;

	if (!m_objectCollectionCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'ObjectCollection' was not set", "CObjectCollectionControllerCompBase");
		return response;
	}

	sdl::V1_0::imtbase::DuplicateElementsRequestArguments arguments = duplicateElementsRequest.GetRequestedArguments();
	QByteArrayList elementIds;
	if (arguments.input->elementIds.HasValue()){
		elementIds = arguments.input->elementIds->ToList();
	}

	QString name;
	if (arguments.input->name.HasValue()){
		name = *arguments.input->name;
	}

	if (elementIds.isEmpty()){
		errorMessage = QString("Unable to duplicate elements. Error: Element-IDs is empty");
		return response;
	}

	response.success = false;

	istd::CChangeGroup changeGroup(m_objectCollectionCompPtr.GetPtr());

	int count = imtbase::narrow_cast<int>(elementIds.size());
	for (const QByteArray& elementId : elementIds){
		imtbase::IObjectCollection::DataPtr dataPtr;
		if (m_objectCollectionCompPtr->GetObjectData(elementId, dataPtr)){
			istd::IChangeableUniquePtr clonedObjectPtr = dataPtr->CloneMe();
			if (clonedObjectPtr.IsValid()){
				QString duplicateName;
				QByteArray typeId = m_objectCollectionCompPtr->GetObjectTypeId(elementId);

				if (count == 1){
					duplicateName = name;
				}
				else{
					QString elementName = m_objectCollectionCompPtr->GetElementInfo(elementId, imtbase::ICollectionInfo::EIT_NAME).toString();
					duplicateName = elementName + " Copy";
				}

				m_objectCollectionCompPtr->InsertNewObject(typeId, duplicateName, "", clonedObjectPtr.GetPtr());
			}
		}
	}

	response.success = true;

	return response;
}


sdl::V1_0::imtbase::CVisualStatus CObjectCollectionControllerCompBase::OnGetObjectVisualStatus(
			const sdl::V1_0::imtbase::CGetObjectVisualStatusGqlRequest& getObjectVisualStatusRequest,
			const ::imtgql::CGqlRequest& /*gqlRequest*/,
			QString& /*errorMessage*/) const
{
	sdl::V1_0::imtbase::CVisualStatus response;

	sdl::V1_0::imtbase::GetObjectVisualStatusRequestArguments arguments = getObjectVisualStatusRequest.GetRequestedArguments();
	QByteArray objectId;
	if (arguments.input->objectId){
		objectId = *arguments.input->objectId;
	}

	QByteArray typeId;
	if (arguments.input->typeId){
		typeId = *arguments.input->typeId;
	}

	QString name = m_objectCollectionCompPtr->GetElementInfo(objectId, imtbase::ICollectionInfo::EIT_NAME).toString();
	QString description = m_objectCollectionCompPtr->GetElementInfo(objectId, imtbase::ICollectionInfo::EIT_DESCRIPTION).toString();

	int index = m_objectTypeIdAttrPtr.FindValue(typeId);
	if (index >= 0){
		if (index < m_objectIconPathsAttrPtr.GetCount()){
			response.icon = m_objectIconPathsAttrPtr[index];
		}
	}
	else{
		SendCriticalMessage(0, QString("Unknown type-ID provided '%1'").arg(qPrintable(typeId)));
	}

	response.objectId = objectId;
	response.text = name;
	response.description = description;

	return response;
}


sdl::V1_0::imtbase::CRemoveElementsPayload CObjectCollectionControllerCompBase::OnRemoveElements(
			const sdl::V1_0::imtbase::CRemoveElementsGqlRequest& removeElementsRequest,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	sdl::V1_0::imtbase::CRemoveElementsPayload response;

	sdl::V1_0::imtbase::RemoveElementsRequestArguments arguments = removeElementsRequest.GetRequestedArguments();
	QByteArray collectionId;
	if (arguments.input->collectionId.has_value()){
		collectionId = *arguments.input->collectionId;
	}

	QByteArrayList elementIds;
	if (arguments.input->elementIds.has_value()){
		elementIds = arguments.input->elementIds->ToList();
	}

	if (elementIds.isEmpty()){
		errorMessage = QString("Unable to remove elements for collection: '%1'. Error: Element-IDs not provided").arg(QString::fromUtf8(collectionId));
		return sdl::V1_0::imtbase::CRemoveElementsPayload();
	}

	imtbase::ICollectionInfo::Ids allElementIds = m_objectCollectionCompPtr->GetElementIds();
	for (const QByteArray& elementId : elementIds){
		if (!allElementIds.contains(elementId)){
			errorMessage = QString("Unable to delete object. Object with ID '%1' does not exists").arg(QString(elementId));
			SendErrorMessage(0, errorMessage, "CObjectCollectionControllerCompBase");
			return sdl::V1_0::imtbase::CRemoveElementsPayload();
		}
	}

	if (!OnBeforeRemoveElements(elementIds, gqlRequest, errorMessage)){
		return sdl::V1_0::imtbase::CRemoveElementsPayload();
	}

	QByteArray typeId = m_objectCollectionCompPtr->GetObjectTypeId(elementIds[0]);

	istd::TDelPtr<imtbase::IOperationContext> operationContextPtr = nullptr;
	if (m_operationContextControllerCompPtr.IsValid()){
		operationContextPtr = m_operationContextControllerCompPtr->CreateOperationContext("Remove", elementIds.toList().join(';'));
	}

	bool ok = m_objectCollectionCompPtr->RemoveElements(elementIds, operationContextPtr.GetPtr());
	if (ok){
		CreateUserActionLog(elementIds[0], typeId, "Delete", gqlRequest);
		OnAfterRemoveElements(elementIds, gqlRequest);
	}

	response.success = ok;

	return response;
}


sdl::V1_0::imtbase::CRemoveElementSetPayload CObjectCollectionControllerCompBase::OnRemoveElementSet(
			const sdl::V1_0::imtbase::CRemoveElementSetGqlRequest& removeElementSetRequest,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	sdl::V1_0::imtbase::CRemoveElementSetPayload response;

	sdl::V1_0::imtbase::RemoveElementSetRequestArguments arguments = removeElementSetRequest.GetRequestedArguments();
	iprm::CParamsSet filterParams;
	if (arguments.input->selectionParams){
		if (filterParams.CopyFrom(m_selectionParams)){
			sdl::V1_0::imtbase::CParamsSet paramsSet = *arguments.input->selectionParams;
			if (!GetParamsSetFromRepresentation(paramsSet, filterParams)){
				errorMessage = QString("Unable to remove element set for collection '%1'. Error: Selection Params parsing failed").arg(QString::fromUtf8(*m_collectionIdAttrPtr));
				return sdl::V1_0::imtbase::CRemoveElementSetPayload();
			}
		}
	}

	imtbase::ICollectionInfo::Ids elementIds = m_objectCollectionCompPtr->GetElementIds(0, -1, &filterParams);
	if (!OnBeforeRemoveElements(elementIds, gqlRequest, errorMessage)){
		return sdl::V1_0::imtbase::CRemoveElementSetPayload();
	}

	QByteArray typeId;
	if (!elementIds.isEmpty()){
		typeId = m_objectCollectionCompPtr->GetObjectTypeId(elementIds[0]);
	}

	istd::TDelPtr<imtbase::IOperationContext> operationContextPtr = nullptr;
	if (m_operationContextControllerCompPtr.IsValid()){
		operationContextPtr = m_operationContextControllerCompPtr->CreateOperationContext("Remove", elementIds.toList().join(';'));
	}

	bool ok = m_objectCollectionCompPtr->RemoveElementSet(&filterParams, operationContextPtr.GetPtr());
	if (ok){
		if (!elementIds.isEmpty()){
			CreateUserActionLog(elementIds[0], typeId, "Delete", gqlRequest);
		}
		OnAfterRemoveElements(elementIds, gqlRequest);
	}

	response.success = ok;

	return response;
}


sdl::V1_0::imtbase::CRestoreObjectsPayload CObjectCollectionControllerCompBase::OnRestoreObjects(
			const sdl::V1_0::imtbase::CRestoreObjectsGqlRequest& restoreObjectsRequest,
			const ::imtgql::CGqlRequest& /*gqlRequest*/,
			QString& /*errorMessage*/) const
{
	sdl::V1_0::imtbase::CRestoreObjectsPayload response;

	sdl::V1_0::imtbase::RestoreObjectsRequestArguments arguments = restoreObjectsRequest.GetRequestedArguments();
	QByteArrayList objectIds;
	if (arguments.input->objectIds){
		objectIds = arguments.input->objectIds->ToList();
	}

	istd::TDelPtr<imtbase::IOperationContext> operationContextPtr = nullptr;
	if (m_operationContextControllerCompPtr.IsValid()){
		operationContextPtr = m_operationContextControllerCompPtr->CreateOperationContext("Restore", objectIds.toList().join(';'));
	}

	response.success = m_objectCollectionCompPtr->RestoreObjects(imtbase::ICollectionInfo::Ids(objectIds.constBegin(), objectIds.constEnd()), operationContextPtr.GetPtr());

	return response;
}


sdl::V1_0::imtbase::CRestoreObjectSetPayload CObjectCollectionControllerCompBase::OnRestoreObjectSet(
			const sdl::V1_0::imtbase::CRestoreObjectSetGqlRequest& restoreObjectSetRequest,
			const ::imtgql::CGqlRequest& /*gqlRequest*/,
			QString& errorMessage) const
{
	sdl::V1_0::imtbase::CRestoreObjectSetPayload response;

	sdl::V1_0::imtbase::RestoreObjectSetRequestArguments arguments = restoreObjectSetRequest.GetRequestedArguments();
	iprm::CParamsSet filterParams;
	if (arguments.input->selectionParams){
		if (filterParams.CopyFrom(m_selectionParams)){
			sdl::V1_0::imtbase::CParamsSet paramsSet = *arguments.input->selectionParams;
			if (!GetParamsSetFromRepresentation(paramsSet, filterParams)){
				errorMessage = QString("Unable to restore object set for collection '%1'. Error: Selection Params parsing failed").arg(QString::fromUtf8(*m_collectionIdAttrPtr));
				return sdl::V1_0::imtbase::CRestoreObjectSetPayload();
			}
		}
	}

	imtbase::ICollectionInfo::Ids elementIds = m_objectCollectionCompPtr->GetElementIds(0, -1, &filterParams);

	istd::TDelPtr<imtbase::IOperationContext> operationContextPtr = nullptr;
	if (m_operationContextControllerCompPtr.IsValid()){
		operationContextPtr = m_operationContextControllerCompPtr->CreateOperationContext("Restore", elementIds.toList().join(';'));
	}

	response.success = m_objectCollectionCompPtr->RestoreObjectSet(&filterParams, operationContextPtr.GetPtr());

	return response;
}


sdl::V1_0::imtbase::CSetObjectNamePayload CObjectCollectionControllerCompBase::OnSetObjectName(
			const sdl::V1_0::imtbase::CSetObjectNameGqlRequest& setObjectNameRequest,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	sdl::V1_0::imtbase::CSetObjectNamePayload response;
	response.success = false;

	sdl::V1_0::imtbase::SetObjectNameRequestArguments arguments = setObjectNameRequest.GetRequestedArguments();
	QByteArray objectId;
	if (arguments.input->objectId){
		objectId = *arguments.input->objectId;
	}

	QString newName;
	if (arguments.input->name){
		newName = *arguments.input->name;
	}

	if (!OnBeforeSetObjectName(objectId, newName, gqlRequest, errorMessage)){
		return sdl::V1_0::imtbase::CSetObjectNamePayload();
	}

	QString oldName = m_objectCollectionCompPtr->GetElementInfo(objectId, imtbase::ICollectionInfo::EIT_NAME).toString();

	if (!m_objectCollectionCompPtr->SetElementName(objectId, newName)){
		errorMessage = QString("Unable to set name '%1' for element with ID: '%2'").arg(newName, QString::fromUtf8(objectId));
		return sdl::V1_0::imtbase::CSetObjectNamePayload();
	}

	OnAfterSetObjectName(objectId, oldName, newName, gqlRequest);

	sdl::V1_0::imtbase::CSetObjectNamePayload retVal;
	retVal = std::move(response);

	retVal.objectId = objectId;
	retVal.name = newName;
	retVal.success = true;

	return retVal;
}


sdl::V1_0::imtbase::CSetObjectDescriptionPayload CObjectCollectionControllerCompBase::OnSetObjectDescription(
			const sdl::V1_0::imtbase::CSetObjectDescriptionGqlRequest& setObjectDescriptionRequest,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	sdl::V1_0::imtbase::CSetObjectDescriptionPayload response;
	response.success = false;

	sdl::V1_0::imtbase::SetObjectDescriptionRequestArguments arguments = setObjectDescriptionRequest.GetRequestedArguments();
	QByteArray objectId;
	if (arguments.input->objectId){
		objectId = *arguments.input->objectId;
	}

	QString description;
	if (arguments.input->description){
		description = *arguments.input->description;
	}

	if (!OnBeforeSetObjectDescription(objectId, description, gqlRequest, errorMessage)){
		return sdl::V1_0::imtbase::CSetObjectDescriptionPayload();
	}

	istd::CChangeGroup changeGroup(m_objectCollectionCompPtr.GetPtr());
	if (!m_objectCollectionCompPtr->SetElementDescription(objectId, description)){
		changeGroup.Reset();

		errorMessage = QString("Unable to set description '%1' for element with ID: '%2'").arg(description, QString::fromUtf8(objectId));
		return sdl::V1_0::imtbase::CSetObjectDescriptionPayload();
	}

	OnAfterSetObjectDescription(objectId, description, gqlRequest);

	sdl::V1_0::imtbase::CSetObjectDescriptionPayload retVal;
	retVal = std::move(response);

	retVal.objectId = objectId;
	retVal.description = description;
	retVal.success = true;

	return retVal;
}


sdl::V1_0::imtbase::CExportObjectPayload CObjectCollectionControllerCompBase::OnExportObject(
			const sdl::V1_0::imtbase::CExportObjectGqlRequest& exportObjectRequest,
			const ::imtgql::CGqlRequest& /*gqlRequest*/,
			QString& errorMessage) const
{
	sdl::V1_0::imtbase::CExportObjectPayload response;

	if (!m_objectCollectionCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'ObjectCollection' was not set", "CObjectCollectionControllerCompBase");
		return sdl::V1_0::imtbase::CExportObjectPayload();
	}

	sdl::V1_0::imtbase::ExportObjectRequestArguments arguments = exportObjectRequest.GetRequestedArguments();
	QByteArray objectId;
	if (arguments.input->objectId){
		objectId = *arguments.input->objectId;
	}

	QString mimeType;
	if (arguments.input->mimeType){
		mimeType = *arguments.input->mimeType;
	}

	imtbase::IObjectCollection::DataPtr dataPtr;
	if (!m_objectCollectionCompPtr->GetObjectData(objectId, dataPtr)){
		errorMessage = QString("Unable to export the object with ID: '%1'. Error: Object does not exists").arg(qPrintable(objectId));
		SendErrorMessage(0, errorMessage, "CObjectCollectionControllerCompBase");
		return sdl::V1_0::imtbase::CExportObjectPayload();
	}

	int index = GetMimeTypeIndex(mimeType);
	if (index < 0){
		errorMessage = "Mime type is invalid";
		SendErrorMessage(0, errorMessage, "CObjectCollectionControllerCompBase");
		return sdl::V1_0::imtbase::CExportObjectPayload();
	}

	if (index >= m_importExportObjectFactCompPtr.GetCount()){
		errorMessage = "Import/Export object factory index out of range";
		SendErrorMessage(0, errorMessage, "CObjectCollectionControllerCompBase");
		return sdl::V1_0::imtbase::CExportObjectPayload();
	}

	if (index >= m_filePersistenceCompPtr.GetCount()){
		errorMessage = "File persistence index out of range";
		SendErrorMessage(0, errorMessage, "CObjectCollectionControllerCompBase");
		return sdl::V1_0::imtbase::CExportObjectPayload();
	}

	imtbase::CMimeType mime;
	if (!mime.FromString(mimeType)){
		errorMessage = QString("Unable to parse mime type");
		SendErrorMessage(0, errorMessage, "CObjectCollectionControllerCompBase");
		return sdl::V1_0::imtbase::CExportObjectPayload();
	}

	QString extension = GetExtensionFromMimeType(mime);
	QString objectName = GetExportFileName(objectId);

	QTemporaryDir tempDir;
	QString fileName = objectName + "." + extension;
	QString filePathTmp = tempDir.path() + "/" + fileName;

	istd::IChangeableUniquePtr objectPersistenceInstancePtr = m_importExportObjectFactCompPtr.CreateInstance(index);
	if (!objectPersistenceInstancePtr.IsValid()){
		errorMessage = QString("Unable to import object to the collection. Error: Object persistence instance is invalid");
		SendErrorMessage(0, errorMessage, "CObjectCollectionControllerCompBase");
		return sdl::V1_0::imtbase::CExportObjectPayload();
	}

	if (!ConvertObject(*dataPtr.GetPtr(), *objectPersistenceInstancePtr.GetPtr(), errorMessage)){
		if (errorMessage.isEmpty()){
			errorMessage = QString("Unable to export the object with ID: '%1'. Error: Object conversion failed").arg(qPrintable(objectId));
		}
		SendErrorMessage(0, errorMessage, "CObjectCollectionControllerCompBase");
		return sdl::V1_0::imtbase::CExportObjectPayload();
	}

	if (m_filePersistenceCompPtr[index]->SaveToFile(*objectPersistenceInstancePtr.GetPtr(), filePathTmp) != ifile::IFilePersistence::OS_OK){
		errorMessage = QString("Unable to export the object with ID: '%1'. Error: Saving data to the file '%1' failed").arg(qPrintable(objectId), filePathTmp);
		SendErrorMessage(0, errorMessage, "CObjectCollectionControllerCompBase");
		return sdl::V1_0::imtbase::CExportObjectPayload();
	}

	QFile file(filePathTmp);
	if (!file.open(QIODevice::ReadOnly)){
		errorMessage = QString("Unable to export the object with ID: '%1'. Error: Unable to open file with name '%1'").arg(qPrintable(objectId), filePathTmp);
		SendErrorMessage(0, errorMessage, "CObjectCollectionControllerCompBase");
		QFile::remove(filePathTmp);
		return sdl::V1_0::imtbase::CExportObjectPayload();
	}

	QByteArray data = file.readAll();
	file.close();

	response.objectId = objectId;
	response.fileData = data.toBase64();
	response.fileName = fileName;

	QFile::remove(filePathTmp);

	return response;
}


sdl::V1_0::imtbase::CImportObjectPayload CObjectCollectionControllerCompBase::OnImportObject(
			const sdl::V1_0::imtbase::CImportObjectGqlRequest& importObjectRequest,
			const ::imtgql::CGqlRequest& /*gqlRequest*/,
			QString& errorMessage) const
{
	sdl::V1_0::imtbase::CImportObjectPayload response;
	response.success = false;

	if (!m_objectCollectionCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'ObjectCollection' was not set", "CObjectCollectionControllerCompBase");
		return sdl::V1_0::imtbase::CImportObjectPayload();
	}

	sdl::V1_0::imtbase::ImportObjectRequestArguments arguments = importObjectRequest.GetRequestedArguments();
	QByteArray objectData;
	if (arguments.input->fileData){
		objectData = *arguments.input->fileData;
	}

	QByteArray data = QByteArray::fromBase64(objectData);

	QByteArray typeId;
	if (arguments.input->typeId){
		typeId = *arguments.input->typeId;
	}

	QString mimeType;
	if (arguments.input->mimeType){
		mimeType = *arguments.input->mimeType;
	}

	QString name;
	if (arguments.input->name){
		name = *arguments.input->name;
	}

	QString description;
	if (arguments.input->description){
		description = *arguments.input->description;
	}

	int index = GetMimeTypeIndex(mimeType);
	if (index < 0){
		errorMessage = "Mime type is invalid";
		SendErrorMessage(0, errorMessage, "CObjectCollectionControllerCompBase");
		return sdl::V1_0::imtbase::CImportObjectPayload();
	}
	if (index >= m_importExportObjectFactCompPtr.GetCount()){
		errorMessage = "Import/Export object factory index out of range";
		SendErrorMessage(0, errorMessage, "CObjectCollectionControllerCompBase");
		return sdl::V1_0::imtbase::CImportObjectPayload();
	}
	if (index >= m_filePersistenceCompPtr.GetCount()){
		errorMessage = "File persistence index out of range";
		SendErrorMessage(0, errorMessage, "CObjectCollectionControllerCompBase");
		return sdl::V1_0::imtbase::CImportObjectPayload();
	}

	istd::IChangeableUniquePtr objectPersistenceInstancePtr = m_importExportObjectFactCompPtr.CreateInstance(index);
	if (!objectPersistenceInstancePtr.IsValid()){
		errorMessage = QString("Unable to import object to the collection. Error: Object instance is invalid");
		SendErrorMessage(0, errorMessage, "CObjectCollectionControllerCompBase");

		return sdl::V1_0::imtbase::CImportObjectPayload();
	}

	imtbase::CMimeType mime;
	if (!mime.FromString(mimeType)){
		errorMessage = QString("Unable to parse mime type");
		SendErrorMessage(0, errorMessage, "CObjectCollectionControllerCompBase");

		return sdl::V1_0::imtbase::CImportObjectPayload();
	}

	QString extension = GetExtensionFromMimeType(mime);

	QTemporaryDir tempDir;
	QString filePathTmp = tempDir.path() + "/" + QUuid::createUuid().toString(QUuid::WithoutBraces) + "." + extension;

	QFile file(filePathTmp);
	if (!file.open(QIODevice::WriteOnly)){
		SendErrorMessage(0, QString("Unable to open file with name '%1'").arg(filePathTmp), "CObjectCollectionControllerCompBase");
		return sdl::V1_0::imtbase::CImportObjectPayload();
	}

	file.write(data);
	file.close();

	if (m_filePersistenceCompPtr[index]->LoadFromFile(*objectPersistenceInstancePtr.GetPtr(), filePathTmp) != ifile::IFilePersistence::OS_OK){
		errorMessage = QString("Unable to import object to the collection");
		SendErrorMessage(0, errorMessage, "CObjectCollectionControllerCompBase");
		QFile::remove(filePathTmp);

		return sdl::V1_0::imtbase::CImportObjectPayload();
	}

	QByteArray objectUuid = QUuid::createUuid().toByteArray(QUuid::WithoutBraces);
	imtbase::IIdentifiable* identifiableObjectPtr = dynamic_cast<imtbase::IIdentifiable*>(objectPersistenceInstancePtr.GetPtr());
	if (identifiableObjectPtr != nullptr){
		objectUuid = identifiableObjectPtr->GetObjectUuid();
	}

	if (m_objectCollectionCompPtr->GetElementIds().contains(objectUuid)){
		errorMessage = QString("Unable to import object with ID: '%1' to the collection. Error: The object already exists inside the collection").arg(qPrintable(objectUuid));
		SendErrorMessage(0, errorMessage, "CObjectCollectionControllerCompBase");
		QFile::remove(filePathTmp);

		return sdl::V1_0::imtbase::CImportObjectPayload();
	}

	int typeIdIndex = GetObjectTypeIdIndex(typeId);
	Q_ASSERT_X(typeIdIndex >= 0, "Type ID is invalid", "CObjectCollectionControllerCompBase");

	istd::IChangeableUniquePtr collectionObjectInstancePtr = m_objectFactCompPtr.CreateInstance(typeIdIndex);
	if (!collectionObjectInstancePtr.IsValid()){
		errorMessage = QString("Unable to import object to the collection. Error: Object instance is invalid");
		SendErrorMessage(0, errorMessage, "CObjectCollectionControllerCompBase");
		return sdl::V1_0::imtbase::CImportObjectPayload();
	}

	if (!ConvertObject(*objectPersistenceInstancePtr.GetPtr(), *collectionObjectInstancePtr.GetPtr(), errorMessage)){
		if (errorMessage.isEmpty()){
			errorMessage = QString("Unable to import object to the collection. Error: Object conversion failed");
		}
		SendErrorMessage(0, errorMessage, "CObjectCollectionControllerCompBase");
		return sdl::V1_0::imtbase::CImportObjectPayload();
	}

	QByteArray insertRetVal = m_objectCollectionCompPtr->InsertNewObject(typeId, name, description, collectionObjectInstancePtr.GetPtr(), objectUuid);
	if (insertRetVal.isEmpty()){
		errorMessage = QString("Unable to import object with ID: '%1' to the collection. Error: The object could not be inserted into the collection").arg(qPrintable(objectUuid));
		SendErrorMessage(0, errorMessage, "CObjectCollectionControllerCompBase");
		QFile::remove(filePathTmp);
		return sdl::V1_0::imtbase::CImportObjectPayload();
	}

	response.objectId = objectUuid;
	response.success = true;
	QFile::remove(filePathTmp);

	return response;
}


sdl::V1_0::imtbase::CGetObjectTypeIdPayload CObjectCollectionControllerCompBase::OnGetObjectTypeId(
			const sdl::V1_0::imtbase::CGetObjectTypeIdGqlRequest& getObjectTypeIdRequest,
			const ::imtgql::CGqlRequest& /*gqlRequest*/,
			QString& /*errorMessage*/) const
{
	if (!m_objectCollectionCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'ObjectCollection' was not set", "CObjectCollectionControllerCompBase");
		return sdl::V1_0::imtbase::CGetObjectTypeIdPayload();
	}

	sdl::V1_0::imtbase::GetObjectTypeIdRequestArguments arguments = getObjectTypeIdRequest.GetRequestedArguments();
	QByteArray objectId;
	if (arguments.input->objectId){
		objectId = *arguments.input->objectId;
	}

	sdl::V1_0::imtbase::CGetObjectTypeIdPayload response;

	QByteArray typeId = m_objectCollectionCompPtr->GetObjectTypeId(objectId);
	response.typeId = typeId;

	return response;
}


sdl::V1_0::imtbase::CGetCollectionHeadersPayload CObjectCollectionControllerCompBase::OnGetCollectionHeaders(
			const sdl::V1_0::imtbase::CGetCollectionHeadersGqlRequest& /*getCollectionHeadersRequest*/,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	sdl::V1_0::imtbase::CGetCollectionHeadersPayload response;

	if (!m_headersProviderCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'HeadersProvider' was not set", "CObjectCollectionControllerCompBase");
		return sdl::V1_0::imtbase::CGetCollectionHeadersPayload();
	}

	const imtgql::IGqlContext* gqlContextPtr = gqlRequest.GetRequestContext();
	if (gqlContextPtr == nullptr){
		errorMessage = QString("Unable to get collection headers. Error: GraphQL context is invalid");
		return sdl::V1_0::imtbase::CGetCollectionHeadersPayload();
	}

	QByteArrayList userPermissions;
	const imtauth::IUserInfo* userInfoPtr = gqlContextPtr->GetUserInfo();
	if (userInfoPtr != nullptr){
		userPermissions = userInfoPtr->GetPermissions();
	}

	QByteArray languageId = gqlContextPtr->GetLanguageId();

	QList<sdl::V1_0::imtbase::CHeaderInfo> headersRepresentationList;
	imtcol::ICollectionHeadersProvider::HeaderIds headerIds = m_headersProviderCompPtr->GetHeaderIds();
	for (const QByteArray& headerId : headerIds){
		imtcol::ICollectionHeadersProvider::HeaderInfo headerInfo;
		if (m_headersProviderCompPtr->GetHeaderInfo(headerId, headerInfo)){
			// Check header permissions
			if (userInfoPtr != nullptr && !userInfoPtr->IsAdmin()){
				QByteArray headerPermission = headerInfo.permissionId;
				if (!headerPermission.isEmpty() && !userPermissions.contains(headerPermission)){
					continue;
				}
			}

			sdl::V1_0::imtbase::CHeaderInfo headerRepresentation;
			headerRepresentation.id = headerInfo.headerId;
			headerRepresentation.filterable = headerInfo.filterable;
			headerRepresentation.sortable = headerInfo.sortable;

			QString headerName = headerInfo.headerName;
			if (m_translationManagerCompPtr.IsValid()){
				headerName = iqt::GetTranslation(m_translationManagerCompPtr.GetPtr(), headerName.toUtf8(), languageId, QByteArrayLiteral("Attribute"));
			}
			headerRepresentation.name = headerName;

			headersRepresentationList << headerRepresentation;
		}
	}

	response.headers.Emplace();
	response.headers->FromList(headersRepresentationList);

	return response;
}


sdl::V1_0::imtbase::CGetElementsCountPayload CObjectCollectionControllerCompBase::OnGetElementsCount(
			const sdl::V1_0::imtbase::CGetElementsCountGqlRequest& getElementsCountRequest,
			const ::imtgql::CGqlRequest& /*gqlRequest*/,
			QString& errorMessage) const
{
	sdl::V1_0::imtbase::CGetElementsCountPayload response;

	if (!m_objectCollectionCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'ObjectCollection' was not set", "CObjectCollectionControllerCompBase");
		return sdl::V1_0::imtbase::CGetElementsCountPayload();
	}

	sdl::V1_0::imtbase::GetElementsCountRequestArguments arguments = getElementsCountRequest.GetRequestedArguments();
	iprm::CParamsSet filterParams;
	if (arguments.input->selectionParams){
		if (filterParams.CopyFrom(m_selectionParams)){
			sdl::V1_0::imtbase::CParamsSet paramsSet = *arguments.input->selectionParams;
			if (!GetParamsSetFromRepresentation(paramsSet, filterParams)){
				errorMessage = QString("Unable to get elements count for collection '%1'. Error: Selection Params parsing failed").arg(QString::fromUtf8(*m_collectionIdAttrPtr));
				return sdl::V1_0::imtbase::CGetElementsCountPayload();
			}
		}
	}

	response.count = m_objectCollectionCompPtr->GetElementsCount(&filterParams);

	return response;
}


sdl::V1_0::imtbase::CGetElementIdsPayload CObjectCollectionControllerCompBase::OnGetElementIds(
			const sdl::V1_0::imtbase::CGetElementIdsGqlRequest& getElementIdsRequest,
			const ::imtgql::CGqlRequest& /*gqlRequest*/,
			QString& errorMessage) const
{
	sdl::V1_0::imtbase::CGetElementIdsPayload response;

	if (!m_objectCollectionCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'ObjectCollection' was not set", "CObjectCollectionControllerCompBase");
		return sdl::V1_0::imtbase::CGetElementIdsPayload();
	}

	sdl::V1_0::imtbase::GetElementIdsRequestArguments arguments = getElementIdsRequest.GetRequestedArguments();
	int offset = 0;
	if (arguments.input->offset){
		offset = *arguments.input->offset;
	}

	int count = -1;
	if (arguments.input->count){
		count = *arguments.input->count;
	}

	iprm::CParamsSet filterParams;
	if (arguments.input->selectionParams){
		if (filterParams.CopyFrom(m_selectionParams)){
			sdl::V1_0::imtbase::CParamsSet paramsSet = *arguments.input->selectionParams;
			if (!GetParamsSetFromRepresentation(paramsSet, filterParams)){
				errorMessage = QString("Unable to get element IDs for collection '%1'. Error: Selection Params parsing failed").arg(QString::fromUtf8(*m_collectionIdAttrPtr));
				return sdl::V1_0::imtbase::CGetElementIdsPayload();
			}
		}
	}

	imtbase::IObjectCollection::Ids ids = m_objectCollectionCompPtr->GetElementIds(offset, count, &filterParams);
	response.elementIds.Emplace();
	response.elementIds->FromList(ids);

	return response;
}


sdl::V1_0::imtbase::CInsertNewObjectPayload CObjectCollectionControllerCompBase::OnInsertNewObject(
			const sdl::V1_0::imtbase::CInsertNewObjectGqlRequest& insertNewObjectRequest,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	sdl::V1_0::imtbase::CInsertNewObjectPayload response;

	if (!m_objectCollectionCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'ObjectCollection' was not set", "CObjectCollectionControllerCompBase");
		return sdl::V1_0::imtbase::CInsertNewObjectPayload();
	}

	sdl::V1_0::imtbase::InsertNewObjectRequestArguments arguments = insertNewObjectRequest.GetRequestedArguments();
	QByteArray objectId;
	if (arguments.input->proposedObjectId){
		objectId = *arguments.input->proposedObjectId;
	}

	QByteArray typeId;
	if (arguments.input->typeId){
		typeId = *arguments.input->typeId;
	}

	QString name;
	if (arguments.input->name){
		name = *arguments.input->name;
	}

	QString description;
	if (arguments.input->description){
		description = *arguments.input->description;
	}

	QByteArray objectData;
	if (arguments.input->objectData){
		objectData = (*arguments.input->objectData).toUtf8();
	}

	istd::IChangeableUniquePtr objectPtr = nullptr;
	if (!objectData.isEmpty()){
		objectPtr = CreateObject(typeId);
		if (!DeSerializeObject(*objectPtr.GetPtr(), objectData)){
			errorMessage = QString("Unable to insert new object to collection '%1'. Error: Object serialization failed").arg(QString::fromUtf8(*m_collectionIdAttrPtr));
			return sdl::V1_0::imtbase::CInsertNewObjectPayload();
		}
	}

	istd::TDelPtr<imtbase::IOperationContext> operationContextPtr = nullptr;
	if (m_operationContextControllerCompPtr.IsValid()){
		operationContextPtr = m_operationContextControllerCompPtr->CreateOperationContext("Create", objectId, objectPtr.GetPtr());
	}

	QByteArray result = m_objectCollectionCompPtr->InsertNewObject(typeId, name, description, objectPtr.GetPtr(), objectId, nullptr, nullptr, operationContextPtr.GetPtr());
	if (result.isEmpty()){
		errorMessage = QString("Unable to insert new object to collection '%1'").arg(QString::fromUtf8(*m_collectionIdAttrPtr));
		return sdl::V1_0::imtbase::CInsertNewObjectPayload();
	}

	CreateUserActionLog(objectId, typeId, "Create", gqlRequest);
	response.objectId = objectId;

	return response;
}


sdl::V1_0::imtbase::CSetObjectDataPayload CObjectCollectionControllerCompBase::OnSetObjectData(
			const sdl::V1_0::imtbase::CSetObjectDataGqlRequest& setObjectDataRequest,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	sdl::V1_0::imtbase::CSetObjectDataPayload response;

	if (!m_objectCollectionCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'ObjectCollection' was not set", "CObjectCollectionControllerCompBase");
		return sdl::V1_0::imtbase::CSetObjectDataPayload();
	}

	sdl::V1_0::imtbase::SetObjectDataRequestArguments arguments = setObjectDataRequest.GetRequestedArguments();
	QByteArray objectId;
	if (arguments.input->objectId){
		objectId = *arguments.input->objectId;
	}

	QByteArray objectData;
	if (arguments.input->objectData){
		objectData = (*arguments.input->objectData).toUtf8();
	}

	QByteArray typeId = m_objectCollectionCompPtr->GetObjectTypeId(objectId);

	istd::IChangeableUniquePtr objectPtr = nullptr;
	if (!objectData.isEmpty()){
		objectPtr = CreateObject(typeId);
		if (!DeSerializeObject(*objectPtr.GetPtr(), objectData)){
			errorMessage = QString("Unable to set object data to collection '%1'. Error: Object serialization failed").arg(QString::fromUtf8(*m_collectionIdAttrPtr));
			return sdl::V1_0::imtbase::CSetObjectDataPayload();
		}
	}

	istd::TDelPtr<imtbase::IOperationContext> operationContextPtr = nullptr;
	if (m_operationContextControllerCompPtr.IsValid()){
		operationContextPtr = m_operationContextControllerCompPtr->CreateOperationContext("Update", objectId, objectPtr.GetPtr());
	}

	bool ok = m_objectCollectionCompPtr->SetObjectData(objectId, *objectPtr.GetPtr(), istd::IChangeable::CM_WITHOUT_REFS, operationContextPtr.GetPtr());
	if (ok){
		CreateUserActionLog(objectId, typeId, "Update", gqlRequest);
	}

	response.success = ok;

	return response;
}


sdl::V1_0::imtbase::CGetObjectDataPayload CObjectCollectionControllerCompBase::OnGetObjectData(
			const sdl::V1_0::imtbase::CGetObjectDataGqlRequest& getObjectDataRequest,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	sdl::V1_0::imtbase::CGetObjectDataPayload response;

	if (!m_objectCollectionCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'ObjectCollection' was not set", "CObjectCollectionControllerCompBase");
		return sdl::V1_0::imtbase::CGetObjectDataPayload();
	}

	sdl::V1_0::imtbase::GetObjectDataRequestArguments arguments = getObjectDataRequest.GetRequestedArguments();
	QByteArray objectId;
	if (arguments.input->objectId){
		objectId = *arguments.input->objectId;
	}

	imtbase::IObjectCollection::DataPtr dataPtr;
	if (!m_objectCollectionCompPtr->GetObjectData(objectId, dataPtr)){
		errorMessage = QString("Unable to get object data '%1'. Error: Object does not exists").arg(QString::fromUtf8(objectId));
		return sdl::V1_0::imtbase::CGetObjectDataPayload();
	}

	istd::IChangeableUniquePtr adaptedObjectPtr = CreateAdaptedObjectData(objectId, *dataPtr.GetPtr(), gqlRequest);
	istd::IChangeable* objectToSerializePtr = adaptedObjectPtr.IsValid() ? adaptedObjectPtr.GetPtr() : dataPtr.GetPtr();

	QByteArray objectData;
	if (!SerializeObject(*objectToSerializePtr, objectData)){
		errorMessage = QString("Unable to get object data '%1'. Error: Object serializaion failed").arg(QString::fromUtf8(objectId));
		return sdl::V1_0::imtbase::CGetObjectDataPayload();
	}

	response.objectData = objectData;

	return response;
}


sdl::V1_0::imtbase::CGetDataMetaInfoPayload CObjectCollectionControllerCompBase::OnGetDataMetaInfo(
			const sdl::V1_0::imtbase::CGetDataMetaInfoGqlRequest& getDataMetaInfoRequest,
			const ::imtgql::CGqlRequest& /*gqlRequest*/,
			QString& errorMessage) const
{
	sdl::V1_0::imtbase::CGetDataMetaInfoPayload response;

	if (!m_objectCollectionCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'ObjectCollection' was not set", "CObjectCollectionControllerCompBase");
		return sdl::V1_0::imtbase::CGetDataMetaInfoPayload();
	}

	sdl::V1_0::imtbase::GetDataMetaInfoRequestArguments arguments = getDataMetaInfoRequest.GetRequestedArguments();
	QByteArray objectId;
	if (arguments.input->objectId){
		objectId = *arguments.input->objectId;
	}

	QByteArray metaInfoData;
	idoc::MetaInfoPtr metaInfo = m_objectCollectionCompPtr->GetDataMetaInfo(objectId);
	if (!SerializeObject(*metaInfo.GetPtr(), metaInfoData)){
		errorMessage = QString("Unable to get data meta info for object '%1'. Error: Meta Info serializaion failed").arg(QString::fromUtf8(objectId));
		return sdl::V1_0::imtbase::CGetDataMetaInfoPayload();
	}

	response.metaInfoData = metaInfoData;

	return response;
}


sdl::V1_0::imtbase::CGetElementInfoPayload CObjectCollectionControllerCompBase::OnGetElementInfo(
			const sdl::V1_0::imtbase::CGetElementInfoGqlRequest& getElementInfoRequest,
			const ::imtgql::CGqlRequest& /*gqlRequest*/,
			QString& /*errorMessage*/) const
{
	sdl::V1_0::imtbase::CGetElementInfoPayload response;

	if (!m_objectCollectionCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'ObjectCollection' was not set", "CObjectCollectionControllerCompBase");
		return sdl::V1_0::imtbase::CGetElementInfoPayload();
	}

	sdl::V1_0::imtbase::GetElementInfoRequestArguments arguments = getElementInfoRequest.GetRequestedArguments();
	QByteArray objectId;
	if (arguments.input->elementId){
		objectId = *arguments.input->elementId;
	}

	sdl::V1_0::imtbase::CParameter parameterInfo;
	parameterInfo.id = objectId;

	QByteArray typeId = m_objectCollectionCompPtr->GetObjectTypeId(objectId);
	parameterInfo.typeId = typeId;

	QString name = m_objectCollectionCompPtr->GetElementInfo(objectId, imtbase::ICollectionInfo::EIT_NAME).toByteArray();
	parameterInfo.name = name;

	QString description = m_objectCollectionCompPtr->GetElementInfo(objectId, imtbase::ICollectionInfo::EIT_DESCRIPTION).toByteArray();
	parameterInfo.description = description;

	bool enabled = m_objectCollectionCompPtr->GetElementInfo(objectId, imtbase::ICollectionInfo::EIT_ENABLED).toBool();
	parameterInfo.enabled = enabled;

	response.elementInfo = parameterInfo;

	return response;
}


sdl::V1_0::imtbase::CGetElementMetaInfoPayload CObjectCollectionControllerCompBase::OnGetElementMetaInfo(
			const sdl::V1_0::imtbase::CGetElementMetaInfoGqlRequest& getElementMetaInfoRequest,
			const ::imtgql::CGqlRequest& /*gqlRequest*/,
			QString& errorMessage) const
{
	sdl::V1_0::imtbase::CGetElementMetaInfoPayload response;

	if (!m_objectCollectionCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'ObjectCollection' was not set", "CObjectCollectionControllerCompBase");
		return sdl::V1_0::imtbase::CGetElementMetaInfoPayload();
	}

	sdl::V1_0::imtbase::GetElementMetaInfoRequestArguments arguments = getElementMetaInfoRequest.GetRequestedArguments();
	QByteArray objectId;
	if (arguments.input->elementId){
		objectId = *arguments.input->elementId;
	}

	idoc::MetaInfoPtr metaInfo = m_objectCollectionCompPtr->GetElementMetaInfo(objectId);
	if (!metaInfo.IsValid()){
		errorMessage = QString("Unable to get element meta info for object '%1'. Error: Meta Info is invalid").arg(QString::fromUtf8(objectId));
		return sdl::V1_0::imtbase::CGetElementMetaInfoPayload();
	}

	QList<sdl::V1_0::imtbase::CParameter> parameterInfos;
	idoc::IDocumentMetaInfo::MetaInfoTypes metaInfoTypes = metaInfo->GetMetaInfoTypes();
	for (const int& infoType : metaInfoTypes){
		sdl::V1_0::imtbase::CParameter parameterInfo;
		parameterInfo.id = metaInfo->GetMetaInfoId(infoType);
		parameterInfo.name = metaInfo->GetMetaInfoName(infoType);
		parameterInfo.description = metaInfo->GetMetaInfoDescription(infoType);

		QVariant metaInfoValue = metaInfo->GetMetaInfo(infoType);
		parameterInfo.typeId = GetTypeIdOfVariant(metaInfoValue);
		parameterInfo.data = ConvertMetaInfoToString(infoType, metaInfoValue);

		parameterInfos << parameterInfo;
	}

	sdl::V1_0::imtbase::CElementMetaInfo elementMetaInfo;
	elementMetaInfo.infoParams.Emplace();
	elementMetaInfo.infoParams->FromList(parameterInfos);
	response.elementMetaInfo = elementMetaInfo;

	return response;
}


sdl::V1_0::imtbase::CCreateSubCollectionPayload CObjectCollectionControllerCompBase::OnCreateSubCollection(
			const sdl::V1_0::imtbase::CCreateSubCollectionGqlRequest& createSubCollectionRequest,
			const ::imtgql::CGqlRequest& /*gqlRequest*/,
			QString& errorMessage) const
{
	sdl::V1_0::imtbase::CCreateSubCollectionPayload response;

	if (!m_objectCollectionCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'ObjectCollection' was not set", "CObjectCollectionControllerCompBase");
		return sdl::V1_0::imtbase::CCreateSubCollectionPayload();
	}

	sdl::V1_0::imtbase::CreateSubCollectionRequestInfo requestInfo = createSubCollectionRequest.GetRequestInfo();

	sdl::V1_0::imtbase::CreateSubCollectionRequestArguments arguments = createSubCollectionRequest.GetRequestedArguments();
	int offset = 0;
	if (arguments.input->offset){
		offset = *arguments.input->offset;
	}

	int count = -1;
	if (arguments.input->count){
		count = *arguments.input->count;
	}

	iprm::CParamsSet filterParams;
	if (arguments.input->selectionParams){
		if (filterParams.CopyFrom(m_selectionParams)){
			sdl::V1_0::imtbase::CParamsSet paramsSet = *arguments.input->selectionParams;
			if (!GetParamsSetFromRepresentation(paramsSet, filterParams)){
				errorMessage = QString("Unable to create sub collection '%1'. Error: Selection Params parsing failed").arg(QString::fromUtf8(*m_collectionIdAttrPtr));
				return sdl::V1_0::imtbase::CCreateSubCollectionPayload();
			}
		}
	}

	if (requestInfo.isItemsRequested){
		imtsdl::TElementList<sdl::V1_0::imtbase::CSubCollectionItem> collectionItems;
		imtbase::ICollectionInfo::Ids ids = m_objectCollectionCompPtr->GetElementIds(offset, count, &filterParams);
		for (const imtbase::ICollectionInfo::Id& id: ids){
			sdl::V1_0::imtbase::CSubCollectionItem collectionItem;

			if (requestInfo.items.isItemInfoRequested){
				sdl::V1_0::imtbase::CParameter parameterInfo;

				if (requestInfo.items.itemInfo.isIdRequested){
					parameterInfo.id = id;
				}

				if (requestInfo.items.itemInfo.isTypeIdRequested){
					parameterInfo.typeId = m_objectCollectionCompPtr->GetObjectTypeId(id);
				}

				if (requestInfo.items.itemInfo.isNameRequested){
					parameterInfo.name = m_objectCollectionCompPtr->GetElementInfo(id, imtbase::ICollectionInfo::EIT_NAME).toString();
				}

				if (requestInfo.items.itemInfo.isEnabledRequested){
					parameterInfo.enabled = m_objectCollectionCompPtr->GetElementInfo(id, imtbase::ICollectionInfo::EIT_ENABLED).toBool();
				}

				if (requestInfo.items.itemInfo.isDescriptionRequested){
					parameterInfo.description = m_objectCollectionCompPtr->GetElementInfo(id, imtbase::ICollectionInfo::EIT_DESCRIPTION).toString();
				}

				collectionItem.itemInfo = parameterInfo;
			}

			if (requestInfo.items.isMetaInfoRequested){
				QByteArray elementMetaInfoData;
				idoc::MetaInfoPtr metaInfo = m_objectCollectionCompPtr->GetElementMetaInfo(id);
				if (SerializeObject(*metaInfo.GetPtr(), elementMetaInfoData)){
					collectionItem.metaInfo = elementMetaInfoData;
				}
			}

			if (requestInfo.items.isDataMetaInfoRequested){
				QByteArray dataMetaInfo;
				idoc::MetaInfoPtr dataMetaInfoPtr = m_objectCollectionCompPtr->GetDataMetaInfo(id);
				if (SerializeObject(*dataMetaInfoPtr.GetPtr(), dataMetaInfo)){
					collectionItem.dataMetaInfo = dataMetaInfo;
				}
			}

			collectionItems << collectionItem;
		}

		response.items = collectionItems;
	}

	return response;
}


// reimplemented (imtservergql::CGqlRepresentationDataControllerComp)

QJsonObject CObjectCollectionControllerCompBase::CreateInternalResponse(
			const imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	imtgql::CGqlParamObject gqlObject;
	int operationType = OT_UNKNOWN;
	if (!GetOperationFromRequest(gqlRequest, gqlObject, errorMessage, operationType)){
		return QJsonObject();
	}

	switch (operationType){
	case OT_NEW:
		return InsertObject(gqlRequest, errorMessage);
	case OT_GET:
		return GetObject(gqlRequest, errorMessage);
	case OT_UPDATE:
		return UpdateObject(gqlRequest, errorMessage);
	case OT_LIST:
		return ListObjects(gqlRequest, errorMessage);
	}

	return BaseClass::CreateInternalResponse(gqlRequest, errorMessage);
}


bool CObjectCollectionControllerCompBase::IsRequestSupported(const imtgql::CGqlRequest& gqlRequest) const
{
	if (!BaseClass::IsRequestSupported(gqlRequest)){
		return false;
	}

	const imtgql::CGqlParamObject* inputParamPtr = gqlRequest.GetParamObject(QByteArrayLiteral("input"));
	if (inputParamPtr == nullptr){
		return false;
	}

	if (m_collectionIdAttrPtr.IsValid() && !(*m_collectionIdAttrPtr).isEmpty()){
		const QByteArray collectionId = inputParamPtr->GetParamArgumentValue(QByteArrayLiteral("collectionId")).toByteArray();

		return *m_collectionIdAttrPtr == collectionId;
	}

	return true;
}


// reimplemented (imtgql::IGqlRequestExtractor)

istd::IChangeableUniquePtr CObjectCollectionControllerCompBase::ExtractObject(const imtgql::CGqlRequest& gqlRequest, QByteArray& newObjectId, QString& errorMessage) const
{
	return CreateObjectFromRequest(gqlRequest, newObjectId, errorMessage);
}


// protected methods

void CObjectCollectionControllerCompBase::ReplaceComplexFilterFields(imtbase::IComplexCollectionFilter& filter) const
{
	for (const imtbase::IComplexCollectionFilter::FieldInfo& info: filter.GetFields()){
		imtbase::IComplexCollectionFilter::FieldInfo* editableInfoPtr = filter.GetEditableFieldInfo(info.id);
		if (editableInfoPtr != nullptr){
			if (m_fieldReplacementMap.contains(editableInfoPtr->id)){
				editableInfoPtr->id = m_fieldReplacementMap[editableInfoPtr->id];
			}
		}
	}

	std::function<void (imtbase::IComplexCollectionFilter::FilterExpression&)> ProcessGroupFilter = [&](imtbase::IComplexCollectionFilter::FilterExpression& groupFilter){
		for (imtbase::IComplexCollectionFilter::FieldFilter& fieldFilter : groupFilter.fieldFilters){
			if (m_fieldReplacementMap.contains(fieldFilter.fieldId)){
				fieldFilter.fieldId = m_fieldReplacementMap[fieldFilter.fieldId];
			}
		}

		for (imtbase::IComplexCollectionFilter::FilterExpression& groupFilterItem : groupFilter.filterExpressions){
			ProcessGroupFilter(groupFilterItem);
		}
	};

	imtbase::IComplexCollectionFilter::FilterExpression fieldsFilter = filter.GetFilterExpression();
	ProcessGroupFilter(fieldsFilter);
	filter.SetFilterExpression(fieldsFilter);

	UpdateFieldsInfoFromHeaders(filter);
}


void CObjectCollectionControllerCompBase::UpdateFieldsInfoFromHeaders(imtbase::IComplexCollectionFilter& filter) const
{
	if (!m_headersProviderCompPtr.IsValid()){
		return;
	}

	imtcol::ICollectionHeadersProvider::HeaderIds headerIds = m_headersProviderCompPtr->GetHeaderIds();
	for (const QByteArray& headerId : headerIds){
		imtcol::ICollectionHeadersProvider::HeaderInfo headerInfo;
		if (m_headersProviderCompPtr->GetHeaderInfo(headerId, headerInfo)){
			imtbase::IComplexCollectionFilter::FieldInfo* fieldInfoPtr = filter.GetEditableFieldInfo(headerId);
			if (fieldInfoPtr != nullptr){
				if (headerInfo.filterable){
					if (!(fieldInfoPtr->metaInfo.flags & imtbase::IComplexCollectionFilter::SO_TEXT_FILTER)){
						fieldInfoPtr->metaInfo.flags &= imtbase::IComplexCollectionFilter::SO_TEXT_FILTER;
					}
				}

				if (headerInfo.sortable){
					if (!(fieldInfoPtr->metaInfo.flags & imtbase::IComplexCollectionFilter::SO_SORT)){
						fieldInfoPtr->metaInfo.flags &= imtbase::IComplexCollectionFilter::SO_SORT;
					}
				}

				if (headerInfo.fieldType == imtcol::ICollectionHeadersProvider::FT_ARRAY){
					fieldInfoPtr->metaInfo.type = imtbase::IComplexCollectionFilter::FT_ARRAY;
				}
			}
		}
	}
}


bool CObjectCollectionControllerCompBase::GetOperationFromRequest(
			const imtgql::CGqlRequest& gqlRequest,
			imtgql::CGqlParamObject& /*gqlObject*/,
			QString& /*errorMessage*/,
			int& operationType) const
{
	const imtgql::CGqlFieldObject& fields = gqlRequest.GetFields();

	const QByteArrayList ids = fields.GetFieldIds();
	for (const QByteArray& fieldId: ids){
		if (fieldId == "items"){
			operationType = OT_LIST;
			break;
		}
		if (fieldId == "item"){
			operationType = OT_GET;
			break;
		}
		if (fieldId == "addedNotification"){
			operationType = OT_NEW;
			break;
		}
		if (fieldId == "updatedNotification"){
			operationType = OT_UPDATE;
			break;
		}
	}

	return true;
}


QByteArray CObjectCollectionControllerCompBase::GetObjectIdFromInputParams(const imtgql::CGqlParamObject& inputParams) const
{
	QByteArray retVal;

	if (inputParams.GetParamIds().contains(QByteArrayLiteral("id"))){
		retVal = inputParams.GetParamArgumentValue(QByteArrayLiteral("id")).toByteArray();
	}
	else if(inputParams.GetParamIds().contains(QByteArrayLiteral("input"))){
		const imtgql::CGqlParamObject* inputObject = inputParams.GetParamArgumentObjectPtr(QByteArrayLiteral("input"));
		if (inputObject != nullptr){
			return GetObjectIdFromInputParams(*inputObject);
		}
	}

	return retVal;
}


QByteArray CObjectCollectionControllerCompBase::GetObjectIdFromRequest(const imtgql::CGqlRequest& gqlRequest) const
{
	return GetObjectIdFromInputParams(gqlRequest.GetParams());
}


QJsonObject CObjectCollectionControllerCompBase::GetObject(
			const imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	if (!m_objectCollectionCompPtr.IsValid()){
		errorMessage = QString("Unable to get data object. Error: Attribute 'm_objectCollectionCompPtr' was not set").toUtf8();
		SendErrorMessage(0, errorMessage, "CObjectCollectionControllerCompBase");

		return QJsonObject();
	}

	const imtgql::CGqlParamObject* inputParamPtr = gqlRequest.GetParamObject("input");
	if (inputParamPtr == nullptr){
		errorMessage = QString("Unable to get data object. Error: GraphQL input params is invalid.").toUtf8();
		SendErrorMessage(0, errorMessage, "CObjectCollectionControllerCompBase");

		return QJsonObject();
	}

	QByteArray objectId = inputParamPtr->GetParamArgumentValue("id").toByteArray();
	QByteArray objectTypeId = GetObjectTypeIdFromRequest(gqlRequest);

	imtbase::IObjectCollection::DataPtr dataPtr;
	if (!m_objectCollectionCompPtr->GetObjectData(objectId, dataPtr)){
		errorMessage = QString("Unable to get document. Error: Document does not exists");
		SendErrorMessage(0, errorMessage, "CObjectCollectionControllerCompBase");

		return QJsonObject();
	}

	QJsonObject rootObj;
	QJsonObject dataObj;

	if (!CreateRepresentationFromObject(*dataPtr, objectTypeId, gqlRequest, dataObj, errorMessage)){
		if (errorMessage.isEmpty()){
			errorMessage = QString("Unable create object representation for the object with ID: '%1'.").arg(qPrintable(objectId));
		}

		SendErrorMessage(0, errorMessage, "CObjectCollectionControllerCompBase");

		return QJsonObject();
	}

	rootObj.insert(QStringLiteral("data"), dataObj);
	return rootObj;
}


QJsonObject CObjectCollectionControllerCompBase::InsertObject(
			const imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	if (!m_objectCollectionCompPtr.IsValid()){
		errorMessage = QT_TR_NOOP("Internal error");
		SendErrorMessage(0, "Internal error", "Object collection controller");

		return QJsonObject();
	}

	const imtgql::CGqlParamObject* gqlInputParamPtr = gqlRequest.GetParamObject("input");
	if (gqlInputParamPtr == nullptr){
		errorMessage = QString("Unable to insert an object. GraphQL input params is invalid.");
		SendErrorMessage(0, errorMessage, "Object collection controller");

		return QJsonObject();
	}

	QByteArray typeId = GetObjectTypeIdFromRequest(gqlRequest);
	QString name = GetObjectNameFromRequest(gqlRequest);
	QString description = gqlInputParamPtr->GetParamArgumentValue("description").toString();

	if (typeId.isEmpty()){
		typeId = "DocumentInfo";
	}

	QByteArray objectId;
	const imtgql::CGqlParamObject* inputParamPtr = gqlRequest.GetParamObject("input");
	if (inputParamPtr != nullptr){
		objectId = inputParamPtr->GetParamArgumentValue("id").toByteArray();
	}

	QByteArray objectIdFromRepresentation;
	istd::IChangeableUniquePtr newObjectPtr = CreateObjectFromRequest(gqlRequest, objectIdFromRepresentation, errorMessage);
	if (!newObjectPtr.IsValid()){
		SendErrorMessage(0, errorMessage, "Object collection controller");

		return QJsonObject();
	}

	if (!objectIdFromRepresentation.isEmpty()){
		objectId = objectIdFromRepresentation;
	}

	imtbase::ICollectionInfo::Ids elementIds = m_objectCollectionCompPtr->GetElementIds();
	if (elementIds.contains(objectId)){
		errorMessage = QString("Object with ID: '%1' already exists").arg(qPrintable(objectId));
		SendErrorMessage(0, errorMessage, "Object collection controller");

		return QJsonObject();
	}

	istd::TDelPtr<imtbase::IOperationContext> operationContextPtr = nullptr;
	if (m_operationContextControllerCompPtr.IsValid()){
		operationContextPtr = m_operationContextControllerCompPtr->CreateOperationContext("Create", objectId, newObjectPtr.GetPtr());
	}

	QByteArray newObjectId = m_objectCollectionCompPtr->InsertNewObject(typeId, name, description, newObjectPtr.GetPtr(), objectId, nullptr, nullptr, operationContextPtr.GetPtr());
	if (newObjectId.isEmpty()){
		errorMessage = QString("Error when creating a new object. Object-ID: '%1'.").arg(qPrintable(objectId));
		SendErrorMessage(0, errorMessage, "Object collection controller");

		return QJsonObject();
	}

	CreateUserActionLog(objectId, typeId, "Create", gqlRequest);

	sdl::V1_0::imtbase::CAddedNotificationPayload response;
	response.id = newObjectId;

	QJsonObject rootObj;
	QJsonObject dataObj;

	if (!response.WriteToJsonObject(dataObj)){
		errorMessage = QString("Unable to insert object '%1'. Error: Unable to write notification data to the model").arg(qPrintable(newObjectId));
		SendErrorMessage(0, errorMessage, "CObjectCollectionControllerCompBase");
		return QJsonObject();
	}

	rootObj.insert(QStringLiteral("data"), dataObj);
	return rootObj;
}


QJsonObject CObjectCollectionControllerCompBase::UpdateObject(
			const imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	if (!m_objectCollectionCompPtr.IsValid()){
		errorMessage = QString("Unable to update an object. Internal error.");
		SendErrorMessage(0, errorMessage, "CObjectCollectionControllerCompBase");

		return QJsonObject();
	}

	const imtgql::CGqlParamObject* inputParamPtr = gqlRequest.GetParamObject("input");
	if (inputParamPtr == nullptr){
		errorMessage = QString("Unable to update an object. GraphQL input params is invalid.");
		SendErrorMessage(0, errorMessage, "Object collection controller");

		return QJsonObject();
	}

	if (!inputParamPtr->ContainsParam("id")){
		errorMessage = "Bad request. Unable to update object. Parameter 'id' missing.";

		return QJsonObject();
	}

	QByteArray objectId = inputParamPtr->GetParamArgumentValue("id").toByteArray();
	QString name = GetObjectNameFromRequest(gqlRequest);
	QString description = inputParamPtr->GetParamArgumentValue("description").toString();

	imtbase::IObjectCollection::DataPtr savedObjectPtr;
	if (!m_objectCollectionCompPtr->GetObjectData(objectId, savedObjectPtr)){
		errorMessage = QString("Unable to find object with id '%1'").arg(qPrintable(objectId));

		return QJsonObject();
	}

	Q_ASSERT(savedObjectPtr.IsValid());

	if (!UpdateObjectFromRequest(gqlRequest, *savedObjectPtr, errorMessage)){
		if (errorMessage.isEmpty()){
			errorMessage = QString("Can't update object in the collection: '%1'").arg(qPrintable(objectId));
		}

		return QJsonObject();
	}

	istd::TDelPtr<imtbase::IOperationContext> operationContextPtr = nullptr;
	if (m_operationContextControllerCompPtr.IsValid()){
		operationContextPtr = m_operationContextControllerCompPtr->CreateOperationContext("Update", objectId, savedObjectPtr.GetPtr());
	}

	if (!m_objectCollectionCompPtr->SetObjectData(objectId, *savedObjectPtr.GetPtr(), istd::IChangeable::CM_WITHOUT_REFS, operationContextPtr.GetPtr())){
		errorMessage = QString("Can not update object: '%1'").arg(qPrintable(objectId));
		SendErrorMessage(0, errorMessage, "Object collection controller");

		return QJsonObject();
	}

	if (name.length() > 0){
		QString currentName = m_objectCollectionCompPtr->GetElementInfo(objectId, imtbase::ICollectionInfo::EIT_NAME).toString();
		if (currentName != name){
			m_objectCollectionCompPtr->SetElementName(objectId, name);
		}
	}

	QString currentDescription = m_objectCollectionCompPtr->GetElementInfo(objectId, imtbase::ICollectionInfo::EIT_DESCRIPTION).toString();
	if (currentDescription != description){
		m_objectCollectionCompPtr->SetElementDescription(objectId, description);
	}

	QByteArray typeId = m_objectCollectionCompPtr->GetObjectTypeId(objectId);
	CreateUserActionLog(objectId, typeId, "Update", gqlRequest);

	sdl::V1_0::imtbase::CUpdatedNotificationPayload response;
	response.id = objectId;

	QJsonObject rootObj;
	QJsonObject dataObj;

	if (!response.WriteToJsonObject(dataObj)){
		errorMessage = QString("Unable to update object '%1'. Error: Unable to write notification data to the model").arg(qPrintable(objectId));
		return QJsonObject();
	}

	rootObj.insert(QStringLiteral("data"), dataObj);
	return rootObj;
}


QJsonObject CObjectCollectionControllerCompBase::RenameObject(
			const imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	if (!m_objectCollectionCompPtr.IsValid()){
		errorMessage = QString("Unable to rename object. Component reference 'ObjectCollection' was not set");
		SendCriticalMessage(0, errorMessage);

		return QJsonObject();
	}

	const imtgql::CGqlParamObject* inputParamPtr = gqlRequest.GetParamObject("input");
	if (inputParamPtr == nullptr){
		errorMessage = QString("Unable to rename object. GraphQL input params is invalid.");
		SendErrorMessage(0, errorMessage, "Object collection controller");

		return QJsonObject();
	}

	QByteArray objectId = inputParamPtr->GetParamArgumentValue("id").toByteArray();
	QString newName = inputParamPtr->GetParamArgumentValue("newName").toString();

	if (!m_objectCollectionCompPtr->SetElementName(objectId, newName)){
		errorMessage = QString("Unable to set name '%1' for element with ID: '%2'").arg(newName, qPrintable(objectId));
		SendErrorMessage(0, errorMessage, "Object collection controller");

		return QJsonObject();
	}

	QJsonObject rootObj;
	QJsonObject dataObj;

	dataObj.insert(QStringLiteral("id"), QJsonValue::fromVariant(objectId));
	dataObj.insert(QStringLiteral("name"), newName);

	rootObj.insert(QStringLiteral("data"), dataObj);
	return rootObj;
}


QJsonObject CObjectCollectionControllerCompBase::SetObjectDescription(
			const imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	if (!m_objectCollectionCompPtr.IsValid()){
		errorMessage = QString("Unable to set the object description. Component reference 'ObjectCollection' was not set");
		SendCriticalMessage(0, errorMessage);

		return QJsonObject();
	}

	const imtgql::CGqlParamObject* inputParamPtr = gqlRequest.GetParamObject("input");
	if (inputParamPtr == nullptr){
		errorMessage = QString("Unable to set description for object. GraphQL input params is invalid.");
		SendErrorMessage(0, errorMessage, "Object collection controller");

		return QJsonObject();
	}

	QByteArray objectId = inputParamPtr->GetParamArgumentValue("id").toByteArray();
	QString description = inputParamPtr->GetParamArgumentValue("description").toString();

	if (!m_objectCollectionCompPtr->SetElementDescription(objectId, description)){
		errorMessage = QString("Unable to set description '%1' for element with ID: '%2'").arg(description, qPrintable(objectId));
		SendErrorMessage(0, errorMessage, "Object collection controller");

		return QJsonObject();
	}

	QJsonObject rootObj;
	QJsonObject dataObj;

	dataObj.insert(QStringLiteral("id"), QJsonValue::fromVariant(objectId));
	dataObj.insert(QStringLiteral("description"), description);

	rootObj.insert(QStringLiteral("data"), dataObj);
	return rootObj;
}


QJsonObject CObjectCollectionControllerCompBase::ListObjects(
			const imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	if (!m_objectCollectionCompPtr.IsValid()){
		errorMessage = QString("Unable to list objects. Component reference 'ObjectCollection' was not set");
		SendCriticalMessage(0, errorMessage);

		return QJsonObject();
	}

	QJsonObject rootObj;
	QJsonObject dataObj;

	iprm::CParamsSet filterParams;

	int offset = 0;
	int count = -1;

	const imtgql::CGqlParamObject* viewParamsPtr = nullptr;
	const imtgql::CGqlParamObject* inputParamsPtr = gqlRequest.GetParamObject("input");
	if (inputParamsPtr != nullptr){
		viewParamsPtr = inputParamsPtr->GetParamArgumentObjectPtr("viewParams");
	}

	if (viewParamsPtr != nullptr){
		offset = viewParamsPtr->GetParamArgumentValue("offset").toInt();
		count = viewParamsPtr->GetParamArgumentValue("count").toInt();

		PrepareFilters(gqlRequest, *viewParamsPtr, filterParams);
	}
	else if (inputParamsPtr->ContainsParam("selectionParams")){
		const imtgql::CGqlParamObject* selectionParamsPtr = inputParamsPtr->GetParamArgumentObjectPtr("selectionParams");
		if (selectionParamsPtr != nullptr){
			offset = inputParamsPtr->GetParamArgumentValue("offset").toInt();
			count = inputParamsPtr->GetParamArgumentValue("count").toInt();
			PrepareFilters(gqlRequest, *selectionParamsPtr, filterParams);
		}
	}

	if (count == 0){
		count = -1;
	}

	istd::TDelPtr<imtbase::IObjectCollectionIterator> objectCollectionIterator(
		m_objectCollectionCompPtr->CreateObjectCollectionIterator(QByteArray(), offset, count, &filterParams));
	if (objectCollectionIterator == nullptr){
		errorMessage = QString("Object collection iterator creation failed");
		SendErrorMessage(0, errorMessage, "CObjectCollectionControllerCompBase");

		return QJsonObject();
	}

	// GetElementsCount() returns the total count across all pages (from COUNT(*) OVER()),
	// not just the number of records in the current page.
	int elementsCount = objectCollectionIterator->GetElementsCount();

	int pagesCount = std::ceil(elementsCount / (double)count);
	if (pagesCount <= 0){
		pagesCount = 1;
	}

	QJsonObject notificationObj;
	notificationObj.insert(QStringLiteral("pagesCount"), pagesCount);
	notificationObj.insert(QStringLiteral("totalCount"), elementsCount);
	dataObj.insert(QStringLiteral("notification"), notificationObj);

	QJsonArray itemsArray;

	const GqlItemSetupContext setupContext = CreateGqlItemSetupContext(gqlRequest, errorMessage);
	if (!errorMessage.isEmpty()){
		return QJsonObject();
	}

	while (objectCollectionIterator->Next()){
		QJsonObject itemObj;
		if (!SetupGqlItemWithContext(gqlRequest, setupContext, itemObj, objectCollectionIterator.GetPtr(), errorMessage)){
			SendWarningMessage(0, errorMessage, "CObjectCollectionControllerCompBase");
		}
		itemsArray.append(itemObj);
	}
	dataObj.insert(QStringLiteral("items"), itemsArray);

	rootObj.insert(QStringLiteral("data"), dataObj);
	return rootObj;
}


QJsonObject CObjectCollectionControllerCompBase::GetElementsCount(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const
{
	if (!m_objectCollectionCompPtr.IsValid()){
		errorMessage = QString("Unable to get the element count. Component reference 'ObjectCollection' was not set");
		SendCriticalMessage(0, errorMessage);

		return QJsonObject();
	}

	const imtgql::CGqlParamObject* inputParamsPtr = gqlRequest.GetParamObject("input");
	if (inputParamsPtr == nullptr){
		errorMessage = QString("Unable to rename object. GraphQL input params is invalid.");
		SendErrorMessage(0, errorMessage, "Object collection controller");

		return QJsonObject();
	}

	iprm::CParamsSet filterParams;
	const imtgql::CGqlParamObject* viewParamsPtr = inputParamsPtr->GetParamArgumentObjectPtr("viewParams");
	if (viewParamsPtr != nullptr){
		PrepareFilters(gqlRequest, *viewParamsPtr, filterParams);
	}
	else if (inputParamsPtr->ContainsParam("selectionParams")){
		const imtgql::CGqlParamObject* selectionParamsPtr = inputParamsPtr->GetParamArgumentObjectPtr("selectionParams");
		if (selectionParamsPtr != nullptr){
			PrepareFilters(gqlRequest, *selectionParamsPtr, filterParams);
		}
	}

	int elementsCount = m_objectCollectionCompPtr->GetElementsCount(&filterParams);

	QJsonObject rootObj;
	QJsonObject dataObj;
	dataObj.insert(QStringLiteral("itemsCount"), elementsCount);

	rootObj.insert(QStringLiteral("data"), dataObj);
	return rootObj;
}


QJsonObject CObjectCollectionControllerCompBase::DeleteObject(
	const imtgql::CGqlRequest& gqlRequest,
	QString& errorMessage) const
{
	if (!m_objectCollectionCompPtr.IsValid()){
		errorMessage = QString("Unable to remove the object from the collection. Component reference 'ObjectCollection' was not set");
		SendCriticalMessage(0, errorMessage);

		return QJsonObject();
	}

	imtbase::ICollectionInfo::Ids objectIds = ExtractObjectIdsForRemoval(gqlRequest, errorMessage);
	if (!errorMessage.isEmpty()){
		return QJsonObject();
	}

	imtbase::ICollectionInfo::Ids elementIds = m_objectCollectionCompPtr->GetElementIds();
	for (const QByteArray& objectId : objectIds){
		if (!elementIds.contains(objectId)){
			errorMessage = QString("Unable to delete object. Object with ID '%1' does not exists").arg(QString(objectId));
			SendErrorMessage(0, errorMessage, "CObjectCollectionControllerCompBase");
			return QJsonObject();
		}
	}

	istd::TDelPtr<imtbase::IOperationContext> operationContextPtr = nullptr;
	if (m_operationContextControllerCompPtr.IsValid()){
		operationContextPtr = m_operationContextControllerCompPtr->CreateOperationContext("Remove", objectIds.toList().join(';'));
	}

	if (!m_objectCollectionCompPtr->RemoveElements(objectIds, operationContextPtr.GetPtr())){
		errorMessage = QString("Can't remove object with ID: '%1'").arg(QString(objectIds.toList().join(';')));
		SendErrorMessage(0, errorMessage, "CObjectCollectionControllerCompBase");

		return QJsonObject();
	}

	sdl::V1_0::imtbase::CRemovedNotificationPayload response;
	response.elementIds.Emplace();
	response.elementIds->FromList(objectIds);

	QJsonObject rootObj;
	QJsonObject dataObj;

	if (!response.WriteToJsonObject(dataObj)){
		errorMessage = QString("Unable to delete object '%1'. Error: Unable to write notification data to the model").arg(qPrintable(objectIds.toList().join(';')));
		SendErrorMessage(0, errorMessage, "CObjectCollectionControllerCompBase");
		return QJsonObject();
	}

	rootObj.insert(QStringLiteral("data"), dataObj);
	return rootObj;
}


QJsonObject CObjectCollectionControllerCompBase::GetMetaInfo(
			const imtgql::CGqlRequest& /*gqlRequest*/,
			QString& /*errorMessage*/) const
{
	return QJsonObject();
}


QJsonObject CObjectCollectionControllerCompBase::GetInfo(
			const imtgql::CGqlRequest& /*gqlRequest*/,
			QString& /*errorMessage*/) const
{
	return QJsonObject();
}


QJsonObject CObjectCollectionControllerCompBase::GetDataMetaInfo(
			const imtgql::CGqlRequest& /*gqlRequest*/,
			QString& /*errorMessage*/) const
{
	return QJsonObject();
}


QJsonObject CObjectCollectionControllerCompBase::GetObjectTypeId(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const
{
	if (!m_objectCollectionCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'ObjectCollection' was not set", "CObjectCollectionControllerCompBase");
		return QJsonObject();
	}

	const imtgql::CGqlParamObject* inputParamPtr = gqlRequest.GetParamObject("input");
	if (inputParamPtr == nullptr){
		errorMessage = QT_TR_NOOP("Unable to import the object. GQL input params is invalid.");
		SendErrorMessage(0, errorMessage, "CObjectCollectionControllerCompBase");
		return QJsonObject();
	}

	QByteArray objectId = inputParamPtr->GetParamArgumentValue("id").toByteArray();

	QByteArray typeId = m_objectCollectionCompPtr->GetObjectTypeId(objectId);

	QJsonObject rootObj;
	rootObj.insert(QStringLiteral("typeId"), QJsonValue::fromVariant(typeId));

	return rootObj;
}


QJsonObject CObjectCollectionControllerCompBase::ImportObject(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const
{
	if (!m_objectCollectionCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'ObjectCollection' was not set", "CObjectCollectionControllerCompBase");
		return QJsonObject();
	}

	const imtgql::CGqlParamObject* inputParamPtr = gqlRequest.GetParamObject("input");
	if (inputParamPtr == nullptr){
		errorMessage = QT_TR_NOOP("Unable to import the object. GQL input params is invalid.");
		SendErrorMessage(0, errorMessage, "CObjectCollectionControllerCompBase");
		return QJsonObject();
	}

	QByteArray objectData = inputParamPtr->GetParamArgumentValue("fileData").toByteArray();
	QByteArray data = QByteArray::fromBase64(objectData);
	QByteArray typeId = inputParamPtr->GetParamArgumentValue("typeId").toByteArray();
	QString mimeType = inputParamPtr->GetParamArgumentValue("mimeType").toString();
	QString name = inputParamPtr->GetParamArgumentValue("name").toString();
	QString description = inputParamPtr->GetParamArgumentValue("description").toString();

	int index = GetMimeTypeIndex(mimeType);
	if (index < 0){
		errorMessage = "Mime type is invalid";
		SendErrorMessage(0, errorMessage, "CObjectCollectionControllerCompBase");
		return QJsonObject();
	}
	if (index >= m_importExportObjectFactCompPtr.GetCount()){
		errorMessage = "Import/Export object factory index out of range";
		SendErrorMessage(0, errorMessage, "CObjectCollectionControllerCompBase");
		return QJsonObject();
	}
	if (index >= m_filePersistenceCompPtr.GetCount()){
		errorMessage = "File persistence index out of range";
		SendErrorMessage(0, errorMessage, "CObjectCollectionControllerCompBase");
		return QJsonObject();
	}

	istd::IChangeableUniquePtr objectPersistenceInstancePtr = m_importExportObjectFactCompPtr.CreateInstance(index);
	if (!objectPersistenceInstancePtr.IsValid()){
		errorMessage = QString("Unable to import object to the collection. Error: Object instance is invalid");
		SendErrorMessage(0, errorMessage, "CObjectCollectionControllerCompBase");

		return QJsonObject();
	}

	imtbase::CMimeType mime;
	if (!mime.FromString(mimeType)){
		errorMessage = QString("Unable to parse mime type");
		SendErrorMessage(0, errorMessage, "CObjectCollectionControllerCompBase");

		return QJsonObject();
	}

	QString extension = GetExtensionFromMimeType(mime);

	QTemporaryDir tempDir;
	QString filePathTmp = tempDir.path() + "/" + QUuid::createUuid().toString(QUuid::WithoutBraces) + "." + extension;

	QFile file(filePathTmp);
	if (!file.open(QIODevice::WriteOnly)){
		SendErrorMessage(0, QString("Unable to open file with name '%1'").arg(filePathTmp), "CObjectCollectionControllerCompBase");
		return QJsonObject();
	}

	file.write(data);
	file.close();

	if (m_filePersistenceCompPtr[index]->LoadFromFile(*objectPersistenceInstancePtr.GetPtr(), filePathTmp) != ifile::IFilePersistence::OS_OK){
		errorMessage = QString("Unable to import object to the collection");
		SendErrorMessage(0, errorMessage, "CObjectCollectionControllerCompBase");
		QFile::remove(filePathTmp);

		return QJsonObject();
	}

	QByteArray objectUuid = QUuid::createUuid().toByteArray(QUuid::WithoutBraces);
	imtbase::IIdentifiable* identifiableObjectPtr = dynamic_cast<imtbase::IIdentifiable*>(objectPersistenceInstancePtr.GetPtr());
	if (identifiableObjectPtr != nullptr){
		objectUuid = identifiableObjectPtr->GetObjectUuid();
	}

	if (m_objectCollectionCompPtr->GetElementIds().contains(objectUuid)){
		errorMessage = QString("Unable to import object with ID: '%1' to the collection. Error: The object already exists inside the collection").arg(qPrintable(objectUuid));
		SendErrorMessage(0, errorMessage, "CObjectCollectionControllerCompBase");
		QFile::remove(filePathTmp);

		return QJsonObject();
	}

	int typeIdIndex = GetObjectTypeIdIndex(typeId);
	Q_ASSERT_X(typeIdIndex >= 0, "Type ID is invalid", "CObjectCollectionControllerCompBase");

	istd::IChangeableUniquePtr collectionObjectInstancePtr = m_objectFactCompPtr.CreateInstance(typeIdIndex);
	if (!collectionObjectInstancePtr.IsValid()){
		errorMessage = QString("Unable to import object to the collection. Error: Object instance is invalid");
		SendErrorMessage(0, errorMessage, "CObjectCollectionControllerCompBase");

		return QJsonObject();
	}

	if (!ConvertObject(*objectPersistenceInstancePtr.GetPtr(), *collectionObjectInstancePtr.GetPtr(), errorMessage)){
		if (errorMessage.isEmpty()){
			errorMessage = QString("Unable to import object to the collection. Error: Object conversion failed");
		}
		SendErrorMessage(0, errorMessage, "CObjectCollectionControllerCompBase");

		return QJsonObject();
	}

	QByteArray retVal = m_objectCollectionCompPtr->InsertNewObject(typeId, name, description, collectionObjectInstancePtr.GetPtr(), objectUuid);
	if (retVal.isEmpty()){
		errorMessage = QString("Unable to import object with ID: '%1' to the collection. Error: The object could not be inserted into the collection").arg(qPrintable(objectUuid));
		SendErrorMessage(0, errorMessage, "CObjectCollectionControllerCompBase");
		QFile::remove(filePathTmp);

		return QJsonObject();
	}

	QJsonObject rootObj;
	rootObj.insert(QStringLiteral("id"), QJsonValue::fromVariant(objectUuid));
	rootObj.insert(QStringLiteral("status"), QStringLiteral("ok"));
	QFile::remove(filePathTmp);

	return rootObj;
}


QJsonObject CObjectCollectionControllerCompBase::ExportObject(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const
{
	if (!m_objectCollectionCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'ObjectCollection' was not set", "CObjectCollectionControllerCompBase");
		return QJsonObject();
	}

	const imtgql::CGqlParamObject* inputParamPtr = gqlRequest.GetParamObject("input");
	if (inputParamPtr == nullptr){
		errorMessage = QT_TR_NOOP("Unable to import the object. GQL input params is invalid.");
		SendErrorMessage(0, errorMessage, "CObjectCollectionControllerCompBase");
		return QJsonObject();
	}

	QByteArray objectId = inputParamPtr->GetParamArgumentValue("id").toByteArray();
	QString mimeType = inputParamPtr->GetParamArgumentValue("mimeType").toString();

	imtbase::IObjectCollection::DataPtr dataPtr;
	if (!m_objectCollectionCompPtr->GetObjectData(objectId, dataPtr)){
		errorMessage = QString("Unable to export the object with ID: '%1'. Error: Object does not exists").arg(qPrintable(objectId));
		SendErrorMessage(0, errorMessage, "CObjectCollectionControllerCompBase");

		return QJsonObject();
	}

	int index = GetMimeTypeIndex(mimeType);
	if (index < 0){
		errorMessage = "Mime type is invalid";
		SendErrorMessage(0, errorMessage, "CObjectCollectionControllerCompBase");

		return QJsonObject();
	}
	if (index >= m_importExportObjectFactCompPtr.GetCount()){
		errorMessage = "Import/Export object factory index out of range";
		SendErrorMessage(0, errorMessage, "CObjectCollectionControllerCompBase");

		return QJsonObject();
	}
	if (index >= m_filePersistenceCompPtr.GetCount()){
		errorMessage = "File persistence index out of range";
		SendErrorMessage(0, errorMessage, "CObjectCollectionControllerCompBase");

		return QJsonObject();
	}

	imtbase::CMimeType mime;
	if (!mime.FromString(mimeType)){
		errorMessage = QString("Unable to parse mime type");
		SendErrorMessage(0, errorMessage, "CObjectCollectionControllerCompBase");

		return QJsonObject();
	}

	QString extension = GetExtensionFromMimeType(mime);
	QString objectName = GetExportFileName(objectId);

	QTemporaryDir tempDir;
	QString fileName = objectName + "." + extension;
	QString filePathTmp = tempDir.path() + "/" + fileName;

	istd::IChangeableUniquePtr objectPersistenceInstancePtr = m_importExportObjectFactCompPtr.CreateInstance(index);
	if (!objectPersistenceInstancePtr.IsValid()){
		errorMessage = QString("Unable to import object to the collection. Error: Object persistence instance is invalid");
		SendErrorMessage(0, errorMessage, "CObjectCollectionControllerCompBase");

		return QJsonObject();
	}

	if (!ConvertObject(*dataPtr.GetPtr(), *objectPersistenceInstancePtr.GetPtr(), errorMessage)){
		if (errorMessage.isEmpty()){
			errorMessage = QString("Unable to export the object with ID: '%1'. Error: Object conversion failed").arg(qPrintable(objectId));
		}
		SendErrorMessage(0, errorMessage, "CObjectCollectionControllerCompBase");

		return QJsonObject();
	}

	if (m_filePersistenceCompPtr[index]->SaveToFile(*objectPersistenceInstancePtr.GetPtr(), filePathTmp) != ifile::IFilePersistence::OS_OK){
		errorMessage = QString("Unable to export the object with ID: '%1'. Error: Saving data to the file '%1' failed").arg(qPrintable(objectId), filePathTmp);
		SendErrorMessage(0, errorMessage, "CObjectCollectionControllerCompBase");

		return QJsonObject();
	}

	QFile file(filePathTmp);
	if (!file.open(QIODevice::ReadOnly)){
		errorMessage = QString("Unable to export the object with ID: '%1'. Error: Unable to open file with name '%1'").arg(qPrintable(objectId), filePathTmp);
		SendErrorMessage(0, errorMessage, "CObjectCollectionControllerCompBase");
		QFile::remove(filePathTmp);

		return QJsonObject();
	}

	QByteArray data = file.readAll();
	file.close();
	QFile::remove(filePathTmp);

	QJsonObject rootObj;
	rootObj.insert(QStringLiteral("fileData"), QString::fromUtf8(data.toBase64()));
	rootObj.insert(QStringLiteral("fileName"), fileName);

	return rootObj;
}


bool CObjectCollectionControllerCompBase::ConvertObject(const istd::IChangeable& source, istd::IChangeable& target, QString& errorMessage) const
{
	Q_UNUSED(errorMessage);
	return target.CopyFrom(source);
}


int CObjectCollectionControllerCompBase::GetObjectTypeIdIndex(const QByteArray& typeId) const
{
	for (int i = 0; i < m_objectTypeIdAttrPtr.GetCount(); i++){
		if (m_objectTypeIdAttrPtr[i] == typeId){
			return i;
		}
	}

	return -1;
}


int CObjectCollectionControllerCompBase::GetMimeTypeIndex(const QString& mimeType) const
{
	for (int i = 0; i < m_mimeTypeAttrPtr.GetCount(); i++){
		if (m_mimeTypeAttrPtr[i] == mimeType){
			return i;
		}
	}

	return -1;
}


QString CObjectCollectionControllerCompBase::GetExtensionFromMimeType(const imtbase::CMimeType& mimeType) const
{
	const QString& suffix = mimeType.GetSuffix();

	if (!suffix.isEmpty()){
		return suffix;
	}

	return mimeType.GetSubType();
}


QString CObjectCollectionControllerCompBase::GetExportFileName(const QByteArray& objectId) const
{
	QString objectName = m_objectCollectionCompPtr->GetElementInfo(objectId, imtbase::ICollectionInfo::EIT_NAME).toString();
	if (objectName.isEmpty()){
		objectName = QUuid::createUuid().toByteArray(QUuid::WithoutBraces);
	}

	return objectName;
}


bool CObjectCollectionControllerCompBase::UpdateObjectFromRequest(
			const imtgql::CGqlRequest& /*gqlRequest*/,
			istd::IChangeable& /*object*/,
			QString& /*errorMessage*/) const
{
	return false;
}


QVariant CObjectCollectionControllerCompBase::GetInputArgumentFromRequest(
			const imtgql::CGqlRequest& gqlRequest,
			const QString& argumentKey) const
{
	const imtgql::CGqlParamObject* inputParamObjectPtr = gqlRequest.GetParamObject("input");
	if (inputParamObjectPtr == nullptr){
		return QVariant();
	}

	return inputParamObjectPtr->GetParamArgumentValue(argumentKey.toUtf8());
}


imtbase::ICollectionInfo::Ids CObjectCollectionControllerCompBase::ExtractObjectIdsForRemoval(
			const imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	imtbase::ICollectionInfo::Ids retVal;

	const imtgql::CGqlParamObject* inputParamPtr = gqlRequest.GetParamObject("input");
	if (inputParamPtr == nullptr){
		errorMessage = QString("Failed to delete objects: 'input' parameter is missing or invalid.");
		SendErrorMessage(0, errorMessage, "Object collection controller");
		return retVal;
	}

	sdl::V1_0::imtbase::CRemoveElementsInput removeElementsInput;
	if (!removeElementsInput.ReadFromGraphQlObject(*inputParamPtr)){
		errorMessage = QString("Failed to delete objects: unable to parse 'input' parameters.");
		SendErrorMessage(0, errorMessage, "Object collection controller");
		return retVal;
	}

	if (removeElementsInput.elementIds.has_value()){
		retVal = removeElementsInput.elementIds->ToList();
	}
	else{
		errorMessage = QString("Failed to delete objects: 'elementIds' list is empty.");
		SendErrorMessage(0, errorMessage, "Object collection controller");
	}

	return retVal;
}


CObjectCollectionControllerCompBase::GqlItemSetupContext CObjectCollectionControllerCompBase::CreateGqlItemSetupContext(
			const imtgql::CGqlRequest& /*gqlRequest*/,
			QString& /*errorMessage*/) const
{
	return {};
}


bool CObjectCollectionControllerCompBase::SetupGqlItemWithContext(
			const imtgql::CGqlRequest& gqlRequest,
			const GqlItemSetupContext& /*setupContext*/,
			QJsonObject& itemObj,
			const imtbase::IObjectCollectionIterator* objectCollectionIterator,
			QString& errorMessage) const
{
	return SetupGqlItem(gqlRequest, itemObj, objectCollectionIterator, errorMessage);
}


bool CObjectCollectionControllerCompBase::SetupGqlItem(
			const imtgql::CGqlRequest& gqlRequest,
			QJsonObject& itemObj,
			const QByteArray& collectionId,
			QString& /*errorMessage*/) const
{
	QByteArrayList informationIds = GetInformationIds(gqlRequest, "items");
	if (informationIds.isEmpty()){
		return false;
	}

	bool retVal = true;

	for (const QByteArray& informationId : informationIds){
		QVariant elementInformation;

		if(informationId == QByteArray("id")){
			elementInformation = QString(collectionId);
		}
		else if(informationId == QByteArray("name")){
			elementInformation = m_objectCollectionCompPtr->GetElementInfo(collectionId, imtbase::ICollectionInfo::EIT_NAME);
		}
		else if(informationId == QByteArray("description")){
			elementInformation = m_objectCollectionCompPtr->GetElementInfo(collectionId, imtbase::ICollectionInfo::EIT_DESCRIPTION);
		}
		else{
			idoc::MetaInfoPtr elementMetaInfo = m_objectCollectionCompPtr->GetElementMetaInfo(collectionId);
			if (elementMetaInfo.IsValid()){
				if (informationId == QByteArray("added")){
					elementInformation = elementMetaInfo->GetMetaInfo(imtbase::IObjectCollection::MIT_INSERTION_TIME)
					.toDateTime().toString("dd.MM.yyyy hh:mm:ss");
				}
				else if (informationId == QByteArray("lastModified")){
					elementInformation = elementMetaInfo->GetMetaInfo(imtbase::IObjectCollection::MIT_LAST_OPERATION_TIME)
					.toDateTime().toString("dd.MM.yyyy hh:mm:ss");
				}
			}
		}

		if(elementInformation.isNull()){
			elementInformation = GetObjectInformation(informationId, collectionId);
		}

		if (elementInformation.isNull()){
			elementInformation = "";
		}

		itemObj.insert(QString::fromUtf8(informationId), QJsonValue::fromVariant(elementInformation));
	}

	return retVal;
}


bool CObjectCollectionControllerCompBase::SetupGqlItem(
			const imtgql::CGqlRequest& gqlRequest,
			QJsonObject& itemObj,
			const imtbase::IObjectCollectionIterator* objectCollectionIterator,
			QString&/*errorMessage*/) const
{
	if (objectCollectionIterator == nullptr){
		return false;
	}

	QByteArrayList informationIds = GetInformationIds(gqlRequest, "items");
	if (informationIds.isEmpty()){
		return false;
	}

	bool retVal = true;

	QByteArray collectionId = objectCollectionIterator->GetObjectId();

	idoc::MetaInfoPtr elementMetaInfo = objectCollectionIterator->GetDataMetaInfo();

	for (const QByteArray& informationId : informationIds){
		QVariant elementInformation;

		if(informationId == QByteArray("id")){
			elementInformation = QString(collectionId);
		}
		else if(informationId == QByteArray("name")){
			elementInformation = objectCollectionIterator->GetElementInfo("Name");
		}
		else if(informationId == QByteArray("description")){
			elementInformation = objectCollectionIterator->GetElementInfo("Description");
		}
		else{
			if (elementMetaInfo.IsValid()){
				if (informationId == QByteArray("added")){
					elementInformation = elementMetaInfo->GetMetaInfo(imtbase::IObjectCollection::MIT_INSERTION_TIME)
					.toDateTime().toString("dd.MM.yyyy hh:mm:ss");
				}
				else if (informationId == QByteArray("lastModified")){
					elementInformation = elementMetaInfo->GetMetaInfo(imtbase::IObjectCollection::MIT_LAST_OPERATION_TIME)
					.toDateTime().toString("dd.MM.yyyy hh:mm:ss");
				}
			}
		}

		if(elementInformation.isNull()){
			elementInformation = GetObjectInformation(informationId, collectionId);
		}
		if (elementInformation.isNull()){
			elementInformation = "";
		}

		itemObj.insert(QString::fromUtf8(informationId), QJsonValue::fromVariant(elementInformation));
	}

	return retVal;
}


QByteArrayList CObjectCollectionControllerCompBase::GetInformationIds(const imtgql::CGqlRequest& gqlRequest, const QByteArray& objectId) const
{
	QByteArrayList retVal;
	const imtgql::CGqlFieldObject& fields = gqlRequest.GetFields();
	const imtgql::CGqlFieldObject* findObject = fields.GetFieldArgumentObjectPtr(objectId);
	if (findObject != nullptr){
		retVal =findObject->GetFieldIds();
	}

	return retVal;
}


QVariant CObjectCollectionControllerCompBase::GetObjectInformation(const QByteArray& /*informationId*/, const QByteArray& /*objectId*/) const
{
	return QVariant();
}


QByteArray CObjectCollectionControllerCompBase::GetObjectTypeIdFromRequest(const imtgql::CGqlRequest& gqlRequest) const
{
	const imtgql::CGqlParamObject* gqlInputParamPtr = gqlRequest.GetParamObject("input");
	if (gqlInputParamPtr != nullptr){
		QVariant typeId = gqlInputParamPtr->GetParamArgumentValue("typeId");
		if (!typeId.isNull()){
			return typeId.toByteArray();
		}
	}

	return QByteArray();
}


bool CObjectCollectionControllerCompBase::CreateRepresentationFromObject(
			const istd::IChangeable& /*data*/,
			const QByteArray& /*objectTypeId*/,
			const imtgql::CGqlRequest& /*gqlRequest*/,
			QJsonObject& /*dataObj*/,
			QString& /*errorMessage*/) const
{
	return false;
}


istd::IChangeableUniquePtr CObjectCollectionControllerCompBase::CreateObjectFromInputParams(
			const QList<imtgql::CGqlParamObject>& /*inputParams*/,
			QByteArray& /*objectId*/,
			QString& /*errorMessage*/) const
{
	return nullptr;
}


istd::IChangeableUniquePtr CObjectCollectionControllerCompBase::CreateObjectFromRequest(
			const imtgql::CGqlRequest& gqlRequest,
			QByteArray& newObjectId,
			QString& errorMessage) const
{
	QList<imtgql::CGqlParamObject> inputParams;
	inputParams.append(gqlRequest.GetParams());

	return CreateObjectFromInputParams(inputParams, newObjectId, errorMessage);
}


void CObjectCollectionControllerCompBase::PrepareFilters(
			const imtgql::CGqlRequest& gqlRequest,
			const imtgql::CGqlParamObject& inputParamsGql,
			iprm::CParamsSet& filterParams) const
{
	this->SetAdditionalFilters(gqlRequest, inputParamsGql, &filterParams);

	// Inject optional tenant filter
	imtauth::CTenantFilterParam* tenantFilterPtr = CreateTenantFilterParam(gqlRequest);
	if (tenantFilterPtr != nullptr){
		filterParams.SetEditableParameter("TenantFilter", tenantFilterPtr, true);
	}

	const imtgql::CGqlParamObject* complexFilterModelPtr = inputParamsGql.GetParamArgumentObjectPtr("filterModel");
	const imtgql::CGqlParamObject* documentFilterModelPtr = inputParamsGql.GetParamArgumentObjectPtr("documentFilterModel");

	sdl::V1_0::imtbase::CParamsSet paramsSet;
	if (inputParamsGql.ContainsParam("parameters") && paramsSet.ReadFromGraphQlObject(inputParamsGql)){
		for (sdl::V1_0::imtbase::CParameter parameter: paramsSet.parameters->ToList()){
			if (parameter.id){
				QByteArray parameterId = *parameter.id;
				const iser::ISerializable* parameterPtr = m_selectionParams.GetParameter(parameterId);
				if (parameterPtr != nullptr){
					iser::ISerializableUniquePtr filterParameterPtr;
					filterParameterPtr.MoveCastedPtr(parameterPtr->CloneMe());
					if (filterParameterPtr.IsValid()){
						if (parameterId == m_complexCollectionFilterRepresentationController.GetTypeId()){
							imtbase::CComplexCollectionFilter* complexFilterPtr = dynamic_cast<imtbase::CComplexCollectionFilter*>(filterParameterPtr.GetPtr());
							if (complexFilterPtr != nullptr){
								ReplaceComplexFilterFields(*complexFilterPtr);
								SetAdditionalFilters(gqlRequest, *complexFilterPtr);
							}
						}

						filterParams.SetEditableParameter(parameterId, filterParameterPtr);
					}
				}
			}
		}
		if (!GetParamsSetFromRepresentation(paramsSet, filterParams)){
			SendErrorMessage(0, QString("Unable to read filter params set for collection '%1'. Error: Selection Params parsing failed").arg(QString::fromUtf8(*m_collectionIdAttrPtr)));
		}
	}

	if (complexFilterModelPtr != nullptr){
		sdl::V1_0::imtbase::CComplexCollectionFilter complexFilterSdl;
		bool isComplexFilterOk = complexFilterSdl.ReadFromGraphQlObject(*complexFilterModelPtr);
		if (isComplexFilterOk){
			istd::TDelPtr<imtbase::CComplexCollectionFilter> complexFilterPtr = new imtbase::CComplexCollectionFilter();
			if (m_complexCollectionFilterRepresentationController.GetDataModelFromSdlRepresentation(*complexFilterPtr, complexFilterSdl)){
				ReplaceComplexFilterFields(*complexFilterPtr);
				SetAdditionalFilters(gqlRequest, *complexFilterPtr);

				filterParams.SetEditableParameter("ComplexFilter", complexFilterPtr.PopPtr(), true);
			}
			else{
				SendErrorMessage(0, QString("Unable to create collection filter from SDL representation"));
			}
		}
		else{
			SendErrorMessage(0, QString("Unable to read SDL filter model from GraphQL object"));
		}
	}

	if (documentFilterModelPtr != nullptr){
		sdl::V1_0::imtbase::CDocumentCollectionFilter documentFilterSdl;
		bool isDocumentFilterOk = documentFilterSdl.ReadFromGraphQlObject(*documentFilterModelPtr);
		if (isDocumentFilterOk){
			istd::TDelPtr<imtcol::CDocumentCollectionFilter> documentFilterPtr = new imtcol::CDocumentCollectionFilter();
			if (imtcol::CDocumentCollectionFilterRepresentationController::DocumentCollectionFilterRepresentationToModel(documentFilterSdl, *documentFilterPtr, GetLogPtr())){
				filterParams.SetEditableParameter("DocumentFilter", documentFilterPtr.PopPtr(), true);
			}
		}
	}
}


void CObjectCollectionControllerCompBase::SetAdditionalFilters(
			const imtgql::CGqlRequest& /*gqlRequest*/,
			const imtgql::CGqlParamObject& /*viewParamsGql*/,
			iprm::CParamsSet* /*filterParamsPtr*/) const
{
}


void CObjectCollectionControllerCompBase::SetAdditionalFilters(
			const imtgql::CGqlRequest& /*gqlRequest*/,
			imtbase::CComplexCollectionFilter& /*complexFilter*/) const
{
}


imtauth::CTenantFilterParam* CObjectCollectionControllerCompBase::CreateTenantFilterParam(
			const imtgql::CGqlRequest& gqlRequest) const
{
	bool filterEnabled = m_tenantFilterEnabledAttrPtr.IsValid() ? *m_tenantFilterEnabledAttrPtr : false;
	if (!filterEnabled){
		return nullptr;
	}

	return imtauthgql::CreateTenantFilterParam(gqlRequest);
}


QString CObjectCollectionControllerCompBase::GetObjectNameFromRequest(const imtgql::CGqlRequest& gqlRequest) const
{
	const imtgql::CGqlParamObject* gqlInputParamPtr = gqlRequest.GetParamObject("input");
	if (gqlInputParamPtr == nullptr){
		SendErrorMessage(0, "'input' parameter not found", "CObjectCollectionControllerCompBase::GetObjectNameFromRequest");

		return QString();
	}
	QString name = gqlInputParamPtr->GetParamArgumentValue("name").toString();

	return name;
}


bool CObjectCollectionControllerCompBase::CreateCollectionFilterFromViewParamsSdl(
			const sdl::V1_0::imtbase::CCollectionViewParams& viewParams,
			int& offset,
			int& count,
			iprm::CParamsSet& filterParams) const
{
	offset = 0;
	if (viewParams.offset){
		offset = *viewParams.offset;
	}

	count = -1;
	if (viewParams.count){
		count = *viewParams.count;
	}

	if (viewParams.filterModel){
		sdl::V1_0::imtbase::CComplexCollectionFilter sdlFilter = *viewParams.filterModel;
		if (!CreateCollectionFilterFromSdl(sdlFilter, filterParams)){
			return false;
		}
	}

	if (viewParams.documentFilterModel){
		sdl::V1_0::imtbase::CDocumentCollectionFilter documentFilterModel = *viewParams.documentFilterModel;
		if (!CreateDocumentFilterFromSdl(documentFilterModel, filterParams)){
			return false;
		}
	}

	return true;
}


bool CObjectCollectionControllerCompBase::CreateCollectionFilterFromSdl(
			sdl::V1_0::imtbase::CComplexCollectionFilter& collectionFilter,
			iprm::CParamsSet& filterParams) const
{
	istd::TDelPtr<imtbase::CComplexCollectionFilter> complexFilterPtr = new imtbase::CComplexCollectionFilter();
	if (m_complexCollectionFilterRepresentationController.GetDataModelFromSdlRepresentation(*complexFilterPtr, collectionFilter)){
		ReplaceComplexFilterFields(*complexFilterPtr);
		filterParams.SetEditableParameter("ComplexFilter", complexFilterPtr.PopPtr(), true);

		return true;
	}

	return false;
}


bool CObjectCollectionControllerCompBase::CreateDocumentFilterFromSdl(
			sdl::V1_0::imtbase::CDocumentCollectionFilter& documentFilter,
			iprm::CParamsSet& filterParams) const
{
	istd::TDelPtr<imtcol::CDocumentCollectionFilter> documentFilterPtr = new imtcol::CDocumentCollectionFilter();
	if (imtcol::CDocumentCollectionFilterRepresentationController::DocumentCollectionFilterRepresentationToModel(
					documentFilter,
					*documentFilterPtr,
					GetLogPtr())){
		filterParams.SetEditableParameter("DocumentFilter", documentFilterPtr.PopPtr(), true);

		return true;
	}

	return  false;
}


QString CObjectCollectionControllerCompBase::ConvertMetaInfoToString(int /*infoType*/, const QVariant& metaInfoValue) const
{
	return metaInfoValue.toString();
}


istd::IChangeableUniquePtr CObjectCollectionControllerCompBase::CreateObject(const QByteArray& typeId) const
{
	int index = GetObjectTypeIdIndex(typeId);
	if (m_objectFactCompPtr.IsValid() && index < m_objectFactCompPtr.GetCount()){
		return m_objectFactCompPtr.CreateInstance(index);
	}

	return nullptr;
}


bool CObjectCollectionControllerCompBase::DoUpdateObjectFromRequest(
			const imtgql::CGqlRequest& gqlRequest,
			istd::IChangeable& object,
			QByteArray& objectId,
			QString& errorMessage) const
{
	if(UpdateObjectFromRequest(gqlRequest, object, errorMessage)){
		return true;
	}

	const imtgql::CGqlParamObject* inputParamPtr = gqlRequest.GetParamObject("input");
	if (inputParamPtr == nullptr){
		errorMessage = QString("Unable to update an object. GraphQL input params is invalid.");
		SendErrorMessage(0, errorMessage, "Object collection controller");

		return false;
	}

	QString createErrorMessage;
	istd::IChangeableUniquePtr savedObjectPtr = CreateObjectFromRequest(gqlRequest, objectId, createErrorMessage);
	if (!savedObjectPtr.IsValid()){
		if (errorMessage.isEmpty()){
			errorMessage = QString("Can not create object for update: '%1'").arg(qPrintable(objectId));
		}

		SendErrorMessage(0, errorMessage, "Object collection controller");

		return false;
	}


	return object.CopyFrom(*savedObjectPtr);
}


bool CObjectCollectionControllerCompBase::GetParamsSetFromRepresentation(sdl::V1_0::imtbase::CParamsSet& representation, iprm::IParamsSet& paramsSet) const
{
	QJsonObject jsonObject;
	if (!representation.WriteToJsonObject(jsonObject)){
		return false;
	}

	if (!m_paramSetRepresentationController.GetDataModelFromRepresentation(jsonObject, paramsSet)){
		return false;
	}

	if (paramsSet.GetParamIds().contains("ComplexFilter")){
		imtbase::IComplexCollectionFilter* complexFilterParamPtr = dynamic_cast<imtbase::IComplexCollectionFilter*>(paramsSet.GetEditableParameter("ComplexFilter"));
		if (complexFilterParamPtr != nullptr){
			ReplaceComplexFilterFields(*complexFilterParamPtr);
		}
	}

	return true;
}


bool CObjectCollectionControllerCompBase::RegisterFilterToSelectionParams(
			iser::ISerializable& filterParam,
			const imtserverapp::IJsonRepresentationController& representationController)
{
	imtserverapp::IJsonRepresentationController::RepresentationInfo info = representationController.GetRepresentationInfo();
	if (!m_selectionParams.SetEditableParameter(info.modelId, &filterParam)){
		return false;
	}

	return m_paramSetRepresentationController.RegisterSubController(representationController);
}


bool CObjectCollectionControllerCompBase::OnBeforeRemoveElements(
			const QByteArrayList& /*elementIds*/,
			const imtgql::CGqlRequest& /*gqlRequest*/,
			QString& /*errorMessage*/) const
{
	return true;
}


void CObjectCollectionControllerCompBase::OnAfterRemoveElements(
			const QByteArrayList& /*elementIds*/,
			const imtgql::CGqlRequest& /*gqlRequest*/) const
{
}


bool CObjectCollectionControllerCompBase::OnBeforeSetObjectName(
			const QByteArray& /*objectId*/,
			QString& /*newName*/,
			const imtgql::CGqlRequest& /*gqlRequest*/,
			QString& /*errorMessage*/) const
{
	return true;
}


void CObjectCollectionControllerCompBase::OnAfterSetObjectName(
			const QByteArray& /*objectId*/,
			const QString& /* oldName */,
			const QString& /*newName*/,
			const imtgql::CGqlRequest& /*gqlRequest*/) const
{
}


bool CObjectCollectionControllerCompBase::OnBeforeSetObjectDescription(
			const QByteArray& /*objectId*/,
			QString& /*description*/,
			const imtgql::CGqlRequest& /*gqlRequest*/,
			QString& /*errorMessage*/) const
{
	return true;
}


void CObjectCollectionControllerCompBase::OnAfterSetObjectDescription(
			const QByteArray& /*objectId*/,
			const QString& /*description*/,
			const imtgql::CGqlRequest& /*gqlRequest*/) const
{
}


istd::IChangeableUniquePtr CObjectCollectionControllerCompBase::CreateAdaptedObjectData(
			const QByteArray& /*objectId*/,
			const istd::IChangeable& /*object*/,
			const imtgql::CGqlRequest& /*gqlRequest*/) const
{
	return istd::IChangeableUniquePtr();
}


QByteArray CObjectCollectionControllerCompBase::ExtractObjectIdFromGetObjectTypeIdGqlRequest(const imtgql::CGqlRequest& gqlRequest) const
{
	sdl::V1_0::imtbase::CGetObjectTypeIdGqlRequest getObjectTypeIdGqlRequest(gqlRequest, false);
	if (getObjectTypeIdGqlRequest.IsValid()){
		auto arguments = getObjectTypeIdGqlRequest.GetRequestedArguments();
		if (arguments.input->objectId.HasValue()){
			return *arguments.input->objectId;
		}
	}

	return QByteArray();
}


QByteArray CObjectCollectionControllerCompBase::ExtractObjectIdFromGetObjectDataGqlRequest(const imtgql::CGqlRequest& gqlRequest) const
{
	sdl::V1_0::imtbase::CGetObjectDataGqlRequest getObjectDataGqlRequest(gqlRequest, false);
	if (getObjectDataGqlRequest.IsValid()){
		auto arguments = getObjectDataGqlRequest.GetRequestedArguments();
		if (arguments.input->objectId.HasValue()){
			return *arguments.input->objectId;
		}
	}

	return QByteArray();
}

bool CObjectCollectionControllerCompBase::SerializeObject(
			istd::IChangeable& object,
			QByteArray& objectData) const
{
	objectData.clear();

	iser::ISerializable* serializableObject = dynamic_cast<iser::ISerializable*>(&object);
	if (serializableObject == nullptr){
		return false;
	}

	istd::TDelPtr<iser::CJsonMemWriteArchive> archivePtr;
	if (m_versionInfoCompPtr.IsValid()){
		archivePtr.SetPtr(new iser::CJsonMemWriteArchive(m_versionInfoCompPtr.GetPtr()));
	}
	else{
		archivePtr.SetPtr(new iser::CJsonMemWriteArchive());
	}

	if (!serializableObject->Serialize(*archivePtr.GetPtr())){
		return false;
	}

	objectData = archivePtr->GetData();

	return true;
}


bool CObjectCollectionControllerCompBase::DeSerializeObject(
			istd::IChangeable& object,
			const QByteArray& objectData) const
{
	iser::ISerializable* serializableObject = dynamic_cast<iser::ISerializable*>(&object);
	if (serializableObject == nullptr){
		return false;
	}

	iser::CJsonMemReadArchive archive(objectData);
	return serializableObject->Serialize(archive);
}


bool CObjectCollectionControllerCompBase::CreateUserActionLog(
			const QByteArray& objectId,
			const QByteArray& objectTypeId,
			const QByteArray& actionTypeId,
			const imtgql::CGqlRequest& gqlRequest) const
{
	if (!m_userActionManagerCompPtr.IsValid()){
		return false;
	}

	if (!m_objectCollectionCompPtr.IsValid()){
		return false;
	}

	imtauth::IUserRecentAction::TargetInfo targetInfo;
	targetInfo.id = objectId;
	if (objectTypeId.isEmpty()){
		targetInfo.typeId = m_objectCollectionCompPtr->GetObjectTypeId(objectId);
	}
	else{
		targetInfo.typeId = objectTypeId;
	}

	const iprm::IOptionsList* optionsListPtr = m_objectCollectionCompPtr->GetObjectTypesInfo();
	if (optionsListPtr != nullptr){
		int optionsCount = optionsListPtr->GetOptionsCount();
		for (int i = 0; i < optionsCount; ++i){
			QByteArray optionId = optionsListPtr->GetOptionId(i);
			if (optionId == targetInfo.typeId){
				targetInfo.typeName = optionsListPtr->GetOptionName(i);
				break;
			}
		}
	}

	if (actionTypeId == "Delete"){
		targetInfo.name = "";
	}
	else{
		if (targetInfo.name.isEmpty()){
			targetInfo.name = m_objectCollectionCompPtr->GetElementInfo(targetInfo.id, imtbase::ICollectionInfo::ElementInfoType::EIT_NAME).toString();
		}

		if (targetInfo.name.isEmpty()){
			targetInfo.name = targetInfo.id;
		}
	}

	targetInfo.source = *m_collectionIdAttrPtr;

	imtauth::IUserRecentAction::ActionTypeInfo actionTypeInfo;
	actionTypeInfo.id = actionTypeId;
	if (actionTypeId == "Create"){
		actionTypeInfo.name = QT_TR_NOOP(QStringLiteral("Create"));
		actionTypeInfo.description = QT_TR_NOOP(QStringLiteral("Object created"));
	}
	else if (actionTypeId == "Update"){
		actionTypeInfo.name = QT_TR_NOOP(QStringLiteral("Update"));
		actionTypeInfo.description = QT_TR_NOOP(QStringLiteral("Object changed"));
	}
	else if (actionTypeId == "Delete"){
		actionTypeInfo.name = QT_TR_NOOP(QStringLiteral("Delete"));
		actionTypeInfo.description = QT_TR_NOOP(QStringLiteral("Objects was deleted"));
	}

	imtauth::IUserRecentAction::UserInfo userInfo;
	const imtgql::IGqlContext* gqlContextPtr = gqlRequest.GetRequestContext();
	if (gqlContextPtr != nullptr){
		const imtauth::CIdentifiableUserInfo* userInfoPtr = dynamic_cast<const imtauth::CIdentifiableUserInfo*>(gqlContextPtr->GetUserInfo());
		if (userInfoPtr != nullptr){
			userInfo.id = userInfoPtr->GetObjectUuid();
			userInfo.name = userInfoPtr->GetName();
		}
	}

	return m_userActionManagerCompPtr->CreateUserAction(userInfo, actionTypeInfo, targetInfo);
}


} // namespace imtservergql
