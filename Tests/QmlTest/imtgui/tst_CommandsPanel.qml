import QtQuick 2.12
import QtTest 1.12
import imtgui 1.0
import imtbaseCommandsSdl 1.0

TestCase {
	id: testCase

	name: "CommandsPanel"
	when: windowShown

	Component {
		id: panelComp

		CommandsPanel {
			width: 1600
		}
	}

	Component {
		id: containerComp

		GuiElementContainer {
		}
	}

	function command(id, priority){
		return {"elementId": id, "elementName": id, "elementItemPath": "", "visible": true, "enabled": true, "priority": priority, "subElements": []}
	}

	// Commands of a group are named by the group and numbered from the highest priority
	function group(prefix, alignment, count){
		let commands = []
		for (let i = 0; i < count; ++i){
			commands.push(command(prefix + i, 100 - i))
		}
		return {"elementId": prefix, "alignment": alignment, "subElements": commands}
	}

	function createPanel(groups){
		let panel = createTemporaryObject(panelComp, testCase.parent)
		let container = createTemporaryObject(containerComp, testCase)
		verify(container.createFromJson(JSON.stringify({"elements": groups})))
		panel.commandsModel = container
		tryVerify(function(){ return commandButton(panel, groups[0].subElements[0].elementId) !== null }, 3000, "The buttons are created")
		wait(100)
		return panel
	}

	function findItem(item, check){
		if (check(item)){
			return item
		}
		for (let i = 0; i < item.children.length; ++i){
			let found = findItem(item.children[i], check)
			if (found){
				return found
			}
		}
		return null
	}

	function commandButton(panel, id){
		return findItem(panel, function(item){ return item.objectName === id + "Button" })
	}

	function moreButton(panel){
		return findItem(panel, function(item){ return item.objectName === "MoreCommandsButton" })
	}

	function groupView(panel, index){
		let views = []
		for (let i = 0; i < panel.children.length; ++i){
			if (panel.children[i].objectName === "CommandsView"){
				views.push(panel.children[i])
			}
		}
		return views[index]
	}

	function setWidth(panel, width){
		panel.width = width
		wait(100)
	}

	function naturalWidth(view){
		let sizes = view.commandSizes()
		let width = 0
		for (let i = 0; i < sizes.length; ++i){
			width += sizes[i].width
		}
		return sizes.length > 0 ? width + (sizes.length - 1) * view.spacing : 0
	}

	// Width the shown commands of a group take, how many of them are shown and the next one
	function shownOf(view){
		let sizes = view.commandSizes()
		let count = 0
		while (count < sizes.length && view.priorityElements[count] && !view.priorityElements[count].hidden){
			count++
		}
		let width = 0
		for (let i = 0; i < count; ++i){
			width += sizes[i].width
		}
		width = count > 0 ? width + (count - 1) * view.spacing : 0
		let next = count < sizes.length ? sizes[count] : null
		return {"count": count, "width": width, "next": next, "sizes": sizes,
			"grown": next ? width + (count > 0 ? view.spacing : 0) + next.width : width}
	}

	// Every shown button is inside the panel and no two of them overlap
	function checkNoOverlap(panel, ids){
		let rects = []
		for (let i = 0; i < ids.length; ++i){
			let button = commandButton(panel, ids[i])
			if (!button.visible){
				continue
			}
			let topLeft = button.mapToItem(panel, 0, 0)
			rects.push({"id": ids[i], "x": topLeft.x, "right": topLeft.x + button.width})
		}
		let more = moreButton(panel)
		if (more.visible){
			let topLeft = more.mapToItem(panel, 0, 0)
			rects.push({"id": "more", "x": topLeft.x, "right": topLeft.x + more.width})
		}
		for (let i = 0; i < rects.length; ++i){
			verify(rects[i].x >= 0 && rects[i].right <= panel.width + 1, rects[i].id + " is inside the panel: " + rects[i].x + ".." + rects[i].right)
			for (let j = 0; j < i; ++j){
				verify(rects[i].right <= rects[j].x || rects[j].right <= rects[i].x, rects[i].id + " does not cover " + rects[j].id)
			}
		}
	}

	function ids(prefix, count){
		let result = []
		for (let i = 0; i < count; ++i){
			result.push(prefix + i)
		}
		return result
	}

	function test_wideShowsAll(){
		let panel = createPanel([group("L", 1, 3), group("R", 2, 2)])
		let all = ids("L", 3).concat(ids("R", 2))
		for (let i = 0; i < all.length; ++i){
			verify(commandButton(panel, all[i]).visible, all[i] + " is shown")
		}
		verify(!moreButton(panel).visible)
		checkNoOverlap(panel, all)
	}

	// A group needing more than half the panel gets the room the other one does not need
	function test_groupTakesTheRoomOfAShortNeighbour(){
		let panel = createPanel([group("L", 1, 1), group("R", 2, 6)])
		let left = naturalWidth(groupView(panel, 0))
		let right = naturalWidth(groupView(panel, 2))
		verify(right > 3 * left, "The right group is the long one: " + left + " / " + right)

		setWidth(panel, left + right + 3 * 16 + 20)
		let all = ids("L", 1).concat(ids("R", 6))
		for (let i = 0; i < all.length; ++i){
			verify(commandButton(panel, all[i]).visible, all[i] + " is shown")
		}
		verify(!moreButton(panel).visible, "Nothing is hidden, so no \"more\" button")
		checkNoOverlap(panel, all)
	}

	// A narrow panel hides the lowest-priority commands first and offers them behind the "more" button
	function test_narrowHidesByPriority(){
		let panel = createPanel([group("L", 1, 4), group("C", 4, 3), group("R", 2, 4)])
		let all = ids("L", 4).concat(ids("C", 3)).concat(ids("R", 4))
		let widths = [1200, 900, 700, 500, 350]
		for (let w = 0; w < widths.length; ++w){
			setWidth(panel, widths[w])
			checkNoOverlap(panel, all)

			let prefixes = ["L", "C", "R"]
			let counts = [4, 3, 4]
			let anyHidden = false
			for (let p = 0; p < prefixes.length; ++p){
				let hiddenSeen = false
				for (let i = 0; i < counts[p]; ++i){
					let shown = commandButton(panel, prefixes[p] + i).visible
					verify(!(hiddenSeen && shown), prefixes[p] + i + " is not shown after a higher-priority one was hidden, at " + widths[w])
					if (!shown){
						hiddenSeen = true
						anyHidden = true
					}
				}
			}
			compare(moreButton(panel).visible, anyHidden, "The \"more\" button only when something is hidden, at " + widths[w])
		}
	}

	// A command shown later lays the whole panel out again, the "more" button included
	function test_commandShownLaterRelayoutsThePanel(){
		let groups = [group("L", 1, 2), group("R", 2, 2)]
		groups[1].subElements.push(command("Late", 1))
		groups[1].subElements[2].visible = false
		let panel = createPanel(groups)
		let left = naturalWidth(groupView(panel, 0))
		let right = naturalWidth(groupView(panel, 2))
		setWidth(panel, left + right + 3 * 16 + 4)
		verify(!moreButton(panel).visible, "All shown commands fit")

		let late = commandButton(panel, "Late")
		late.element.m_visible = true
		tryVerify(function(){ return moreButton(panel).visible }, 1000, "The command that does not fit goes behind the \"more\" button")
		verify(!late.visible)
	}

	// A layout pass leaves a button that stays shown alone: it does not blink
	function test_shownButtonsDoNotBlink(){
		let panel = createPanel([group("L", 1, 3), group("R", 2, 3)])
		let button = commandButton(panel, "L0")
		verify(button.visible)

		let changes = 0
		let counter = function(){ changes++ }
		button.visibleChanged.connect(counter)
		setWidth(panel, 1500)
		setWidth(panel, 1400)
		button.visibleChanged.disconnect(counter)

		compare(changes, 0, "The button stayed shown without a change")
	}

	function test_priorityAcrossGroups_data(){
		return [
			{"tag": "sides", "groups": [
				{"elementId": "L", "alignment": 1, "subElements": [command("E", 3), command("NNNNNNN", 2), command("RRRR", 1)]},
				{"elementId": "R", "alignment": 2, "subElements": [command("L", 3), command("SSSSSSSS", 2)]}]},
			{"tag": "sides and center", "groups": [
				{"elementId": "L", "alignment": 1, "subElements": [command("E", 3), command("NNNNNNN", 2), command("RRRR", 1)]},
				{"elementId": "C", "alignment": 4, "subElements": [command("CC", 2), command("CCCCCC", 0)]},
				{"elementId": "R", "alignment": 2, "subElements": [command("L", 3), command("SSSSSSSS", 1)]}]}
		]
	}

	/*
		At any width: no shown command has a lower priority than a hidden one, each stays in its own group,
		and no room is lost - every group whose next command has the highest hidden priority could not take it.
	*/
	function test_priorityAcrossGroups(data){
		let panel = createPanel(data.groups)
		let views = [groupView(panel, 0), groupView(panel, 1), groupView(panel, 2)]
		let total = naturalWidth(views[0]) + naturalWidth(views[1]) + naturalWidth(views[2])
		let allIds = []
		for (let g = 0; g < data.groups.length; ++g){
			for (let i = 0; i < data.groups[g].subElements.length; ++i){
				allIds.push(data.groups[g].subElements[i].elementId)
			}
		}

		for (let width = total + 200; width > 150; width -= 37){
			setWidth(panel, width)
			checkNoOverlap(panel, allIds)
			let reserved = moreButton(panel).visible ? moreButton(panel).width : 0
			let shown = [shownOf(views[0]), shownOf(views[1]), shownOf(views[2])]
			verify(panel.__fits(shown[0].width, shown[1].width, shown[2].width, reserved), "The shown commands fit at " + width)

			let topHidden = -1
			let lowestShown = 1000
			for (let g = 0; g < 3; ++g){
				if (shown[g].next){
					topHidden = Math.max(topHidden, shown[g].next.priority)
				}
				for (let i = 0; i < shown[g].count; ++i){
					lowestShown = Math.min(lowestShown, shown[g].sizes[i].priority)
				}
			}
			if (topHidden < 0){
				verify(!moreButton(panel).visible, "All shown at " + width)
				continue
			}
			verify(moreButton(panel).visible, "Something is hidden at " + width)
			verify(lowestShown >= topHidden, "Priority " + lowestShown + " is shown while " + topHidden + " is hidden, at " + width)

			for (let g = 0; g < 3; ++g){
				if (shown[g].next && shown[g].next.priority === topHidden){
					let trial = [shown[0].width, shown[1].width, shown[2].width]
					trial[g] = shown[g].grown
					verify(!panel.__fits(trial[0], trial[1], trial[2], reserved), "Group " + g + " could still take its next command at " + width)
				}
			}
		}
	}
}
