/********************************************************************************
**
**	Copyright (C) 2017-2020 ImagingTools GmbH
**
**	This file is part of the ImagingTools SDK.
**
**	This file may be used under the terms of the GNU Lesser
**	General Public License version 2.1 as published by the Free Software
**	Foundation and appearing in the file LicenseLGPL.txt included in the
**	packaging of this file.  Please review the following information to
**	ensure the GNU Lesser General Public License version 2.1 requirements
**	will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
**	If you are unsure which license is appropriate for your use, please
**	contact us at info@imagingtools.de.
**
**
********************************************************************************/


#pragma once


// ImtCore includes
#include <imtgql/IGqlRequest.h>
#include <imtgql/CGqlObject.h>


namespace imtgql
{


class CGqlRequest: virtual public IGqlRequest
{
public:
	CGqlRequest(RequestType requestType = RT_QUERY, const QByteArray& commandId = QByteArray());

	void AddParam(const CGqlObject& param);
	void AddField(const CGqlObject& field);
	void AddSimpleField(const QByteArray& fieldId);
	const QList<CGqlObject>* GetFields() const;
	const QList<CGqlObject>* GetParams() const;

	// reimplemented (IGqlRequest)
	virtual QByteArray GetCommandId() const override;
	virtual RequestType GetRequestType() const override;
	virtual QByteArray GetQuery() const override;
	virtual bool ParseQuery(const QByteArray& query, int &errorPosition);

	// reimplemented (iser::IObject)
	virtual QByteArray GetFactoryId() const override;

	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive& archive) override;

	// reimplemented (istd::IChangeable)
	virtual int GetSupportedOperations() const override;
	virtual bool CopyFrom(const IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
	virtual IChangeable* CloneMe(CompatibilityMode mode = CM_WITHOUT_REFS) const override;
	virtual bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;

protected:
	virtual QByteArray CreateQueryFields() const;
	virtual QByteArray CreateQueryParams() const;
	virtual QByteArray AddObjectFieldPart(const CGqlObject& gqlObject) const;
	virtual QByteArray AddObjectParamPart(const CGqlObject& gqlObject) const;
	virtual void ParceObjectFieldPart(CGqlObject &gqlObject, const QJsonObject& object) const;
	virtual void ParceObjectParamPart(CGqlObject &gqlObject, const QJsonObject& object) const;

private:
	void SetParseObject(const QByteArray& commandId);
	void SetParseText(const QByteArray& text);
protected:
	QByteArray m_commandId;
	RequestType m_requestType;
	QList<CGqlObject> m_params;
	QList<CGqlObject> m_fields;

	bool m_startKey;
	bool m_startValue;
	bool m_startParams;
	bool m_startFields;
	QByteArray m_currentField;

	CGqlObject* m_activeGqlObjectPtr;
};


} // namespace imtgql


