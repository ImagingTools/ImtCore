import QtQuick 2.15
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0

/*!
	\qmltype UserGeneralDeclarativeEditor
	\inqmlmodule imtauthgui
	\brief Declarative pilot of the general user fields
	(ADR-001_DeclarativeModelBinding, Phase 4 migration order).

	Declarative counterpart of the imperative \l UserGeneralEditor: the
	input controls bind directly to the properties of the ViewModel
	provided by the \l DataModelController, instead of the
	updateGui()/updateModel()/emitUpdateModel() choreography. Display is
	a property binding; writing happens through the ViewModel setter in
	the control handler. Echo suppression lives centrally in the
	ViewModel, so no blockingUpdateModel guards are needed here.

	The same editor works against the desktop bridge and the GraphQL
	bridge (modelId "imtauth.User"); only the plugged bridge differs.
*/
DeclarativeViewBase {
	id: editor;

	width: fields.width;
	height: fields.height;

	controller: DataModelController {
		id: userController;

		modelId: "imtauth.User";
	}

	Component.onCompleted: userController.getModel();

	Column {
		id: fields;

		spacing: Style.marginXL;

		GroupElementView {
			id: generalGroup;

			width: parent.width;

			TextInputElementView {
				id: usernameInput_;

				objectName: "UsernameInput";
				name: qsTr("Username");
				placeHolderText: qsTr("Enter the username");
				readOnly: editor.readOnly;
				showErrorWhenInvalid: true;
				errorText: qsTr("Please enter the username");
				textInputValidator: notEmptyRegexp;

				text: editor.model ? editor.model.username : "";
				onEditingFinished: editor.model.username = usernameInput_.text;

				KeyNavigation.tab: nameInput_;
			}

			RegularExpressionValidator {
				id: notEmptyRegexp;

				regularExpression: /^(?!\s*$).+/;
			}

			TextInputElementView {
				id: nameInput_;

				objectName: "UserNameInput";
				name: qsTr("Name");
				placeHolderText: qsTr("Enter the name");
				readOnly: editor.readOnly;
				showErrorWhenInvalid: true;
				errorText: qsTr("Please enter the name");
				textInputValidator: notEmptyRegexp;

				text: editor.model ? editor.model.name : "";
				onEditingFinished: editor.model.name = nameInput_.text;

				KeyNavigation.tab: mailInput_;
				KeyNavigation.backtab: usernameInput_;
			}

			RegularExpressionValidator {
				id: mailValid;

				regularExpression: /\w+([-+.']\w+)*@\w+([-.]\w+)*\.\w+([-.]\w+)*/;
			}

			TextInputElementView {
				id: mailInput_;

				objectName: "MailInput";
				name: qsTr("Email Address");
				placeHolderText: qsTr("Enter the email");
				readOnly: editor.readOnly;
				showErrorWhenInvalid: true;
				errorText: qsTr("Please enter the email");
				textInputValidator: mailValid;

				text: editor.model ? editor.model.email : "";
				onEditingFinished: editor.model.email = mailInput_.text;

				KeyNavigation.backtab: nameInput_;
			}
		}
	}
}
