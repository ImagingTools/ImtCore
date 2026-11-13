import QtQuick 2.12
import QtTest 1.12
import Acf 1.0
import imtcontrols 1.0

TestCase {
	id: testCase

	name: "StackViewHeader"
	when: windowShown

	Component {
		id: headerComp

		StackViewHeader {
			width: 1200
			height: 40
		}
	}

	function findTexts(item, result){
		if (item.text !== undefined && item.elide !== undefined && item.visible)
			result.push(item)
		for (let i = 0; i < item.children.length; i++)
			findTexts(item.children[i], result)
		return result
	}

	function rightmostEdge(header){
		let edge = 0
		let texts = findTexts(header, [])
		for (let i = 0; i < texts.length; i++){
			// The transparent measuring copy is not shown
			let shown = true
			for (let p = texts[i]; p && p !== header; p = p.parent){
				if (p.opacity === 0)
					shown = false
			}
			if (!shown)
				continue
			edge = Math.max(edge, texts[i].mapToItem(header, texts[i].width, 0).x)
		}
		return edge
	}

	function createHeader(first, second){
		return createHeaders([first, second])
	}

	function createHeaders(names){
		// TestCase itself is invisible, the header needs a visible parent
		let header = createTemporaryObject(headerComp, testCase.parent)
		for (let i = 0; i < names.length; i++)
			header.addHeader("h" + i, names[i])
		// the layout follows the names on the next turn of the event loop
		wait(50)
		return header
	}

	function isShown(item, header){
		for (let p = item; p && p !== header; p = p.parent){
			if (!p.visible || p.opacity === 0)
				return false
		}
		return true
	}

	// The texts the user sees, left to right
	function shownTexts(header){
		let texts = findTexts(header, []).filter(function(text){ return isShown(text, header) })
		texts.sort(function(a, b){ return a.mapToItem(header, 0, 0).x - b.mapToItem(header, 0, 0).x })
		return texts
	}

	function shownNames(header){
		return shownTexts(header).map(function(text){ return text.text })
	}

	Component {
		id: spyComp

		SignalSpy {
		}
	}

	function test_wideHeaderShowsTheWholeNames(){
		let header = createHeader("Organizations", "Test")
		verify(header.implicitContentWidth < header.width, "the names fit: " + header.implicitContentWidth)
		let texts = findTexts(header, [])
		for (let i = 0; i < texts.length; i++)
			compare(texts[i].width, texts[i].implicitWidth, texts[i].text + " is not elided")
		verify(rightmostEdge(header) <= header.width + 1)
	}

	function test_narrowHeaderStaysInside(){
		let header = createHeader("Organizations of the whole company", "A document with a rather long name")
		let natural = header.implicitContentWidth

		header.width = 400
		wait(50)
		compare(header.implicitContentWidth, natural, "the natural width does not depend on the width")
		verify(rightmostEdge(header) <= header.width + 1, "the headers end at " + rightmostEdge(header) + " of " + header.width)
	}

	// The current header reads as the title, the ones before it as the way back
	function test_currentHeaderStandsOut(){
		let header = createHeaders(["Organizations", "Acme Holding", "Members"])
		let texts = shownTexts(header)
		compare(shownNames(header), ["Organizations", "Acme Holding", "Members"])

		verify(texts[2].font.pixelSize > texts[0].font.pixelSize)
		verify(texts[2].font.bold)
		verify(!texts[0].font.bold)
		verify(!Qt.colorEqual(texts[0].color, texts[2].color), "the earlier headers are muted")
	}

	// The middle goes first: the first header and the last two stay whole
	function test_middleHeadersCollapse(){
		let header = createHeaders(["Organizations", "Northwest Regional Sales Division", "Cross Organization Grants", "Grant to Contoso Logistics"])
		header.width = header.implicitContentWidth - 40
		wait(50)

		compare(shownNames(header), ["Organizations", "…", "Cross Organization Grants", "Grant to Contoso Logistics"])
		let texts = shownTexts(header)
		for (let i = 0; i < texts.length; i++)
			compare(texts[i].width, texts[i].implicitWidth, texts[i].text + " is not elided")
		verify(rightmostEdge(header) <= header.width + 1)
	}

	// Too narrow even for that: the way back is the "..." and the back button
	function test_veryNarrowKeepsTheCurrentHeader(){
		let header = createHeaders(["Organizations", "Acme Holding", "Members"])
		header.width = 200
		wait(50)

		compare(shownNames(header), ["…", "Members"])
		verify(rightmostEdge(header) <= header.width + 1)
	}

	function test_widenedHeaderShowsEverythingAgain(){
		let header = createHeaders(["Organizations", "Acme Holding", "Members"])
		header.width = 200
		wait(50)
		header.width = 1200
		wait(50)

		compare(shownNames(header), ["Organizations", "Acme Holding", "Members"])
	}

	// The unsaved mark is beside the name, which keeps its place
	function test_dirtyMarkLeavesTheName(){
		let header = createHeaders(["Organizations", "Acme Holding"])
		let before = shownTexts(header)[1].mapToItem(header, 0, 0).x

		header.setHeaderDirty("h1", true)
		wait(50)

		verify(header.getHeaderDirty("h1"))
		compare(shownNames(header), ["Organizations", "Acme Holding"])
		compare(shownTexts(header)[1].mapToItem(header, 0, 0).x, before)
		let dots = []
		let collect = function(item){
			if (item.radius !== undefined && item.radius > 0 && item.width === item.height && item.width > 0 && isShown(item, header) && item.color && Qt.colorEqual(item.color, Style.linkColor))
				dots.push(item)
			for (let i = 0; i < item.children.length; i++)
				collect(item.children[i])
		}
		collect(header)
		compare(dots.length, 1, "one mark is shown")
	}

	function test_earlierHeaderIsClickedTheCurrentIsNot(){
		let header = createHeaders(["Organizations", "Acme Holding", "Members"])
		let spy = createTemporaryObject(spyComp, testCase, {"target": header, "signalName": "headerItemClicked"})
		let texts = shownTexts(header)

		mouseClick(texts[1])
		mouseClick(texts[2])

		compare(spy.count, 1)
		compare(spy.signalArguments[0][0], 1)
		compare(spy.signalArguments[0][1], "h1")
	}
}
