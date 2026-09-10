import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0
import imtdocgui 1.0

Item {
	id: administrationContainer;
	objectName: "AdministrationView"

	anchors.fill: parent;

	property alias multiPageView: multiPageView;
	signal multiPageUpdated();

	property string productId: AuthorizationController.productId
	// Part of the product this view works in; scopes its permission checks.
	property string permissionPath: ""
	property GqlBasedUserAdministrationApiClient apiClient: GqlBasedUserAdministrationApiClient {
		productId: administrationContainer.productId
		permissionPath: administrationContainer.permissionPath
	}

	Component.onCompleted: {
		Events.subscribeEvent("OnLocalizationChanged", administrationContainer.onLocalizationChanged);
		apiClient.setRolePermissionsTenantId(apiClient.tenantId);
	}

	Component.onDestruction: {
		Events.unSubscribeEvent("OnLocalizationChanged", administrationContainer.onLocalizationChanged);
	}

	Connections {
		target: administrationContainer.apiClient
		function onTenantIdChanged() {
			administrationContainer.apiClient.setRolePermissionsTenantId(administrationContainer.apiClient.tenantId);
		}
	}

	function closeAllDocumentsForManager(manager){
		if (!manager){
			return;
		}

		let ids = manager.getOpenedDocumentIds();
		for (let i = 0; i < ids.length; ++i){
			manager.closeDocument(ids[i]);
		}
	}

	function updatePageName(pageId, displayName) {
		let index = multiPageView.getIndexById(pageId);
		if (index >= 0) {
			multiPageView.pagesModel.setProperty(index, "name", displayName);
		}
	}

	function onLocalizationChanged(language) {
		updatePageName("Roles", qsTr("Roles"));
		updatePageName("Users", qsTr("Users"));
		updatePageName("Groups", qsTr("Groups"));

		onCustomPagesLocalizationChanged();
	}

	function onCustomPagesLocalizationChanged(){
		// Override this function to update custom page names on localization change
	}

	Rectangle {
		anchors.fill: parent;
		color: Style.baseColor;
	}

	Component {
		id: roleCollectionComp;

		MultiDocumentCollectionView {
			documentManager: administrationContainer.apiClient.roleDocumentManager;
			contentColor: Style.baseColor;

			Component.onCompleted: {
				setCollectionViewComp(qsTr("Roles"), roleListComp)
			}
		}
	}

	Component {
		id: roleListComp;

		RoleCollectionView {
			productId: administrationContainer.productId;
			documentManager: administrationContainer.apiClient.roleDocumentManager;
			contentColor: Style.baseColor;
		}
	}

	Component {
		id: userCollectionComp;

		MultiDocumentCollectionView {
			documentManager: administrationContainer.apiClient.userDocumentManager;
			contentColor: Style.baseColor;

			Component.onCompleted: {
				setCollectionViewComp(qsTr("Users"), userListComp)
			}
		}
	}

	Component {
		id: userListComp;

		UserCollectionView {
			productId: administrationContainer.productId;
			documentManager: administrationContainer.apiClient.userDocumentManager;
			contentColor: Style.baseColor;
		}
	}

	Component {
		id: userGroupCollectionComp;

		MultiDocumentCollectionView {
			documentManager: administrationContainer.apiClient.groupDocumentManager;
			contentColor: Style.baseColor;

			Component.onCompleted: {
				setCollectionViewComp(qsTr("Groups"), userGroupListComp)
			}
		}
	}

	Component {
		id: userGroupListComp;

		UserGroupCollectionView {
			productId: administrationContainer.productId;
			documentManager: administrationContainer.apiClient.groupDocumentManager;
			contentColor: Style.baseColor;
		}
	}

	MultiPageView {
		id: multiPageView;

		anchors.top: parent.top;
		anchors.left: parent.left;
		anchors.right: parent.right;
		anchors.bottom: parent.bottom;

		NavigableItem {
			parentSegment: "Administration"
			paths: ["Roles", "Users", "Groups"]
			onActivated: {
				let index = paths.indexOf(matchedPath)
				multiPageView.block = true
				multiPageView.currentIndex = index

				administrationContainer.closeAllDocumentsForManager(administrationContainer.apiClient.roleDocumentManager)
				administrationContainer.closeAllDocumentsForManager(administrationContainer.apiClient.userDocumentManager)
				administrationContainer.closeAllDocumentsForManager(administrationContainer.apiClient.groupDocumentManager)

				if (restPath.length >= 2){
					let documentTypeId = restPath[0]
					let documentId = restPath[1]

					let manager = null
					if (matchedPath === "Roles") manager = administrationContainer.apiClient.roleDocumentManager
					else if (matchedPath === "Users") manager = administrationContainer.apiClient.userDocumentManager
					else if (matchedPath === "Groups") manager = administrationContainer.apiClient.groupDocumentManager

					if (manager){
						manager.openDocument(documentTypeId, documentId)
					}
				}

				multiPageView.block = false
			}
		}

		Component.onCompleted: {
			updateModel();
		}

		property bool block: false
		onCurrentIndexChanged: {
			if (currentIndex >= 0 && multiPageView.pagesModel.count > currentIndex){
				let pageId = multiPageView.pagesModel.get(currentIndex).id
				if (!block){
					NavigationController.push("Administration/" + pageId)
				}
			}
		}

		function addSubPage(pageId, pageName, pageComp, pageIcon){
			let index = multiPageView.getIndexById(pageId);
			if (index >= 0){
				return
			}

			let accessGranted = PermissionsController.checkPermission("View" + pageId)
			if (!accessGranted){
				console.warn("Subpage '" + pageId + "' cannot be displayed. Error: Access denied (check permissions)")

				return
			}

			multiPageView.addPage(pageId, pageName, pageComp, pageIcon);
		}

		function updateModel(){
			multiPageView.clear();

			addSubPage("Roles", qsTr("Roles"), roleCollectionComp, "Icons/Role")
			addSubPage("Users", qsTr("Users"), userCollectionComp, "Icons/Account")
			addSubPage("Groups", qsTr("Groups"), userGroupCollectionComp, "Icons/MultipleUser")

			multiPageView.currentIndex = 0;
			administrationContainer.multiPageUpdated();
		}
	}
}
