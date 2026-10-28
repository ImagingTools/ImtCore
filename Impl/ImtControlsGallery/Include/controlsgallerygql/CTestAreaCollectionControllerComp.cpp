#include <controlsgallerygql/CTestAreaCollectionControllerComp.h>


// STL includes
#include <algorithm>

// Qt includes
#include <QtCore/QDebug>
#include <QtCore/QElapsedTimer>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QRegularExpression>

// ImtCore includes
#include <imtgql/CGqlRequest.h>


namespace controlsgallerygql
{


namespace
{


const int s_columnCount = 15;
const int s_elementCount = 500;
const QByteArray s_collectionId = QByteArrayLiteral("TestArea");
const QByteArray s_listCommandId = QByteArrayLiteral("TestAreaList");
const QByteArray s_headersCommandId = QByteArrayLiteral("GetCollectionHeaders");


QString RowLabel(int row)
{
	return QStringLiteral("%1").arg(row + 1, 3, 10, QChar('0'));
}


QByteArray HeaderId(int column)
{
	return QByteArrayLiteral("column") + QByteArray::number(column + 1);
}


QString CellValue(int row, int column)
{
	return RowLabel(row) + QStringLiteral("-") + QString::number(column + 1);
}


QString JsonText(const QJsonObject& object, const QString& key, const QString& prefixedKey)
{
	if (object.contains(key)){
		return object.value(key).toString();
	}

	return object.value(prefixedKey).toString();
}


void ReadSortingJson(const QJsonValue& value, QByteArray& fieldId, bool& descending)
{
	if (value.isObject()){
		const QJsonObject object = value.toObject();
		const QString id = JsonText(object, QStringLiteral("fieldId"), QStringLiteral("m_fieldId"));
		const QString order = JsonText(object, QStringLiteral("sortingOrder"), QStringLiteral("m_sortingOrder"));
		if (!id.isEmpty() && !order.isEmpty()){
			fieldId = id.toUtf8();
			descending = order.compare(QStringLiteral("DESC"), Qt::CaseInsensitive) == 0;
		}

		for (auto it = object.begin(); it != object.end(); ++it){
			ReadSortingJson(it.value(), fieldId, descending);
		}
		return;
	}

	if (value.isArray()){
		const QJsonArray array = value.toArray();
		for (const QJsonValue& item : array){
			ReadSortingJson(item, fieldId, descending);
		}
		return;
	}

	if (value.isString()){
		const QJsonDocument nested = QJsonDocument::fromJson(value.toString().toUtf8());
		if (!nested.isNull()){
			ReadSortingJson(nested.isArray() ? QJsonValue(nested.array()) : QJsonValue(nested.object()), fieldId, descending);
		}
	}
}


void ReadSorting(const imtgql::CGqlParamObject& object, QByteArray& fieldId, bool& descending)
{
	if (object.ContainsParam(QByteArrayLiteral("fieldId")) && object.ContainsParam(QByteArrayLiteral("sortingOrder"))){
		fieldId = object.GetParamArgumentValue(QByteArrayLiteral("fieldId")).toByteArray();
		descending = object.GetParamArgumentValue(QByteArrayLiteral("sortingOrder")).toString().compare(QStringLiteral("DESC"), Qt::CaseInsensitive) == 0;
	}

	const QByteArrayList ids = object.GetParamIds();
	for (const QByteArray& id : ids){
		const QList<const imtgql::CGqlParamObject*> children = object.GetParamArgumentObjectPtrList(id);
		for (const imtgql::CGqlParamObject* childPtr : children){
			if (childPtr != nullptr){
				ReadSorting(*childPtr, fieldId, descending);
			}
		}

		ReadSortingJson(QJsonValue(object.GetParamArgumentValue(id).toString()), fieldId, descending);
	}
}


void ReadSortingFromQuery(const QByteArray& query, QByteArray& fieldId, bool& descending)
{
	const QString text = QString::fromUtf8(query);
	const QRegularExpression fieldExpression(QStringLiteral("(?:m_)?fieldId\\s*[:=]\\s*\\\\?\"([^\"\\\\]+)\\\\?\""));
	const QRegularExpression orderExpression(QStringLiteral("(?:m_)?sortingOrder\\s*[:=]\\s*\\\\?\"([^\"\\\\]+)\\\\?\""));
	const QRegularExpressionMatch fieldMatch = fieldExpression.match(text);
	const QRegularExpressionMatch orderMatch = orderExpression.match(text);
	if (fieldMatch.hasMatch()){
		fieldId = fieldMatch.captured(1).toUtf8();
	}
	if (orderMatch.hasMatch()){
		descending = orderMatch.captured(1).compare(QStringLiteral("DESC"), Qt::CaseInsensitive) == 0;
	}
}


} // namespace


// public methods

// reimplemented (imtgql::IGqlRequestHandler)

bool CTestAreaCollectionControllerComp::IsRequestSupported(const imtgql::CGqlRequest& gqlRequest) const
{
	if (!BaseClass::IsRequestSupported(gqlRequest)){
		return false;
	}

	if (gqlRequest.GetCommandId() == s_listCommandId){
		return true;
	}

	const imtgql::CGqlParamObject* inputParamPtr = gqlRequest.GetParamObject(QByteArrayLiteral("input"));
	if (inputParamPtr == nullptr){
		return false;
	}

	return inputParamPtr->GetParamArgumentValue(QByteArrayLiteral("collectionId")).toByteArray() == s_collectionId;
}


// protected methods

// reimplemented (imtservergql::CGqlRequestHandlerCompBase)

QJsonObject CTestAreaCollectionControllerComp::CreateInternalResponse(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const
{
	Q_UNUSED(errorMessage);

	if (gqlRequest.GetCommandId() == s_headersCommandId){
		return CreateHeadersResponse();
	}

	return CreateListResponse(gqlRequest);
}


// private methods

QJsonObject CTestAreaCollectionControllerComp::CreateHeadersResponse() const
{
	QJsonArray headers;
	for (int column = 0; column < s_columnCount; ++column){
		QJsonObject header;
		header.insert(QStringLiteral("id"), QString::fromUtf8(HeaderId(column)));
		header.insert(QStringLiteral("name"), QStringLiteral("Column %1").arg(column + 1));
		header.insert(QStringLiteral("filterable"), false);
		header.insert(QStringLiteral("sortable"), true);
		headers.append(header);
	}

	QJsonObject data;
	data.insert(QStringLiteral("headers"), headers);

	QJsonObject root;
	root.insert(QStringLiteral("data"), data);
	return root;
}


QJsonObject CTestAreaCollectionControllerComp::CreateListResponse(const imtgql::CGqlRequest& gqlRequest) const
{
	int offset = 0;
	int count = -1;
	QByteArray sortFieldId;
	bool descending = false;

	QElapsedTimer timer;
	timer.start();

	const imtgql::CGqlParamObject* inputParamPtr = gqlRequest.GetParamObject(QByteArrayLiteral("input"));
	if (inputParamPtr != nullptr){
		offset = inputParamPtr->GetParamArgumentValue(QByteArrayLiteral("offset")).toInt();
		count = inputParamPtr->GetParamArgumentValue(QByteArrayLiteral("count")).toInt();
		ReadSorting(*inputParamPtr, sortFieldId, descending);
	}

	if (sortFieldId.isEmpty()){
		ReadSortingFromQuery(gqlRequest.GetQuery(), sortFieldId, descending);
	}

	if (sortFieldId.isEmpty()){
		const QByteArray query = gqlRequest.GetQuery();
		const int fieldPosition = query.indexOf("fieldId");
		qDebug() << "[TestArea] sort field not found, query size" << query.size()
				 << "fieldId at" << fieldPosition
				 << query.mid(qMax(0, fieldPosition - 40), 180);
	}

	qDebug() << "[TestArea] request parsed in" << timer.elapsed() << "ms"
			 << "offset" << offset
			 << "count" << count
			 << "sort" << sortFieldId
			 << (descending ? "DESC" : "ASC");

	if (offset < 0){
		offset = 0;
	}

	QList<int> order;
	order.reserve(s_elementCount);
	for (int row = 0; row < s_elementCount; ++row){
		order.append(row);
	}

	if (!sortFieldId.isEmpty()){
		std::stable_sort(order.begin(), order.end(), [sortFieldId, descending](int left, int right) {
			QString leftText;
			QString rightText;
			if (sortFieldId == QByteArrayLiteral("id") || sortFieldId == QByteArrayLiteral("name")){
				leftText = RowLabel(left);
				rightText = RowLabel(right);
			}
			else{
				for (int column = 0; column < s_columnCount; ++column){
					if (HeaderId(column) == sortFieldId){
						leftText = CellValue(left, column);
						rightText = CellValue(right, column);
						break;
					}
				}
			}

			if (leftText == rightText){
				return false;
			}

			const bool less = leftText < rightText;
			return descending ? !less : less;
		});
	}

	int pageSize = count;
	if (pageSize < 1){
		pageSize = s_elementCount;
	}

	int itemCount = pageSize;
	if (offset >= order.size()){
		itemCount = 0;
	}
	else if (offset + itemCount > order.size()){
		itemCount = order.size() - offset;
	}

	QJsonArray items;
	for (int position = offset; position < offset + itemCount; ++position){
		const int row = order.at(position);
		const QString label = RowLabel(row);

		QJsonObject item;
		item.insert(QStringLiteral("id"), label);
		item.insert(QStringLiteral("name"), QStringLiteral("Item ") + label);
		item.insert(QStringLiteral("typeId"), QStringLiteral("TestAreaItem"));
		for (int column = 0; column < s_columnCount; ++column){
			item.insert(QString::fromUtf8(HeaderId(column)), CellValue(row, column));
		}
		items.append(item);
	}

	int pagesCount = (s_elementCount + pageSize - 1) / pageSize;
	if (pagesCount < 1){
		pagesCount = 1;
	}

	QJsonObject notification;
	notification.insert(QStringLiteral("pagesCount"), pagesCount);
	notification.insert(QStringLiteral("totalCount"), s_elementCount);

	QJsonObject data;
	data.insert(QStringLiteral("items"), items);
	data.insert(QStringLiteral("notification"), notification);

	QJsonObject root;
	root.insert(QStringLiteral("data"), data);

	const QString firstId = items.isEmpty() ? QString() : items.at(0).toObject().value(QStringLiteral("id")).toString();
	const QString lastId = items.isEmpty() ? QString() : items.at(items.size() - 1).toObject().value(QStringLiteral("id")).toString();
	qDebug() << "[TestArea] response ready in" << timer.elapsed() << "ms"
			 << "rows" << items.size()
			 << "pages" << pagesCount
			 << "first" << firstId
			 << "last" << lastId;
	return root;
}


} // namespace controlsgallerygql
