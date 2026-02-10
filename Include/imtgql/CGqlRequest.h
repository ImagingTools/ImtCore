// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtgql/IGqlRequest.h>
#include <imtgql/CGqlParamObject.h>
#include <imtgql/CGqlFieldObject.h>


namespace imtgql
{


class CGqlRequest: virtual public IGqlRequest
{
public:
	CGqlRequest(RequestType requestType = RT_QUERY, const QByteArray& commandId = QByteArray());
	~CGqlRequest();

	void AddParamValue(const QByteArray& paramId, const QVariant& value);
	void AddParamValue(const QByteArray& paramId, const CGqlEnum& value);
	void AddParam(const QByteArray& paramId, const CGqlParamObject& param);
	void AddField(const QByteArray& fieldId, const CGqlFieldObject& field);
	void AddSimpleField(const QByteArray& fieldId);
	void SetGqlContext(IGqlContextSharedPtr gqlContext);
	void SetRequestType(RequestType requestType);
	void SetCommandId(const QByteArray& commandId);
	QByteArray GetHeader(const QByteArray& headerId) const;
	void SetHeader(const QByteArray& headerId, const QByteArray& value);

	const CGqlFieldObject& GetFields() const;
	const CGqlParamObject& GetParams() const;

	void SetParams(const CGqlParamObject& params);

	const CGqlFieldObject* GetFieldObject(const QByteArray& fieldId) const;
	const CGqlParamObject* GetParamObject(const QByteArray& paramId) const;

	void SetProtocolVersion(const QByteArray& protocolVersion);

	bool ParseQuery(const QByteArray& query, qsizetype& errorPosition);

	// reimplemented (IGqlRequest)
	virtual QByteArray GetCommandId() const override;
	virtual RequestType GetRequestType() const override;
	virtual QByteArray GetQuery() const override;
	virtual const IGqlContext* GetRequestContext() const override;
	virtual QByteArray GetProtocolVersion() const override;

	// reimplemented (iser::IObject)
	virtual QByteArray GetFactoryId() const override;

	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive& archive) override;

	// reimplemented (istd::IChangeable)
	virtual int GetSupportedOperations() const override;
	virtual bool CopyFrom(const IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
	virtual istd::IChangeableUniquePtr CloneMe(CompatibilityMode mode = CM_WITHOUT_REFS) const override;
	virtual bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;

protected:
	virtual QByteArray CreateQueryFields() const;
	virtual QByteArray CreateQueryParams() const;
	virtual QByteArray AddObjectFieldPart(const CGqlFieldObject& gqlObject) const;
	virtual QByteArray AddObjectParamPart(const CGqlParamObject& gqlObject) const;
	virtual QByteArray AddObjectParamValue(const QVariant& value) const;
	virtual void ParceObjectFieldPart(CGqlFieldObject& gqlObject, const QJsonObject& object) const;
	virtual void ParceObjectParamPart(CGqlParamObject& gqlObject, const QJsonObject& object) const;

private:
	void SetParseObject(const QByteArray& commandId);
	void SetParseText(const QByteArray& text);
	void StartArray(const QByteArray& text);
	bool CloseArray();

protected:
	QByteArray m_commandId;
	RequestType m_requestType;
	CGqlParamObject m_params;
	CGqlFieldObject m_fields;

	bool m_startKey;
	bool m_startValue;
	bool m_startParams;
	bool m_startFields;
	bool m_startFragment;
	bool m_startArrayPrimitives;
	bool m_textString;
	QByteArray m_currentField;
	QByteArray m_protocolVersion;

	CGqlParamObject* m_activeGqlObjectPtr;
	CGqlFieldObject* m_activeFieldObjectPtr;
	QList<CGqlParamObject*> m_objectArrayList;
	IGqlContextSharedPtr m_gqlContextPtr;
	QList<QByteArray> m_activeArrayIds;

	static const QHash<QByteArray, IGqlRequest::RequestType> s_requestNameMap;
};


} // namespace imtgql


