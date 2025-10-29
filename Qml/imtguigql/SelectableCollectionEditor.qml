import QtQuick 2.12
import Acf 1.0
import imtcontrols 1.0
import imtguigql 1.0
import imtgui 1.0
import imtcolgui 1.0
import imtbaseComplexCollectionFilterSdl 1.0
import imtbaseDocumentIdFilterSdl 1.0

ElementView {
	id: container

	bottomComp: stackViewComp
	controlWidth: container.width
	controlComp: Component {
		Rectangle {
			height: Style.itemSizeM
			width: container.controlWidth - Style.marginXXL
			color: Style.baseColor

			Connections {
				target: container.stackView
				function onCurrentIndexChanged(){
					if (container.stackView.currentIndex === 0){
						stackViewHeader.removeHeader("source")
					}

					if (container.stackView.currentIndex === 1){
						stackViewHeader.addHeader("source", container.sourceTitle)
					}
				}
			}
			
			Connections {
				target: container
				function onTargetTitleChanged(){
					stackViewHeader.setHeaderName("target", container.targetTitle)
				}
				function onSourceTitleChanged(){
					stackViewHeader.setHeaderName("source", container.sourceTitle)
				}
				function onSourceCollectionViewChanged(){
					sourceCollectionViewConnections.target = container.sourceCollectionView
				}
			}
			
			StackViewHeader {
				id: stackViewHeader
				anchors.left: parent.left
				anchors.right: buttonsRow.left
				height: Style.itemSizeM
				Component.onCompleted: {
					addHeader("target", container.targetTitle)
				}
				
				onCloseClicked: {
					container.stackView.setCurrentIndex(0)
					container.stackView.removePage(1)
					newButton2.enabled = false
				}
			}

			Row {
				id: buttonsRow
				anchors.right: parent.right
				height: Style.itemSizeM
				spacing: Style.spacingM

				Button {
					id: newButton
					text: qsTr("New")
					icon.source: enabled ?	"qrc:/" + Style.getIconPath("Icons/Add", Icon.State.On, Icon.Mode.Normal):
											"qrc:/" + Style.getIconPath("Icons/Add", Icon.State.Off, Icon.Mode.Disabled)
					visible: !container.stackView ? false : container.stackView.currentIndex === 0
					onClicked: {
						if (!container.stackView){
							return
						}
						
						ModalDialogManager.openDialog(remoteCollectionViewDialogComp)
						// container.stackView.addPage(remoteCollectionViewComp2)
						
						// container.stackView.setCurrentIndex(1)
					}
				}
	
				Button {
					id: newButton2
					text: qsTr("Add")
					icon.source: enabled ?	"qrc:/" + Style.getIconPath("Icons/Add", Icon.State.On, Icon.Mode.Normal):
											"qrc:/" + Style.getIconPath("Icons/Add", Icon.State.Off, Icon.Mode.Disabled)
					visible: !container.stackView ? false : container.stackView.currentIndex === 1
					enabled: false
					onClicked: {
						container.addElementsFromSourceCollection()
					}

					Connections {
						id: sourceCollectionViewConnections
						function onSelectionChanged(ids, indexes){
							newButton2.enabled = ids.length > 0
						}
					}
				}
	
				Button {
					id: removeButton
					text: qsTr("Remove")
					enabled: false
					icon.source: enabled ?	"qrc:/" + Style.getIconPath("Icons/Delete", Icon.State.On, Icon.Mode.Normal):
											"qrc:/" + Style.getIconPath("Icons/Delete", Icon.State.Off, Icon.Mode.Disabled)
					visible: !container.stackView ? false : container.stackView.currentIndex === 0
					onClicked: {
						if (!container.targetCollectionView){
							return
						}
	
						let selectedIds = container.targetCollectionView.getSelectedIds()
						if (selectedIds.length <= 0){
							return
						}
	
						for (let i = 0; i < selectedIds.length; ++i){
							let index = container.selectedIds.indexOf(selectedIds[i])
							if (index >= 0){
								container.selectedIds.splice(index, 1)
							}
						}
	
						container.targetCollectionView.table.resetSelection()
						container.selectionChanged()
					}
					
					Connections {
						target: container.targetCollectionView
						function onSelectionChanged(ids, indexes){
							removeButton.enabled = indexes.length > 0
						}
					}
				}
			}
		}
	}

	property string collectionId

	property string targetTitle: qsTr("Selected Elements")
	property string sourceTitle: qsTr("New Elements")

	property var selectedIds: []
	property var targetRequestedFields: []
	property RemoteCollectionView targetCollectionView: null
	property RemoteCollectionView sourceCollectionView: null
	property StackView stackView: null

	signal selectionChanged()

	onSelectedIdsChanged: {
		updateTargetCollection()
		updateSourceCollection()
	}

	onSelectionChanged: {
		updateTargetCollection()
		updateSourceCollection()
	}

	DocumentIdFilter {
		id: documentIdFilter1
		m_documentIds: container.selectedIds
		m_conditionType: "In"
	}

	DocumentIdFilter {
		id: documentIdFilter2
		m_documentIds: container.selectedIds
		m_conditionType: "NotIn"
	}

	function setSourceAdditionalFilters(sourceCollectionView){}

	function updateSourceCollection(){
		if (!sourceCollectionView){
			return
		}

		sourceCollectionView.collectionFilter.clearAllFilters()
		
		sourceCollectionView.registerFilter("DocumentIdFilter", documentIdFilter2)
		sourceCollectionView.filterMenu.setFilterIsEnabled("DocumentIdFilter", true)
		documentIdFilter1.m_documentIds = selectedIds
	
		setSourceAdditionalFilters(sourceCollectionView)

		sourceCollectionView.doUpdateGui()
	}

	function setTargetAdditionalFilters(collectionFilter){}

	function updateTargetCollection(){
		if (!targetCollectionView){
			return
		}

		targetCollectionView.collectionFilter.clearAllFilters()
		
		targetCollectionView.registerFilter("DocumentIdFilter", documentIdFilter1)
		targetCollectionView.filterMenu.setFilterIsEnabled("DocumentIdFilter", true)
		documentIdFilter1.m_documentIds = selectedIds

		setTargetAdditionalFilters(targetCollectionView.collectionFilter)

		targetCollectionView.doUpdateGui()
	}

	function addElementsFromSourceCollection(){
		if (!container || !container.sourceCollectionView){
			return
		}

		let selectedIds = container.sourceCollectionView.getSelectedIds()
		if (selectedIds.length <= 0){
			return
		}

		container.sourceCollectionView.table.resetSelection()
		container.selectedIds = container.selectedIds.concat(selectedIds)

		selectionChanged()
	}

	Component {
		id: remoteCollectionViewDialogComp
		RemoteCollectionViewDialog {
			id: remoteCollectionViewDialog
			title: container.sourceTitle
			collectionId: container.collectionId
			Component.onCompleted: {
				addButton(Enums.apply, qsTr("Add"), false)
				addButton(Enums.cancel, qsTr("Cancel"), true)
			}

			onCollectionViewChanged: {
				container.sourceCollectionView = collectionView
				container.updateSourceCollection()
			}
			
			onStarted: {
				container.updateSourceCollection()
			}

			onFinished: {
				if (buttonId === Enums.apply){
					container.addElementsFromSourceCollection()
				}
			}

			Connections {
				target: container && container.sourceCollectionView ? container.sourceCollectionView : undefined
				function onSelectionChanged(ids, indexes){
					remoteCollectionViewDialog.setButtonEnabled(Enums.apply,ids.length > 0)
				}
			}
		}
	}

	Component {
		id: remoteCollectionViewComp1
		RemoteCollectionView {
			id: remoteCollectionView
			width: container.contentWidth
			height: contentHeight
			collectionId: container.collectionId
			commandsControllerComp: null
			documentCollectionFilter: null
			canResetFilters: false
			filterMenuVisible: false
			loadingDataAfterHeadersReceived: false
			tableViewParamsStoredServer: false
			pagination.visible: false
			headerRightClickEnabled: false
			requestedFields: container.targetRequestedFields
			Component.onCompleted: {
				container.targetCollectionView = this
				container.updateTargetCollection()
			}
			Component.onDestruction: {
				container.targetCollectionView = null
			}

			onElementsCountChanged: {
				pagination.visible = elementsCount >= 25
			}
			
			onContentHeightChanged: {
				height = contentHeight
			}
		}
	}

	Component {
		id: remoteCollectionViewComp2
		RemoteCollectionView {
			id: remoteCollectionView2
			width: container.contentWidth
			collectionId: container.collectionId
			commandsControllerComp: null
			documentCollectionFilter: null
			canResetFilters: false
			filterMenuVisible: false
			tableViewParamsStoredServer: false
			headerRightClickEnabled: false
			Component.onCompleted: {
				container.sourceCollectionView = this
				container.updateSourceCollection()
				updateHeight()
			}
			
			Component.onDestruction: {
				container.sourceCollectionView = null
			}
			
			onContentHeightChanged: {
				height = contentHeight
			}
		}
	}

	Component {
		id: stackViewComp

		StackView {
			id: stackView
			adaptSizeToCurrentPage: true

			Component.onCompleted: {
				container.stackView = this
				addPage(remoteCollectionViewComp1)
				setCurrentIndex(0)
			}
		}
	}
}
