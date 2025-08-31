import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0
import imtcolgui 1.0


PopupMenuDialog {
	id: popup;

	opacity: 0;

	property string commandId: "";
	property string dataProverState: ""
	property string filterText: "";

	property var comboDecorator: null;
	property var textField_: null;

	signal editSignal();
	signal clearSignal();
	signal closeSignal();

	onHeightChanged: {
		popup.setY();
	}

	onDataProverStateChanged: {
		if(popup.decorator_){
			let visible_ = popup.dataProverState.toLowerCase() == "loading"
			popup.decorator_.setLoadingSplashRecVisible(visible_)
		}
	}

	onDecorator_Changed: {
		if(!popup.decorator_){
			return
		}

		popup.forceActiveFocus()
		popup.decorator_.topContentLoaderSourceComp = searchField
		popup.decorator_.bottomContentLoaderSourceComp = searchField
	}

	function setY(){
	}

	function requestNextBatch(){
	}

	function setTextFocus(focus){
		if(popup.textField_){
			popup.textField_.setFocus(focus);
		}
	}

	function onKeyboardUp(){
		popup.setTextFocus(false)

		popup.hoverBlocked = true;
		if(popup.selectedIndex > 0){
			popup.selectedIndex--;
			popup.decorator_.contentYCorrection(false);
		}
	}

	function onKeyboardDown(){
		popup.setTextFocus(false)

		popup.hoverBlocked = true;

		let elementCount = popup.model.getItemsCount()
		if(popup.selectedIndex >= elementCount - 1){
			popup.createAdditionalQuery();
		}
		else{
			popup.selectedIndex++;
			popup.decorator_.contentYCorrection(true);
		}
	}

	Component {
		id: searchField

		CustomTextField {
			id: filterField;

			z: 100;
			width: popup.width;
			height: popup.comboDecorator ? popup.comboDecorator.height : Style.controlHeightM;
			margin: Style.marginM;
			radius: popup.decorator_ ? popup.decorator_.radius : Style.buttonRadius;
			anchors.top: parent.top;
			anchors.left: parent.left;
			textSize: popup.textSize;
			fontColor: popup.fontColor;

			Component.onCompleted: {
				popup.textField_ = filterField;
			}

			Binding {
				target: filterField
				property: "text"
				value: popup.filterText
				when: filterField.text !== popup.filterText
			}

			onAccepted: {
				popup.closeSignal(popup.model);
				popup.close();
			}

			onTextChanged: {
				popup.filterText = text

				popup.editSignal();
			}

			Button{
				id: clearButton;

				anchors.verticalCenter: filterField.verticalCenter;
				anchors.right: filterField.right;
				anchors.rightMargin: Style.marginXS;

				width: height;
				height: 12;
				z: filterField.z + 1

				decorator: Component{IconButtonDecorator{}}

				iconSource: "../../../" + Style.getIconPath("Icons/Close", Icon.State.On, Icon.Mode.Normal);

				visible: filterField.text !== "";
				enabled: visible;

				onClicked: {
					popup.filterText = "";
					popup.clearSignal();
				}
			}
		}
	}
}
