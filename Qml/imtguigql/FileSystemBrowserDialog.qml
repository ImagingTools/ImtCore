import QtQuick 2.12
import Acf 1.0
import imtcontrols 1.0
import imtguigql 1.0
import imtbaseFileSystemSdl 1.0


/**
    Dialog for browsing folders/files via GetFileSystemEntries.

    Windows-Explorer-style navigation: a single pane lists one directory at a
    time; double-clicking a folder navigates into it, "Up" goes to the parent.
    Only the current directory is ever loaded (one GetFileSystemEntries call per
    navigation — never a recursive sweep). Large directories are paged via a
    "Load more…" footer.

    Selection: single by default. With multiSelect the rows carry checkboxes and
    support Ctrl-click (toggle), Shift-click (range) and a select-all header box.
    On confirm both pathSelected(path) and pathsSelected(paths) are emitted.
*/

Dialog {
	id: root

	// A binding (not a one-shot assignment in Component.onCompleted) because the
	// dialog manager applies ModalDialogManager params - including pathKind and
	// titleText - only in Loader.onLoaded, which fires AFTER this item's own
	// Component.onCompleted already ran. A one-shot assignment would always see
	// the property defaults; this recomputes whenever pathKind/titleText change.
	title: root.computeTitle()
	width: 1000
	height: 800
	modal: true
	centered: true
	backgroundColor: Style.baseColor

	// Public contract (set by ServerPathPicker via ModalDialogManager params).
	property string startPath: ""
	// Bit-mask of Enums.pathKindFile / Enums.pathKindDir. Combine with | to
	// allow picking either (e.g. Enums.pathKindFile | Enums.pathKindDir).
	property int pathKind: Enums.pathKindDir
	property bool multiSelect: false
	property string titleText: ""
	property string clientIdHeader: ""
	property string serviceIdHeader: ""
	// Semicolon-separated file extensions to show (without dot, e.g. "exe;dll").
	// Empty = all files. Files-only; folders always listed for navigation. Sent
	// verbatim to the server, which filters before sort/pagination.
	property string extensionFilter: ""

	property string currentPath: ""
	// Whether a parent exists, kept apart from parentPath: an EMPTY parentPath is
	// a real address - the virtual drives root above "C:/" - so emptiness alone
	// can no longer mean "nowhere to go up to".
	property bool hasParent: false
	property string parentPath: ""
	// The virtual drives root ("This PC"), served only by a whole-file-system
	// provider. It has no real path, so Date/Size are meaningless there and the
	// columns switch to volume capacity instead.
	readonly property bool atDrivesRoot: root.currentPath === ""
	property string selectedPath: ""
	property var selectedPaths: []
	property bool isLoading: false
	property string errorText: ""
	property bool hasMore: false
	property int pageSize: 200
	property int loadedCount: 0
	// Size of the full filtered/sorted set on the server (before pagination),
	// for the "Load more" progress label. Requested every page for simplicity;
	// it is a cheap Int already computed server-side for hasMore.
	property int totalCount: 0

	// Server-side name filter (substring, case-insensitive). Cleared on navigation.
	// Sent as GetFileSystemEntriesInput.nameFilter; totalCount/hasMore reflect it.
	property string filterText: ""
	// "name" | "date" | "size" → SDL FileSystemSortBy Name/Date/Size.
	// At drives root Date/Size map to free/total on the server.
	property string sortColumn: "name"
	property bool sortAscending: true

	// Column widths (Name column takes the remaining space).
	property int checkColumnWidth: 32
	property int sizeColumnWidth: 110
	property int dateColumnWidth: 170
	property int filterFieldWidth: 300
	// Right gutter reserved for the scrollbar so columns never slide under it.
	property int scrollGutter: Style.marginS + Style.marginM

	// Reveal-on-open + range-anchor bookkeeping.
	// __anchorIndex is an index into entriesModel (server page order).
	property string __revealTarget: ""
	property int revealIndex: -1
	property int __anchorIndex: -1
	property bool __appending: false
	property bool __navigated: false
	// Path passed to the most recent browse() call, kept only so the "Retry"
	// affordance in the first-load-failure empty state can repeat it exactly.
	property string __lastAttemptedPath: ""
	// True while the name filter field has focus — suppresses Enter→activate so
	// typing a filter does not open the highlighted row.
	property bool filterFieldFocused: false
	// Set when the filter changes while a request is in flight; applied after load.
	property bool __pendingFilterRefresh: false
	// Set for a same-folder reload (sort / filter / refresh): keep the current
	// selection instead of resetting it, and re-focus a still-visible selected row.
	property bool __preserveSelection: false
	// True once a listing has loaded at least once. Distinguishes the very first
	// navigation (currentPath still "") from a genuine same-folder reload, which
	// also has currentPath "" at the drives root - only the latter preserves.
	property bool __hasLoaded: false

	signal pathSelected(string selectedPath)
	signal pathsSelected(var paths)

	// ─── Entry-type helpers ─────────────────────────────────────────────────

	// A drive is a container like a folder: navigable, no size of its own, and
	// pickable wherever a directory is. Every Dir branch must accept it too.
	function isDirectoryEntry(entryType){
		if (entryType === FileSystemEntryTypeEnum.s_dir)
			return true
		if (entryType === FileSystemEntryTypeEnum.s_symlink)
			return true
		if (entryType === FileSystemEntryTypeEnum.s_drive)
			return true
		return false
	}

	// Whether double-clicking a leaf confirms the dialog (folders navigate instead).
	function isConfirmableLeaf(entryType){
		if (root.isDirectoryEntry(entryType))
			return false
		return (root.pathKind & Enums.pathKindFile) !== 0
	}

	// Whether this entry type may become the confirmed answer at all (single
	// click, multi-select, "Select all"). A folder browsed into is always
	// navigable via double-click regardless of pathKind, but must only be
	// pickable as the FINAL result when Dir is actually allowed - and likewise
	// for files vs File. This is what keeps a file-only picker from letting a
	// folder be highlighted-then-confirmed, and vice versa.
	function isSelectableEntry(entryType){
		if (root.isDirectoryEntry(entryType))
			return (root.pathKind & Enums.pathKindDir) !== 0
		return (root.pathKind & Enums.pathKindFile) !== 0
	}

	// In directory-only pick mode plain files are hidden (only navigable entries).
	function isListableEntry(entryType){
		if ((root.pathKind & Enums.pathKindFile) !== 0)
			return true
		return root.isDirectoryEntry(entryType)
	}

	function iconNameFor(entryType){
		if (entryType === FileSystemEntryTypeEnum.s_dir)
			return "Icons/FolderClosed"
		if (entryType === FileSystemEntryTypeEnum.s_symlink)
			return "Icons/Link"
		if (entryType === FileSystemEntryTypeEnum.s_drive)
			return "Icons/LogicalUnit"
		return "Icons/Assignment"
	}

	// Plain byte formatter. Goes up to TB because volume capacities live there -
	// a 2 TB drive must not read as "2048.0 GB".
	function formatBytes(byteValue){
		var size = Number(byteValue)
		if (isNaN(size))
			return ""
		if (size < 0)
			return ""
		if (size < 1024)
			return size + " B"
		if (size < 1024 * 1024)
			return (size / 1024).toFixed(1) + " KB"
		if (size < 1024 * 1024 * 1024)
			return (size / (1024 * 1024)).toFixed(1) + " MB"
		if (size < 1024 * 1024 * 1024 * 1024)
			return (size / (1024 * 1024 * 1024)).toFixed(1) + " GB"
		return (size / (1024 * 1024 * 1024 * 1024)).toFixed(1) + " TB"
	}

	// Type-aware wrapper: containers have no size of their own.
	function formatSize(sizeValue, entryType){
		if (root.isDirectoryEntry(entryType))
			return ""
		return root.formatBytes(sizeValue)
	}

	function formatDate(isoDate){
		if (!isoDate)
			return ""

		var dt = new Date(isoDate)
		if (isNaN(dt.getTime()))
			return isoDate

		return dt.toLocaleDateString(Qt.locale(), "dd.MM.yyyy") + " " + dt.toLocaleTimeString(Qt.locale(), "HH:mm")
	}

	function resolveHeaders(){
		var headers = {}
		if (root.clientIdHeader !== "")
			headers["clientid"] = root.clientIdHeader
		if (root.serviceIdHeader !== "")
			headers["serviceid"] = root.serviceIdHeader
		return headers
	}

	// Normalize separators; browse/reveal compare against server forward-slash paths.
	function normalizePath(path){
		return path ? String(path).replace(/\\/g, "/") : ""
	}

	// Parent directory of a path, keeping drive roots valid ("C:/foo" -> "C:/").
	function parentDirOf(path){
		var norm = root.normalizePath(path)
		if (norm === "")
			return ""
		var idx = norm.lastIndexOf("/")
		if (idx < 0)
			return ""
		var parentStr = norm.substring(0, idx)
		if (/^[A-Za-z]:$/.test(parentStr))
			parentStr += "/"
		return parentStr
	}

	// Short display name of the current folder, for the filter placeholder
	// ("Search Documents" - Explorer-style, not the full path).
	function currentFolderName(){
		if (root.atDrivesRoot)
			return qsTr("This PC")
		var norm = root.normalizePath(root.currentPath)
		var trimmed = (norm.length > 1 && norm.endsWith("/")) ? norm.slice(0, -1) : norm
		var idx = trimmed.lastIndexOf("/")
		var name = idx >= 0 ? trimmed.substring(idx + 1) : trimmed
		return name !== "" ? name : trimmed
	}

	// ─── Selection ──────────────────────────────────────────────────────────

	function isPathSelected(path){
		return root.selectedPaths.indexOf(path) >= 0
	}

	// Row for a selected path, or null if it is not on the current page (e.g.
	// the dir-only "current folder itself" self-selection, which is not a row
	// in entriesModel, or a revealed path that landed off-page).
	// Search the full loaded set so a filtered-out selection still validates.
	function findEntryByPath(path){
		for (var i = 0; i < entriesModel.count; i++){
			if (entriesModel.get(i).path === path)
				return entriesModel.get(i)
		}
		return null
	}

	function findDisplayIndexByPath(path){
		for (var i = 0; i < entriesModel.count; i++){
			if (entriesModel.get(i).path === path)
				return i
		}
		return -1
	}

	// Whether every currently selected path is of a kind pathKind actually
	// allows. A path not found on the current page (current-folder self-select,
	// an off-page reveal) is trusted, since it was set by code that already
	// established it is a valid choice - only paths we can verify here are
	// checked, so a wrong-kind row can never sneak past via click/multi-select.
	function selectionIsValid(){
		if (root.selectedPaths.length <= 0)
			return false
		for (var i = 0; i < root.selectedPaths.length; i++){
			var entry = root.findEntryByPath(root.selectedPaths[i])
			if (entry && !root.isSelectableEntry(entry.entryType))
				return false
		}
		return true
	}

	function setSingleSelection(path){
		root.selectedPath = path
		root.selectedPaths = (path === "" ? [] : [path])
	}

	function toggleSelection(path){
		var arr = root.selectedPaths.slice()
		var i = arr.indexOf(path)
		if (i >= 0)
			arr.splice(i, 1)
		else
			arr.push(path)
		root.selectedPaths = arr
		root.selectedPath = arr.length > 0 ? arr[arr.length - 1] : ""
	}

	// Skips wrong-kind rows (e.g. folders in a file-only picker) so a Shift-click
	// range spanning mixed content only ever sweeps in selectable entries -
	// consistent with their checkboxes, which show disabled for those rows.
	// Indices are entriesModel (visible) rows.
	function selectRange(fromIndex, toIndex){
		var lo = Math.min(fromIndex, toIndex)
		var hi = Math.max(fromIndex, toIndex)
		var set = {}
		var i
		for (i = 0; i < root.selectedPaths.length; i++)
			set[root.selectedPaths[i]] = true
		for (i = lo; i <= hi && i < entriesModel.count; i++){
			var rangeEntry = entriesModel.get(i)
			if (root.isSelectableEntry(rangeEntry.entryType))
				set[rangeEntry.path] = true
		}
		root.selectedPaths = Object.keys(set)
		if (hi >= 0 && hi < entriesModel.count){
			var hiEntry = entriesModel.get(hi)
			if (root.isSelectableEntry(hiEntry.entryType))
				root.selectedPath = hiEntry.path
		}
	}

	function onRowClicked(index, path, modifiers){
		var ctrl = (modifiers & Qt.ControlModifier) !== 0
		var shift = (modifiers & Qt.ShiftModifier) !== 0

		if (!root.multiSelect){
			// Any row can be highlighted here, wrong kind included - Explorer does
			// the same, and "Select" simply stays disabled until a valid row is
			// chosen (see selectionIsValid()/updateOkEnabled()).
			if (ctrl && root.isPathSelected(path))
				root.setSingleSelection("")
			else
				root.setSingleSelection(path)
			root.__anchorIndex = index
			return
		}

		// Multi-select: the row's checkbox IS the selection state, so a
		// wrong-kind row (checkbox shown disabled) cannot be toggled into it.
		if (index < 0 || index >= entriesModel.count)
			return
		var item = entriesModel.get(index)
		if (!root.isSelectableEntry(item.entryType))
			return

		if (shift && root.__anchorIndex >= 0){
			root.selectRange(root.__anchorIndex, index)
			return
		}

		root.toggleSelection(path)
		root.__anchorIndex = index
	}

	// "All"/"any" only consider selectable rows currently visible (entriesModel),
	// so "Select all" with a name filter applies to the filtered view only.
	function pageAllSelected(){
		var any = false
		for (var i = 0; i < entriesModel.count; i++){
			var entry = entriesModel.get(i)
			if (!root.isSelectableEntry(entry.entryType))
				continue
			any = true
			if (!root.isPathSelected(entry.path))
				return false
		}
		return any
	}

	function pageAnySelected(){
		for (var i = 0; i < entriesModel.count; i++){
			var entry = entriesModel.get(i)
			if (root.isSelectableEntry(entry.entryType) && root.isPathSelected(entry.path))
				return true
		}
		return false
	}

	function headerCheckState(){
		if (root.pageAllSelected())
			return Qt.Checked
		if (root.pageAnySelected())
			return Qt.PartiallyChecked
		return Qt.Unchecked
	}

	function toggleSelectAll(){
		if (root.pageAllSelected()){
			root.setSingleSelection("")
			return
		}
		var arr = []
		for (var i = 0; i < entriesModel.count; i++){
			var entry = entriesModel.get(i)
			if (root.isSelectableEntry(entry.entryType))
				arr.push(entry.path)
		}
		root.selectedPaths = arr
		root.selectedPath = arr.length > 0 ? arr[arr.length - 1] : ""
	}

	// ─── Filter / sort (server-side via GetFileSystemEntriesInput) ───────────

	function sortMark(columnId){
		if (root.sortColumn !== columnId)
			return ""
		return root.sortAscending ? " \u25B2" : " \u25BC"
	}

	function sortByToSdl(){
		if (root.sortColumn === "date")
			return "Date"
		if (root.sortColumn === "size")
			return "Size"
		return "Name"
	}

	function toggleSort(columnId){
		if (root.isLoading)
			return
		if (root.sortColumn === columnId){
			root.sortAscending = !root.sortAscending
		}
		else{
			root.sortColumn = columnId
			// Name: A→Z first. Date/size: descending first (newest / largest).
			root.sortAscending = (columnId === "name")
		}
		// Re-fetch page 0 with the new sort (server applies before pagination).
		root.browse(root.currentPath, false)
	}

	function applyFilterFromField(text){
		var next = text !== undefined && text !== null ? String(text) : ""
		if (root.filterText === next)
			return
		root.filterText = next
		if (root.isLoading){
			// Re-query once the in-flight listing finishes so the filter is not lost.
			root.__pendingFilterRefresh = true
			return
		}
		root.browse(root.currentPath, false)
	}

	function fillEntriesInput(path, append){
		getEntriesInput.m_path = path !== undefined && path !== null ? path : ""
		getEntriesInput.m_offset = append ? root.loadedCount : 0
		getEntriesInput.m_limit = root.pageSize
		// Assign unconditionally: gating the clear behind hasNameFilter()/
		// removeNameFilter() is unreliable in the web build (those methods may be
		// absent), which left the previous filter value stuck in m_nameFilter and
		// re-sent on the wire after the field was cleared. The server treats an
		// empty string as "no filter" (ApplyNameFilter checks isEmpty), so sending
		// "" is correct and unambiguous.
		getEntriesInput.m_nameFilter = root.filterText
		// Caller-provided extension whitelist (files only); "" = all. Same
		// unconditional-assign reasoning as m_nameFilter above.
		getEntriesInput.m_extensionFilter = root.extensionFilter
		getEntriesInput.m_sortBy = root.sortByToSdl()
		getEntriesInput.m_sortAscending = root.sortAscending
	}

	// ─── Loading ────────────────────────────────────────────────────────────

	// Loaded page (server filter/sort + client append for "Load more").
	ListModel {
		id: entriesModel
	}

	GetFileSystemEntriesInput {
		id: getEntriesInput
	}

	GqlSdlRequestSender {
		id: requestSender

		gqlCommandId: ImtbaseFileSystemSdlCommandIds.s_getFileSystemEntries
		requestType: 0

		sdlObjectComp: Component {
			GetFileSystemEntriesPayload {
				onFinished: {
					root.applyPayload(this)
				}
			}
		}

		function getHeaders(){
			return root.resolveHeaders()
		}

		function onError(message, type){
			root.onLoadFailed(message)
		}

		function createQueryParams(query){
			var gqlObject = Gql.GqlObject("input")
			gqlObject.fromObject(getEntriesInput)
			query.AddParam(gqlObject)

			query.AddField(Gql.GqlObject("path"))
			query.AddField(Gql.GqlObject("parentPath"))
			query.AddField(Gql.GqlObject("totalCount"))
			query.AddField(Gql.GqlObject("hasMore"))

			var entriesFields = Gql.GqlObject("entries")
			entriesFields.InsertField("name")
			entriesFields.InsertField("path")
			entriesFields.InsertField("entryType")
			entriesFields.InsertField("size")
			entriesFields.InsertField("lastModified")
			entriesFields.InsertField("totalBytes")
			entriesFields.InsertField("freeBytes")
			query.AddField(entriesFields)
		}
	}

	function browse(path, append){
		if (root.isLoading)
			return

		root.__appending = append === true
		// Fresh navigation into a different folder: clear the name filter.
		// Sort preference is kept across folders (Explorer-like).
		if (!root.__appending){
			var nextPath = path !== undefined && path !== null ? path : ""
			if (nextPath !== root.currentPath && nextPath !== root.__lastAttemptedPath)
				root.filterText = ""
			// Same-folder, non-append reload (sort / filter / refresh) keeps the
			// current selection; a reveal-on-open/browseUp target takes precedence,
			// and the very first navigation (nothing loaded yet) never preserves.
			root.__preserveSelection = root.__hasLoaded
				&& (nextPath === root.currentPath) && (root.__revealTarget === "")
		}

		root.isLoading = true
		root.errorText = ""
		root.__lastAttemptedPath = path !== undefined && path !== null ? path : ""

		root.fillEntriesInput(root.__lastAttemptedPath, root.__appending)
		requestSender.send()
	}

	// A failure here means a genuine problem opening this location (access
	// denied, folder vanished, agent unreachable, ...) - a merely empty folder
	// is a normal success with no entries, handled by applyPayload instead.
	// Stay exactly where we were, like Explorer, which reports the error and
	// does not navigate into a folder it could not open - so Up/Refresh and
	// the current listing (if any) are never disturbed by a failed attempt.
	function onLoadFailed(message){
		root.isLoading = false
		root.__appending = false
		root.__preserveSelection = false
		root.errorText = message
	}

	// Repeats the navigation that just failed (offered only while nothing has
	// ever loaded, i.e. even the initial folder could not be opened).
	function retryLastAttempt(){
		root.browse(root.__lastAttemptedPath, false)
	}

	function loadMore(){
		if (root.isLoading || !root.hasMore)
			return
		root.browse(root.currentPath, true)
	}

	// Going up keeps the folder we are leaving selected, so Enter-into / Backspace-out
	// returns the cursor to where it was (Explorer behaviour).
	function browseUp(){
		// atDrivesRoot is a second, independent guard alongside hasParent: the
		// server always resolves the virtual drives root to an empty currentPath,
		// so this catches it even if hasParent's presence-tracking (has_value on
		// an omitted parentPath) does not survive the web build faithfully.
		if (root.isLoading || !root.hasParent || root.atDrivesRoot)
			return
		root.__revealTarget = root.normalizePath(root.currentPath)
		root.browse(root.parentPath, false)
	}

	// hasParentPath() reads TNullableValue::HasValue() on the field that
	// ReadFromJsonObject() only ever sets when the JSON key was actually
	// present - so it correctly reflects presence, not m_parentPath's value.
	// (fromObject() also always builds a fresh C++ object per response, so
	// there is no stale-instance reuse to worry about either.)
	function payloadHasParentPath(payload){
		return payload && typeof payload.hasParentPath === "function" && payload.hasParentPath()
	}

	function applyPayload(payload){
		root.isLoading = false
		root.errorText = ""
		root.__hasLoaded = true

		var reveal = root.__revealTarget
		var freshLoad = !root.__appending

		root.currentPath = payload.m_path
		// Presence, not emptiness: an empty parentPath still addresses the virtual
		// drives root, so hasParent is what decides whether Up is available.
		// At the drives root the server omits parentPath entirely → Up disabled.
		var parentPresent = root.payloadHasParentPath(payload)
		root.hasParent = parentPresent
		root.parentPath = parentPresent ? payload.m_parentPath : ""
		root.hasMore = payload.hasHasMore() ? payload.m_hasMore : false
		root.totalCount = payload.hasTotalCount() ? payload.m_totalCount : 0

		// Replace the listing only once the response arrives, so a failed
		// navigation leaves the previous directory visible.
		if (freshLoad){
			entriesModel.clear()
			root.loadedCount = 0
			root.__anchorIndex = -1
		}

		if (payload.hasEntries() && payload.m_entries){
			var count = payload.m_entries.getItemsCount()
			for (var i = 0; i < count; i++){
				var entryType = payload.m_entries.getData("m_entryType", i)
				if (!root.isListableEntry(entryType))
					continue
				var name = payload.m_entries.getData("m_name", i)
				var path = payload.m_entries.getData("m_path", i)
				var sizeValue = payload.m_entries.getData("m_size", i)
				var lastModified = payload.m_entries.getData("m_lastModified", i)
				// Drive-only; absent for every other entry kind.
				var totalBytes = payload.m_entries.getData("m_totalBytes", i)
				var freeBytes = payload.m_entries.getData("m_freeBytes", i)

				entriesModel.append({
					name: name,
					path: path,
					entryType: entryType,
					isDir: root.isDirectoryEntry(entryType),
					iconName: root.iconNameFor(entryType),
					sizeText: root.formatSize(sizeValue, entryType),
					dateText: root.formatDate(lastModified),
					totalText: root.formatBytes(totalBytes),
					freeText: root.formatBytes(freeBytes)
				})
			}
			// Advance the page offset by the raw server count (not the filtered
			// view) so the next page starts after the full server slice.
			root.loadedCount += count
		}

		if (freshLoad)
			root.applySelectionAfterLoad(reveal)

		root.__appending = false

		if (root.__pendingFilterRefresh){
			root.__pendingFilterRefresh = false
			root.browse(root.currentPath, false)
		}
	}

	function applySelectionAfterLoad(reveal){
		root.revealIndex = -1

		if (root.__preserveSelection){
			root.__preserveSelection = false
			root.refocusSelection()
			return
		}

		if (reveal !== ""){
			// Reveal a pre-selected path: highlight and scroll it into view.
			root.__revealTarget = ""
			var revealIdx = root.findDisplayIndexByPath(reveal)
			if (revealIdx >= 0){
				root.setSingleSelection(entriesModel.get(revealIdx).path)
				root.__anchorIndex = revealIdx
				root.revealIndex = revealIdx
				return
			}
			// Not on this page (or filtered out) — keep it as the selection anyway.
			root.setSingleSelection(reveal)
			return
		}

		// Default selection on a plain navigation: current folder in pure
		// directory-pick mode (so "Select" returns it without an extra click -
		// there is nothing else selectable in that mode). Any mode that also
		// allows files leaves the selection empty; rows remain individually
		// clickable regardless of kind.
		if (root.pathKind === Enums.pathKindDir)
			root.setSingleSelection(root.currentPath)
		else
			root.setSingleSelection("")
	}

	// Same-folder reload (sort / filter / refresh): the selection values are
	// untouched by the reload, so only the visible cursor needs restoring. Move
	// the anchor onto a still-visible selected row and scroll it into view; a
	// selection filtered off the page keeps its value but has nothing to focus.
	function refocusSelection(){
		var idx = root.selectedPath !== "" ? root.findDisplayIndexByPath(root.selectedPath) : -1
		for (var i = 0; i < root.selectedPaths.length && idx < 0; i++)
			idx = root.findDisplayIndexByPath(root.selectedPaths[i])
		root.__anchorIndex = idx
		if (idx >= 0){
			root.revealIndex = -1
			root.revealIndex = idx
		}
	}

	function activateEntry(index){
		if (index < 0 || index >= entriesModel.count)
			return
		var item = entriesModel.get(index)
		if (root.isDirectoryEntry(item.entryType)){
			root.browse(item.path, false)
			return
		}
		if (root.isConfirmableLeaf(item.entryType)){
			root.setSingleSelection(item.path)
			root.accepted()
		}
	}

	function moveSelectionTo(index){
		if (entriesModel.count <= 0)
			return
		var idx = index
		if (idx < 0)
			idx = 0
		if (idx >= entriesModel.count)
			idx = entriesModel.count - 1
		root.__anchorIndex = idx
		root.setSingleSelection(entriesModel.get(idx).path)
		root.revealIndex = -1
		root.revealIndex = idx
	}

	function moveSelection(delta){
		var start = root.__anchorIndex < 0 ? (delta > 0 ? -1 : 0) : root.__anchorIndex
		root.moveSelectionTo(start + delta)
	}

	// Return/Enter reach this from two independent paths (Keys.onPressed below
	// AND the ApplicationShortcut items further down, kept so a Button that
	// stole focus - or a competing WindowShortcut elsewhere - cannot swallow
	// Explorer-style Enter navigation). Qt/QML can deliver a single physical
	// keypress to both a focus-item's Keys.onPressed and an ApplicationShortcut,
	// so guard against running this twice for the same press: a second, wrongly
	// duplicated accepted() would fire finished(ok) twice, and the dialog
	// manager's closeDialog() removes "whatever is now last" - the second call
	// could close an unrelated dialog stacked underneath this one.
	property bool __activating: false

	Timer {
		id: activateGuardTimer
		interval: 0
		repeat: false
		onTriggered: root.__activating = false
	}

	function activateCurrent(){
		if (root.__activating)
			return
		root.__activating = true
		activateGuardTimer.restart()

		if (root.filterFieldFocused)
			return
		if (root.__anchorIndex >= 0 && root.__anchorIndex < entriesModel.count){
			root.activateEntry(root.__anchorIndex)
			return
		}
		// No row focus: confirm only when the current selection is a valid pick
		// (e.g. dir-only mode with the current folder pre-selected). Do not call
		// accepted() for wrong-kind highlights — that would close via Dialog's
		// onAccepted → finished(ok) even when Select is disabled.
		if (root.selectionIsValid())
			root.accepted()
	}

	// Keyboard model mirrors BasicTreeView: arrows move, Enter opens/confirms,
	// Backspace goes up, Home/End jump, Space toggles in multi-select.
	function handleKey(event){
		if (root.filterFieldFocused){
			// Leave typing keys to the filter field; only Backspace for "up" is
			// suppressed while the field is empty (field uses Backspace itself).
			return false
		}
		if (event.key === Qt.Key_Down){
			root.moveSelection(1)
			return true
		}
		if (event.key === Qt.Key_Up){
			root.moveSelection(-1)
			return true
		}
		if (event.key === Qt.Key_Return || event.key === Qt.Key_Enter){
			root.activateCurrent()
			return true
		}
		if (event.key === Qt.Key_Backspace){
			root.browseUp()
			return true
		}
		if (event.key === Qt.Key_Home){
			root.moveSelectionTo(0)
			return true
		}
		if (event.key === Qt.Key_End){
			root.moveSelectionTo(entriesModel.count - 1)
			return true
		}
		if (event.key === Qt.Key_Space && root.multiSelect){
			if (root.__anchorIndex >= 0 && root.__anchorIndex < entriesModel.count)
				root.toggleSelection(entriesModel.get(root.__anchorIndex).path)
			return true
		}
		return false
	}

	function selectionKindHint(){
		if (root.pathKind === Enums.pathKindDir)
			return qsTr("A file is selected - choose a folder instead")
		if (root.pathKind === Enums.pathKindFile)
			return qsTr("A folder is selected - choose a file instead")
		return qsTr("Selection is not allowed here")
	}

	// Explorer-style "N items" for the whole filtered/sorted folder (not just the
	// page loaded so far - loaded-vs-total is a paging detail nobody needs to see).
	function itemCountLabel(){
		return root.totalCount === 1 ? qsTr("1 item") : qsTr("%1 items").arg(root.totalCount)
	}

	// Kept apart from statusLabel() - a dedicated counter, not folded into
	// whatever the main status text happens to say (path / hint / error / ...).
	function selectionCountLabel(){
		var n = root.selectedPaths.length
		if (n <= 0)
			return ""
		return n === 1 ? qsTr("1 selected") : qsTr("%1 selected").arg(n)
	}

	function statusLabel(){
		if (root.errorText !== "")
			return root.errorText
		if (root.isLoading)
			return qsTr("Loading…")
		if (root.filterText !== "" && entriesModel.count === 0)
			return qsTr("No items match \"%1\"").arg(root.filterText)
		if (root.selectedPaths.length > 0 && !root.selectionIsValid())
			return root.selectionKindHint()
		if (root.multiSelect && root.selectedPaths.length > 1)
			return qsTr("%1 items selected").arg(root.selectedPaths.length)
		if (root.selectedPath !== "")
			return root.selectedPath
		return qsTr("Nothing selected")
	}

	function updateOkEnabled(){
		root.setButtonEnabled(Enums.ok, root.selectionIsValid())
	}

	function computeTitle(){
		if (root.titleText !== "")
			return root.titleText
		if (root.pathKind === Enums.pathKindDir)
			return qsTr("Select Folder")
		if (root.pathKind === Enums.pathKindFile)
			return qsTr("Select File")
		return qsTr("Select File or Folder")
	}

	onSelectedPathsChanged: root.updateOkEnabled()

	// ─── Dialog lifecycle ───────────────────────────────────────────────────

	Component.onCompleted: {
		addButton(Enums.ok, qsTr("Select"), true)
		addButton(Enums.cancel, qsTr("Cancel"), true)

		root.updateOkEnabled()
	}

	function navigateInitial(){
		if (root.__navigated)
			return
		root.__navigated = true

		if (root.startPath !== ""){
			// Open inside the folder that holds the current value and reveal it.
			root.__revealTarget = root.normalizePath(root.startPath)
			var parentDir = root.parentDirOf(root.startPath)
			root.browse(parentDir, false)
		}
		else{
			root.browse("", false)
		}
	}

	// DialogManagerView.onLoaded applies every ModalDialogManager param (startPath,
	// clientIdHeader, pathKind, ...) in a single synchronous loop and only calls
	// started() once that loop has fully completed - so started() is the one safe
	// place to kick off navigation. An earlier version also triggered navigation
	// from onStartPathChanged to "not miss" a param assigned before started() -
	// but that fires mid-loop, before later params (e.g. clientIdHeader) are
	// applied, and sent the very first GetFileSystemEntries request with missing
	// headers and a wrong path whenever startPath happened to be assigned before
	// them. started() alone is both necessary and sufficient.
	onStarted: root.navigateInitial()

	// Enter failed while Backspace worked because other window-level Shortcuts
	// bind sequence "Return" (TopologyPage goInside, always enabled). Those win
	// or race with a plain WindowShortcut on this dialog; Backspace has no such
	// competitor. Fix:
	// 1) ApplicationShortcut for Return/Enter — active while this dialog lives,
	//    independent of child Button focus inside the decorator FocusScope.
	// 2) Keys.BeforeItem — arrows/space still work; also catches Enter when
	//    focus is inside this FocusScope so Buttons do not treat it as click.
	// TopologyPage Return is disabled while ModalDialogManager.count > 0.
	// Keys.priority: Keys.BeforeItem

	Keys.onPressed: {
		if (root.handleKey(event))
			event.accepted = true
	}

	Shortcut {
		sequence: "Return"
		context: Qt.ApplicationShortcut
		enabled: !root.filterFieldFocused
		onActivated: root.activateCurrent()
	}

	Shortcut {
		sequence: "Enter"
		context: Qt.ApplicationShortcut
		enabled: !root.filterFieldFocused
		onActivated: root.activateCurrent()
	}

	Shortcut {
		sequence: "Backspace"
		// Without this guard, deleting a character in the filter field also
		// navigates the dialog up a folder - this Shortcut fires independently
		// of handleKey()'s own filterFieldFocused check (that one only covers
		// Keys.onPressed, not this separate window-level accelerator).
		enabled: !root.filterFieldFocused
		onActivated: root.browseUp()
	}

	Shortcut {
		sequence: "Ctrl+F"
		context: Qt.ApplicationShortcut
		onActivated: filterField.forceActiveFocus()
	}

	onFinished: {
		if (buttonId == Enums.ok){
			var paths = root.selectedPaths.slice()
			if (paths.length === 0 && root.selectedPath !== "")
				paths = [root.selectedPath]
			root.pathsSelected(paths)
			root.pathSelected(paths.length > 0 ? paths[0] : root.selectedPath)
		}
	}

	onLocalizationChanged: {
		setButtonName(Enums.ok, qsTr("Select"))
		setButtonName(Enums.cancel, qsTr("Cancel"))
	}

	contentComp: Component {
		Item {
			id: contentRoot

			width: root.width
			height: root.height - Style.sizeHintXXS

			// Scroll a revealed row into view once the model is populated.
			Connections {
				target: root

				function onRevealIndexChanged(){
					if (root.revealIndex >= 0)
						entriesView.positionViewAtIndex(root.revealIndex, ListView.Center)
				}
			}

			// ─── Toolbar ────────────────────────────────────────────────────
			Item {
				id: toolbar

				anchors.top: parent.top
				anchors.left: parent.left
				anchors.right: parent.right
				anchors.leftMargin: Style.marginL
				anchors.rightMargin: Style.marginL
				anchors.topMargin: Style.marginM
				height: Style.controlHeightM

				Row {
					id: navButtons
					anchors.left: parent.left
					anchors.verticalCenter: parent.verticalCenter
					spacing: Style.spacingXS

					Button {
						id: upButton
						widthFromDecorator: true
						// atDrivesRoot backstops hasParent - see browseUp()'s comment.
						enabled: root.hasParent && !root.isLoading && !root.atDrivesRoot
						iconSource: "../../../" + Style.getIconPath("Icons/Up", Icon.State.On,
							enabled ? Icon.Mode.Normal : Icon.Mode.Disabled)
						decorator: Component {
							ButtonDecorator {
								color: parent.hovered ? Style.buttonHoverColor : "transparent"
								radius: Style.radiusS
								border.width: 0
							}
						}
						onClicked: {
							root.browseUp()
							root.forceActiveFocus()
						}
					}

					Button {
						id: refreshButton
						widthFromDecorator: true
						enabled: !root.isLoading
						iconSource: "../../../" + Style.getIconPath("Icons/AutoUpdate", Icon.State.On,
							enabled ? Icon.Mode.Normal : Icon.Mode.Disabled)
						decorator: Component {
							ButtonDecorator {
								color: parent.hovered ? Style.buttonHoverColor : "transparent"
								radius: Style.radiusS
								border.width: 0
							}
						}
						onClicked: {
							root.browse(root.currentPath, false)
							root.forceActiveFocus()
						}
					}
				}

				Rectangle {
					anchors.left: navButtons.right
					anchors.leftMargin: Style.marginM
					anchors.right: filterField.left
					anchors.rightMargin: Style.marginM
					anchors.verticalCenter: parent.verticalCenter
					height: parent.height
					radius: height / 2
					color: Style.backgroundColor2

					Text {
						anchors.fill: parent
						anchors.leftMargin: Style.marginL
						anchors.rightMargin: Style.marginL
						verticalAlignment: Text.AlignVCenter
						elide: Text.ElideMiddle
						text: root.atDrivesRoot ? qsTr("This PC") : root.currentPath
						color: Style.textColor
						opacity: 0.9
						font.pixelSize: Style.fontSizeM
					}
				}

				SearchTextInput {
					id: filterField

					anchors.right: parent.right
					anchors.verticalCenter: parent.verticalCenter
					width: root.filterFieldWidth
					height: parent.height
					radius: height / 2
					color: Style.backgroundColor2
					borderWidth: filterField.textInputActiveFocus ? 1 : 0
					placeHolderText: qsTr("Search %1").arg(root.currentFolderName())

					// Debounced re-query so each keystroke does not hit the agent.
					onSearchChanged: {
						root.applyFilterFromField(filterField.text)
					}

					onTextInputActiveFocusChanged: {
						root.filterFieldFocused = filterField.textInputActiveFocus
					}

					// Escape is the expected way to leave a search field, but
					// CustomTextField only emits cancelled() on it - nothing moved
					// focus away, so once in the field there was no keyboard way out.
					onCancelled: {
						root.forceActiveFocus()
					}
				}
			}

			// A directly-declared onTextChanged on filterField would shadow
			// SearchTextInput's own internal onTextChanged (which restarts its
			// debounce timer), leaving searchChanged() stuck on a stale timer -
			// Connections attaches independently so both handlers actually run.
			Connections {
				target: filterField
				function onTextChanged(){
					if (filterField.text === "")
						root.applyFilterFromField("")
				}
			}

			// Sync filter field when navigation clears filterText.
			Connections {
				target: root
				function onFilterTextChanged(){
					if (filterField.text !== root.filterText)
						filterField.text = root.filterText
				}
			}

			// ─── Column header ──────────────────────────────────────────────
			Item {
				id: headerRow

				anchors.top: toolbar.bottom
				anchors.topMargin: Style.marginL
				anchors.left: parent.left
				anchors.leftMargin: Style.marginL + Style.marginXS
				anchors.right: parent.right
				anchors.rightMargin: Style.marginL + root.scrollGutter
				height: Style.controlHeightS

				property int nameWidth: Math.max(140,
					width - (root.multiSelect ? root.checkColumnWidth : 0)
					- root.sizeColumnWidth - root.dateColumnWidth)

				Row {
					anchors.fill: parent

					Item {
						width: root.multiSelect ? root.checkColumnWidth : 0
						height: parent.height
						visible: root.multiSelect

						CheckBox {
							anchors.centerIn: parent
							tristate: true
							checkState: root.headerCheckState()
							isActive: entriesModel.count > 0

							MouseArea {
								anchors.fill: parent
								cursorShape: Qt.PointingHandCursor
								onClicked: {
									root.toggleSelectAll()
									root.forceActiveFocus()
								}
							}
						}
					}

					Item {
						width: headerRow.nameWidth
						height: parent.height

						Text {
							anchors.fill: parent
							leftPadding: Style.marginM
							verticalAlignment: Text.AlignVCenter
							text: qsTr("Name") + root.sortMark("name")
							font.pixelSize: Style.fontSizeS
							font.bold: true
							color: Style.textColor
							opacity: root.sortColumn === "name" ? 0.95 : 0.6
							elide: Text.ElideRight
						}

						MouseArea {
							anchors.fill: parent
							cursorShape: Qt.PointingHandCursor
							onClicked: {
								root.toggleSort("name")
								root.forceActiveFocus()
							}
						}
					}

					// Drives have neither a date nor a size of their own, so at the
					// drives root these two slots carry volume capacity instead of
					// standing empty (same widths - only label and alignment swap).
					Item {
						width: root.dateColumnWidth
						height: parent.height

						Text {
							anchors.fill: parent
							leftPadding: root.atDrivesRoot ? 0 : Style.marginM
							rightPadding: root.atDrivesRoot ? Style.marginM : 0
							horizontalAlignment: root.atDrivesRoot ? Text.AlignRight : Text.AlignLeft
							verticalAlignment: Text.AlignVCenter
							text: (root.atDrivesRoot ? qsTr("Free space") : qsTr("Date modified"))
								+ root.sortMark("date")
							font.pixelSize: Style.fontSizeS
							font.bold: true
							color: Style.textColor
							opacity: root.sortColumn === "date" ? 0.95 : 0.6
							elide: Text.ElideRight
						}

						MouseArea {
							anchors.fill: parent
							cursorShape: Qt.PointingHandCursor
							onClicked: {
								root.toggleSort("date")
								root.forceActiveFocus()
							}
						}
					}

					Item {
						width: root.sizeColumnWidth
						height: parent.height

						Text {
							anchors.fill: parent
							rightPadding: Style.marginM
							horizontalAlignment: Text.AlignRight
							verticalAlignment: Text.AlignVCenter
							text: (root.atDrivesRoot ? qsTr("Total size") : qsTr("Size"))
								+ root.sortMark("size")
							font.pixelSize: Style.fontSizeS
							font.bold: true
							color: Style.textColor
							opacity: root.sortColumn === "size" ? 0.95 : 0.6
							elide: Text.ElideRight
						}

						MouseArea {
							anchors.fill: parent
							cursorShape: Qt.PointingHandCursor
							onClicked: {
								root.toggleSort("size")
								root.forceActiveFocus()
							}
						}
					}
				}
			}

			// ─── Entries panel ──────────────────────────────────────────────
			Rectangle {
				id: entriesPanel

				anchors.top: headerRow.bottom
				anchors.topMargin: Style.marginXS
				anchors.left: parent.left
				anchors.leftMargin: Style.marginL
				anchors.right: parent.right
				anchors.rightMargin: Style.marginL
				anchors.bottom: statusBar.top
				anchors.bottomMargin: Style.marginM
				radius: Style.radiusM
				color: Style.baseColor
				border.color: Style.borderColor
				border.width: 1
				clip: true

				// Click on empty list space clears the selection (Explorer behaviour).
				MouseArea {
					id: deselectArea
					anchors.fill: parent
					onClicked: {
						root.setSingleSelection("")
						root.forceActiveFocus()
					}
				}

				ListView {
					id: entriesView

					anchors.top: parent.top
					anchors.left: parent.left
					anchors.leftMargin: Style.marginXS
					anchors.topMargin: Style.marginXS
					anchors.right: parent.right
					anchors.rightMargin: root.scrollGutter
					height: Math.min(entriesView.contentHeight, entriesPanel.height - loadMoreBar.height - 2 * Style.marginXS)
					clip: true
					boundsBehavior: Flickable.StopAtBounds
					model: entriesModel
					cacheBuffer: 0
					opacity: root.isLoading && !root.__appending ? 0.35 : 1

					// Auto-page the next slice once the user scrolls near the bottom -
					// avoids "atYEnd" (unused elsewhere in this codebase, so unverified
					// against the JQML web transpiler) in favor of the same
					// contentY/contentHeight arithmetic already used for this view's
					// own height binding above.
					onContentYChanged: {
						if (root.hasMore && !root.isLoading
								&& entriesView.contentY + entriesView.height >= entriesView.contentHeight - Style.spacingL)
							root.loadMore()
					}

					delegate: Rectangle {
						id: rowRoot

						width: entriesView.width
						height: Style.controlHeightM
						property bool rowHovered: false
						property bool rowSelected: root.isPathSelected(model.path)
						property bool rowSelectable: root.isSelectableEntry(model.entryType)

						color: rowSelected ? Style.selectedColor
							: rowHovered ? Style.hover
							: "transparent"
						radius: Style.radiusS

						Row {
							anchors.fill: parent

							// Checkbox cell (multi-select only; visual — row click drives it)
							Item {
								width: root.multiSelect ? root.checkColumnWidth : 0
								height: parent.height
								visible: root.multiSelect

								CheckBox {
									anchors.centerIn: parent
									checkState: rowRoot.rowSelected ? Qt.Checked : Qt.Unchecked
									isActive: rowRoot.rowSelectable
								}
							}

							// Name cell (icon + text)
							Item {
								width: headerRow.nameWidth
								height: parent.height

								Row {
									anchors.left: parent.left
									anchors.leftMargin: Style.marginM
									anchors.right: parent.right
									anchors.rightMargin: Style.marginS
									anchors.verticalCenter: parent.verticalCenter
									spacing: Style.spacingS

									Image {
										id: entryIcon
										width: Style.iconSizeS
										height: Style.iconSizeS
										anchors.verticalCenter: parent.verticalCenter
										sourceSize.width: width
										sourceSize.height: height
										fillMode: Image.PreserveAspectFit
										source: "../../../" + Style.getIconPath(model.iconName, Icon.State.On, Icon.Mode.Normal)
									}

									Text {
										width: Math.max(0, parent.width - entryIcon.width - parent.spacing)
										anchors.verticalCenter: parent.verticalCenter
										text: model.name
										color: rowRoot.rowSelected ? Style.textSelectedColor : Style.textColor
										font.pixelSize: Style.fontSizeM
										font.bold: model.isDir
										elide: Text.ElideRight
										verticalAlignment: Text.AlignVCenter
									}
								}
							}

							Text {
								width: root.dateColumnWidth
								height: parent.height
								leftPadding: root.atDrivesRoot ? 0 : Style.marginM
								rightPadding: root.atDrivesRoot ? Style.marginM : 0
								horizontalAlignment: root.atDrivesRoot ? Text.AlignRight : Text.AlignLeft
								verticalAlignment: Text.AlignVCenter
								text: root.atDrivesRoot ? model.freeText : model.dateText
								color: rowRoot.rowSelected ? Style.textSelectedColor : Style.textColor
								opacity: 0.75
								font.pixelSize: Style.fontSizeS
								elide: Text.ElideRight
							}

							Text {
								width: root.sizeColumnWidth
								height: parent.height
								rightPadding: Style.marginM
								horizontalAlignment: Text.AlignRight
								verticalAlignment: Text.AlignVCenter
								text: root.atDrivesRoot ? model.totalText : model.sizeText
								color: rowRoot.rowSelected ? Style.textSelectedColor : Style.textColor
								opacity: 0.75
								font.pixelSize: Style.fontSizeS
							}
						}

						MouseArea {
							anchors.fill: parent
							hoverEnabled: true
							onContainsMouseChanged: rowRoot.rowHovered = containsMouse
							onClicked: {
								root.forceActiveFocus()
								root.onRowClicked(index, model.path, mouse.modifiers)
							}
							onDoubleClicked: {
								root.activateEntry(index)
							}
						}
					}

				}

				Item {
					id: loadMoreBar

					anchors.left: parent.left
					anchors.right: parent.right
					anchors.bottom: parent.bottom
					height: root.hasMore ? Style.controlHeightL : 0
					visible: root.hasMore

					Button {
						anchors.centerIn: parent
						widthFromDecorator: true
						// Scrolling near the bottom (see entriesView.onContentYChanged)
						// already pages the next slice automatically - this stays as a
						// manual fallback for non-scroll interaction.
						text: qsTr("Load more...")
						enabled: !root.isLoading
						onClicked: {
							root.loadMore()
							root.forceActiveFocus()
						}
					}
				}

				CustomScrollbar {
					id: entriesScrollbar

					z: entriesView.z + 1
					anchors.right: parent.right
					anchors.rightMargin: Style.marginXS
					anchors.top: entriesView.top
					anchors.bottom: entriesView.bottom
					secondSize: Style.marginS
					targetItem: entriesView
					radius: Style.radiusS
				}

				// Empty state. "Never loaded anything yet" (currentPath still empty)
				// and it failed is shown here, since there is no prior listing to
				// fall back to; any other failure stays on the previous listing and
				// is reported via the status bar instead, so this panel only ever
				// describes the folder actually on screen (never an unrelated,
				// separately-failed navigation attempt).
				Column {
					id: emptyState

					readonly property bool loadFailed: root.currentPath === "" && root.errorText !== ""
					readonly property bool filterEmpty: !emptyState.loadFailed
						&& entriesModel.count === 0 && root.filterText !== "" && root.errorText === ""

					anchors.centerIn: parent
					width: Math.min(parent.width - 2 * Style.marginXL, Style.sizeHintM)
					spacing: Style.spacingM
					visible: !root.isLoading && entriesModel.count === 0

					Text {
						width: parent.width
						horizontalAlignment: Text.AlignHCenter
						text: emptyState.loadFailed ? qsTr("Can't load this location")
							: (emptyState.filterEmpty ? qsTr("No matching items") : qsTr("This folder is empty"))
						color: Style.textColor
						font.pixelSize: Style.fontSizeL
						opacity: 0.8
						elide: Text.ElideRight
					}

					Text {
						width: parent.width
						horizontalAlignment: Text.AlignHCenter
						wrapMode: Text.WordWrap
						text: emptyState.loadFailed
							? root.errorText
							: (emptyState.filterEmpty
								? qsTr("Try a different filter, or clear it to see all items")
								: ((root.hasParent && !root.atDrivesRoot) ? qsTr("Go up or pick another location") : qsTr("Pick another location")))
						color: Style.textColor
						font.pixelSize: Style.fontSizeS
						opacity: 0.5
					}

					Button {
						anchors.horizontalCenter: parent.horizontalCenter
						widthFromDecorator: true
						text: qsTr("Retry")
						visible: emptyState.loadFailed
						onClicked: {
							root.retryLastAttempt()
							root.forceActiveFocus()
						}
					}

					Button {
						anchors.horizontalCenter: parent.horizontalCenter
						widthFromDecorator: true
						text: qsTr("Clear filter")
						visible: emptyState.filterEmpty
						onClicked: {
							root.applyFilterFromField("")
							root.forceActiveFocus()
						}
					}
				}

				// Loading overlay (full reload only, not "load more")
				Rectangle {
					anchors.fill: parent
					radius: Style.radiusM
					color: Style.baseColor
					opacity: (root.isLoading && !root.__appending) ? 0.85 : 0
					visible: opacity > 0.01
					z: 10

					Loading {
						anchors.centerIn: parent
						width: Style.iconSizeL
						height: Style.iconSizeL
						indicatorSize: Style.iconSizeM
						visible: root.isLoading
						background.color: "transparent"
					}
				}
			}

			// ─── Bottom status bar ──────────────────────────────────────────
			Item {
				id: statusBar

				anchors.left: parent.left
				anchors.leftMargin: Style.marginL
				anchors.right: parent.right
				anchors.rightMargin: Style.marginL
				anchors.bottom: parent.bottom
				anchors.bottomMargin: Style.marginM
				height: Style.controlHeightM

				Rectangle {
					width: Style.spacingXS
					height: parent.height - Style.marginXS
					anchors.left: parent.left
					anchors.verticalCenter: parent.verticalCenter
					radius: width / 2
					color: root.errorText !== "" ? Style.errorColor
						: (root.isLoading ? Style.highlightColor : Style.imaginToolsAccentColor)
				}

				Text {
					anchors.left: parent.left
					anchors.leftMargin: Style.marginM
					anchors.right: itemCountText.left
					anchors.rightMargin: Style.marginM
					anchors.verticalCenter: parent.verticalCenter
					elide: Text.ElideMiddle
					text: root.statusLabel()
					color: root.errorText !== "" ? Style.errorColor : Style.textColor
					opacity: root.errorText !== "" ? 1 : 0.85
					font.pixelSize: Style.fontSizeM
				}

				// Item/selection counts, kept apart from the status text above (which
				// carries the path / hint / error instead) - Explorer-style, always
				// visible regardless of what the main status line is currently saying.
				Text {
					id: itemCountText

					anchors.right: parent.right
					anchors.verticalCenter: parent.verticalCenter
					elide: Text.ElideLeft
					text: root.selectionCountLabel() !== ""
						? qsTr("%1  •  %2").arg(root.itemCountLabel()).arg(root.selectionCountLabel())
						: root.itemCountLabel()
					color: Style.textColor
					opacity: 0.6
					font.pixelSize: Style.fontSizeS
				}
			}
		}
	}
}
