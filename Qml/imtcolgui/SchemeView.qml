import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0

import imtcontrols 1.0
import imtgui 1.0


Rectangle {
	id: canvasPage;
	
	clip: true;
	
	color: Style.imagingToolsGradient0;
	
	property BaseModel objectsModel: BaseModel {}

	// Additional links between the shown objects, besides the m_links of the objects: [{fromId, toId}].
	// E.g. links of composite objects derived from the links of their content.
	property var extraLinks: []
	onExtraLinksChanged: canvas.requestPaint()

	// Objects with a link icon in the top right corner: {objectId: tooltip text}. E.g. links to
	// objects that are not shown on the scheme. The icon is highlighted and shows the text on hover.
	property var linkHints: ({})
	property string linkHintIcon: "Icons/Link"
	onLinkHintsChanged: {
		canvas.hoverLinkHintIndex = -1
		linkHintTooltip.hide()
		canvas.requestPaint()
	}

	function hasLinkHint(item){
		return item && canvasPage.linkHints && !!canvasPage.linkHints[item.m_id]
	}
	
	// property TreeItemModel objectsModel: TreeItemModel{};
	// Current object (the last selected one); setting it from outside selects this object only.
	property int selectedIndex: -1;

	// IDs of all selected objects; the current object (selectedIndex) is one of them. A click selects
	// one object, Ctrl+click adds or removes an object, a drag with Ctrl selects the objects touched
	// by the selection rectangle (Escape cancels it). Use setSelection() to change it.
	property var selectedIds: []
	// Emitted once after selectedIds or selectedIndex were changed.
	signal selectionChanged()

	// Clicked link {fromId, toId} (the same direction as in extraLinks), null if none. Selecting an
	// object or clicking an empty place clears it.
	property var selectedLink: null
	signal linkClicked(string fromId, string toId)

	function isObjectSelected(index){
		if (index < 0 || index >= canvasPage.objectsModel.count){
			return false
		}
		let item = canvasPage.objectsModel.get(index).item
		return item ? canvasPage.selectedIds.indexOf(item.m_id) >= 0 : false
	}

	// Indexes of the selected objects in the model.
	function selectedObjectIndexes(){
		let result = []
		for (let i = 0; i < canvasPage.objectsModel.count; i++){
			if (canvasPage.isObjectSelected(i)){
				result.push(i)
			}
		}
		return result
	}

	// ids: IDs of the selected objects; currentIndex: the current object (added to ids if missing), -1 for none.
	function setSelection(ids, currentIndex){
		let newIds = []
		for (let i = 0; ids && i < ids.length; i++){
			if (newIds.indexOf(ids[i]) < 0 && canvasPage.findModelIndex(ids[i]) >= 0){
				newIds.push(ids[i])
			}
		}
		let current = currentIndex >= 0 && currentIndex < canvasPage.objectsModel.count ? currentIndex : -1
		if (current >= 0){
			let currentId = canvasPage.objectsModel.get(current).item.m_id
			if (newIds.indexOf(currentId) < 0){
				newIds.push(currentId)
			}
		}
		else if (newIds.length > 0){
			current = canvasPage.findModelIndex(newIds[newIds.length - 1])
		}

		let changed = current !== canvas.selectedIndex || newIds.length !== canvasPage.selectedIds.length
		for (let i = 0; !changed && i < newIds.length; i++){
			changed = canvasPage.selectedIds.indexOf(newIds[i]) < 0
		}

		if (newIds.length > 0){
			canvasPage.setSelectedLink(null)
		}
		canvasPage.selectedIds = newIds
		canvas.selectedIndex = current
		if (changed){
			canvasPage.selectionChanged()
		}
		canvas.requestPaint()
	}

	// Ctrl+click: adds the object to the selection or removes it.
	function toggleObjectSelection(index){
		if (index < 0 || index >= canvasPage.objectsModel.count){
			return
		}
		let objectId = canvasPage.objectsModel.get(index).item.m_id
		let ids = canvasPage.selectedIds.slice()
		let position = ids.indexOf(objectId)
		if (position < 0){
			ids.push(objectId)
			canvasPage.setSelection(ids, index)
			return
		}

		ids.splice(position, 1)
		let current = canvas.selectedIndex
		if (current === index){
			current = ids.length > 0 ? canvasPage.findModelIndex(ids[ids.length - 1]) : -1
		}
		canvasPage.setSelection(ids, current)
	}

	function setSelectedLink(link){
		let fromId = link ? link.fromId : ""
		let toId = link ? link.toId : ""
		let currentFromId = canvasPage.selectedLink ? canvasPage.selectedLink.fromId : ""
		let currentToId = canvasPage.selectedLink ? canvasPage.selectedLink.toId : ""
		if (fromId === currentFromId && toId === currentToId){
			return
		}
		canvasPage.selectedLink = link ? {"fromId": fromId, "toId": toId} : null
		canvas.requestPaint()
	}

	function isLinkSelected(fromId, toId){
		let link = canvasPage.selectedLink
		return link !== null && link !== undefined
				&& ((link.fromId === fromId && link.toId === toId) || (link.fromId === toId && link.toId === fromId))
	}

	//for scrollBars
	property real contentHeight: backgroundRec.height;
	property real contentY: -canvas.deltaY;
	property real originY: 0;
	
	property real contentWidth: backgroundRec.width;
	property real contentX: -canvas.deltaX;
	property real originX: 0;
	
	property real scaleStep: 0.1;
	property real maximumObjectWidth: 0;
	property int objectFontSize: Style.fontSizeXXL
	property int objectSecondaryFontSize: Style.fontSizeM
	property bool strokeSecondaryText: true
	
	property bool autoFit: true;
	
	signal copySignal(int index);
	signal pasteSignal(int index);
	signal deleteSignal(int index);
	signal renameSignal(int index);
	signal revertSignal();
	signal modelDataChanged();
	signal objectMoveFinished();
	// Right click on an object (it is selected first); menuX/menuY are window coordinates.
	signal contextMenuRequested(int index, real menuX, real menuY);

	// Dropping a dragged object onto another object (e.g. into a group). Disabled while dropTargetFilter
	// is null. When the dragged object overlaps an accepted target, the target is enlarged, highlighted
	// and blinks after dropArmDelay; releasing the mouse then emits objectDropped (instead of
	// objectMoveFinished). Escape or leaving the target cancels; a release before the target blinks
	// is an ordinary move.
	// function(objectId, targetId): true if the object can be dropped onto the target.
	property var dropTargetFilter: null
	property int dropArmDelay: 600
	// objectX/objectY: position of the dropped object at the release (scheme coordinates).
	signal objectDropped(string objectId, string targetId, real objectX, real objectY);

	// All selected objects are dragged together; the target is found under the current one and must
	// accept every dragged object.
	function findDropCandidate(){
		if (!canvasPage.dropTargetFilter || canvas.selectedIndex < 0 || canvas.selectedIndex >= canvasPage.objectsModel.count){
			return -1
		}

		let dragged = canvasPage.objectsModel.get(canvas.selectedIndex).item
		let draggedWidth = dragged.m_width ? dragged.m_width : canvas.mainRec_width
		let draggedIds = canvasPage.selectedIds.length > 0 ? canvasPage.selectedIds : [dragged.m_id]
		let acceptsAll = function(targetId){
			for (let k = 0; k < draggedIds.length; k++){
				if (!canvasPage.dropTargetFilter(draggedIds[k], targetId)){
					return false
				}
			}
			return true
		}
		let bestIndex = -1
		let bestArea = 0
		for (let i = 0; i < canvasPage.objectsModel.count; i++){
			if (i === canvas.selectedIndex || canvasPage.isObjectSelected(i)){
				continue
			}
			let item = canvasPage.objectsModel.get(i).item
			let width_ = item.m_width ? item.m_width : canvas.mainRec_width
			let overlapX = Math.min(dragged.m_x + draggedWidth, item.m_x + width_) - Math.max(dragged.m_x, item.m_x)
			let overlapY = Math.min(dragged.m_y + canvas.mainRec_height, item.m_y + canvas.mainRec_height) - Math.max(dragged.m_y, item.m_y)
			if (overlapX <= 0 || overlapY <= 0){
				continue
			}
			let area = overlapX * overlapY
			if (area > bestArea && acceptsAll(item.m_id)){
				bestArea = area
				bestIndex = i
			}
		}
		return bestIndex
	}

	function updateDropTarget(){
		let index = canvasPage.findDropCandidate()
		// A target cancelled with Escape is ignored until the dragged object leaves it.
		if (canvas.dropSuppressedIndex >= 0){
			if (canvas.dropSuppressedIndex >= canvasPage.objectsModel.count
					|| canvas.selectedIndex < 0 || canvas.selectedIndex >= canvasPage.objectsModel.count){
				canvas.dropSuppressedIndex = -1
			}
			else{
				let dragged = canvasPage.objectsModel.get(canvas.selectedIndex).item
				let target = canvasPage.objectsModel.get(canvas.dropSuppressedIndex).item
				let draggedWidth = dragged.m_width ? dragged.m_width : canvas.mainRec_width
				let targetWidth = target.m_width ? target.m_width : canvas.mainRec_width
				if (dragged.m_x >= target.m_x + targetWidth || target.m_x >= dragged.m_x + draggedWidth
						|| dragged.m_y >= target.m_y + canvas.mainRec_height || target.m_y >= dragged.m_y + canvas.mainRec_height){
					canvas.dropSuppressedIndex = -1
				}
			}
		}
		if (index === canvas.dropSuppressedIndex){
			index = -1
		}

		if (index === canvas.dropCandidateIndex){
			return
		}

		canvasPage.resetDrop()
		canvas.dropCandidateIndex = index
		if (index >= 0){
			dropArmTimer.restart()
		}
	}

	function resetDrop(){
		dropArmTimer.stop()
		dropBlinkTimer.stop()
		dropScaleAnimation.stop()
		let wasActive = canvas.dropTargetIndex >= 0
		canvas.dropCandidateIndex = -1
		canvas.dropTargetIndex = -1
		canvas.dropScale = 1.0
		canvas.dropBlinkOn = false
		if (wasActive){
			canvas.requestPaint()
		}
	}

	Timer {
		id: dropArmTimer

		interval: canvasPage.dropArmDelay
		repeat: false
		onTriggered: {
			if (canvas.dropCandidateIndex < 0){
				return
			}
			canvas.dropTargetIndex = canvas.dropCandidateIndex
			canvas.dropBlinkOn = true
			dropScaleAnimation.restart()
			dropBlinkTimer.restart()
			canvas.requestPaint()
		}
	}

	Timer {
		id: dropBlinkTimer

		interval: 250
		repeat: true
		onTriggered: {
			canvas.dropBlinkOn = !canvas.dropBlinkOn
			canvas.requestPaint()
		}
	}

	NumberAnimation {
		id: dropScaleAnimation

		target: canvas
		property: "dropScale"
		from: 1.0
		to: 1.1
		duration: 150
	}

	Shortcut {
		sequence: "Escape"
		enabled: canvasPage.visible && canvas.dropCandidateIndex >= 0
		onActivated: {
			canvas.dropSuppressedIndex = canvas.dropCandidateIndex
			canvasPage.resetDrop()
		}
	}

	// Escape hides the selection rectangle and restores the selection it was started with.
	Shortcut {
		sequence: "Escape"
		enabled: canvasPage.visible && canvas.rubberBandActive
		onActivated: {
			canvasPage.cancelRubberBand()
		}
	}

	function cancelRubberBand(){
		if (!canvas.rubberBandActive){
			return
		}
		canvas.rubberBandActive = false
		canvas.rubberBandCanceled = true
		canvasPage.setSelection(controlArea.rubberBandBaseIds, controlArea.rubberBandBaseIndex)
	}

	// Selects the objects touched by the rectangle (window corners in canvas coordinates) in addition
	// to the selection the rectangle was started with.
	function updateRubberBand(fromPoint, toPoint){
		canvas.rubberBandX = Math.min(fromPoint.x, toPoint.x)
		canvas.rubberBandY = Math.min(fromPoint.y, toPoint.y)
		canvas.rubberBandWidth = Math.abs(toPoint.x - fromPoint.x)
		canvas.rubberBandHeight = Math.abs(toPoint.y - fromPoint.y)
		if (canvas.scaleCoeff < 0.0000001){
			return
		}

		let left = (canvas.rubberBandX - canvas.deltaX) / canvas.scaleCoeff
		let top = (canvas.rubberBandY - canvas.deltaY) / canvas.scaleCoeff
		let right = left + canvas.rubberBandWidth / canvas.scaleCoeff
		let bottom = top + canvas.rubberBandHeight / canvas.scaleCoeff

		let ids = controlArea.rubberBandBaseIds.slice()
		let current = controlArea.rubberBandBaseIndex
		let lastHitIndex = -1
		for (let i = 0; i < canvasPage.objectsModel.count; i++){
			let item = canvasPage.objectsModel.get(i).item
			let width_ = item.m_width ? item.m_width : canvas.mainRec_width
			if (item.m_x > right || item.m_x + width_ < left || item.m_y > bottom || item.m_y + canvas.mainRec_height < top){
				continue
			}
			if (ids.indexOf(item.m_id) < 0){
				ids.push(item.m_id)
			}
			lastHitIndex = i
		}
		if (current < 0){
			current = lastHitIndex
		}
		canvasPage.setSelection(ids, current)
	}

	ApplicationEvents {
		onDesignSchemeChanged: {
			canvasPage.designSchemeChanged(scheme)
		}
		
		onAppWidthChanged: {
			canvasPage.appSizeChanged(width)
		}
		
		onAppHeightChanged: {
			canvasPage.appSizeChanged(height)
		}
	}
	
	onContentXChanged: {
		canvas.deltaX = -contentX
	}
	
	onContentYChanged: {
		canvas.deltaY = -canvasPage.contentY
	}
	
	onObjectsModelChanged: {
		controlArea.setLinkHintHover(-1);
		canvasPage.resetDrop();
		canvas.dropSuppressedIndex = -1;
		canvas.rubberBandActive = false;
		// The selection keeps the objects of the new model only.
		if (canvasPage.selectedIds.length > 0){
			canvasPage.setSelection(canvasPage.selectedIds, -1);
		}
		if (canvasPage.selectedLink !== null && (canvasPage.findModelIndex(canvasPage.selectedLink.fromId) < 0
				|| canvasPage.findModelIndex(canvasPage.selectedLink.toId) < 0)){
			canvasPage.setSelectedLink(null);
		}
		canvas.requestPaint();
	}
	
	onAutoFitChanged: {
		if (autoFit){
			appSizeChanged();
		}
	}
	
	onSelectedIndexChanged: {
		// Selection set from outside (e.g. after the model was replaced) must reach the canvas too;
		// only this object is selected then.
		if (canvas.selectedIndex !== selectedIndex){
			canvas.selectedIndex = selectedIndex
			let item = selectedIndex >= 0 && selectedIndex < canvasPage.objectsModel.count
					? canvasPage.objectsModel.get(selectedIndex).item : null
			canvasPage.selectedIds = item ? [item.m_id] : []
			if (item){
				canvasPage.setSelectedLink(null)
			}
			canvasPage.selectionChanged()
			canvas.requestPaint()
		}
	}

	function designSchemeChanged(scheme){
		canvas.requestPaint();
	}
	
	function appSizeChanged(params){
		if (autoFit){
			zoomToFit();
			
			requestPaint()
		}
	}
	
	function setAutoFit(autoFit){
		canvasPage.autoFit = autoFit;
	}
	
	function zoomIn(){
		if (canvas.scaleCoeff < 3){
			let scaleCoeff_ = canvas.scaleCoeff;
			scaleCoeff_ += scaleStep;
			canvas.setScale(scaleCoeff_, canvas.width / 2, canvas.height / 2)
		}
	}
	
	function zoomOut(){
		if(canvas.scaleCoeff > scaleStep * 2){
			let scaleCoeff_ = canvas.scaleCoeff;
			scaleCoeff_ -= scaleStep;
			canvas.setScale(scaleCoeff_, canvas.width / 2, canvas.height / 2)
		}
	}
	
	function resetZoom(){
		canvas.scaleCoeff = 1.0;
		canvas.scaleCoeffPrev = 1.0;
		
		requestPaint()
	}
	
	function fitToCanvas(){
		if (canvas.backgroundWidth == 0 | canvas.backgroundHeight == 0){
			return
		}

		let scaleCoeff_ = (canvas.width - Style.marginM * 2) / canvas.backgroundWidth;
		let scaleCoeff_h = (canvas.height - Style.marginM * 2) / canvas.backgroundHeight;
		if (scaleCoeff_ > scaleCoeff_h){
			scaleCoeff_ = scaleCoeff_h
		}
		canvas.setScale(scaleCoeff_, canvas.width / 2, canvas.height / 2)

		canvas.deltaX = canvasPage.width / 2 - backgroundRec.width / 2
		canvas.deltaY = canvasPage.height / 2 - backgroundRec.height / 2
	}


	// Fits the objects of the scheme, or the whole sheet when there are none
	function zoomToFit(){
		let bounds = canvasPage.getContentBounds()
		if (bounds.width <= 0 || bounds.height <= 0 || canvas.width <= 0 || canvas.height <= 0){
			return
		}

		let margin = Style.marginXL * 2
		let scaleCoeff_ = Math.min((canvas.width - margin * 2) / bounds.width, (canvas.height - margin * 2) / bounds.height)
		// A few objects are not blown up
		scaleCoeff_ = Math.min(scaleCoeff_, 1.2)
		if (scaleCoeff_ <= 0){
			return
		}

		canvas.scaleCoeff = scaleCoeff_
		canvas.scaleCoeffPrev = scaleCoeff_
		canvas.deltaX = canvas.width / 2 - (bounds.x + bounds.width / 2) * scaleCoeff_
		canvas.deltaY = canvas.height / 2 - (bounds.y + bounds.height / 2) * scaleCoeff_
	}

	function getContentBounds(){
		if (canvasPage.objectsModel.count === 0){
			return {"x": 0, "y": 0, "width": canvas.backgroundWidth, "height": canvas.backgroundHeight}
		}

		let left = Number.MAX_VALUE
		let top = Number.MAX_VALUE
		let right = -Number.MAX_VALUE
		let bottom = -Number.MAX_VALUE
		for (let i = 0; i < canvasPage.objectsModel.count; i++){
			let item = canvasPage.objectsModel.get(i).item
			let width_ = item.m_width ? item.m_width : canvas.mainRec_width
			left = Math.min(left, item.m_x)
			top = Math.min(top, item.m_y)
			right = Math.max(right, item.m_x + width_ + canvas.shadowSize)
			bottom = Math.max(bottom, item.m_y + canvas.mainRec_height + canvas.shadowSize)
		}

		return {"x": left, "y": top, "width": right - left, "height": bottom - top}
	}
	
	function requestPaint(){
		canvas.requestPaint();
	}
	
	function findModelIndex(id){
		let ind = -1;
		if(!id) return ind
		for(let i = 0; i < objectsModel.count; i++){
			if(objectsModel.get(i).item.m_id == id){
				ind = i;
				break;
			}
		}
		return ind;
	}
	
	function goInside(){
		if(objectsModel.count > canvas.selectedIndex && canvas.selectedIndex >= 0){
			// let isComposite = objectsModel.isValidData("isComposite", canvas.selectedIndex) ? objectsModel.getData("isComposite", canvas.selectedIndex) : false;
			// if(!isComposite){
			//     return;
			// }
			// let id = objectsModel.getData("id", canvas.selectedIndex);
		}
	}
	
	function correctPosition(addX, addY){
		for(let i = 0; i < canvasPage.objectsModel.count; i++){
			let item = canvasPage.objectsModel.get(i).item;
			
			item.m_x = item.m_x + addX;
			item.m_y = item.m_y + addY;
			
			// canvasPage.objectsModel.set(i, item);
		}
	}
	
	function correctPositionScaled(addX, addY, scale_){
		for(let i = 0; i < objectsModel.count; i++){
			let item = objectsModel.get(i).item;
			
			let newWidth = scale_ * item.m_width;
			item.m_width = newWidth;
			
			// canvasPage.objectsModel.set(i, item);
		}
	}
	
	
	TreeItemModel {
		id: bufferModel;
	}
	
	TreeItemModel {
		id: emptyModel;
	}
	
	Item{
		id: mainContainer;
		
		anchors.fill: parent
		
		ControlArea {
			id: controlArea;
			
			anchors.fill: parent;
			cursorShape: Qt.OpenHandCursor;
			acceptedButtons: Qt.LeftButton | Qt.RightButton;
			
			// The right button only selects an object, it neither moves objects nor pans the scheme.
			property bool rightButtonPressed: false;
			// What the pressed left button does: "move" (the selected objects), "pan" (the scheme),
			// "select" (Ctrl: toggles the object by a click, selects by a rectangle when dragged), "none".
			property string pressMode: "none";
			// Object under the cursor at the press, -1 for none.
			property int pressedIndex: -1;
			// Link under the cursor at the press ({fromId, toId} or null); selected by a click.
			property var pressedLink: null;
			// A press on one of several selected objects keeps them for moving; a click without moving selects this object only.
			property bool pendingSingleSelect: false;
			// Selection when the selection rectangle was started (restored by Escape).
			property var rubberBandBaseIds: [];
			property int rubberBandBaseIndex: -1;
			// Distance (pixels) the mouse must move before a press becomes a drag.
			property real dragThreshold: 4;

			function movedFromPress(){
				return Math.abs(controlArea.coord.x - controlArea.coordPressed.x) > controlArea.dragThreshold
						|| Math.abs(controlArea.coord.y - controlArea.coordPressed.y) > controlArea.dragThreshold;
			}

			onRightClicked: {
				if (canvas.selectedIndex >= 0){
					let point = controlArea.mapToItem(null, mouseX, mouseY);
					canvasPage.contextMenuRequested(canvas.selectedIndex, point.x, point.y);
				}
			}

			onClicked: {
				if(!wasMoving){
					for(let i = 0; i < canvasPage.objectsModel.count; i++){
						let item = canvasPage.objectsModel.get(i).item;
						item.m_selected = canvasPage.isObjectSelected(i);
					}
					canvas.requestPaint();
				}
			}

			onPressed: {
				controlArea.setLinkHintHover(-1);
				canvas.dropSuppressedIndex = -1;
				canvas.rubberBandActive = false;
				canvas.rubberBandCanceled = false;
				controlArea.rightButtonPressed = mouse.button == Qt.RightButton;
				controlArea.pendingSingleSelect = false;
				controlArea.pressedLink = null;
				controlArea.pressedIndex = canvas.findObjectAt(controlArea.coordPressed.x, controlArea.coordPressed.y);

				// Ctrl: the selection is changed on the release (click) or while dragging (rectangle);
				// neither the objects nor the scheme are moved.
				if (!controlArea.rightButtonPressed && (mouse.modifiers & Qt.ControlModifier)){
					controlArea.pressMode = "select";
					controlArea.rubberBandBaseIds = canvasPage.selectedIds.slice();
					controlArea.rubberBandBaseIndex = canvas.selectedIndex;
					controlArea.cursorShape = Qt.ArrowCursor;
					return;
				}

				let hitIndex = controlArea.pressedIndex;
				if (hitIndex >= 0){
					let hitId = canvasPage.objectsModel.get(hitIndex).item.m_id;
					if (canvasPage.selectedIds.length > 1 && canvasPage.selectedIds.indexOf(hitId) >= 0){
						// A selected object of several: all of them are kept (and moved together).
						canvasPage.setSelection(canvasPage.selectedIds, hitIndex);
						controlArea.pendingSingleSelect = !controlArea.rightButtonPressed;
					}
					else {
						canvasPage.setSelection([hitId], hitIndex);
					}
					controlArea.pressMode = controlArea.rightButtonPressed ? "none" : "move";
					return;
				}

				canvasPage.setSelection([], -1);
				if (controlArea.rightButtonPressed){
					controlArea.pressMode = "none";
					return;
				}
				controlArea.pressMode = "pan";
				controlArea.pressedLink = canvas.findLinkAt(controlArea.coordPressed.x, controlArea.coordPressed.y);
				controlArea.cursorShape = Qt.ClosedHandCursor;
			}

			onCanceled: {
				controlArea.isPressed = false;
				controlArea.wasMoving = false;
				controlArea.pressMode = "none";
				canvas.rubberBandActive = false;
				canvas.rubberBandCanceled = false;
				canvasPage.resetDrop();
				canvas.dropSuppressedIndex = -1;
			}

			onReleased: {
				let pressMode = controlArea.pressMode;
				controlArea.pressMode = "none";

				if (pressMode === "select"){
					if (canvas.rubberBandActive){
						// The selection was updated while dragging.
						canvas.rubberBandActive = false;
					}
					else if (!canvas.rubberBandCanceled && !controlArea.movedFromPress() && controlArea.pressedIndex >= 0){
						canvasPage.toggleObjectSelection(controlArea.pressedIndex);
					}
					canvas.rubberBandCanceled = false;
					controlArea.cursorShape = Qt.ArrowCursor;
					canvas.requestPaint();
					return;
				}

				// Released over a blinking target: the object is dropped onto it instead of being moved.
				let dropTargetIndex = canvas.dropTargetIndex
				let droppedIndex = canvas.selectedIndex
				canvasPage.resetDrop()
				canvas.dropSuppressedIndex = -1
				if (pressMode === "move" && dropTargetIndex >= 0 && droppedIndex >= 0
						&& dropTargetIndex < canvasPage.objectsModel.count && droppedIndex < canvasPage.objectsModel.count
						&& canvasPage.findDropCandidate() === dropTargetIndex){
					let droppedItem = canvasPage.objectsModel.get(droppedIndex).item
					let targetItem = canvasPage.objectsModel.get(dropTargetIndex).item
					controlArea.cursorShape = Qt.ArrowCursor
					canvasPage.objectDropped(droppedItem.m_id, targetItem.m_id, droppedItem.m_x, droppedItem.m_y)
					return
				}

				// A click on one of several selected objects selects this object only.
				if (pressMode === "move" && controlArea.pendingSingleSelect && !wasMoving && controlArea.pressedIndex >= 0
						&& controlArea.pressedIndex < canvasPage.objectsModel.count){
					canvasPage.setSelection([canvasPage.objectsModel.get(controlArea.pressedIndex).item.m_id], controlArea.pressedIndex);
				}
				controlArea.pendingSingleSelect = false;

				// A click on a link (not on an object) selects it, a click on an empty place clears the selected link.
				if (pressMode === "pan" && !controlArea.movedFromPress()){
					let link = controlArea.pressedLink;
					canvasPage.setSelectedLink(link);
					if (link){
						canvasPage.linkClicked(link.fromId, link.toId);
					}
				}

				if(canvas.selectedIndex >= 0){
					controlArea.cursorShape = Qt.ArrowCursor;
				}
				else {
					controlArea.cursorShape = Qt.OpenHandCursor
				}
				if (pressMode === "move" && canvas.selectedIndex >= 0 && wasMoving){
					canvasPage.objectMoveFinished()
				}
			}

			onDoubleClicked: {
				wasMoving = false;
				isPressed = false;

				if(canvas.selectedIndex >= 0 && mouse.button != Qt.RightButton && !(mouse.modifiers & Qt.ControlModifier)){
					canvasPage.goInside();
				}
			}

			onDeltaSignal: {
				if (controlArea.rightButtonPressed){
					return;
				}

				if (controlArea.pressMode === "select"){
					if (canvas.rubberBandCanceled){
						return;
					}
					if (!canvas.rubberBandActive){
						if (!controlArea.movedFromPress()){
							return;
						}
						canvas.rubberBandActive = true;
					}
					canvasPage.updateRubberBand(controlArea.coordPressed, controlArea.coord);
					return;
				}

				if(controlArea.pressMode === "pan"){
					canvasPage.autoFit = false;

					canvas.deltaX += delta.x
					canvas.deltaY += delta.y
					canvas.requestPaint();
				}
				else if (controlArea.pressMode === "move"){
					if (controlArea.movedFromPress()){
						controlArea.pendingSingleSelect = false;
					}
					movingFunction(delta);
					canvasPage.updateDropTarget();
				}
			}

			onWheel: {
				if (canvas.scaleCoeff == 0){
					return
				}

				canvasPage.autoFit = false;

				let deltaX = (wheel.x + canvas.deltaX) / canvas.scaleCoeff
				let wheelDelta = wheel.angleDelta.y
				let scaleCoeff_ = canvas.scaleCoeff;
				if(wheelDelta > 0){//up
					if (canvas.scaleCoeff < 3){
						scaleCoeff_ += scaleStep;
					}
				}
				else{//down
					if(canvas.scaleCoeff > scaleStep * 2){
						scaleCoeff_ -= scaleStep;
					}
				}

				canvas.setScale(scaleCoeff_, wheel.x, wheel.y)
			}


			// Moves all selected objects by the same offset, keeping them inside the sheet.
			function movingFunction(delta){//reimplemented
				if (canvas.selectedIndex < 0 || canvas.scaleCoeff < 0.0000001){
					return;
				}

				let indexes = canvasPage.selectedObjectIndexes();
				if (indexes.indexOf(canvas.selectedIndex) < 0){
					indexes.push(canvas.selectedIndex);
				}

				let offsetX = delta.x / canvas.scaleCoeff;
				let offsetY = delta.y / canvas.scaleCoeff;

				// fit to borders
				let margin_ = 10;
				let minOffsetX = -Number.MAX_VALUE;
				let maxOffsetX = Number.MAX_VALUE;
				let minOffsetY = -Number.MAX_VALUE;
				let maxOffsetY = Number.MAX_VALUE;
				for (let i = 0; i < indexes.length; i++){
					let item = canvasPage.objectsModel.get(indexes[i]).item;
					if (!item){
						continue;
					}
					let width_ = item.m_width ? item.m_width : canvas.mainRec_width;
					minOffsetX = Math.max(minOffsetX, margin_ - item.m_x);
					maxOffsetX = Math.min(maxOffsetX, canvas.backgroundWidth - width_ - margin_ - item.m_x);
					minOffsetY = Math.max(minOffsetY, margin_ - item.m_y);
					maxOffsetY = Math.min(maxOffsetY, canvas.backgroundHeight - canvas.mainRec_height - margin_ - item.m_y);
				}
				// Objects that do not fit together are moved without the correction.
				if (minOffsetX <= maxOffsetX){
					offsetX = Math.min(Math.max(offsetX, minOffsetX), maxOffsetX);
				}
				if (minOffsetY <= maxOffsetY){
					offsetY = Math.min(Math.max(offsetY, minOffsetY), maxOffsetY);
				}

				for (let i = 0; i < indexes.length; i++){
					let item = canvasPage.objectsModel.get(indexes[i]).item;
					if (item){
						item.m_x = item.m_x + offsetX;
						item.m_y = item.m_y + offsetY;
					}
				}

				canvas.requestPaint();

				canvasPage.modelDataChanged()
			}
			
			onPositionSignal: {
				positionChangedPause.position = position;
				positionChangedPause.restart();
			}
			PauseAnimation {
				id: positionChangedPause;
				
				duration: 100;
				property point position;
				onFinished: {
					controlArea.hoverReaction(position);
				}
			}
			
			function hoverReaction(position){
				canvas.hoverIndex = -1;
				for(let i = 0; i < canvasPage.objectsModel.count; i++){
					let item = canvasPage.objectsModel.get(i).item;
					
					let x_  = item.m_x;
					let y_  = item.m_y;
					let width_ = item.m_width ? item.m_width  : canvas.mainRec_width;
					let height_ = canvas.mainRec_height ;
					
					x_ = x_ * canvas.scaleCoeff + canvas.deltaX;
					y_ = y_ * canvas.scaleCoeff + canvas.deltaY;
					width_ = width_ * canvas.scaleCoeff;
					height_ = height_  * canvas.scaleCoeff;
					
					let ok = controlArea.checkHoverItem(x_, y_, width_, height_, position);
					
					if(ok){
						canvas.hoverIndex = i;
						
					}
				}
				if(canvas.hoverIndex >=0){
					controlArea.cursorShape = Qt.ArrowCursor;
					canvas.linkSelected = true;
					canvas.requestPaint();
				}
				else {
					// A link can be clicked
					controlArea.cursorShape = canvas.findLinkAt(position.x, position.y) ? Qt.PointingHandCursor : Qt.OpenHandCursor;
					if(canvas.linkSelected){
						canvas.linkSelected = false;
						canvas.requestPaint();
						
					}
				}

				// The link icon of the hovered object
				let linkHintIndex = -1;
				if(canvas.hoverIndex >= 0 && !controlArea.pressed){
					let hoverItem = canvasPage.objectsModel.get(canvas.hoverIndex).item;
					if(canvasPage.hasLinkHint(hoverItem)){
						let rect = canvas.linkHintRect(hoverItem);
						let ok = controlArea.checkHoverItem(
									rect.x * canvas.scaleCoeff + canvas.deltaX,
									rect.y * canvas.scaleCoeff + canvas.deltaY,
									rect.width * canvas.scaleCoeff,
									rect.height * canvas.scaleCoeff,
									position);
						if(ok){
							linkHintIndex = canvas.hoverIndex;
						}
					}
				}
				controlArea.setLinkHintHover(linkHintIndex, position);
				if(linkHintIndex >= 0){
					controlArea.cursorShape = Qt.PointingHandCursor;
				}
			}

			function setLinkHintHover(index, position){
				if(index === canvas.hoverLinkHintIndex){
					return;
				}

				canvas.hoverLinkHintIndex = index;
				linkHintTooltip.hide();
				if(index >= 0){
					let item = canvasPage.objectsModel.get(index).item;
					linkHintTooltip.text = canvasPage.linkHints[item.m_id];
					linkHintTooltip.show(position.x, position.y);
				}
				canvas.requestPaint();
			}

			onExited: {
				positionChangedPause.stop();
				controlArea.setLinkHintHover(-1);
			}

			CustomTooltip {
				id: linkHintTooltip;

				componentWidth: Style.sizeHintS;
				horizontalAlignment: Text.AlignLeft;
				// The same frame as the highlighted link icon
				borderColor: canvas.selectionColor;
				borderWidth: 1;
			}
			//hover reaction
			
		}
		
		Rectangle{
			id: backgroundRec;
			
			x: canvas.deltaX;
			y: canvas.deltaY;
			
			width: canvas.backgroundWidth * canvas.scaleCoeff;
			height: canvas.backgroundHeight * canvas.scaleCoeff;
			property real topY: y;
			property real bottomY: y + height;
			property real leftX: x;
			property real rightX: x + width;
			
			color: Style.baseColor;
		}
		
		Canvas {
			id: canvas
			
			anchors.fill: parent;
			
			antialiasing: true;
			
			property real scaleCoeff: 1.0;
			property real scaleCoeffPrev: 1.0;
			property real deltaX: 0.0;
			property real deltaY: 0.0;
			
			property int selectedIndex: -1;
			property int hoverIndex: -1;
			// Object whose link icon is under the cursor
			property int hoverLinkHintIndex: -1;
			// Dropping the dragged object onto another object (see dropTargetFilter)
			property int dropCandidateIndex: -1;
			property int dropTargetIndex: -1;
			property int dropSuppressedIndex: -1;
			property real dropScale: 1.0;
			property bool dropBlinkOn: false;
			onDropScaleChanged: requestPaint()
			property bool linkSelected: false;
			property bool hasTailSelection: false;
			// Selection rectangle (Ctrl + drag), canvas coordinates
			property bool rubberBandActive: false;
			// Escape cancelled the rectangle; the drag is ignored until the release
			property bool rubberBandCanceled: false;
			property real rubberBandX: 0;
			property real rubberBandY: 0;
			property real rubberBandWidth: 0;
			property real rubberBandHeight: 0;
			// Distance (pixels) from a link at which a click selects it
			property real linkHitTolerance: 6;

			//sizes
			property real mainRec_width: 250
			property real mainRec_height: 60
			
			property int fontSize: canvasPage.objectFontSize
			property int fontSizeS: canvasPage.objectSecondaryFontSize
			property int radius_: Style.radiusXS
			property int borderShift: 4
			property int shadowSize: 6
			// Offset of the back card that marks composite objects (groups)
			property int compositeStackOffset: 3
			
			property int backgroundStep: 30
			property int backgroundWidth: 1024
			property int backgroundHeight: 768
			
			property int intersectionSize: 16
			property int arcRadius: Style.radiusL
			property int textMargin: Style.marginS
			property int textVerticalOffset: 22
			property real imageSize: Style.iconSizeM
			property real imageMargin: 4
			
			//colors
			property string mainColor: Style.borderColor2
			property string errorColor: Style.errorColor
			property string compositeColor: Style.alternateBaseColor;
			// Composite objects have a double frame of the same color as the frame of the other objects
			property string compositeBorderColor: canvas.mainColor;
			// Frame of the selected objects (and of the selected link, the drop target, the selection
			// rectangle); their background is the same color with selectionFillOpacity
			property string selectionColor: Style.imagingToolsAccentColor;
			property real selectionFillOpacity: 0.25;
			property string selectedLinkFromColor: Style.selectedLinkFromColor
			property string selectedLinkToColor: Style.selectedLinkToColor
			property string linkColor: Style.borderColor2
			property string linkHintHoverColor: Style.hover
			property string mainTextColor: Style.textColor
			property string secondTextColor: Style.inactiveTextColor
			// The grid is drawn faint with the border color, which suits every theme
			property string gridColor: Style.borderColor;
			property real gridOpacity: 0.35;
			property string backgroundBorderColor: Style.borderColor; // Style.alternateBaseColor;
			property string backgroundColor: Style.baseColor;
			property string innerFrameColor: "transparent";
			
			onSelectedIndexChanged: {
				canvasPage.selectedIndex = selectedIndex
			}
			
			// A theme sets the colors of Style after its change event, so the canvas follows the colors themselves
			onBackgroundColorChanged: {
				requestPaint()
			}

			onMainTextColorChanged: {
				requestPaint()
			}

			onGridColorChanged: {
				requestPaint()
			}

			onWidthChanged: {
				requestPaint()
			}
			
			onHeightChanged: {
				requestPaint()
			}
			
			onImageLoaded: {
				requestPaint()
			}
			
			onScaleCoeffChanged: {
				requestPaint()
			}
			
			onDeltaXChanged: {
				canvasPage.contentX = -deltaX
			}
			
			onDeltaYChanged: {
				canvasPage.contentY = -canvas.deltaY
			}
			
			function setScale(newScale, scaleX, scaleY){
				if (newScale < 0.0000001){
					return;
				}
				
				let scaleCoeffBack = canvas.scaleCoeff;
				if (scaleCoeffBack * newScale + canvas.deltaX === 0 ||
						scaleCoeffBack * newScale + canvas.deltaY === 0){
					
					return;
				}
				
				let newX = (scaleX - canvas.deltaX) / scaleCoeffBack * newScale + canvas.deltaX
				let newY = (scaleY - canvas.deltaY) / scaleCoeffBack * newScale + canvas.deltaY
				canvas.deltaX -= (newX - scaleX)
				canvas.deltaY -= (newY - scaleY)
				
				canvas.scaleCoeff = newScale
				canvas.scaleCoeffPrev = newScale;
			}
			
			onPaint: {
				if (canvasPage.autoFit){
					canvasPage.zoomToFit();
				}
				
				var ctx = canvas.getContext('2d');
				ctx.reset()
				ctx.clearRect(0, 0, canvas.width, canvas.height)
				
				let transtateX = canvas.deltaX
				let transtateY = canvas.deltaY
				ctx.translate(transtateX, transtateY)
				ctx.scale(canvas.scaleCoeff, canvas.scaleCoeff)
				
				drawBackground(ctx);
				
				//width calculation
				for(let i = 0; i < canvasPage.objectsModel.count; i++){
					setObjectWidth(ctx, i);
				}
				
				//drawLink
				for(let i = 0; i < canvasPage.objectsModel.count; i++){
					let item = canvasPage.objectsModel.get(i).item;
					
					let links = item.m_links;
					for(let k = 0; k < links.count; k++){
						let objectId = links.get(k).item.m_id;
						let ind = canvasPage.findModelIndex(objectId);
						if(ind === -1) continue
						canvas.drawLink(ctx, ind, i);
					}
				}

				for(let i = 0; i < canvasPage.extraLinks.length; i++){
					let fromIndex = canvasPage.findModelIndex(canvasPage.extraLinks[i].fromId);
					let toIndex = canvasPage.findModelIndex(canvasPage.extraLinks[i].toId);
					if(fromIndex === -1 || toIndex === -1 || fromIndex === toIndex) continue
					canvas.drawLink(ctx, fromIndex, toIndex);
				}

				// The selected link over the others
				if(canvasPage.selectedLink){
					let fromIndex = canvasPage.findModelIndex(canvasPage.selectedLink.fromId);
					let toIndex = canvasPage.findModelIndex(canvasPage.selectedLink.toId);
					if(fromIndex >= 0 && toIndex >= 0 && fromIndex !== toIndex){
						canvas.drawLink(ctx, fromIndex, toIndex);
					}
				}

				//drawObject: the selected objects over the others, the current one on top
				let selectedIndexes = [];
				for(let i = 0; i < canvasPage.objectsModel.count; i++){
					if(i === canvas.selectedIndex){
						continue;
					}
					if(canvasPage.isObjectSelected(i)){
						selectedIndexes.push(i);
					}
					else {
						drawObject(ctx, i);
					}
				}
				for(let i = 0; i < selectedIndexes.length; i++){
					drawObject(ctx, selectedIndexes[i]);
				}
				if(canvas.selectedIndex >= 0 && canvas.selectedIndex < canvasPage.objectsModel.count){
					drawObject(ctx, canvas.selectedIndex);
				}
			}//onPaint

			// Index of the object at the point (canvas coordinates), -1 if none; the object drawn on top wins.
			function findObjectAt(pointX, pointY){
				let found = -1;
				let foundSelected = false;
				for(let i = 0; i < canvasPage.objectsModel.count; i++){
					let item = canvasPage.objectsModel.get(i).item;
					let width_ = item.m_width ? item.m_width : canvas.mainRec_width;
					let x_ = item.m_x * canvas.scaleCoeff + canvas.deltaX;
					let y_ = item.m_y * canvas.scaleCoeff + canvas.deltaY;
					if(pointX < x_ || pointY < y_ || pointX > x_ + width_ * canvas.scaleCoeff || pointY > y_ + canvas.mainRec_height * canvas.scaleCoeff){
						continue;
					}
					let selected = i === canvas.selectedIndex || canvasPage.isObjectSelected(i);
					if(found < 0 || selected || !foundSelected){
						found = i;
						foundSelected = selected;
					}
					if(i === canvas.selectedIndex){
						return i;
					}
				}
				return found;
			}

			// Link {fromId, toId} drawn at the point (canvas coordinates), null if none; the nearest link wins.
			function findLinkAt(pointX, pointY){
				if(canvas.scaleCoeff < 0.0000001){
					return null;
				}
				let px = (pointX - canvas.deltaX) / canvas.scaleCoeff;
				let py = (pointY - canvas.deltaY) / canvas.scaleCoeff;
				let bestDistance = canvas.linkHitTolerance / canvas.scaleCoeff;
				let best = null;
				let check = function(fromIndex, toIndex){
					let fromItem = canvasPage.objectsModel.get(fromIndex).item;
					let toItem = canvasPage.objectsModel.get(toIndex).item;
					let x1 = fromItem.m_x + (fromItem.m_width ? fromItem.m_width : canvas.mainRec_width) / 2;
					let y1 = fromItem.m_y + canvas.mainRec_height / 2;
					let x2 = toItem.m_x + (toItem.m_width ? toItem.m_width : canvas.mainRec_width) / 2;
					let y2 = toItem.m_y + canvas.mainRec_height / 2;
					let dx = x2 - x1;
					let dy = y2 - y1;
					let lengthSquared = dx * dx + dy * dy;
					let t = lengthSquared > 0 ? ((px - x1) * dx + (py - y1) * dy) / lengthSquared : 0;
					t = Math.max(0, Math.min(1, t));
					let distance = Math.sqrt(Math.pow(px - (x1 + t * dx), 2) + Math.pow(py - (y1 + t * dy), 2));
					if(distance <= bestDistance){
						bestDistance = distance;
						best = {"fromId": fromItem.m_id, "toId": toItem.m_id};
					}
				}

				for(let i = 0; i < canvasPage.objectsModel.count; i++){
					let links = canvasPage.objectsModel.get(i).item.m_links;
					for(let k = 0; links && k < links.count; k++){
						let ind = canvasPage.findModelIndex(links.get(k).item.m_id);
						if(ind >= 0 && ind !== i){
							check(ind, i);
						}
					}
				}
				for(let i = 0; i < canvasPage.extraLinks.length; i++){
					let fromIndex = canvasPage.findModelIndex(canvasPage.extraLinks[i].fromId);
					let toIndex = canvasPage.findModelIndex(canvasPage.extraLinks[i].toId);
					if(fromIndex >= 0 && toIndex >= 0 && fromIndex !== toIndex){
						check(fromIndex, toIndex);
					}
				}
				return best;
			}
			
			
			function drawBackground(ctx){
				let step = canvas.backgroundStep;
				
				ctx.lineCap = "round"
				ctx.lineJoin = "round"
				ctx.lineWidth = 1;
				
				ctx.fillStyle = canvas.gridColor;
				ctx.strokeStyle = canvas.gridColor;
				ctx.globalAlpha = canvas.gridOpacity;

				for(let i = 0; i * step < canvas.backgroundWidth; i++){//vertical lines
					let x1 = i * step;
					let y1 =  0 ;
					let x2 = i * step;
					let y2 = canvas.backgroundHeight ;
					
					ctx.beginPath()
					ctx.moveTo(x1, y1);
					ctx.lineTo(x2, y2);
					ctx.stroke();
				}
				
				for(let i = 0; i * step < canvas.backgroundHeight; i++){//horizontal lines
					let x1 = 0 ;
					let y1 =  i * step ;
					let x2 =  canvas.backgroundWidth;
					let y2 =  i * step;
					
					ctx.beginPath()
					ctx.moveTo(x1, y1);
					ctx.lineTo(x2, y2);
					ctx.stroke();
				}
				
				ctx.globalAlpha = 1;
				ctx.strokeStyle = canvas.backgroundBorderColor;
				ctx.lineWidth = 2;
				ctx.beginPath()
				ctx.moveTo(1, 1);
				ctx.lineTo(canvas.backgroundWidth, 1);
				ctx.lineTo(canvas.backgroundWidth, canvas.backgroundHeight - 1);
				ctx.lineTo(1, canvas.backgroundHeight - 1);
				ctx.lineTo(1, 1);
				ctx.stroke();
			}
			
			function setObjectWidth(ctx, index){
				//width calculation
				let item = canvasPage.objectsModel.get(index).item;
				
				let mainText  = item.m_mainText
				let secondText  = item.m_secondText
				let thirdText = item.m_thirdText
				
				ctx.lineWidth = 1;
				let fontStr_main = String(canvas.fontSize) + "px " + Style.fontFamily
				ctx.font = fontStr_main;

				let textWidth_main = ctx.measureText(mainText).width
				
				ctx.lineWidth = 0.5;
				let fontStr_second = String(canvas.fontSizeS) + "px " + Style.fontFamily
				ctx.font = fontStr_second;
				let textWidth_second = ctx.measureText(secondText).width
				let textWidth_third = ctx.measureText(thirdText).width
				
				let horizontalMargins = 2 * (canvas.borderShift + canvas.textMargin)
				let iconsWidth = 0
				if (item.m_icon1 !== ""){
					iconsWidth += canvas.imageSize + canvas.imageMargin
				}
				if (item.m_icon2 !== ""){
					iconsWidth += canvas.imageSize + canvas.imageMargin
				}
				if (canvasPage.hasLinkHint(item)){
					iconsWidth += canvas.imageSize + canvas.imageMargin
				}
				let bottomSpacing = thirdText !== "" ? canvas.textMargin : 0
				let mainRecWidth = Math.max(textWidth_main + horizontalMargins + iconsWidth,
					textWidth_second + textWidth_third + horizontalMargins + bottomSpacing,
					canvas.mainRec_width)
				if (canvasPage.maximumObjectWidth > 0){
					mainRecWidth = Math.min(mainRecWidth, canvasPage.maximumObjectWidth)
				}
				// canvasPage.objectsModel.setData("width", mainRecWidth /*/ scaleCoeff*/, index);
				
				item.m_width = mainRecWidth;
				
				return mainRecWidth;
			}

			function elideText(ctx, text, maximumWidth){
				if (!text || ctx.measureText(text).width <= maximumWidth){
					return text
				}

				let suffix = "..."
				let elidedText = text
				while (elidedText.length > 0 && ctx.measureText(elidedText + suffix).width > maximumWidth){
					elidedText = elidedText.slice(0, -1)
				}
				return elidedText + suffix
			}
			
			// The drop target is drawn enlarged, highlighted and blinking.
			function drawObject(ctx, index){
				if (index !== canvas.dropTargetIndex){
					drawObjectBody(ctx, index);
					return;
				}

				let item = canvasPage.objectsModel.get(index).item;
				let width_ = item.m_width ? item.m_width : canvas.mainRec_width;
				let centerX = item.m_x + width_ / 2;
				let centerY = item.m_y + canvas.mainRec_height / 2;

				ctx.save();
				ctx.translate(centerX, centerY);
				ctx.scale(canvas.dropScale, canvas.dropScale);
				ctx.translate(-centerX, -centerY);

				drawObjectBody(ctx, index);

				width_ = item.m_width ? item.m_width : width_;
				ctx.beginPath();
				ctx.roundedRect(item.m_x, item.m_y, width_, canvas.mainRec_height, canvas.radius_, canvas.radius_);
				if (canvas.dropBlinkOn){
					ctx.globalAlpha = 0.3;
					ctx.fillStyle = canvas.selectionColor;
					ctx.fill();
					ctx.globalAlpha = 1.0;
				}
				ctx.lineWidth = 3;
				ctx.strokeStyle = canvas.selectionColor;
				ctx.stroke();

				ctx.restore();
			}

			function drawObjectBody(ctx, index){
				let item = canvasPage.objectsModel.get(index).item;
				
				let x_  = item.m_x
				let y_  = item.m_y
				let mainText  =  item.m_mainText
				let secondText  = item.m_secondText
				let thirdText  = item.m_thirdText
				let backgroundColor = item.m_backgroundColor ? item.m_backgroundColor : canvas.backgroundColor;
				let icon = item.m_icon1
				let iconUrl_1  = icon === "" ? "" : "../../../" + Style.getIconPath(icon, Icon.State.On, Icon.Mode.Normal)
				icon = item.m_icon2
				let iconUrl_2  = icon === "" ? "" : "../../../" + Style.getIconPath(icon, Icon.State.On, Icon.Mode.Normal)
				let selected = index === canvas.selectedIndex || canvasPage.isObjectSelected(index)

				let hasError = item.m_hasError
				let isComposite = item.m_isComposite === true
				// A selected object is framed with the selection color, the others with the common color
				// (composite objects as well) or with the error color
				let frameColor = selected ? canvas.selectionColor
										  : hasError ? canvas.errorColor : isComposite ? canvas.compositeBorderColor : canvas.mainColor

				ctx.lineCap = "round"
				ctx.lineJoin = "round"
				
				//width calculation
				let mainRecWidth = setObjectWidth(ctx, index);
				
				//shadow rectangle
				let shadowSize = canvas.shadowSize;
				ctx.lineWidth = 2 ;
				// ctx.fillStyle = selected ? Qt.rgba(0.2, 0.8, 0, 0.3) : Qt.rgba(0, 0, 0, 0.2);
				ctx.fillStyle = Qt.rgba(0, 0, 0, 0.1);
				ctx.beginPath()
				ctx.roundedRect(x_ + shadowSize, y_ + shadowSize, mainRecWidth, canvas.mainRec_height , canvas.radius_, canvas.radius_);
				ctx.fill();
				

				
				//main rectangle
				ctx.lineWidth = 2 ;
				ctx.strokeStyle = frameColor;
				ctx.fillStyle = isComposite ? canvas.compositeColor : backgroundColor;
				ctx.beginPath()
				ctx.roundedRect(x_, y_, mainRecWidth, canvas.mainRec_height , canvas.radius_, canvas.radius_);
				ctx.fill();
				// The background of a selected object: the selection color with transparency (over the
				// usual background, so the links and the grid do not show through)
				if (selected){
					ctx.globalAlpha = canvas.selectionFillOpacity;
					ctx.fillStyle = canvas.selectionColor;
					ctx.fill();
					ctx.globalAlpha = 1.0;
				}
				ctx.stroke();
				
				//inner rectangle
				ctx.strokeStyle = canvas.innerFrameColor;
				
				ctx.beginPath()
				ctx.roundedRect(x_ + canvas.borderShift, y_ + canvas.borderShift, mainRecWidth - 2 * canvas.borderShift, canvas.mainRec_height  - 2 * canvas.borderShift, canvas.radius_, canvas.radius_);
				ctx.stroke();
				
				//frame of the composite object (stack of objects)
				if (isComposite){
					let stackOffset = canvas.compositeStackOffset
					ctx.lineWidth = 1
					ctx.strokeStyle = frameColor
					ctx.beginPath()
					ctx.roundedRect(x_ + stackOffset, y_ + stackOffset, mainRecWidth - 2 * stackOffset, canvas.mainRec_height - 2 * stackOffset, canvas.radius_, canvas.radius_);

					//ctx.fill();
					ctx.stroke();
				}

				//Main text
				ctx.strokeStyle = canvas.mainTextColor;
				ctx.fillStyle = canvas.mainTextColor;
				ctx.lineWidth = 1;
				ctx.font = String(canvas.fontSize) + "px " + Style.fontFamily
				let text_x = x_ + canvas.borderShift + canvas.textMargin;
				let text_y = y_ + canvas.borderShift + canvas.textVerticalOffset;
				let iconsWidth = 0
				if (iconUrl_1 !== ""){
					iconsWidth += canvas.imageSize + canvas.imageMargin
				}
				if (iconUrl_2 !== ""){
					iconsWidth += canvas.imageSize + canvas.imageMargin
				}
				let hasLinkHint = canvasPage.hasLinkHint(item)
				if (hasLinkHint){
					iconsWidth += canvas.imageSize + canvas.imageMargin
				}
				let mainTextWidth = mainRecWidth - 2 * (canvas.borderShift + canvas.textMargin) - iconsWidth
				let textStr = elideText(ctx, mainText, mainTextWidth)
				
				ctx.beginPath()
				ctx.fillText(textStr, text_x, text_y);
				//                ctx.strokeText(textStr, text_x, text_y);
				
				//Second text
				ctx.strokeStyle = canvas.secondTextColor;
				ctx.fillStyle = canvas.secondTextColor;
				ctx.lineWidth = 0.5;
				let fontStr2 = String(canvas.fontSizeS) + "px " + Style.fontFamily
				ctx.font = fontStr2;
				let availableBottomWidth = mainRecWidth - 2 * (canvas.borderShift + canvas.textMargin)
				let thirdTextWidth = Math.min(ctx.measureText(thirdText).width, availableBottomWidth / 2)
				let textStr3 = elideText(ctx, thirdText, thirdTextWidth)
				let textWidth3 = ctx.measureText(textStr3).width
				let secondTextWidth = mainRecWidth - 2 * (canvas.borderShift + canvas.textMargin) - textWidth3
				if (thirdText !== ""){
					secondTextWidth -= canvas.textMargin
				}
				let textStr2 = elideText(ctx, secondText, secondTextWidth)
				
				let text_x2 = x_ + canvas.borderShift + canvas.textMargin;
				let text_y2 = y_ + canvas.mainRec_height - canvas.borderShift - canvas.textMargin;
				
				ctx.beginPath()
				ctx.fillText(textStr2, text_x2, text_y2);
				if (canvasPage.strokeSecondaryText){
					ctx.strokeText(textStr2, text_x2, text_y2);
				}
				
				//Third text
				ctx.strokeStyle = canvas.secondTextColor;
				ctx.fillStyle = canvas.secondTextColor;
				ctx.lineWidth = 0.5;
				ctx.font = fontStr2;
				
				let text_x3 = x_ + mainRecWidth - textWidth3 - canvas.borderShift - canvas.textMargin ;
				let text_y3 = y_ + canvas.mainRec_height - canvas.borderShift - canvas.textMargin;
				
				ctx.beginPath()
				ctx.fillText(textStr3, text_x3, text_y3);
				if (canvasPage.strokeSecondaryText){
					ctx.strokeText(textStr3, text_x3, text_y3);
				}
				
				//images; the link icon is the rightmost one (top right corner)
				let image1_x = x_ + mainRecWidth - canvas.borderShift - canvas.imageSize - canvas.imageMargin;
				if (hasLinkHint){
					let linkRect = canvas.linkHintRect(item)
					let hovered = index === canvas.hoverLinkHintIndex
					if (hovered){
						ctx.fillStyle = canvas.linkHintHoverColor
						ctx.strokeStyle = canvas.selectionColor
						ctx.lineWidth = 1
						ctx.beginPath()
						ctx.roundedRect(linkRect.x, linkRect.y, linkRect.width, linkRect.height, canvas.radius_, canvas.radius_)
						ctx.fill()
						ctx.stroke()
					}
					let linkIconUrl = "../../../" + Style.getIconPath(canvasPage.linkHintIcon, Icon.State.On,
								hovered ? Icon.Mode.Active : Icon.Mode.Normal)
					ctx.beginPath()
					ctx.drawImage(linkIconUrl, image1_x, text_y - canvas.imageSize + canvas.imageMargin, canvas.imageSize, canvas.imageSize);
					image1_x -= canvas.imageSize + canvas.imageMargin
				}
				let image1_y = text_y - canvas.imageSize + canvas.imageMargin;
				let image2_x = iconUrl_1 !== "" ? image1_x - canvas.imageSize - canvas.imageMargin : image1_x;
				let image2_y = text_y - canvas.imageSize + canvas.imageMargin;
				ctx.beginPath()
				if (iconUrl_1 !== ""){
					ctx.drawImage(iconUrl_1, image1_x, image1_y, canvas.imageSize, canvas.imageSize);
				}
				if (iconUrl_2 !== ""){
					ctx.drawImage(iconUrl_2, image2_x, image2_y, canvas.imageSize, canvas.imageSize);
				}
				
			}
			
			// Area of the link icon of the object (scheme coordinates), slightly larger than the icon.
			function linkHintRect(item){
				let width_ = item.m_width ? item.m_width : canvas.mainRec_width
				let iconX = item.m_x + width_ - canvas.borderShift - canvas.imageSize - canvas.imageMargin
				let iconY = item.m_y + canvas.borderShift + canvas.textVerticalOffset - canvas.imageSize + canvas.imageMargin
				let padding = 2
				return {
					"x": iconX - padding,
					"y": iconY - padding,
					"width": canvas.imageSize + 2 * padding,
					"height": canvas.imageSize + 2 * padding
				}
			}

			function drawLink(ctx, fromIndex, toIndex){
				let fromItem = canvasPage.objectsModel.get(fromIndex).item;
				let toItem = canvasPage.objectsModel.get(toIndex).item;
				
				let x1 = fromItem.m_x;
				let y1 = fromItem.m_y;
				let x2 = toItem.m_x;
				let y2 = toItem.m_y;
				
				let width1 = fromItem.m_width ? fromItem.m_width : canvas.mainRec_width;
				let width2 = toItem.m_width ? toItem.m_width : canvas.mainRec_width;
				
				let x1_link = x1 + width1/2;
				let y1_link = y1 + canvas.mainRec_height/2;
				let x2_link = x2 + width2/2;
				let y2_link = y2 + canvas.mainRec_height/2;
				
				// The clicked link is drawn thicker with the selection color
				let linkSelected = canvasPage.isLinkSelected(fromItem.m_id, toItem.m_id);

				ctx.lineCap = "round"
				ctx.lineJoin = "round"
				ctx.lineWidth = linkSelected ? 3 : 2;

				ctx.strokeStyle = linkSelected ? canvas.selectionColor
											   : canvas.hoverIndex === fromIndex ? canvas.selectedLinkFromColor : canvas.hoverIndex === toIndex ? canvas.selectedLinkToColor : canvas.linkColor;
				ctx.fillStyle = ctx.strokeStyle
				
				ctx.beginPath()
				ctx.moveTo(x1_link, y1_link);
				ctx.lineTo(x2_link, y2_link);
				ctx.stroke();
				
				//draw intersection
				let x1_rec2;
				let y1_rec2;
				let x2_rec2;
				let y2_rec2;
				
				let intersection;
				
				if(y1 > y2){
					//bottom line
					x1_rec2 = x2;
					y1_rec2 = y2 + canvas.mainRec_height;
					x2_rec2 = x2 + width2;
					y2_rec2 = y2 + canvas.mainRec_height;
					
					intersection = findIntersection(x1_link, y1_link, x2_link, y2_link, x1_rec2, y1_rec2, x2_rec2, y2_rec2);
					
					if(intersection.x < x2 + width2 && intersection.x > x2){
						//bottom line
						x1_rec2 = x2;
						y1_rec2 = y2 + canvas.mainRec_height;
						x2_rec2 = x2 + width2;
						y2_rec2 = y2 + canvas.mainRec_height;
					}
					else if(intersection.x >= x2 + width2){
						//right line
						x1_rec2 = x2  + width2;
						y1_rec2 = y2 + canvas.mainRec_height;
						x2_rec2 = x2 + width2;
						y2_rec2 = y2;
					}
					else if(intersection.x <= x2){
						//left line
						x1_rec2 = x2;
						y1_rec2 = y2 + canvas.mainRec_height;
						x2_rec2 = x2;
						y2_rec2 = y2;
					}
				}//y1 > y2
				
				else if (y1 < y2){
					//top line
					x1_rec2 = x2;
					y1_rec2 = y2;
					x2_rec2 = x2 + width2;
					y2_rec2 = y2;
					
					intersection = findIntersection(x1_link, y1_link, x2_link, y2_link, x1_rec2, y1_rec2, x2_rec2, y2_rec2);
					
					if(intersection.x < x2 + width2 && intersection.x > x2){
						//top line
						x1_rec2 = x2;
						y1_rec2 = y2;
						x2_rec2 = x2 + width2;
						y2_rec2 = y2;
					}
					else if(intersection.x >= x2 + width2){
						//right line
						x1_rec2 = x2  + width2;
						y1_rec2 = y2 + canvas.mainRec_height;
						x2_rec2 = x2 + width2;
						y2_rec2 = y2;
					}
					else if(intersection.x <= x2){
						//left line
						x1_rec2 = x2;
						y1_rec2 = y2 + canvas.mainRec_height;
						x2_rec2 = x2;
						y2_rec2 = y2;
					}
					
				}//y1 < y2
				
				else if(y1 == y2){
					if(x1 >= x2){
						//right line
						x1_rec2 = x2  + width2;
						y1_rec2 = y2 + canvas.mainRec_height;
						x2_rec2 = x2 + width2;
						y2_rec2 = y2;
						
						
					}
					else {
						//left line
						x1_rec2 = x2;
						y1_rec2 = y2 + canvas.mainRec_height;
						x2_rec2 = x2;
						y2_rec2 = y2;
						
					}
				}//y1 == y2
				
				intersection = findIntersection(x1_link, y1_link, x2_link, y2_link, x1_rec2, y1_rec2, x2_rec2, y2_rec2);
				
				//for intersection margin
				let angle
				let offset
				
				let hasMargin = true;
				let complexIntersection = true;
				if(hasMargin){
					angle = findAngle(x1_link, y1_link,intersection.x, intersection.y)
					
					let quarter =  Math.trunc(angle/(Math.PI/2))
					let add = 0;
					
					//offset length correction
					if(quarter % 2){
						add = angle  - quarter * Math.PI/2  < Math.PI/4 ? Math.abs(Math.sin(angle)) : Math.abs(Math.cos(angle))
					}
					else {
						add = angle  - quarter * Math.PI/2  < Math.PI/4 ? Math.abs(Math.cos(angle)) : Math.abs(Math.sin(angle))
					}
					
					let smallAngle = Math.abs(angle  - (quarter + 0) * Math.PI/2) < (Math.PI/180) * 30
						|| Math.abs(angle  - (quarter + 1) * Math.PI/2) < (Math.PI/180) * 30;
					if(smallAngle){
						//add = 0.5;
					}
					//offset length correction
					
					offset = 20 * (1 + add);
					
					let newX = intersection.x + offset * Math.cos(angle)
					let newY = intersection.y + offset * Math.sin(angle)
					
					intersection = Qt.point(newX, newY);
					
				}//for intersection margin
				
				if(complexIntersection && hasMargin){
					drawIntersectionExt(ctx, intersection,angle, false);
					drawIntersectionArc(ctx, intersection, angle + Math.PI, offset, false)
					
				}
				else {//simple
					drawIntersection(ctx, intersection, false);
				}
			}
			
			function drawIntersection(ctx, intersection, selected){
				let size = canvas.intersectionSize;
				
				ctx.lineWidth = 0.5;
				ctx.beginPath()
				ctx.roundedRect(intersection.x - size/2, intersection.y  - size/2, size, size, size, size);
				ctx.fill();
				ctx.stroke();
			}
			
			function drawIntersectionExt(ctx, intersection, angle, selected){
				let size = canvas.intersectionSize;
				let sizeSmall = size/2;
				
				ctx.beginPath()
				ctx.roundedRect(intersection.x - sizeSmall/2, intersection.y  - sizeSmall/2, sizeSmall, sizeSmall, sizeSmall, sizeSmall);
				ctx.fill();
				ctx.stroke();
				
				//draw tail
				
				ctx.beginPath();
				ctx.moveTo(intersection.x, intersection.y)
				ctx.lineWidth = 0.5;
				let offset = size/2;
				let newX = intersection.x + offset * Math.cos(angle)
				let newY = intersection.y + offset * Math.sin(angle)
				ctx.lineTo(newX, newY);
				ctx.stroke();
				
				
			}
			
			function drawIntersectionArc(ctx, point, angle, offset, selected){
				let rad = canvas.arcRadius;
				let endAngle = 0.6 * Math.PI;
				ctx.lineWidth = 2;
				ctx.beginPath()
				ctx.arc(point.x, point.y, rad, angle , angle - endAngle, true);
				ctx.stroke();
				ctx.beginPath()
				ctx.arc(point.x, point.y, rad, angle , angle + endAngle, false);
				ctx.stroke();
				
				ctx.beginPath();//draw tail
				let startX = point.x + rad * Math.cos(angle);
				let startY = point.y  + rad * Math.sin(angle);
				ctx.moveTo(startX, startY);
				ctx.lineWidth = 0.6;
				let finishX = startX + offset * Math.cos(angle)
				let finishY = startY + offset * Math.sin(angle)
				ctx.lineTo(finishX, finishY);
				ctx.stroke();
				
			}
			
			function findIntersection(x1, y1, x2, y2, x3, y3, x4, y4){
				
				let px = ((x1 * y2 - y1 * x2) * (x3 - x4) - (x1 - x2) * (x3 * y4 - y3 * x4)) /
					((x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4));
				
				let py = ((x1 * y2 - y1 * x2) * (y3 - y4) - (y1 - y2) * (x3 * y4 - y3 * x4)) /
					((x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4));
				
				
				return Qt.point(px, py);
			}
			
			function findAngle(x1, y1, x2, y2){
				if (x2 - x1 == 0){
					return 0
				}
				
				let angle;
				angle = Math.atan((y2 - y1) / (x2 - x1))
				
				if((x2 == x1) && (y1 == y2)){
					angle = 0;
				}
				else {
					let angleBase = Math.atan((y2 - y1) / (x2 - x1))
					
					if((y1 == y2) && (x1 > x2)){
						angle = 0
					}
					else if((y1 == y2) && (x1 < x2)){
						angle = Math.PI;
					}
					else if((y1 < y2) && (x1 == x2)){
						angle = 1.5 * Math.PI;
					}
					else if((y1 > y2) && (x1 == x2)){
						angle = Math.PI/2;
					}
					//
					else if((y1 > y2) && (x1 < x2)){//boottom left
						angle = angleBase + Math.PI;
					}
					else if((y1 < y2) && (x1 < x2)){//top left
						angle = angleBase + Math.PI;
					}
					else if((y1 < y2) && (x1 > x2)){//top right
						angle = angleBase + 2 * Math.PI;
					}
				}
				return angle;
			}
		}//canvas

		// Selection rectangle (Ctrl + drag): a semi-transparent area with a frame of the selection color.
		Rectangle {
			id: rubberBand

			x: canvas.rubberBandX
			y: canvas.rubberBandY
			width: canvas.rubberBandWidth
			height: canvas.rubberBandHeight
			visible: canvas.rubberBandActive

			color: "transparent"
			border.width: 1
			border.color: canvas.selectionColor

			Rectangle {
				anchors.fill: parent
				anchors.margins: 1

				color: canvas.selectionColor
				opacity: canvas.selectionFillOpacity
			}
		}
	}//mainContainer
	
	
	Shortcut {
		sequence: "Ctrl+C";
		enabled: canvasPage.visible;
		onActivated: {
			canvasPage.copySignal(canvas.selectedIndex);
		}
	}
	
	Shortcut {
		id: pasteShortcut;
		
		sequence: "Ctrl+V";
		enabled: canvasPage.visible;
		onActivated: {
			canvasPage.pasteSignal(canvas.selectedIndex);
		}
	}
	
	
	Shortcut {
		sequence: "Delete";
		enabled: canvasPage.visible;
		onActivated: {
			canvasPage.deleteSignal(canvas.selectedIndex);
		}
	}
	
	Shortcut {
		sequence: "Ctrl+Z";
		enabled: canvasPage.visible;
		onActivated: {
			canvasPage.revertSignal();
		}
	}
	
	Shortcut {
		sequence: "F2";
		enabled: canvasPage.visible;
		onActivated: {
			canvasPage.renameSignal(canvas.selectedIndex);
		}
	}
	
	
	function copyObjectFunc(index){
		if(index >= 0){
			// bufferModel.clear();
			// bufferModel.copyItemDataFromModel(0, canvasPage.objectsModel, index);
		}
	}
	
	function pasteObjectFunc(){
		if(bufferModel.getItemsCount()){
			bufferModel.setExternTreeModel("links", emptyModel, 0);
			let mainText = bufferModel.getData("mainText") + "_1";
			let secondText = bufferModel.getData("secondText") + "_1";
			let x_ = bufferModel.getData("x") + 10;
			let y_ = bufferModel.getData("y") + 10;
			
			//TO DO: unique Id
			var date = new Date();
			let datString = String(date.valueOf());
			bufferModel.setData("id", datString, 0);
			
			bufferModel.setData("mainText", mainText, 0);
			bufferModel.setData("secondText", secondText, 0);
			bufferModel.setData("x", x_, 0);
			bufferModel.setData("y", y_, 0);
			bufferModel.setData("Selected", false, 0);
			// let index = canvasPage.objectsModel.insertNewItem();
			// canvasPage.objectsModel.copyItemDataFromModel(index, bufferModel,0);
			
			canvas.selectedIndex = 100//index;
			
			canvas.requestPaint();
		}
	}
	
	function deleteObjectFunc(index){
		let id = canvasPage.objectsModel.get(index).item.m_id;
		
		//remove links
		for(let i = 0; i < canvasPage.objectsModel.count; i++){
			let item = canvasPage.objectsModel.get(i).item;
			let links = item.m_links;
			if(links !== undefined){
				for(let k = 0; k < links.count; k++){
					let itemLink = links.get(k).item;
					let objectId = itemLink.m_objectId;
					if(objectId == id){
						links.removeItem(k);
						break;
					}
				}
			}
		}
		
		//remove object
		canvasPage.objectsModel.removeItem(index);
		
		canvas.selectedIndex = -1;
		
		canvas.requestPaint();
	}
	
	
	CustomScrollbar{
		id: scrollVert;
		
		anchors.right: parent.right;
		anchors.rightMargin: 1;
		anchors.bottom: parent.bottom;
		
		secondSize: 12;
		visible: (backgroundRec.topY >=0 && backgroundRec.bottomY <= canvasPage.height) ? false : true;
		alwaysVisible: true;
		canDragOutOfBounds: true;
		targetItem: parent;
		z: (scrollVert.isOnTop && scrollHoriz.isOnTop) ? 21 : (!scrollVert.isOnTop && !scrollHoriz.isOnTop) ? 21 : 20 + scrollVert.isOnTop;
		property bool isOnTop: true;
		onContentYSignal: {
			canvas.requestPaint();
		}
		onMovingSignal: {
			scrollVert.isOnTop = true;
			scrollHoriz.isOnTop = false;
		}
		
	}
	
	CustomScrollbar{
		id: scrollHoriz;
		
		anchors.right: parent.right;
		anchors.bottom: parent.bottom;
		anchors.bottomMargin: 1;
		
		secondSize: 12;
		
		visible: (backgroundRec.leftX >=0 && backgroundRec.rightX <= canvasPage.width) ? false : true;
		alwaysVisible: true;
		canDragOutOfBounds: true;
		
		vertical: false;
		targetItem: parent;
		z: (scrollHoriz.isOnTop && scrollVert.isOnTop) ? 20 : (!scrollHoriz.isOnTop && !scrollVert.isOnTop) ? 20 : 20 + scrollHoriz.isOnTop;
		property bool isOnTop: false;
		onContentXSignal: {
			canvas.requestPaint();
		}
		onMovingSignal: {
			scrollVert.isOnTop = false;
			scrollHoriz.isOnTop = true;
		}
	}
	
}
