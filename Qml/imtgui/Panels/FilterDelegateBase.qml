import QtQuick 2.12
import Acf 1.0
import imtcontrols 1.0
import imtcolgui 1.0

Item {
	id: filterDelegateBase

	width: rowDelegate.width
	height: Style.controlHeightM

	property string filterId
	property string name
	property string mainButtonText: name
	property bool isActive: false
	property bool readOnly: false
	property CollectionFilter collectionFilter: null
	property DocCollectionFilter documentFilter: null
	property FilterMenu filterMenu: null

	signal clearFilter()
	signal openFilter()
	
	Connections {
		target: filterDelegateBase.collectionFilter
		function onFilterChanged(){
			if (filterDelegateBase.collectionFilter){
				filterDelegateBase.collectionFilter.isEmpty()
			}
		}
	}
	
	Row {
		id: rowDelegate
		height: filterDelegateBase.height
		spacing: 1
		Button {
			id: mainButton
			icon.width: Style.fontSizeXS
			icon.height: Style.fontSizeXS
			icon.source: filterDelegateBase.isActive ? "qrc:/" + Style.getIconPath("Icons/Down", Icon.State.Off, Icon.Mode.Disabled) : "qrc:/" + Style.getIconPath("Icons/Down", Icon.State.On, Icon.Mode.Normal)
			text: filterDelegateBase.mainButtonText
			widthFromDecorator: true
			enabled: !filterDelegateBase.readOnly

			onClicked: {
				filterDelegateBase.openFilter()
			}
			
			decorator: Component{
				ButtonDecorator {
					id: buttonDecorator
					radius: Style.radiusM
					color: filterDelegateBase.isActive ? Style.buttonAccentColor : Style.baseColor
					textColor: filterDelegateBase.isActive ? Style.baseColor : Style.textColor
					border.width: filterDelegateBase.isActive ? 0 : 1
					opacity: mainButton.hovered ? 0.85 : 1

					Rectangle {
						id: helpRectangle
						anchors.right: buttonDecorator.right
						height: buttonDecorator.height
						width: buttonDecorator.radius
						color: buttonDecorator.color
						border.width: buttonDecorator.border.width
						border.color: buttonDecorator.border.color
						visible: filterDelegateBase.isActive
					}
					
					Rectangle {
						anchors.verticalCenter: helpRectangle.verticalCenter
						anchors.left: helpRectangle.left
						color: helpRectangle.color
						height: helpRectangle.height - 2 * helpRectangle.border.width
						width: helpRectangle.border.width
						visible: helpRectangle.visible
					}
				}
			}
		}

		Button {
			id: clearButton
			width: filterDelegateBase.height
			height: width
			visible: filterDelegateBase.isActive
			icon.source: filterDelegateBase.readOnly ?	"qrc:/" + Style.getIconPath("Icons/Lock", Icon.State.Off, Icon.Mode.Disabled) :
						(filterDelegateBase.isActive ?	"qrc:/" + Style.getIconPath("Icons/Close", Icon.State.Off, Icon.Mode.Disabled) :
														"qrc:/" + Style.getIconPath("Icons/Close", Icon.State.On, Icon.Mode.Normal))
			icon.width: Style.iconSizeS
			icon.height: Style.iconSizeS
			// widthFromDecorator: true
			enabled: !filterDelegateBase.readOnly
			onClicked: {
				filterDelegateBase.clearFilter()
			}

			decorator: Component{
				ButtonDecorator {
					id: buttonDecorator
					radius: Style.radiusM
					color: filterDelegateBase.isActive ? Style.buttonAccentColor : Style.baseColor
					border.width: filterDelegateBase.isActive ? 0 : 1
					opacity: clearButton.hovered ? 0.85 : 1
					Rectangle {
						id: helpRectangle
						anchors.left: buttonDecorator.left
						height: buttonDecorator.height
						width: buttonDecorator.radius
						color: buttonDecorator.color
						border.width: buttonDecorator.border.width
						border.color: buttonDecorator.border.color
						visible: filterDelegateBase.isActive
					}
					
					Rectangle {
						anchors.verticalCenter: helpRectangle.verticalCenter
						anchors.right: helpRectangle.right
						color: helpRectangle.color
						height: helpRectangle.height - 2 * helpRectangle.border.width
						width: helpRectangle.border.width
						visible: helpRectangle.visible
					}
				}
			}
		}
	}
}

