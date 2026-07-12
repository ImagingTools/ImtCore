// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.15
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0

/*!
	\qmltype TenantPermissionsDeclarativePage
	\inqmlmodule imtauthgui
	\brief Declarative collection pilot for the tenant permission list
	(ADR-001_DeclarativeModelBinding, Phase 4, list-adapter follow-up).

	Declarative counterpart of the imperative \l TenantPermissionsPage:
	the list field "tenantPermissions" of the model "imtauth.Tenant" is
	exposed by the ViewModel as a \l ListViewModel role adapter, and the
	Repeater binds directly to it instead of the
	updateGui()/updateModel()/rebuildFromFlatArray() choreography.

	Display is a binding to the adapter (model.tenantPermissions); edits
	go through the adapter slots (append / remove) which mark the
	ViewModel dirty and drive write-back. Echo suppression lives
	centrally in the ViewModel / adapter, so no blockingUpdateModel
	guards are needed. Each element is a scalar permission id, reached
	through the modelData role.

	The editor is transport-independent: the same QML works against the
	desktop bridge and the GraphQL bridge; only the plugged bridge
	differs.
*/
DeclarativeViewBase {
	id: editor;

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
			id: permissionsGroup;

			width: parent.width;

			Column {
				id: permissionsColumn;

				width: parent.width;
				spacing: Style.marginM;

				BaseText {
					id: titleText_;

					text: qsTr("Permissions");
					font.pixelSize: Style.fontSizeXL;
					font.bold: true;
					color: Style.textColor;
				}

				BaseText {
					id: hintText_;

					text: qsTr("Select which product features are available for this tenant.");
					font.pixelSize: Style.fontSizeS;
					color: Style.inactiveTextColor;
				}

				Repeater {
					id: permissionsRepeater_;

					model: editor.model ? editor.model.tenantPermissions : null;

					Row {
						id: permissionRow_;

						spacing: Style.marginM;

						BaseText {
							id: permissionIdText_;

							text: model.modelData;
							font.pixelSize: Style.fontSizeM;
							color: Style.textColor;
						}

						Button {
							id: removeButton_;

							text: qsTr("Remove");
							enabled: !editor.readOnly;
							onClicked: editor.model.tenantPermissions.remove(index);
						}
					}
				}

				Row {
					id: addRow_;

					spacing: Style.marginM;

					TextInputElementView {
						id: newPermissionInput_;

						objectName: "NewPermissionInput";
						name: qsTr("Permission id");
						placeHolderText: qsTr("Enter a permission id");
						readOnly: editor.readOnly;
					}

					Button {
						id: addButton_;

						text: qsTr("Add");
						enabled: !editor.readOnly && newPermissionInput_.text !== "";
						onClicked: editor.model.tenantPermissions.append(newPermissionInput_.text);
					}
				}
			}
		}
	}
}
