import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0

Rectangle {
	id: deleg
	width:!list ? 0 : list.width - Style.marginM
	height: 40
	opacity: !reorderManager ? 0 : reorderManager.selectedId == "" ? 1 : roleIdValue == reorderManager.selectedId ? 1 : 0.7

	color: "transparent"

	property var roleIdValue: model[deleg.idRole] !== undefined ? model[deleg.idRole] : model.item[deleg.sdlVarPrefix + deleg.idRole]

	property Item list
	property var reorderManager
	property Item dragTarget: content
	property alias dragArea: da
	property alias contentContainer: content

	property Component contentComp: null

	property bool isEntered: false

	property bool hasButtons: true;

	property string defaultBorderColor: Style.borderColor
	property string selectionBorderColor: "orange"

	property string idRole: "id"
	property string sdlVarPrefix: "m_"

	signal isEnteredUpdate()

	signal pressed()
	signal released()


	Connections{
		target: deleg

		function onIsEnteredUpdate(){
			deleg.isEntered = deleg.reorderManager.enteredInfo[model.index]
		}
	}

	MouseArea {
		id: da

		height: parent.height
		width: 40

		property bool held: false

		drag.target: !held ? undefined : deleg.dragTarget ? deleg.dragTarget : content
		drag.axis: Drag.YAxis

		onPressed:{
			deleg.pressed()
			deleg.reorderManager.selectedId = model[deleg.idRole] !== undefined ? model[deleg.idRole] : model.item[deleg.sdlVarPrefix + deleg.idRole]
			held = true
			deleg.dragTarget.parent.z = 100
			deleg.reorderManager.fromIndex = model.index
			deleg.reorderManager.toIndex = -1
			deleg.dragTarget.anchors.verticalCenter = undefined
		}

		onReleased: {
			deleg.released()
			deleg.dragTarget.parent.z = 0
			let hasToIndex = deleg.reorderManager.toIndex < 0 ? false : deleg.reorderManager.enteredInfo[deleg.reorderManager.toIndex]
			held = false
			if(hasToIndex){
				deleg.reorderManager.moveItem(deleg.reorderManager.fromIndex, deleg.reorderManager.toIndex)
			}
			deleg.dragTarget.anchors.verticalCenter = deleg.dragTarget.parent.verticalCenter

			deleg.reorderManager.selectedId = ""
			deleg.reorderManager.clearSelection()
			deleg.isEnteredUpdate()
		}
	}

	Rectangle{
		id: content

		width: parent.width
		height: parent.height
		radius: parent.radius

		color: "transparent"

		Drag.active: deleg.dragTarget == content ? da.held : false
		Drag.source: da
		Drag.hotSpot.x: width / 2
		Drag.hotSpot.y: height / 2

		Loader{
			anchors.fill: parent
			sourceComponent: deleg.contentComp
		}

		Rectangle{
			id: containerDecorator

			width: parent.width
			height: parent.height
			radius: parent.radius

			color: "transparent"
			border.color: deleg.reorderManager.selectedId == deleg.roleIdValue ? deleg.defaultBorderColor : deleg.isEntered ? deleg.selectionBorderColor : deleg.defaultBorderColor
		}

		Item {
			id: controlsContainer

			anchors.left: parent.left
			anchors.right: parent.right
			anchors.top: parent.top
			anchors.bottom: parent.bottom

			anchors.leftMargin: Style.marginM
			anchors.rightMargin: Style.marginM
			anchors.topMargin: Style.marginS/2
			anchors.bottomMargin: Style.marginS/2

			Item{
				id: dragItem
				anchors.verticalCenter: parent.verticalCenter

				width: 20
				height: parent.height

				Column{
					id: dragIndicator
					anchors.verticalCenter: parent.verticalCenter

					spacing: 2

					width: 20

					Rectangle{
						width: parent.width
						height: 2
						radius: height
						color: Style.borderColor2
					}

					Rectangle{
						width: parent.width
						height: 2
						radius: height
						color: Style.borderColor2
					}
				}
			}

			Column {
				id: buttonsColumn
				anchors.verticalCenter: parent.verticalCenter
				anchors.right: parent.right
				spacing: 6
				visible: deleg.hasButtons

				ToolButton {
					width: 10
					height: 10
					iconSource: "../../../../" + Style.getIconPath(
									"Icons/Up",
									Icon.State.On,
									model.index > 0 ? Icon.Mode.Normal : Icon.Mode.Disabled
									)

					visible: deleg.hasButtons
					onClicked: {
						deleg.reorderManager.moveItemUp(model.index)
					}
				}

				ToolButton {
					width: 10
					height: 10
					iconSource: "../../../../" + Style.getIconPath(
									"Icons/Down",
									Icon.State.On,
									isNotLast ? Icon.Mode.Normal : Icon.Mode.Disabled
									)

					visible: deleg.hasButtons
					property bool  isNotLast: !deleg.list ? false: model.index < (deleg.list.count -1)
					onClicked: {
						deleg.reorderManager.moveItemDown(model.index)
					}
				}
			}
		}
	}

	DropArea {
		width: parent.width
		height: parent.height

		onEntered:  {
			deleg.reorderManager.enteredInfo[model.index] = true
			deleg.reorderManager.toIndex = model.index
			deleg.isEnteredUpdate()
		}
		onExited: {
			deleg.reorderManager.enteredInfo[model.index] = false
			deleg.isEnteredUpdate()
		}
	}
}
