const { QtObject } = require('./QtObject')
const { QBool, QReal, QString, QVar, QVector3D, QQuaternion } = require('../utils/properties')

/**
 * Node3D - base class for everything that lives inside a View3D scene graph.
 *
 * Mirrors Qt Quick 3D's Node type. Each Node3D wraps a THREE.Object3D whose
 * lifecycle is tied to the JQML object lifecycle.
 *
 * Node3D extends QtObject (not Item) on purpose: 3D nodes are NOT part of
 * the DOM tree. Their parent inside the 3D world is determined either by
 * the enclosing View3D (root nodes) or by the enclosing Node3D (nested).
 *
 * The actual THREE construction is deferred via $createObject3D() so that
 * subclasses (Model, lights, cameras, ...) can return their own primitive.
 * Three.js itself is loaded lazily by View3D before any Node3D is attached
 * to a scene; until then, the Node3D simply caches its property values.
 */
class Node3D extends QtObject {
    static defaultProperties = {
        // transform
        position:        { type: QVector3D, value: { x: 0, y: 0, z: 0 },     changed: '$transformChanged' },
        eulerRotation:   { type: QVector3D, value: { x: 0, y: 0, z: 0 },     changed: '$rotationChanged' },
        rotation:        { type: QQuaternion, value: { scalar: 1, x: 0, y: 0, z: 0 }, changed: '$quaternionChanged' },
        scale:           { type: QVector3D, value: { x: 1, y: 1, z: 1 },     changed: '$transformChanged' },
        pivot:           { type: QVector3D, value: { x: 0, y: 0, z: 0 },     changed: '$transformChanged' },
        // visibility / opacity
        visible3D:       { type: QBool,   value: true,   changed: '$visibility3DChanged' },
        opacity3D:       { type: QReal,   value: 1,      changed: '$opacity3DChanged' },
        // optional name for picking / lookup
        objectName3D:    { type: QString, value: '' },
        // raw THREE handle (exposed for advanced users / pick())
        $three:          { type: QVar,   value: undefined },
    }

    constructor(parent, exCtx, exModel){
        super(parent, exCtx, exModel)

        // we use a deferred-init pattern: attach to scene happens in $attach3D()
        // which is called by the enclosing View3D / Node3D once THREE is loaded.
        this.$three3D = null
        this.$pendingAttach = false

        // walk up to find a View3D ancestor and queue ourselves
        let host = this.$findHost3D()
        if(host){
            this.$host3D = host
            host.$registerNode3D(this)
        }
    }

    /**
     * Locate the enclosing View3D (or Node3D whose host is known).
     * Returns null if this Node3D is not nested under a View3D yet.
     */
    $findHost3D(){
        let p = this.parent
        while(p){
            if(p.$isView3D) return p
            if(p.$host3D)  return p.$host3D
            p = p.parent
        }
        return null
    }

    /**
     * Subclasses must override this to construct and return their THREE
     * object (Mesh, Light, Camera, ...). Default: a transform-only group.
     *
     * @param {object} THREE the lazily-imported three.js module
     */
    $createObject3D(THREE){
        return new THREE.Group()
    }

    /**
     * Called by View3D once THREE is available. Builds the underlying
     * THREE object and attaches it to the scene-graph parent.
     */
    $attach3D(THREE, parentObject3D){
        if(this.$three3D) return this.$three3D

        this.$three3D = this.$createObject3D(THREE)
        this.$three = this.$three3D
        this.$three3D.userData.jqmlNode = this
        if(this.objectName3D) this.$three3D.name = this.objectName3D

        // Set Euler order to ZYX to match Qt Quick 3D defaults.
        if(this.$three3D.rotation && this.$three3D.rotation.order !== undefined){
            this.$three3D.rotation.order = 'ZYX'
        }

        this.$applyTransform()
        this.$applyVisibility()
        this.$applyOpacity()

        if(parentObject3D) parentObject3D.add(this.$three3D)

        // Attach any children that were registered before we existed
        if(this.$childNodes3D){
            for(let child of this.$childNodes3D){
                child.$attach3D(THREE, this.$three3D)
            }
        }

        return this.$three3D
    }

    /**
     * Register a child Node3D so that, once we're attached, the child
     * gets attached too.
     */
    $registerNode3D(child){
        if(!this.$childNodes3D) this.$childNodes3D = []
        if(this.$childNodes3D.indexOf(child) < 0) this.$childNodes3D.push(child)
        child.$host3D = this.$host3D || (this.$isView3D ? this : null)

        if(this.$three3D && this.$host3D && this.$host3D.$THREE){
            child.$attach3D(this.$host3D.$THREE, this.$three3D)
            this.$host3D.$requestRender()
        }
    }

    $applyTransform(){
        if(!this.$three3D) return
        let p = this.position || { x: 0, y: 0, z: 0 }
        let s = this.scale    || { x: 1, y: 1, z: 1 }
        if(this.$three3D.position){
            this.$three3D.position.set(p.x || 0, p.y || 0, p.z || 0)
        }
        if(this.$three3D.scale){
            this.$three3D.scale.set(s.x || 0, s.y || 0, s.z || 0)
        }
        this.$applyRotation()
        if(this.$host3D) this.$host3D.$requestRender()
    }

    $applyRotation(){
        if(!this.$three3D || !this.$three3D.rotation) return
        // If a non-identity quaternion has been set, prefer it; otherwise use eulerRotation (degrees, ZYX).
        let q = this.rotation
        let isIdentityQ = !q || (q.scalar === 1 && q.x === 0 && q.y === 0 && q.z === 0)
        if(!isIdentityQ && this.$three3D.quaternion){
            this.$three3D.quaternion.set(q.x, q.y, q.z, q.scalar)
        } else {
            let e = this.eulerRotation || { x: 0, y: 0, z: 0 }
            const DEG = Math.PI / 180
            this.$three3D.rotation.set(e.x * DEG, e.y * DEG, e.z * DEG, 'ZYX')
        }
    }

    $applyVisibility(){
        if(this.$three3D) this.$three3D.visible = !!this.visible3D
    }

    $applyOpacity(){
        if(!this.$three3D) return
        let mat = this.$three3D.material
        if(mat){
            mat.transparent = (this.opacity3D < 1)
            mat.opacity = this.opacity3D
        }
    }

    $transformChanged(){ this.$applyTransform() }
    $rotationChanged(){ this.$applyRotation(); if(this.$host3D) this.$host3D.$requestRender() }
    $quaternionChanged(){ this.$applyRotation(); if(this.$host3D) this.$host3D.$requestRender() }
    $visibility3DChanged(){ this.$applyVisibility(); if(this.$host3D) this.$host3D.$requestRender() }
    $opacity3DChanged(){ this.$applyOpacity(); if(this.$host3D) this.$host3D.$requestRender() }

    destroy(){
        if(this.$three3D){
            if(this.$three3D.parent) this.$three3D.parent.remove(this.$three3D)
            // dispose geometry / material if they're owned by us (subclass hook)
            if(typeof this.$disposeThree === 'function') this.$disposeThree()
            this.$three3D = null
        }
        if(this.$host3D && typeof this.$host3D.$unregisterNode3D === 'function'){
            this.$host3D.$unregisterNode3D(this)
        }
        super.destroy()
    }
}

module.exports.Node3D = Node3D
