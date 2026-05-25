const Item = require("../Item")
const Popup = require("./Popup")
const Real = require("../../QtQml/Real")
const Int = require("../../QtQml/Int")
const Var = require("../../QtQml/Var")
const StringProp = require("../../QtQml/String")
const Variant = require("../../QtQml/Variant")
const Signal = require("../../QtQml/Signal")

// StandardButton codes (mirrors Qt::StandardButton)
const StandardButtons = {
    Ok:              0x00000400,
    Open:            0x00002000,
    Save:            0x00000800,
    Cancel:          0x00400000,
    Close:           0x00200000,
    Discard:         0x00800000,
    Apply:           0x02000000,
    Reset:           0x04000000,
    RestoreDefaults: 0x08000000,
    Help:            0x01000000,
    SaveAll:         0x00001000,
    Yes:             0x00004000,
    YesToAll:        0x00008000,
    No:              0x00010000,
    NoToAll:         0x00020000,
    Abort:           0x00040000,
    Retry:           0x00080000,
    Ignore:          0x00100000,
    NoButton:        0x00000000,
}

const StandardButtonLabels = {
    [StandardButtons.Ok]:              'OK',
    [StandardButtons.Open]:            'Open',
    [StandardButtons.Save]:            'Save',
    [StandardButtons.Cancel]:          'Cancel',
    [StandardButtons.Close]:           'Close',
    [StandardButtons.Discard]:         'Discard',
    [StandardButtons.Apply]:           'Apply',
    [StandardButtons.Reset]:           'Reset',
    [StandardButtons.RestoreDefaults]: 'Restore Defaults',
    [StandardButtons.Help]:            'Help',
    [StandardButtons.SaveAll]:         'Save All',
    [StandardButtons.Yes]:             'Yes',
    [StandardButtons.YesToAll]:        'Yes to All',
    [StandardButtons.No]:              'No',
    [StandardButtons.NoToAll]:         'No to All',
    [StandardButtons.Abort]:           'Abort',
    [StandardButtons.Retry]:           'Retry',
    [StandardButtons.Ignore]:          'Ignore',
}

// Which buttons are "accept" vs "reject"
const AcceptButtons = new Set([
    StandardButtons.Ok, StandardButtons.Open, StandardButtons.Save,
    StandardButtons.SaveAll, StandardButtons.Yes, StandardButtons.YesToAll,
    StandardButtons.Apply, StandardButtons.RestoreDefaults
])

class Dialog extends Popup {
    // Expose StandardButton constants
    static OkButton              = StandardButtons.Ok
    static OpenButton            = StandardButtons.Open
    static SaveButton            = StandardButtons.Save
    static CancelButton          = StandardButtons.Cancel
    static CloseButton           = StandardButtons.Close
    static DiscardButton         = StandardButtons.Discard
    static ApplyButton           = StandardButtons.Apply
    static ResetButton           = StandardButtons.Reset
    static RestoreDefaultsButton = StandardButtons.RestoreDefaults
    static HelpButton            = StandardButtons.Help
    static SaveAllButton         = StandardButtons.SaveAll
    static YesButton             = StandardButtons.Yes
    static YesToAllButton        = StandardButtons.YesToAll
    static NoButton              = StandardButtons.No
    static NoToAllButton         = StandardButtons.NoToAll
    static AbortButton           = StandardButtons.Abort
    static RetryButton           = StandardButtons.Retry
    static IgnoreButton          = StandardButtons.Ignore
    static NoStandardButton      = StandardButtons.NoButton

    // Compat aliases
    static OkAn             = StandardButtons.Ok
    static OpenAn           = StandardButtons.Open
    static SaveA            = StandardButtons.Save
    static CancelA          = StandardButtons.Cancel
    static CloseA           = StandardButtons.Close
    static DiscardA         = StandardButtons.Discard
    static ApplyAn          = StandardButtons.Apply
    static ResetA           = StandardButtons.Reset
    static RestoreDefaultsA = StandardButtons.RestoreDefaults
    static HelpA            = StandardButtons.Help
    static SaveAllA         = StandardButtons.SaveAll
    static YesA             = StandardButtons.Yes
    static YesToAllA        = StandardButtons.YesToAll
    static NoA              = StandardButtons.No
    static NoToAllA         = StandardButtons.NoToAll
    static AbortAn          = StandardButtons.Abort
    static RetryA           = StandardButtons.Retry
    static IgnoreAn         = StandardButtons.Ignore
    static NoButtonAn       = StandardButtons.NoButton

    static meta = Object.assign({}, Popup.meta, {
        footer:               { type: Variant, targetType: Item },
        header:               { type: Variant, targetType: Item },
        implicitFooterHeight: { type: Real,    value: 0 },
        implicitFooterWidth:  { type: Real,    value: 0 },
        implicitHeaderHeight: { type: Real,    value: 0 },
        implicitHeaderWidth:  { type: Real,    value: 0 },
        result:               { type: Int,     value: 0 },
        standardButtons:      { type: Real,    value: 0 },
        title:                { type: StringProp, value: '' },

        footerChanged:               { type: Signal, args: [] },
        headerChanged:               { type: Signal, args: [] },
        implicitFooterHeightChanged: { type: Signal, args: [] },
        implicitFooterWidthChanged:  { type: Signal, args: [] },
        implicitHeaderHeightChanged: { type: Signal, args: [] },
        implicitHeaderWidthChanged:  { type: Signal, args: [] },
        resultChanged:               { type: Signal, args: [] },
        standardButtonsChanged:      { type: Signal, args: [] },
        titleChanged:                { type: Signal, args: [] },

        accepted:     { type: Signal, args: [] },
        applied:      { type: Signal, args: [] },
        discarded:    { type: Signal, args: [] },
        helpRequested: { type: Signal, args: [] },
        rejected:     { type: Signal, args: [] },
        reset:        { type: Signal, args: [] },
    })

