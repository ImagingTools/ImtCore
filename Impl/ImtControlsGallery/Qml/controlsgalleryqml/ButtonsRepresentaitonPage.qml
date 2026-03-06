import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0


Rectangle {
	id: buttonsRepresentaitonPage;

	anchors.fill: parent;
	clip: true;

	color: Style.baseColor

	Component.onCompleted: {
		iconButton.forceActiveFocus();
		iconButton.focus = true;
	}

	// MenuButton{
	// 	id: menuButton;

	// 	anchors.top: parent.top;
	// 	anchors.topMargin: 70
	// 	anchors.horizontalCenter: parent.horizontalCenter
	// }

	Row{
		id: buttonsRow;

		anchors.verticalCenterOffset: 0;
		anchors.centerIn: parent;
		spacing: 50;


		Button{
			id: longButton;
			objectName: "LongButton"
			anchors.verticalCenter:  parent.verticalCenter;

			width: 300

			KeyNavigation.right: roundButton
			KeyNavigation.tab: roundButton

			text: "Text";
			tooltipText: "Hellow!!!";
			onClicked: {
				pauseMessage.show("Text button clicked!")
			}
		}
		Button{
			id: textButton;
			objectName: "TextButton"
			anchors.verticalCenter:  parent.verticalCenter;

			KeyNavigation.right: roundButton
			KeyNavigation.tab: roundButton

			text: "Text button";
			tooltipText: "Hellow!!!";
			onClicked: {
				pauseMessage.show("Text button clicked!")
			}
		}

		MenuButton{
			id: menuButton;
			objectName: "MenuButton"
			anchors.verticalCenter:  parent.verticalCenter;

			KeyNavigation.right: roundButton
			KeyNavigation.tab: roundButton

			text: "Button1"
			//tooltipText: "Hellow!!!";

			onAccepted: {
				pauseMessage.show(buttonId + " clicked!")
			}

			onClicked: {
				//pauseMessage.show("Menu button clicked!")
			}

			Component.onCompleted: {
				let index
				index = testModel.insertNewItem()
				testModel.setData("id", "button1", index)
				testModel.setData("name", "Button1", index)
				//testModel.setData("m_visible", false, index)


				index = testModel.insertNewItem()
				testModel.setData("id", "button2_", index)
				testModel.setData("name", "Button2_Button2", index)
				//testModel.setData("visible", false, index)

				index = testModel.insertNewItem()
				testModel.setData("id", "button3", index)
				testModel.setData("name", "Button3", index)

				menuModel = testModel

			}

			TreeItemModel{
				id: testModel;
			}

		}

		Button{
			id: roundButton;
			objectName: "RoundButton"
			anchors.verticalCenter:  parent.verticalCenter;

			KeyNavigation.right: iconButton
			KeyNavigation.tab: iconButton

			decorator: Style.roundButtonDecorator;

			iconSource: "../../../" + Style.theme + "/Icons" + "/Close.svg";
			onClicked: {
				pauseMessage.show("Round button clicked!")

			}
		}

		Button{
			id: iconButton;

			anchors.verticalCenter:  parent.verticalCenter;

			KeyNavigation.right: toggledButton
			KeyNavigation.tab: toggledButton

			iconSource: "../../../" + Style.theme + "/Icons" + "/Eye.svg";

			text: "Icon button";
			onClicked: {
				pauseMessage.show("Icon button clicked!")
			}
		}
		Button{
			id: iconMenuButton;

			anchors.verticalCenter:  parent.verticalCenter;

			KeyNavigation.right: toggledButton
			KeyNavigation.tab: toggledButton

			iconSource: "../../../" + Style.theme + "/Icons" + "/Eye.svg";

			isMenuButton: true


			text: "Icon button";
			onClicked: {
				pauseMessage.show("Icon button clicked!")
			}
		}

		Rectangle{
			height: 50
			width: 200
			anchors.verticalCenter:  parent.verticalCenter;
			color: Style.backgroundColor

			Button{
				id: toggledButton;
				anchors.centerIn: parent

				KeyNavigation.right: textButton
				KeyNavigation.tab: textButton

				checkable: true


				iconSource: "../../../" + Style.theme + "/Icons" + "/Eye.svg";
				text: "Toggled button";

				onClicked: {

				}

				onToggled: {
					if(checked){
						pauseMessage.show("toggled on!")
					}
					else {
						pauseMessage.show("toggled off!")
					}
				}
			}
		}

		SegmentedButton {

			height: Style.buttonHeightM
			isExclusive: true

			Button{
				anchors.verticalCenter:  parent.verticalCenter;

				KeyNavigation.right: roundButton
				KeyNavigation.tab: roundButton

				iconSource: "../../../../" + Style.getIconPath("Icons/Info", Icon.State.On, Icon.Mode.Normal);

				text: "Info";
				tooltipText: "Info!!!";
				onClicked: {
					pauseMessage.show("Info button clicked!")
				}
			}

			Button{
				anchors.verticalCenter:  parent.verticalCenter;

				KeyNavigation.right: roundButton
				KeyNavigation.tab: roundButton

				iconSource: "../../../../" + Style.getIconPath("Icons/Warning", Icon.State.On, Icon.Mode.Normal);

				text: "Warning";
				tooltipText: "Warning!!!";
				onClicked: {
					pauseMessage.show("Warning button clicked!")
				}
			}

			Button{
				anchors.verticalCenter:  parent.verticalCenter;

				KeyNavigation.right: roundButton
				KeyNavigation.tab: roundButton

				iconSource: "../../../../" + Style.getIconPath("Icons/Error", Icon.State.On, Icon.Mode.Normal);

				text: "Error";
				tooltipText: "Error!!!";
				onClicked: {
					pauseMessage.show("Error button clicked!")
				}
			}
		}


	}


	///////////////////

	PauseAnimation {
		id: pauseMessage;
		function show(text) {
			messageRec.text = text
			pauseMessage.restart()
		}

		duration: 1000;
		onStarted: {
			messageRec.visible = true;
		}
		onFinished: {
			messageRec.visible = false;
		}
	}

	Rectangle{
		id: darkBackground;

		anchors.fill: parent;

		color: "#000000";
		opacity: 0.3;
		visible: false;

	}

	Rectangle{
		id: messageRec;

		anchors.horizontalCenter: parent.horizontalCenter;
		anchors.top: parent.top
		anchors.topMargin: 50

		property alias text: messageText.text

		width: parent.width - 50;
		height: 40;
		radius:4;

		visible: false;

		Text {
			id: messageText;

			anchors.centerIn: parent;

			width: parent.width - 80;
			wrapMode: Text.WordWrap;
			horizontalAlignment: Text.AlignHCenter;
			font.family: Style.fontFamily;
			font.pixelSize: Style.fontSizeXXL;
			color: Style.errorTextColor;

			text: "Click!";
		}

	}
}
