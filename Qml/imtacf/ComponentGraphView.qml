import QtQuick 2.0
import com.imtcore.imtqml 1.0

/**
	ComponentGraphView provides an interactive visualization of
	all QML ACF components registered in AcfRegistry.

	It displays components as rectangular nodes arranged in a grid
	and draws connection lines for all resolved references.

	Usage:
	\code
	ComponentGraphView {
		anchors.fill: parent
	}
	\endcode

	The view automatically refreshes whenever the registry changes.
	You can also call refresh() manually.
*/
Item {
	id: root

	/// Spacing between graph nodes (horizontal)
	property int nodeSpacingX: 200

	/// Spacing between graph nodes (vertical)
	property int nodeSpacingY: 120

	/// Width of each node box
	property int nodeWidth: 170

	/// Height of each node box
	property int nodeHeight: 90

	/// Background color
	property color backgroundColor: "#f5f5f5"

	/// Node fill color for active components
	property color activeNodeColor: "#e3f2fd"

	/// Node fill color for inactive components
	property color inactiveNodeColor: "#fce4ec"

	/// Node border color
	property color nodeBorderColor: "#78909c"

	/// Reference edge color
	property color referenceEdgeColor: "#1565c0"

	/// Factory edge color
	property color factoryEdgeColor: "#2e7d32"

	/// Node text color
	property color nodeTextColor: "#263238"

	/// Internal graph data
	property var __graphData: ({nodes: [], edges: []})

	/// Map from componentId -> {x, y} for node positions
	property var __nodePositions: ({})

	/**
		Refresh the graph from the current AcfRegistry state.
	*/
	function refresh() {
		__graphData = AcfRegistry.exportGraph();
		__computeLayout();
		canvas.requestPaint();
	}

	function __computeLayout() {
		var nodes = __graphData.nodes || [];
		var positions = {};
		var columns = Math.max(1, Math.floor(Math.sqrt(nodes.length)));

		for (var i = 0; i < nodes.length; ++i){
			var col = i % columns;
			var row = Math.floor(i / columns);
			positions[nodes[i].id] = {
				x: 30 + col * root.nodeSpacingX,
				y: 30 + row * root.nodeSpacingY
			};
		}

		__nodePositions = positions;
	}

	function __getNodeCenter(nodeId) {
		var pos = __nodePositions[nodeId];
		if (!pos) return {x: 0, y: 0};
		return {
			x: pos.x + root.nodeWidth / 2,
			y: pos.y + root.nodeHeight / 2
		};
	}

	Rectangle {
		anchors.fill: parent
		color: root.backgroundColor
	}

	Canvas {
		id: canvas
		anchors.fill: parent

		onPaint: {
			var ctx = getContext("2d");
			ctx.clearRect(0, 0, width, height);

			var nodes = root.__graphData.nodes || [];
			var edges = root.__graphData.edges || [];

			// Draw edges
			for (var e = 0; e < edges.length; ++e){
				var edge = edges[e];
				var fromCenter = root.__getNodeCenter(edge.from);
				var toCenter = root.__getNodeCenter(edge.to);

				if (toCenter.x === 0 && toCenter.y === 0) continue;

				ctx.beginPath();
				ctx.strokeStyle = edge.type === "factory"
					? root.factoryEdgeColor
					: root.referenceEdgeColor;
				ctx.lineWidth = edge.isResolved ? 2 : 1;

				if (!edge.isResolved){
					ctx.setLineDash([5, 5]);
				} else {
					ctx.setLineDash([]);
				}

				ctx.moveTo(fromCenter.x, fromCenter.y);
				ctx.lineTo(toCenter.x, toCenter.y);
				ctx.stroke();

				// Draw arrowhead
				var angle = Math.atan2(toCenter.y - fromCenter.y, toCenter.x - fromCenter.x);
				var arrowLen = 10;
				var ax = toCenter.x - Math.cos(angle) * (root.nodeWidth / 2 + 5);
				var ay = toCenter.y - Math.sin(angle) * (root.nodeHeight / 2 + 5);

				ctx.beginPath();
				ctx.setLineDash([]);
				ctx.moveTo(ax, ay);
				ctx.lineTo(ax - arrowLen * Math.cos(angle - 0.3), ay - arrowLen * Math.sin(angle - 0.3));
				ctx.moveTo(ax, ay);
				ctx.lineTo(ax - arrowLen * Math.cos(angle + 0.3), ay - arrowLen * Math.sin(angle + 0.3));
				ctx.stroke();

				// Edge label
				var midX = (fromCenter.x + toCenter.x) / 2;
				var midY = (fromCenter.y + toCenter.y) / 2;
				ctx.font = "9px sans-serif";
				ctx.fillStyle = ctx.strokeStyle;
				var label = edge.refId || edge.factoryId || "";
				ctx.fillText(label, midX + 3, midY - 3);
			}

			// Draw nodes
			for (var n = 0; n < nodes.length; ++n){
				var node = nodes[n];
				var pos = root.__nodePositions[node.id];
				if (!pos) continue;

				var x = pos.x;
				var y = pos.y;
				var w = root.nodeWidth;
				var h = root.nodeHeight;

				// Node background
				ctx.fillStyle = node.isActive ? root.activeNodeColor : root.inactiveNodeColor;
				ctx.strokeStyle = root.nodeBorderColor;
				ctx.lineWidth = 1.5;
				ctx.setLineDash([]);

				// Rounded rectangle
				var r = 6;
				ctx.beginPath();
				ctx.moveTo(x + r, y);
				ctx.lineTo(x + w - r, y);
				ctx.quadraticCurveTo(x + w, y, x + w, y + r);
				ctx.lineTo(x + w, y + h - r);
				ctx.quadraticCurveTo(x + w, y + h, x + w - r, y + h);
				ctx.lineTo(x + r, y + h);
				ctx.quadraticCurveTo(x, y + h, x, y + h - r);
				ctx.lineTo(x, y + r);
				ctx.quadraticCurveTo(x, y, x + r, y);
				ctx.closePath();
				ctx.fill();
				ctx.stroke();

				// Component ID (title)
				ctx.font = "bold 11px sans-serif";
				ctx.fillStyle = root.nodeTextColor;
				ctx.fillText(node.id, x + 8, y + 18);

				// Package ID
				ctx.font = "9px sans-serif";
				ctx.fillStyle = "#546e7a";
				if (node.packageId){
					ctx.fillText(node.packageId, x + 8, y + 32);
				}

				// Interfaces
				var ifaces = node.interfaces || [];
				if (ifaces.length > 0){
					ctx.font = "italic 9px sans-serif";
					ctx.fillStyle = "#1565c0";
					var ifaceText = ifaces.join(", ");
					// Truncate if too long
					if (ifaceText.length > 25){
						ifaceText = ifaceText.substring(0, 22) + "...";
					}
					ctx.fillText(ifaceText, x + 8, y + 46);
				}

				// Status indicator
				ctx.font = "8px sans-serif";
				ctx.fillStyle = node.isActive ? "#2e7d32" : "#c62828";
				ctx.fillText(node.isActive ? "● active" : "○ inactive", x + 8, y + h - 8);
			}
		}
	}

	// Auto-refresh when registry changes
	Connections {
		target: AcfRegistry

		function onComponentRegistered() {
			root.refresh();
		}

		function onComponentUnregistered() {
			root.refresh();
		}

		function onReferencesResolved() {
			root.refresh();
		}
	}

	Component.onCompleted: {
		root.refresh();
	}
}