    static create(parent = null, properties = {}) {
        let obj = super.create(parent, properties)
        let box = obj.__contentDOM
        if (!box) return obj

        box.classList.add('Dialog')
        // Modal by default
        obj.modal = true
        // Remove default padding – we'll control layout manually
        box.style.padding = '0'
        box.style.minWidth = '320px'

        // ── Title bar ─────────────────────────────────────────────────────────
        let titleBar = document.createElement('div')
        titleBar.style.cssText = 'display:flex;align-items:center;padding:10px 16px;border-bottom:1px solid #e0e0e0;background:#f5f5f5;border-radius:4px 4px 0 0;font-weight:600;font-size:14px;user-select:none;'
        let titleText = document.createTextNode(obj.title || '')
        titleBar.appendChild(titleText)
        box.appendChild(titleBar)
        obj.__titleBarDOM = titleBar
        obj.__titleTextNode = titleText

        // ── Content area (children go here) ──────────────────────────────────
        let contentArea = document.createElement('div')
        contentArea.style.cssText = 'flex:1;padding:16px;display:flex;flex-direction:column;overflow:auto;'
        box.appendChild(contentArea)
        obj.__dialogContentDOM = contentArea

        // ── Footer / standard buttons ─────────────────────────────────────────
        let footer = document.createElement('div')
        footer.style.cssText = 'display:flex;flex-direction:row;justify-content:flex-end;align-items:center;gap:8px;padding:10px 16px;border-top:1px solid #e0e0e0;background:#f5f5f5;border-radius:0 0 4px 4px;'
        box.appendChild(footer)
        obj.__footerDOM = footer

        return obj
    }

    // Children go into the content area, not the outer box
    __getDOM() {
        return this.__dialogContentDOM || this.__contentDOM
    }

    // ── Property change handlers ──────────────────────────────────────────────

    SLOT_titleChanged(oldValue, newValue) {
        if (this.__titleTextNode) this.__titleTextNode.nodeValue = newValue || ''
    }

    SLOT_standardButtonsChanged(oldValue, newValue) {
        this.__rebuildStandardButtons()
    }

    // ── Internal helpers ──────────────────────────────────────────────────────

    __rebuildStandardButtons() {
        let footer = this.__footerDOM
        if (!footer) return
        footer.innerHTML = ''
        this.__stdBtnMap = {}

        let flags = this.standardButtons || 0
        if (!flags) return

        // Collect which buttons are set and sort: accept buttons right-most
        const allCodes = Object.values(StandardButtons).filter(v => v > 0)
        for (let code of allCodes) {
            if (!(flags & code)) continue
            let label = StandardButtonLabels[code] || '?'
            let isAccept = AcceptButtons.has(code)
            let btn = document.createElement('button')
            btn.textContent = label
            btn.style.cssText = 'padding:6px 16px;font-size:13px;cursor:pointer;border-radius:4px;border:1px solid #c0c0c0;background:#fff;min-width:64px;'
            if (isAccept) btn.style.background = '#0078d4'; btn.style.color = '#fff'; btn.style.borderColor = '#0078d4'

            btn.addEventListener('click', () => {
                if (isAccept) {
                    this.accept()
                } else if (code === StandardButtons.Cancel || code === StandardButtons.Close || code === StandardButtons.No || code === StandardButtons.NoToAll || code === StandardButtons.Abort) {
                    this.reject()
                } else if (code === StandardButtons.Apply) {
                    if (this.applied) this.applied()
                } else if (code === StandardButtons.Discard) {
                    if (this.discarded) this.discarded()
                } else if (code === StandardButtons.Reset) {
                    if (this.reset) this.reset()
                } else if (code === StandardButtons.Help) {
                    if (this.helpRequested) this.helpRequested()
                }
            })

            footer.appendChild(btn)
            this.__stdBtnMap = this.__stdBtnMap || {}
            this.__stdBtnMap[code] = btn
        }
    }

    // ── Public API ────────────────────────────────────────────────────────────

    accept() {
        this.result = 1
        if (this.accepted) this.accepted()
        this.close()
    }

    reject() {
        this.result = 0
        if (this.rejected) this.rejected()
        this.close()
    }

    done(result) {
        this.result = result
        if (result === 1) { if (this.accepted) this.accepted() }
        else              { if (this.rejected) this.rejected() }
        this.close()
    }

    standardButton(code) {
        return this.__stdBtnMap && this.__stdBtnMap[code] ? this.__stdBtnMap[code] : null
    }

    // ── Override open to rebuild buttons lazily ───────────────────────────────

    open() {
        this.__rebuildStandardButtons()
        super.open()
    }
}



module.exports = Dialog
