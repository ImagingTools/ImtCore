// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtbase/CObjectCollectionComp.h>

// Acula includes
#include <imthype/ITaskResultCollection.h>


namespace imthype
{


class CTaskResultCollectionComp:
			public imtbase::CObjectCollectionComp,
			virtual public ITaskResultCollection
{
public:
	typedef imtbase::CObjectCollectionComp BaseClass;

	I_BEGIN_COMPONENT(CTaskResultCollectionComp);
		I_REGISTER_INTERFACE(imthype::ITaskResultCollection);
	I_END_COMPONENT;

	// reimplemented (ITaskResultCollection)
	virtual void SetArtifactType(const QByteArray& artifactId, ArtifactType artifactType) override;
	virtual void SetArtifactStatus(const QByteArray& artifactId, istd::IInformationProvider::InformationCategory status) override;
	virtual QByteArray InsertNewResult(
				ArtifactType artifactType,
				const QByteArray& typeId,
				const QString& name,
				const QString& description,
				const istd::IInformationProvider* statusProviderPtr = nullptr,
				const istd::IChangeable* defaultValuePtr = nullptr) override;

	// reimplemented (ITaskMetaInfo)
	virtual ArtifactType GetArtifactType(const QByteArray& artifactId) const override;
	virtual QByteArray GetArtifactDataTypeId(const QByteArray& artifactId) const override;

	// reimplemented (imtbase::IMultiStatusProvider)
	virtual const imtbase::ICollectionInfo& GetStatusList() const override;
	virtual const istd::IInformationProvider* GetStatus(const QByteArray& statusId) const override;

	// reimplemented (imtbase::IObjectCollection)
	virtual Id InsertNewObject(
				const QByteArray& typeId,
				const QString& name,
				const QString& description,
				const istd::IChangeable* defaultValuePtr = nullptr,
				const Id& proposedElementId = Id(),
				const idoc::IDocumentMetaInfo* dataMetaInfoPtr = nullptr,
				const idoc::IDocumentMetaInfo* elementMetaInfoPtr = nullptr,
				const imtbase::IOperationContext* operationContextPtr = nullptr) override;

	// reimplemented (istd::IChangeable)
	virtual bool CopyFrom(const IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS) override;

protected:
	struct ResultInfo: virtual public istd::IInformationProvider
	{
		ResultInfo()
			:artifactType(AT_NONE),
			status(istd::IInformationProvider::IC_NONE)
		{
		}

		// reimplemented (istd::IInformationProvider)
		virtual QDateTime GetInformationTimeStamp() const override;
		virtual InformationCategory GetInformationCategory() const override;
		virtual int GetInformationId() const override;
		virtual QString GetInformationDescription() const override;
		virtual QString GetInformationSource() const override;
		virtual int GetInformationFlags() const override;

		ArtifactType artifactType;
		istd::IInformationProvider::InformationCategory status;
		QString statusText;
		QDateTime statusTimestamp;
		QByteArray dataTypeId;
	};

	typedef QMap<QByteArray, ResultInfo> ResultInfoMap;
	ResultInfoMap m_resultInfoMap;
};


} // namespace imthype


