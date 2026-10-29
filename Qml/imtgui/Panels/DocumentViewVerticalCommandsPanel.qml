import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0
import imtgui 1.0
import imtdocgui 1.0

Item{
	id: container
	anchors.fill: parent;
	parent: documentView.leftPanelAlias

	property Item documentView
	property BaseModel buttonPanelModel: null

	Connections{
		target: container.documentView

		function onCommandsModelChanged(commandsModel){
			console.log("COMMANDS_MODEL!!!!!!")
			container.buttonPanelModel = null

			if (!commandsModel){
				return
			}
			container.buttonPanelModel = container.documentView.commandsController.getAdditionalCommandsGroup()
		}
	}

	Rectangle{
		anchors.top: parent.top
		anchors.bottom: parent.bottom
		anchors.right: parent.right

		width: 1

		color: Style.borderColor;
		opacity: 0.5

	}

	ListView{
		id: buttonsList

		anchors.horizontalCenter: parent.horizontalCenter
		anchors.verticalCenter: parent.verticalCenter

		width: parent.width - Style.marginM
		height: Math.min(parent.height - 2*Style.marginM, contentHeight)

		boundsBehavior: Flickable.StopAtBounds

		model: container.buttonPanelModel

		delegate: Item{
			anchors.horizontalCenter: parent.horizontalCenter
			width: buttonsList.width
			height: visible ? width: 0

			visible: model.item.m_visible

			Item {
				anchors.horizontalCenter: parent.horizontalCenter
				width: parent.width - Style.marginM
				height: width

				Image {
					id: imageName

					width: parent.width
					height: parent.height
					sourceSize.width: width
					sourceSize.height: height
					source: "../../../../" + Style.getIconPath(model.item.m_elementItemPath, Icon.State.On, Icon.Mode.Normal);
				}

				MouseArea{
					id: ma
					anchors.fill: parent

					hoverEnabled: true
					cursorShape: Qt.PointingHandCursor
					onClicked: {
						let params = ({})
						params.target = this
						params.x = mouse.x
						params.y = mouse.y
						container.documentView.commandActivated(model.item.m_elementId)
						container.documentView.commandActivatedWithParams(model.item.m_elementId, params)
					}
				}
				TooltipArea {
					anchors.fill: parent;

					mouseArea: ma;
					text: model.item.m_elementName
				}
			}
		}
	}
}




