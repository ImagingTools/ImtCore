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
			historyPath: "Administration/Roles";
			contentColor: Style.baseColor;
			pageIcon: "Icons/Role";

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
			historyPath: "Administration/Users";
			contentColor: Style.baseColor;
			pageIcon: "Icons/Account";

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
			historyPath: "Administration/Groups";
			contentColor: Style.baseColor;
			pageIcon: "Icons/MultipleUser";

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
			id: adminNavigation
			parentSegment: "Administration"
			paths: ["Roles", "Users", "Groups"]
			// The sub-page a path asked for, applied again when the page model is (re)built. Kept here and not on
			// the page view: a path delivered while the page is being created arrives before the view sets its defaults.
			property string requestedPageId: ""
			// The bare "Administration" entry shows the first sub-page, as the page does when it opens.
			onParentActivated: {
				if (restPath.length > 0){
					return
				}

				adminNavigation.requestedPageId = ""
				multiPageView.block = true
				multiPageView.currentIndex = 0
				multiPageView.block = false
			}
			onActivated: {
				// By id: sub-pages hidden by permissions shift the indexes. Remembered for a page whose model is not built yet.
				adminNavigation.requestedPageId = matchedPath
				let index = multiPageView.getIndexById(matchedPath)
				multiPageView.block = true
				if (index >= 0){
					multiPageView.currentIndex = index
				}

				// A sub-page path lands on its list; open editors stay open (they used to be closed, losing unsaved edits).
				if (restPath.length < 2){
					let subPage = multiPageView.getPageById(matchedPath)
					if (subPage && subPage.setCurrentTabIndex){
						subPage.setCurrentTabIndex(0)
					}
				}

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
					adminNavigation.requestedPageId = pageId
					NavigationController.push("Administration/" + pageId)
				}
			}
		}

		function addSubPage(pageId, pageName, pageComp, pageIcon){
			let index = multiPageView.getIndexById(pageId);
			if (index >= 0){
				return
			}

			let accessGranted = PermissionsController.checkPermission("/Administration/" + pageId + "/ViewPage")
			if (!accessGranted){
				console.warn("Subpage '" + pageId + "' cannot be displayed. Error: Access denied (check permissions)")

				return
			}

			multiPageView.addPage(pageId, pageName, pageComp, pageIcon);
		}

		function updateModel(){
			multiPageView.block = true;
			multiPageView.clear();

			addSubPage("Roles", qsTr("Roles"), roleCollectionComp, "Icons/Role")
			addSubPage("Users", qsTr("Users"), userCollectionComp, "Icons/Account")
			addSubPage("Groups", qsTr("Groups"), userGroupCollectionComp, "Icons/MultipleUser")

			// The initial sub-page is part of opening the page, not a navigation of its own.
			let requestedIndex = adminNavigation.requestedPageId !== "" ? multiPageView.getIndexById(adminNavigation.requestedPageId) : -1;
			multiPageView.currentIndex = requestedIndex >= 0 ? requestedIndex : 0;
			multiPageView.block = false;
			administrationContainer.multiPageUpdated();
		}
	}
}
