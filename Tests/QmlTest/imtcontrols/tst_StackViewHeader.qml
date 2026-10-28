import QtQuick 2.12
import QtTest 1.12
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
		// TestCase itself is invisible, the header needs a visible parent
		let header = createTemporaryObject(headerComp, testCase.parent)
		header.addHeader("list", first)
		header.addHeader("document", second)
		waitForRendering(header)
		return header
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
		waitForRendering(header)
		compare(header.implicitContentWidth, natural, "the natural width does not depend on the width")
		verify(rightmostEdge(header) <= header.width + 1, "the headers end at " + rightmostEdge(header) + " of " + header.width)
	}
}
