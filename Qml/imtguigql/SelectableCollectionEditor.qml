import QtQuick 2.12
import Acf 1.0
import imtcontrols 1.0
import imtguigql 1.0
import imtgui 1.0
import imtcolgui 1.0
import imtbaseComplexCollectionFilterSdl 1.0
import imtbaseDocumentIdFilterSdl 1.0

ElementView {
	id: root

	bottomComp: stackViewComp
	controlWidth: root.width
	controlComp: Component {
		Rectangle {
			height: Style.itemSizeM
			width: root.controlWidth - Style.marginXXL
			color: Style.baseColor

			Connections {
				target: root.stackView
				function onCurrentIndexChanged(){
					if (root.stackView.currentIndex === 0){
						stackViewHeader.removeHeader("source")
					}

					if (root.stackView.currentIndex === 1){
						stackViewHeader.addHeader("source", root.sourceTitle)
					}
				}
			}
			
			Connections {
				target: root
				function onTargetTitleChanged(){
					stackViewHeader.setHeaderName("target", root.targetTitle)
				}
				function onSourceTitleChanged(){
					stackViewHeader.setHeaderName("source", root.sourceTitle)
				}
				function onSourceCollectionViewChanged(){
					sourceCollectionViewConnections.target = root.sourceCollectionView
				}
			}
			
			StackViewHeader {
				id: stackViewHeader
				anchors.left: parent.left
				anchors.right: buttonsRow.left
				height: Style.itemSizeM
				Component.onCompleted: {
					addHeader("target", root.targetTitle)
				}
				
				onCloseClicked: {
					root.stackView.setCurrentIndex(0)
					root.stackView.removePage(1)
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
					visible: !root.stackView ? false : root.stackView.currentIndex === 0
					onClicked: {
						if (!root.stackView){
							return
						}
						
						ModalDialogManager.openDialog(remoteCollectionViewDialogComp)
						// root.stackView.addPage(remoteCollectionViewComp2)
						
						// root.stackView.setCurrentIndex(1)
					}
				}
	
				Button {
					id: newButton2
					text: qsTr("Add")
					icon.source: enabled ?	"qrc:/" + Style.getIconPath("Icons/Add", Icon.State.On, Icon.Mode.Normal):
											"qrc:/" + Style.getIconPath("Icons/Add", Icon.State.Off, Icon.Mode.Disabled)
					visible: !root.stackView ? false : root.stackView.currentIndex === 1
					enabled: false
					onClicked: {
						root.addElementsFromSourceCollection()
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
					visible: !root.stackView ? false : root.stackView.currentIndex === 0
					onClicked: {
						if (!root.targetCollectionView){
							return
						}
	
						let selectedIds = root.targetCollectionView.getSelectedIds()
						if (selectedIds.length <= 0){
							return
						}
	
						for (let i = 0; i < selectedIds.length; ++i){
							let index = root.selectedIds.indexOf(selectedIds[i])
							if (index >= 0){
								root.selectedIds.splice(index, 1)
							}
						}
	
						root.targetCollectionView.table.resetSelection()
						root.selectionChanged()
					}
					
					Connections {
						target: root.targetCollectionView
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
		m_documentIds: root.selectedIds
		m_conditionType: "In"
	}

	DocumentIdFilter {
		id: documentIdFilter2
		m_documentIds: root.selectedIds
		m_conditionType: "NotIn"
	}

	function setSourceAdditionalFilters(collectionFilter){}

	function updateSourceCollection(){
		if (!sourceCollectionView){
			return
		}

		sourceCollectionView.collectionFilter.clearAllFilters()
		
		sourceCollectionView.registerFilter("DocumentIdFilter", documentIdFilter2)
		sourceCollectionView.filterMenu.setFilterIsEnabled("DocumentIdFilter", true)
		documentIdFilter1.m_documentIds = selectedIds
	
		setSourceAdditionalFilters(sourceCollectionView.collectionFilter)

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
		if (!root.sourceCollectionView){
			return
		}

		let selectedIds = root.sourceCollectionView.getSelectedIds()
		if (selectedIds.length <= 0){
			return
		}

		root.sourceCollectionView.table.resetSelection()
		root.selectedIds = root.selectedIds.concat(selectedIds)

		root.selectionChanged()
	}

	Component {
		id: remoteCollectionViewDialogComp
		RemoteCollectionViewDialog {
			id: remoteCollectionViewDialog
			title: root.sourceTitle
			collectionId: root.collectionId
			Component.onCompleted: {
				addButton(Enums.apply, qsTr("Add"), false)
				addButton(Enums.cancel, qsTr("Cancel"), true)
			}

			onCollectionViewChanged: {
				root.sourceCollectionView = collectionView
				root.updateSourceCollection()
			}
			
			onStarted: {
				root.updateSourceCollection()
			}

			onFinished: {
				if (buttonId === Enums.apply){
					root.addElementsFromSourceCollection()
				}
			}

			Connections {
				target: root.sourceCollectionView
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
			width: root.contentWidth
			height: contentHeight
			collectionId: root.collectionId
			commandsControllerComp: null
			documentCollectionFilter: null
			canResetFilters: false
			filterMenuVisible: false
			loadingDataAfterHeadersReceived: false
			tableViewParamsStoredServer: false
			pagination.visible: false
			headerRightClickEnabled: false
			requestedFields: root.targetRequestedFields
			Component.onCompleted: {
				root.targetCollectionView = this
				root.updateTargetCollection()
			}
			Component.onDestruction: {
				root.targetCollectionView = null
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
			width: root.contentWidth
			collectionId: root.collectionId
			commandsControllerComp: null
			documentCollectionFilter: null
			canResetFilters: false
			filterMenuVisible: false
			tableViewParamsStoredServer: false
			headerRightClickEnabled: false
			Component.onCompleted: {
				root.sourceCollectionView = this
				root.updateSourceCollection()
				updateHeight()
			}
			
			Component.onDestruction: {
				root.sourceCollectionView = null
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
				root.stackView = this
				addPage(remoteCollectionViewComp1)
				setCurrentIndex(0)
			}
		}
	}
}
