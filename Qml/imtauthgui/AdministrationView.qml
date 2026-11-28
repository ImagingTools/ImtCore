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

	// The application has the assistant (imtassistgui AssistantCenter answers its state); the page "Assistant" configures it
	property bool assistantSupported: false
	// The assistant of the application is enabled for the organization
	property bool assistantActive: false

	Component.onCompleted: {
		Events.subscribeEvent("OnLocalizationChanged", administrationContainer.onLocalizationChanged);
		Events.subscribeEvent("AssistantStateChanged", administrationContainer.onAssistantStateChanged);
		apiClient.setRolePermissionsTenantId(apiClient.tenantId);
		Events.sendEvent("AssistantStateRequested");
	}

	Component.onDestruction: {
		Events.unSubscribeEvent("OnLocalizationChanged", administrationContainer.onLocalizationChanged);
		Events.unSubscribeEvent("AssistantStateChanged", administrationContainer.onAssistantStateChanged);
	}

	function onAssistantStateChanged(state){
		administrationContainer.assistantActive = state.active;

		if (!administrationContainer.assistantSupported){
			administrationContainer.assistantSupported = true;
			multiPageView.addSubPage("Assistant", qsTr("Assistant"), assistantSettingsComp, "Icons/Magic");
			multiPageView.updateNavigationPaths();
		}
	}

	/**
		Open the assistant of the application with the current sub-page and its selection as context.
		The context is shown in the assistant and sent only with the next message of the user.
	*/
	function openAssistant(){
		let index = multiPageView.currentIndex;
		if (index < 0 || index >= multiPageView.pagesModel.count){
			Events.sendEvent("OpenAssistant", []);

			return;
		}

		let pageId = multiPageView.pagesModel.get(index).id;
		let pageName = multiPageView.pagesModel.get(index).name;
		let data = {"page": "Administration/" + pageId};

		let page = multiPageView.getPageById(pageId);
		if (page && page.collectionView){
			data["selectedIds"] = page.collectionView.getSelectedIds();
		}

		Events.sendEvent("OpenAssistant", [{"kind": "administration", "label": qsTr("Administration: %1").arg(pageName), "data": data}]);
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
		updatePageName("Assistant", qsTr("Assistant"));

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

	// Loaded by URL: applications without the assistant do not need imtassistgui
	Component {
		id: assistantSettingsComp;

		Loader {
			source: "qrc:/qml/imtassistgui/AssistantSettingsPage.qml";
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
				multiPageView.activatePage(0)
				multiPageView.block = false
			}
			onActivated: {
				// By id: sub-pages hidden by permissions shift the indexes. Remembered for a page whose model is not built yet.
				adminNavigation.requestedPageId = matchedPath
				let index = multiPageView.getIndexById(matchedPath)
				multiPageView.block = true
				if (index >= 0){
					multiPageView.activatePage(index)
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

		function updateNavigationPaths(){
			let pageIds = [];
			for (let i = 0; i < multiPageView.pagesModel.count; ++i){
				pageIds.push(multiPageView.pagesModel.get(i).id);
			}

			adminNavigation.paths = pageIds;
		}

		Button {
			id: assistantButton;
			objectName: "AdministrationAssistantButton";

			anchors.left: parent.left;
			anchors.leftMargin: Style.marginM;
			anchors.bottom: parent.bottom;
			anchors.bottomMargin: Style.marginM;
			z: 1;

			width: multiPageView.compact ? Style.iconSizeS : multiPageView.panelWidth - 2 * Style.marginM;
			visible: administrationContainer.assistantActive;
			text: multiPageView.compact ? "" : qsTr("Ask the assistant");
			iconSource: "qrc:/" + Style.getIconPath("Icons/Magic", Icon.State.On, Icon.Mode.Normal);

			onClicked: {
				administrationContainer.openAssistant();
			}
		}

		function updateModel(){
			multiPageView.block = true;
			multiPageView.clear();

			addSubPage("Roles", qsTr("Roles"), roleCollectionComp, "Icons/Role")
			addSubPage("Users", qsTr("Users"), userCollectionComp, "Icons/Account")
			addSubPage("Groups", qsTr("Groups"), userGroupCollectionComp, "Icons/MultipleUser")

			if (administrationContainer.assistantSupported){
				addSubPage("Assistant", qsTr("Assistant"), assistantSettingsComp, "Icons/Magic")
			}

			// Products add their own sub-pages here, so the initial sub-page is chosen among all of them.
			administrationContainer.multiPageUpdated();

			/**
				The initial sub-page is part of opening the page, not a navigation of its own.
				Activated, not only selected: when it already was the current index, nothing else loads it.
			*/
			let requestedIndex = adminNavigation.requestedPageId !== "" ? multiPageView.getIndexById(adminNavigation.requestedPageId) : -1;
			multiPageView.activatePage(requestedIndex >= 0 ? requestedIndex : 0);
			multiPageView.block = false;

			// Every sub-page can be navigated to, the ones of the product too. A path waiting for one is delivered now.
			multiPageView.updateNavigationPaths();
		}
	}
}
