import QtQuick 2.15
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0

/*!
	\qmltype TenantGeneralDeclarativePage
	\inqmlmodule imtauthgui
	\brief Declarative facade of the general tenant settings
	(ADR-001_DeclarativeModelBinding, Phase 4 migration order, item 1).

	Declarative counterpart of the imperative \l TenantGeneralPage: the
	scalar fields (name, description, isActive) of the model
	"imtauth.Tenant" bind directly to the ViewModel provided by the
	\l DataModelController, replacing the
	updateGui()/updateModel()/doUpdateModel() choreography with property
	bindings and ViewModel setters. Echo suppression lives centrally in
	the ViewModel, so no blockingUpdateModel guards are needed.

	The editor is transport-independent: the same QML works against the
	desktop bridge and the GraphQL bridge; only the plugged bridge
	differs. Collection fields (members, invitations, tenantPermissions)
	are handled declaratively via the CListViewModel list adapter; see
	the collection pilot TenantPermissionsDeclarativePage.
*/
DeclarativeViewBase {
	id: editor;

	property bool canEditName: true;
	property bool canEditDescription: true;
	property bool canEditIsActive: true;

	width: fields.width;
	height: fields.height;

	controller: DataModelController {
		id: tenantController;

		modelId: "imtauth.Tenant";
	}

	Component.onCompleted: tenantController.getModel();

	Column {
		id: fields;

		spacing: Style.marginXL;

		GroupElementView {
			id: generalGroup;

			width: parent.width;

			TextInputElementView {
				id: nameInput_;

				objectName: "TenantNameInput";
				name: qsTr("Tenant Name");
				placeHolderText: qsTr("Enter the tenant name");
				readOnly: editor.readOnly || !editor.canEditName;

				text: editor.model ? editor.model.name : "";
				onEditingFinished: editor.model.name = nameInput_.text;

				KeyNavigation.tab: descriptionInput_;
			}

			TextInputElementView {
				id: descriptionInput_;

				objectName: "TenantDescriptionInput";
				name: qsTr("Description");
				placeHolderText: qsTr("Enter the description");
				readOnly: editor.readOnly || !editor.canEditDescription;

				text: editor.model ? editor.model.description : "";
				onEditingFinished: editor.model.description = descriptionInput_.text;

				KeyNavigation.backtab: nameInput_;
			}

			SwitchElementView {
				id: isActiveSwitch_;

				objectName: "TenantIsActiveSwitch";
				name: qsTr("Active");
				readOnly: editor.readOnly || !editor.canEditIsActive;

				checked: editor.model ? editor.model.isActive : false;
				onCheckedChanged: {
					if (editor.model && editor.model.isActive !== isActiveSwitch_.checked){
						editor.model.isActive = isActiveSwitch_.checked;
					}
				}
			}
		}
	}
}
