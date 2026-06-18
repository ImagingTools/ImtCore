// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauthgql/CPermissionsProviderComp.h>
#include <GeneratedFiles/imtauthsdl/SDL/1.0/CPP/Permissions.h>


// Qt includes
#include <QtCore/QSet>

// ACF includes
#include <iqt/iqt.h>

// ImtCore includes
#include <imtgql/IGqlContext.h>


namespace imtauthgql
{


// protected methods

bool CPermissionsProviderComp::CollectPermissionEntries(
			const imtlic::CFeatureInfo& featureInfo,
			imtsdl::TElementList<sdl::V1_0::imtauth::CPermissionEntry>& entries,
			const QByteArray& languageId,
			const QSet<QByteArray>* allowedPermissionsPtr,
			const QString& parentPath) const
{
	if (!featureInfo.IsPermission()){
		return false;
	}

	QByteArray featureId = featureInfo.GetFeatureId();
	const imtlic::IFeatureInfo::FeatureInfoList& subFeatures = featureInfo.GetSubFeatures();

	QString featureName = featureInfo.GetFeatureName();
	if (m_translationManagerCompPtr.IsValid()){
		featureName = iqt::GetTranslation(m_translationManagerCompPtr.GetPtr(), featureName.toUtf8(), languageId, QByteArrayLiteral("Feature"));
	}

	// Build display path
	QString displayName = parentPath.isEmpty() ? featureName : (parentPath + QStringLiteral(" / ") + featureName);

	// Keep only leaf permissions in entries. Non-leaf nodes are represented in display path only.
	if (subFeatures.isEmpty()){
		if (allowedPermissionsPtr != nullptr && !allowedPermissionsPtr->contains(featureId)){
			return false;
		}

		QString featureDescription = featureInfo.GetFeatureDescription();
		if (m_translationManagerCompPtr.IsValid()){
			featureDescription = iqt::GetTranslation(m_translationManagerCompPtr.GetPtr(), featureDescription.toUtf8(), languageId, QByteArrayLiteral("Feature"));
		}

		sdl::V1_0::imtauth::CPermissionEntry entry;
		entry.permissionId = featureId;
		entry.displayName = displayName;
		entry.description = featureDescription;
		entries.append(entry);

		return true;
	}

	imtsdl::TElementList<sdl::V1_0::imtauth::CPermissionEntry> childEntries;
	int childCount = 0;

	for (int i = 0; i < subFeatures.count(); i++){
		const imtlic::IFeatureInfo::FeatureInfoPtr& subFeaturePtr = subFeatures.at(i);
		if (!subFeaturePtr.IsValid()){
			continue;
		}

		const imtlic::CFeatureInfo* subFeatureInfoPtr = dynamic_cast<const imtlic::CFeatureInfo*>(subFeaturePtr.GetPtr());
		if (subFeatureInfoPtr == nullptr){
			continue;
		}

		if (CollectPermissionEntries(*subFeatureInfoPtr, childEntries, languageId, allowedPermissionsPtr, displayName)){
			childCount++;
		}
	}

	if (allowedPermissionsPtr != nullptr && childCount == 0){
		return false;
	}

	for (const sdl::V1_0::imtauth::CPermissionEntry& childEntry : childEntries.ToList()){
		entries.append(childEntry);
	}

	return childCount > 0;
}


// reimplemented (sdl::V1_0::imtauth::CPermissionsGqlHandlerCompBase)

sdl::V1_0::imtauth::CGetProductPermissionsPayload CPermissionsProviderComp::OnGetProductPermissions(
			const sdl::V1_0::imtauth::CGetProductPermissionsGqlRequest& getProductPermissionsRequest,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	sdl::V1_0::imtauth::CGetProductPermissionsPayload response;

	if (!m_productInfoCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'FeatureContainer' was not set", "CPermissionsProviderComp");
		return response;
	}

	auto arguments = getProductPermissionsRequest.GetRequestedArguments();
	if (!arguments.input.has_value()){
		Q_ASSERT(false);
		return response;
	}

	if (!arguments.input->productId || arguments.input->productId->isEmpty()){
		errorMessage = QString("Unable to get permissions. Product-ID is empty.");
		SendErrorMessage(0, errorMessage, "CPermissionsProviderComp");
		response.errorMessage = errorMessage;
		return response;
	}

	QByteArray languageId;
	const imtgql::IGqlContext* gqlContextPtr = gqlRequest.GetRequestContext();
	if (gqlContextPtr != nullptr){
		languageId = gqlContextPtr->GetLanguageId();
	}

	// Determine tenant filtering
	QSet<QByteArray> allowedPermissions;
	const QSet<QByteArray>* allowedPermissionsPtr = nullptr;

	QByteArray tenantId;
	if (arguments.input->tenantId){
		tenantId = *arguments.input->tenantId;
	}

	if (m_tenantManagerCompPtr.IsValid() && !tenantId.isEmpty()){
		imtauth::ITenantInfoUniquePtr tenantPtr = m_tenantManagerCompPtr->GetTenant(tenantId);
		if (tenantPtr.IsValid()){
			QByteArrayList tenantPermissions = tenantPtr->GetTenantPermissions();
			for (const QByteArray& permId : tenantPermissions){
				allowedPermissions.insert(permId);
			}
			allowedPermissionsPtr = &allowedPermissions;
		}
	}

	response.groups.Emplace();

	imtbase::IObjectCollection* featureCollectionPtr = m_productInfoCompPtr->GetFeatures();
	if (featureCollectionPtr != nullptr){
		imtbase::ICollectionInfo::Ids elementIds = featureCollectionPtr->GetElementIds();
		for (imtbase::ICollectionInfo::Id& elementId : elementIds){
			imtbase::IObjectCollection::DataPtr dataPtr;
			if (featureCollectionPtr->GetObjectData(elementId, dataPtr)){
				const imtlic::CFeatureInfo* featureInfoPtr = dynamic_cast<const imtlic::CFeatureInfo*>(dataPtr.GetPtr());
				if (featureInfoPtr == nullptr || !featureInfoPtr->IsPermission()){
					continue;
				}

				QByteArray groupId = featureInfoPtr->GetFeatureId();
				QString groupName = featureInfoPtr->GetFeatureName();
				if (m_translationManagerCompPtr.IsValid()){
					groupName = iqt::GetTranslation(m_translationManagerCompPtr.GetPtr(), groupName.toUtf8(), languageId, QByteArrayLiteral("Feature"));
				}

				sdl::V1_0::imtauth::CPermissionGroup group;
				group.groupId = groupId;
				group.groupName = groupName;
				group.entries.Emplace();

				// Collect all descendant entries for this group.
				// If top-level feature has no children, include it as a single entry.
				const imtlic::IFeatureInfo::FeatureInfoList& subFeatures = featureInfoPtr->GetSubFeatures();
				bool hasEntries = false;

				if (subFeatures.isEmpty()){
					hasEntries = CollectPermissionEntries(*featureInfoPtr, *group.entries, languageId, allowedPermissionsPtr, QString());
				}
				else{
					for (int i = 0; i < subFeatures.count(); i++){
						const imtlic::IFeatureInfo::FeatureInfoPtr& subFeaturePtr = subFeatures.at(i);
						if (!subFeaturePtr.IsValid()){
							continue;
						}

						const imtlic::CFeatureInfo* subFeatureInfoPtr = dynamic_cast<const imtlic::CFeatureInfo*>(subFeaturePtr.GetPtr());
						if (subFeatureInfoPtr != nullptr){
							if (CollectPermissionEntries(*subFeatureInfoPtr, *group.entries, languageId, allowedPermissionsPtr, QString())){
								hasEntries = true;
							}
						}
					}
				}

				// Skip empty groups when filtering
				if (allowedPermissionsPtr != nullptr && !hasEntries){
					continue;
				}

				response.groups->append(group);
			}
		}
	}

	return response;
}


} // namespace imtauthgql
