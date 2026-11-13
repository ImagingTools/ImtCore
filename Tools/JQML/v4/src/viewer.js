// The viewer window: a toolbar (open button, path, status, statistics) above the loaded document.
// Like the original viewer it is built from the same items as a QML document.

import { QmlError } from './lexer.js';
import { Item, MouseArea, Rectangle, Text } from './items.js';

const BUTTON_COLORS = { pressed: '#325aaa', hovered: '#5a82dc', normal: '#466ec8' };
const STATUS_ERROR = '#ff6e6e';
const STATUS_OK = '#78dc8c';
const MAX_PATH_CHARS = 60;
const STATS_INTERVAL = 0.5;

function make(Type, parent) {
	const item = new Type();
	parent.$adopt(item);
	return item;
}

function countItems(item) {
	let count = 1;
	for (const child of item.$children) count += countItems(child);
	return count;
}

function elide(path) {
	return path.length <= MAX_PATH_CHARS ? path : `...${path.slice(path.length - (MAX_PATH_CHARS - 3))}`;
}

export class Viewer {
	// onOpen: called when the user presses the open button.
	constructor(scene, engine, onOpen) {
		this.scene = scene;
		this.engine = engine;
		this.onOpen = onOpen;
		this.shownErrors = 0;
		this.statsStart = 0;
		this.statsFrames = 0;
		this.buttonColor = '';

		const root = scene.root;

		this.content = make(Item, root);

		const toolbar = make(Rectangle, root);
		toolbar.color = '#24242c';
		toolbar.height = 52;
		toolbar.z = 1;
		toolbar.anchors.left = root.left;
		toolbar.anchors.right = root.right;
		toolbar.anchors.top = root.top;

		this.content.anchors.left = root.left;
		this.content.anchors.right = root.right;
		this.content.anchors.bottom = root.bottom;
		this.content.anchors.top = toolbar.bottom;

		const button = make(Rectangle, toolbar);
		button.width = 200;
		button.height = 32;
		button.anchors.left = toolbar.left;
		button.anchors.leftMargin = 10;
		button.anchors.verticalCenter = toolbar.verticalCenter;
		this.button = button;

		const buttonText = make(Text, button);
		buttonText.text = 'Open QML...';
		buttonText.font.pixelSize = 16;
		buttonText.anchors.centerIn = button;

		this.area = make(MouseArea, button);
		this.area.anchors.fill = button;
		this.area.$connect('clicked', () => this.onOpen());

		const label = (color) => {
			const text = make(Text, toolbar);
			text.color = color;
			text.font.pixelSize = 12;
			return text;
		};

		this.pathLabel = label('#c8c8d2');
		this.pathLabel.anchors.left = button.right;
		this.pathLabel.anchors.leftMargin = 16;
		this.pathLabel.anchors.top = toolbar.top;
		this.pathLabel.anchors.topMargin = 10;

		this.statusLabel = label(STATUS_OK);
		this.statusLabel.anchors.left = button.right;
		this.statusLabel.anchors.leftMargin = 16;
		this.statusLabel.anchors.bottom = toolbar.bottom;
		this.statusLabel.anchors.bottomMargin = 10;

		this.statsLabel = label('#96aac8');
		this.statsLabel.anchors.right = toolbar.right;
		this.statsLabel.anchors.rightMargin = 12;
		this.statsLabel.anchors.verticalCenter = toolbar.verticalCenter;

		this.setButtonColor();
		scene.frameHandler = (seconds) => this.frame(seconds);
	}

	setButtonColor() {
		const color = this.area.$pressed ? BUTTON_COLORS.pressed : this.area.$containsMouse ? BUTTON_COLORS.hovered : BUTTON_COLORS.normal;
		if (color !== this.buttonColor) {
			this.buttonColor = color;
			this.button.color = color;
		}
	}

	setStatus(text, error) {
		this.statusLabel.text = text;
		this.statusLabel.color = error ? STATUS_ERROR : STATUS_OK;
		this.scene.skipNextCount = true;
	}

	// Called before every frame.
	frame(seconds) {
		const { engine, scene } = this;
		engine.setTime(seconds);
		if (engine.isAnimated) scene.root.$update();
		this.setButtonColor();

		if (engine.errors.length > this.shownErrors) {
			this.setStatus(engine.errors[this.shownErrors], true);
			this.shownErrors = engine.errors.length;
		}

		if (seconds - this.statsStart >= STATS_INTERVAL) {
			const elapsed = seconds - this.statsStart;
			const fps = scene.frames / elapsed;
			const times = `layout ${scene.layoutMs.toFixed(1)} ms | paint ${scene.paintMs.toFixed(1)} ms`;
			const text = fps >= 5 ? `${fps.toFixed(0)} fps | ${times}` : `static | ${times}`;
			this.statsStart = seconds;
			scene.frames = 0;
			if (text !== this.statsLabel.text) {
				this.statsLabel.text = text;
				scene.skipNextCount = true;
			}
		}
	}

	// Replaces the shown document with the main document of a loaded project.
	load(project) {
		this.engine.reset();
		this.shownErrors = 0;
		this.content.$clearChildren();
		this.pathLabel.text = elide(project.mainPath);
		try {
			const root = this.engine.load(project, this.content);
			this.setStatus(`OK: ${countItems(root)} items, ${this.engine.bindingCount} bindings`, false);
		} catch (error) {
			this.engine.reset();
			this.content.$clearChildren();
			this.showError(error);
		}
		this.shownErrors = this.engine.errors.length;
	}

	showError(error) {
		const message = error instanceof Error ? error.message : String(error);
		if (error instanceof QmlError && error.line > 0) {
			this.setStatus(`Error at ${error.line}:${error.column}: ${message}`, true);
		} else {
			this.setStatus(`Error: ${message}`, true);
		}
		console.error(message);
	}

	// Shows the path of a document that failed to load.
	showPath(path) {
		this.engine.reset();
		this.content.$clearChildren();
		this.pathLabel.text = elide(path);
	}
}
