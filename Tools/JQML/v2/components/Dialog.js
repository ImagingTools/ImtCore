const { Popup } = require('./Popup')
const { QString, QBool, QVar, QInt } = require('../utils/properties')

/**
 * Dialog — Qt Quick Controls Dialog implementation for JQML v2
 * https://doc.qt.io/qt-6/qml-qtquick-controls-dialog.html
 *
 * A dialog popup with header (title), content area, and standard buttons footer.
 * Extends Popup with title, standardButtons, header/footer items, and
 * accepted/rejected/applied/discarded/helpRequested/reset signals.
 */

// StandardButton flags (matching Qt::DialogButtonRole)
const StandardButton = {
    NoButton: 0x00000000,
    Ok: 0x00000400,
    Save: 0x00000800,
    SaveAll: 0x00001000,
    Open: 0x00002000,
    Yes: 0x00004000,
    YesToAll: 0x00008000,
    No: 0x00010000,
    NoToAll: 0x00020000,
    Abort: 0x00040000,
    Retry: 0x00080000,
    Ignore: 0x00100000,
    Close: 0x00200000,
    Cancel: 0x00400000,
    Discard: 0x00800000,
    Help: 0x01000000,
    Apply: 0x02000000,
    Reset: 0x04000000,
    RestoreDefaults: 0x08000000,
}

class Dialog extends Popup {
    // Expose StandardButton enum on the class
    static NoButton = StandardButton.NoButton
    static Ok = StandardButton.Ok
    static Save = StandardButton.Save
    static SaveAll = StandardButton.SaveAll
    static Open = StandardButton.Open
    static Yes = StandardButton.Yes
    static YesToAll = StandardButton.YesToAll
    static No = StandardButton.No
    static NoToAll = StandardButton.NoToAll
    static Abort = StandardButton.Abort
    static Retry = StandardButton.Retry
    static Ignore = StandardButton.Ignore
    static Close = StandardButton.Close
    static Cancel = StandardButton.Cancel
    static Discard = StandardButton.Discard
    static Help = StandardButton.Help
    static Apply = StandardButton.Apply
    static Reset = StandardButton.Reset
    static RestoreDefaults = StandardButton.RestoreDefaults

    static defaultProperties = {
        title: { type: QString, value: '', changed: '$titleChanged' },
        standardButtons: { type: QInt, value: StandardButton.NoButton, changed: '$standardButtonsChanged' },
        header: { type: QVar },
        footer: { type: QVar },
        result: { type: QInt, value: 0 },
        implicitHeaderWidth: { type: QInt, value: 0 },
        implicitHeaderHeight: { type: QInt, value: 0 },
        implicitFooterWidth: { type: QInt, value: 0 },
        implicitFooterHeight: { type: QInt, value: 0 },
    }

    static defaultSignals = {
        accepted: { params: [] },
        rejected: { params: [] },
        applied: { params: [] },
        discarded: { params: [] },
        helpRequested: { params: [] },
        reset: { params: [] },
    }

    constructor(parent, exCtx, exModel){
        super(parent, exCtx, exModel)

        // Default modal for dialogs
        this.getProperty('modal').reset(true)

        // Header element
        this.$headerEl = document.createElement('div')
        this.$headerEl.style.cssText = 'padding:12px 16px;font-weight:bold;font-size:16px;border-bottom:1px solid #e0e0e0;'
        this.$contentBox.insertBefore(this.$headerEl, this.$contentBox.firstChild)

        // Footer element (button row)
        this.$footerEl = document.createElement('div')
        this.$footerEl.style.cssText = 'padding:8px 16px;display:flex;flex-direction:row;justify-content:flex-end;gap:8px;border-top:1px solid #e0e0e0;'
        this.$contentBox.appendChild(this.$footerEl)

        this.$titleChanged()
        this.$standardButtonsChanged()
    }

    // --- Public methods ---

    accept(){
        this.getProperty('result').reset(1) // Dialog.Accepted
        if (this.$signals.accepted) this.$signals.accepted()
        this.close()
    }

    reject(){
        this.getProperty('result').reset(0) // Dialog.Rejected
        if (this.$signals.rejected) this.$signals.rejected()
        this.close()
    }

    done(result){
        this.getProperty('result').reset(result)
        if (result === 1){
            if (this.$signals.accepted) this.$signals.accepted()
        } else {
            if (this.$signals.rejected) this.$signals.rejected()
        }
        this.close()
    }

    standardButton(button){
        // Return the button DOM element if present
        return this.$footerEl.querySelector(`[data-button="${button}"]`) || null
    }

    // --- Change handlers ---

    $titleChanged(){
        if (this.$headerEl){
            this.$headerEl.textContent = this.getPropertyValue('title')
            this.$headerEl.style.display = this.getPropertyValue('title') ? 'block' : 'none'
        }
    }

    $standardButtonsChanged(){
        if (!this.$footerEl) return
        this.$footerEl.innerHTML = ''

        let buttons = this.getPropertyValue('standardButtons')
        if (buttons === StandardButton.NoButton){
            this.$footerEl.style.display = 'none'
            return
        }
        this.$footerEl.style.display = 'flex'

        const buttonDefs = [
            { flag: StandardButton.Ok, label: 'OK', action: () => this.accept() },
            { flag: StandardButton.Save, label: 'Save', action: () => this.accept() },
            { flag: StandardButton.Open, label: 'Open', action: () => this.accept() },
            { flag: StandardButton.Yes, label: 'Yes', action: () => this.accept() },
            { flag: StandardButton.YesToAll, label: 'Yes to All', action: () => this.accept() },
            { flag: StandardButton.No, label: 'No', action: () => this.reject() },
            { flag: StandardButton.NoToAll, label: 'No to All', action: () => this.reject() },
            { flag: StandardButton.Abort, label: 'Abort', action: () => this.reject() },
            { flag: StandardButton.Retry, label: 'Retry', action: () => this.accept() },
            { flag: StandardButton.Ignore, label: 'Ignore', action: () => this.reject() },
            { flag: StandardButton.Close, label: 'Close', action: () => this.reject() },
            { flag: StandardButton.Cancel, label: 'Cancel', action: () => this.reject() },
            { flag: StandardButton.Discard, label: 'Discard', action: () => { if (this.$signals.discarded) this.$signals.discarded(); this.reject() } },
            { flag: StandardButton.Help, label: 'Help', action: () => { if (this.$signals.helpRequested) this.$signals.helpRequested() } },
            { flag: StandardButton.Apply, label: 'Apply', action: () => { if (this.$signals.applied) this.$signals.applied() } },
            { flag: StandardButton.Reset, label: 'Reset', action: () => { if (this.$signals.reset) this.$signals.reset() } },
            { flag: StandardButton.RestoreDefaults, label: 'Restore Defaults', action: () => { if (this.$signals.reset) this.$signals.reset() } },
        ]

        for (let def of buttonDefs){
            if (buttons & def.flag){
                let btn = document.createElement('button')
                btn.textContent = def.label
                btn.setAttribute('data-button', String(def.flag))
                btn.style.cssText = 'padding:6px 16px;border:1px solid #cccccc;border-radius:4px;background:#f5f5f5;cursor:pointer;font-size:14px;'
                btn.addEventListener('click', (e) => {
                    e.stopPropagation()
                    def.action()
                })
                this.$footerEl.appendChild(btn)
            }
        }
    }

    destroy(){
        delete this.$headerEl
        delete this.$footerEl
        super.destroy()
    }
}

module.exports.Dialog = Dialog
