# JQML v4: Lisa runtime fixes

The runtime fixes live in [items.js](../../Tools/JQML/v4/src/items.js),
[engine.js](../../Tools/JQML/v4/src/engine.js), [host.js](../../Tools/JQML/v4/src/host.js)
and [scene.js](../../Tools/JQML/v4/src/scene.js). Recompile Lisa with the v4
compiler to include them in its generated JavaScript bundle.

## Object lifetime and notifications

- `object.Component.destruction.connect(handler)` is supported. Destruction is
  emitted once, before the object's children and property-owned objects are released.
- Built-in properties expose lazy `propertyChanged.connect(handler)` and
  `propertyChanged.disconnect(handler)` signals using the existing change-handler
  machinery. This includes `visibleChanged`, `heightChanged` and `widthChanged`.
- Connections to QML object methods are removed when the receiver is destroyed.
- Access to the attached object preserves the existing `Component` enum values.

## Nested table values

Nested objects and arrays in `TreeItemModel` delegate roles are exposed as
`TreeItemModel` values, matching `getData()`. The same conversion applies to
`model.role`, `modelData.role` and unqualified roles.
Existing QML objects are preserved. `ListModel` and JavaScript array roles
retain their previous representation.

## WebGL images

Image requests set `crossOrigin = "anonymous"` before assigning `src`, including
fallback requests. A cross-origin image must be served with a suitable
`Access-Control-Allow-Origin` header. Otherwise it fails with `Image.Error`
instead of supplying a tainted canvas to WebGL.

Network SVGs are fetched under the same CORS restrictions and decoded through a
data URL without changing their content. This avoids Chromium's tainting of
network SVGs containing `foreignObject`, including the unused Illustrator branch
in the AdminPanel icon. CORS alone does not prevent this SVG-specific failure.
Stale image requests do not replace a newer source.

A rasterized image is checked before submission to WebGL. A `SecurityError`
sets `Image.Error` and reports the source once; unrelated exceptions still
propagate. A broken image therefore does not abort rendering every subsequent frame.

## Initial bindings and validators

A declared property's binding is initialized when first read after construction,
even if its scheduled binding pass has not reached it yet. This ensures an
`onOkChanged` subscription handler can read a manager binding declared later in
the derived component. Already initialized bindings retain their normal update
order.

`RegularExpressionValidator` objects are instantiated rather than skipped, so
their ids can be assigned to editor validators. `TextInput.acceptableInput`
checks the complete text against the validator expression and follows changes
to the text and expression. Matching handles alternative branches and does
not retain the state of global or sticky JavaScript regexes.

## Document index updates

Changes to `ListModel.count` notify dependent bindings synchronously before the
mutating operation returns. In `DocumentService`, this updates `documentsCount`
and runs `onDocumentsCountChanged` to rebuild `documentIndexById` before
`documentAdded` is emitted. Previously, a newly added or loaded document could
not be found yet, causing `MultiDocWorkspaceView` to read `documentId` or
`viewComp` from `null`.

The regression covers append, insert, remove and clear without an intervening
frame, including a comparison with desktop Qt.

## Mouse input and icons

The browser host forwards native double-click events to the hit-tested QML
`MouseArea`, so `onDoubleClicked` handlers run on real user input. When an image
changes source, its previously loaded picture remains visible until the
replacement loads. This prevents state-dependent menu and tab icons from
disappearing during hover and tab selection. Replacement is atomic at the
presentation level: `Image.status` stays `Ready` while a previously loaded
picture is displayed. This also keeps parents bound to
`visible: image.status === Image.Ready` visible and preserves their width;
no workaround is needed in `TabPanelDecorator.qml`.

The first request (or a retry after an error) still reports `Loading`. A failed
replacement clears the old picture and reports `Error`; an empty source clears
it immediately and reports `Null`. Stale requests cannot replace a newer source.
While a retained picture is displayed, `progress` remains `1`, describing the
displayed image rather than the pending replacement. This deliberately avoids
exposing a browser-only asynchronous gap for already displayed icons.

## Segmented button sizes

When a Loader creates a SegmentedButton during a binding pass, completing the
outer segment decorators also creates their background patches. Content
measurement can then leave size readers pending after the normal restoration
passes. Before the final Binding synchronization, the engine drains these
readers using its existing synchronous subscriber queue. Otherwise ControlBase
can write its stale width back to the decorator and lose the measured size.

The regression loads the actual controls and StyleBase from the repository. It
checks all three segment widths, the combined row width, text changes, exclusive
selection and preservation of an explicitly assigned width.

Rebuild the gallery's JavaScript bundle after updating the runtime. A running
ControlsGallery server can cache the old bundle; restart it before reloading
the browser.

## Completion during binding evaluation

When a Loader creates a component from a binding update, its
`Component.onCompleted` handlers run in a script context, not in the outer
binding pass. Writes inside them notify dependent bindings and change handlers
synchronously before the next statement. Assignments replace existing bindings,
and script reads do not become dependencies of the interrupted binding.
The engine restores the outer evaluation context after completion, including
nested component creation.

For ControlsGallery's SplitView, `compl = true` now updates `ready` and runs
`onReadyChanged` (including `fillSizeModelDefault`) before the remainder of
`Component.onCompleted` reads `ready`. No QML workaround or delayed callback
is required. A regression loads the actual SplitView with three panes in both
orientations and both binding modes; the use-site completion handler sees
`ready === true` and three initialized model rows. A separate minimal case
compares the event order and immediate derived values with native Qt.
A browser run of the ControlsGallery SplitView source with diagnostic logging
produced `Filling size model with count: 3` before
`Component completed, splitView ready: true 192 792`, with three model rows,
five panes/splitters in the row, no runtime errors and no WebGL errors.

## Validation

Run the focused regressions from `Tools/JQML/v4`:

```text
node test/lisa-runtime.mjs
node test/segmented-button.mjs
node test/split-view.mjs
node test/incremental.mjs
node test/compiler.mjs
node test/paint.mjs
node test/animation.mjs
node test/editing.mjs
node test/desktop/run.mjs ListModel_007_synchronous_count_binding
node test/desktop/run.mjs Binding_020_completion_during_pass
```

The image regressions check request ordering, fallback loading, SVG preservation,
stale requests, failure status and source-specific taint diagnostics.
The runtime regression also checks native double-click dispatch and that a
replacement image keeps painting its previous picture until loading completes.
It also exercises the tab's `Image.Ready` visibility/width bindings across
selection changes and stale completions, plus failure, retry and source clearing.
A browser check using the unmodified repository `TabPanelDecorator.qml` and
SVG replacements delayed by 80 ms covered 12 selection changes and 120 rendered
frames: the icon was drawn in every frame, its slot stayed visible at width 16,
every replacement completed, and WebGL reported no errors.
A browser WebGL2 check additionally reproduced the original tainted-canvas
`SecurityError`, uploaded the CORS-enabled image successfully, and verified
`Image.Error` for a response without CORS permission.
The AdminPanel SVG was also reproduced as tainted before conversion and uploaded
successfully afterward, with its source preserved and visible pixels rendered.
