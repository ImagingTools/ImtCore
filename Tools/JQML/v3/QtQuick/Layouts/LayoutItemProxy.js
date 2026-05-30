const Item   = require("../Item")
const Var    = require("../../QtQml/Var")
const Signal = require("../../QtQml/Signal")

// ── LayoutItemProxy ───────────────────────────────────────────────────────────
// Represents a target Item within a layout hierarchy.
//
// Behaviour:
//  • When this proxy becomes visible AND has a target, the target is positioned
//    and sized to match the proxy geometry and made visible.
//  • When this proxy becomes invisible (or loses its target), the target is
//    hidden (if no other visible proxy claims it).
//  • The proxy forwards all Layout attached-property hints from the target
//    unless they have been explicitly set on the proxy itself.
//  • Multiple proxies may share the same target; only the visible one controls.
// ─────────────────────────────────────────────────────────────────────────────

class LayoutItemProxy extends Item {

    static meta = Object.assign({}, Item.meta, {
        target: { type: Var, value: null },

        targetChanged: { type: Signal, args: [] },
    })

    static create(parent = null, properties = {}){
        let obj = super.create(parent, properties)
        obj.__DOM.classList.add('LayoutItemProxy')
        return obj
    }

    // ── lifecycle ─────────────────────────────────────────────────────────────

    __updateProperties(){
        super.__updateProperties()
        this.__syncTarget()
    }

    'SLOT_Component.completed'(){
        this.__syncTarget()
    }

    // ── target change ─────────────────────────────────────────────────────────

    SLOT_targetChanged(oldTarget, newTarget){
        // Detach from old target
        if(oldTarget && oldTarget.__layoutProxies){
            const idx = oldTarget.__layoutProxies.indexOf(this.__proxy)
            if(idx >= 0) oldTarget.__layoutProxies.splice(idx, 1)
            // If no proxy owns it, hide the target
            if(!oldTarget.__layoutProxies.some(p => p.visible)){
                oldTarget.visible = false
            }
        }
        // Attach to new target
        if(newTarget){
            if(!newTarget.__layoutProxies) newTarget.__layoutProxies = []
            if(newTarget.__layoutProxies.indexOf(this.__proxy) < 0){
                newTarget.__layoutProxies.push(this.__proxy)
            }
        }
        this.__syncTarget()
        // Notify parent layout
        if(this.parent) JQApplication.updateLater(this.parent)
    }

    // ── visibility change ─────────────────────────────────────────────────────

    SLOT_visibleChanged(oldValue, newValue){
        JQApplication.beginUpdate()
        super.SLOT_visibleChanged(oldValue, newValue)
        this.__syncTarget()
        JQApplication.endUpdate()
    }

    // ── geometry changes: mirror to target ────────────────────────────────────

    SLOT_widthChanged(oldValue, newValue){
        super.SLOT_widthChanged(oldValue, newValue)
        this.__syncTargetGeometry()
    }

    SLOT_heightChanged(oldValue, newValue){
        super.SLOT_heightChanged(oldValue, newValue)
        this.__syncTargetGeometry()
    }

    SLOT_xChanged(oldValue, newValue){
        super.SLOT_xChanged(oldValue, newValue)
        this.__syncTargetGeometry()
    }

    SLOT_yChanged(oldValue, newValue){
        super.SLOT_yChanged(oldValue, newValue)
        this.__syncTargetGeometry()
    }

    // After any layout update cycle, re-sync geometry to target
    __endUpdate(){
        super.__endUpdate()
        this.__syncTargetGeometry()
    }

    // ── helpers ───────────────────────────────────────────────────────────────

    /**
     * Sync target visibility: make target visible when proxy is visible,
     * hide when no visible proxy claims it.
     */
    __syncTarget(){
        const target  = this.__proxy.target
        const visible = this.__proxy.visible

        if(!target) return

        if(visible){
            target.visible = true
            this.__syncTargetGeometry()
        } else {
            // Only hide if no other proxy claims this target
            if(!target.__layoutProxies || !target.__layoutProxies.some(p => p !== this.__proxy && p.visible)){
                target.visible = false
            }
        }
    }

    /**
     * Copy proxy width/height to target (proxy is a layout placeholder;
     * target is positioned independently).
     */
    __syncTargetGeometry(){
        const target  = this.__proxy.target
        const visible = this.__proxy.visible
        if(!target || !visible) return

        const pw = this.__proxy.width
        const ph = this.__proxy.height
        if(pw > 0 && target.width  !== pw) target.width  = pw
        if(ph > 0 && target.height !== ph) target.height = ph
    }

    /**
     * Mirror Layout attached-property values from target onto this proxy,
     * so that the layout system respects the target's size hints.
     */
    __syncLayoutHints(target){
        if(!target || !target.Layout) return
        const srcL = target.Layout
        const dstL = this.__proxy.Layout

        const forward = [
            'fillWidth', 'fillHeight',
            'preferredWidth', 'preferredHeight',
            'minimumWidth', 'minimumHeight',
            'maximumWidth', 'maximumHeight',
            'horizontalStretchFactor', 'verticalStretchFactor',
            'alignment',
            'margins', 'leftMargin', 'rightMargin', 'topMargin', 'bottomMargin',
            'row', 'column', 'rowSpan', 'columnSpan',
        ]

        for(const prop of forward){
            try { dstL[prop] = srcL[prop] } catch(_){}
        }

        if(target.implicitWidth  !== undefined) this.__proxy.implicitWidth  = target.implicitWidth
        if(target.implicitHeight !== undefined) this.__proxy.implicitHeight = target.implicitHeight
    }
}

module.exports = LayoutItemProxy
