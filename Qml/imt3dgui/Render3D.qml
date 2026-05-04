import QtQuick 2.15

Canvas {
    id: root

    property var camera: undefined
    property var environment: undefined
    property color clearColor: "#1a1a2e"
    default property list<QtObject> sceneNodes

    onPaint: __render()
    onSceneNodesChanged: requestPaint()
    onCameraChanged: requestPaint()
    onEnvironmentChanged: requestPaint()
    onClearColorChanged: requestPaint()
    onWidthChanged: requestPaint()
    onHeightChanged: requestPaint()

    function __render() {
        var ctx = getContext("2d")
        if (!ctx) return

        var w = width
        var h = height
        ctx.reset()
        ctx.clearRect(0, 0, w, h)

        // background
        var bg = environment ? environment.clearColor : clearColor
        ctx.fillStyle = "" + bg
        ctx.fillRect(0, 0, w, h)

        // camera
        var camPos = camera ? camera.position : Qt.vector3d(0, 200, 600)
        var camRot = camera ? camera.eulerRotation : Qt.vector3d(0, 0, 0)
        var fov = camera ? camera.fieldOfView : 60
        var aspect = w / (h > 0 ? h : 1)
        var fovRad = fov * Math.PI / 180.0
        var focalLen = (h / 2.0) / Math.tan(fovRad / 2.0)

        // collect lights
        var lights = []
        for (var li = 0; li < sceneNodes.length; ++li) {
            var ln = sceneNodes[li]
            if (__isLight(ln)) {
                lights.push(ln)
            }
        }
        // default light if none
        if (lights.length === 0) {
            lights.push({ position: Qt.vector3d(200, 400, 300), color: "#ffffff", brightness: 1.0 })
        }

        // render models
        for (var i = 0; i < sceneNodes.length; ++i) {
            var node = sceneNodes[i]
            if (__isModel(node)) {
                __renderModel(ctx, node, camPos, camRot, focalLen, w, h, lights)
            }
        }
    }

    function __isModel(obj) {
        return obj && obj.hasOwnProperty("source") && obj.hasOwnProperty("materials")
    }

    function __isLight(obj) {
        return obj && obj.hasOwnProperty("brightness") && obj.hasOwnProperty("color") && !obj.hasOwnProperty("source")
    }

    function __renderModel(ctx, model, camPos, camRot, focalLen, w, h, lights) {
        var src = model.source
        if (src !== "#Cube") return // only cubes for now

        var pos = model.position
        var rot = model.eulerRotation
        var scl = model.scale

        // cube half-size = 50 (unit cube is 100x100x100 matching Three.js convention)
        var s = 50
        var verts = [
            Qt.vector3d(-s, -s, -s), Qt.vector3d( s, -s, -s),
            Qt.vector3d( s,  s, -s), Qt.vector3d(-s,  s, -s),
            Qt.vector3d(-s, -s,  s), Qt.vector3d( s, -s,  s),
            Qt.vector3d( s,  s,  s), Qt.vector3d(-s,  s,  s)
        ]

        // faces: [indices, normal]
        var faces = [
            { idx: [0,1,2,3], normal: Qt.vector3d( 0, 0,-1) }, // front
            { idx: [5,4,7,6], normal: Qt.vector3d( 0, 0, 1) }, // back
            { idx: [4,0,3,7], normal: Qt.vector3d(-1, 0, 0) }, // left
            { idx: [1,5,6,2], normal: Qt.vector3d( 1, 0, 0) }, // right
            { idx: [3,2,6,7], normal: Qt.vector3d( 0, 1, 0) }, // top
            { idx: [4,5,1,0], normal: Qt.vector3d( 0,-1, 0) }  // bottom
        ]

        // transform vertices: scale -> rotate -> translate -> camera
        var transformed = []
        for (var vi = 0; vi < verts.length; ++vi) {
            var v = verts[vi]
            // scale
            v = Qt.vector3d(v.x * scl.x, v.y * scl.y, v.z * scl.z)
            // model rotation (euler Y then X then Z)
            v = __rotateEuler(v, rot)
            // translate
            v = Qt.vector3d(v.x + pos.x, v.y + pos.y, v.z + pos.z)
            // camera transform
            v = Qt.vector3d(v.x - camPos.x, v.y - camPos.y, v.z - camPos.z)
            v = __rotateEuler(v, Qt.vector3d(-camRot.x, -camRot.y, -camRot.z))
            transformed.push(v)
        }

        // transform normals
        var transNormals = []
        for (var ni = 0; ni < faces.length; ++ni) {
            var fn = faces[ni].normal
            fn = __rotateEuler(fn, rot)
            transNormals.push(fn)
        }

        // material
        var baseColor = "#cccccc"
        var metalness = 0
        var roughness = 1
        if (model.materials) {
            var mat = __getFirstMaterial(model.materials)
            if (mat) {
                baseColor = "" + (mat.baseColor ? mat.baseColor : (mat.diffuseColor ? mat.diffuseColor : "#cccccc"))
                metalness = mat.metalness !== undefined ? mat.metalness : 0
                roughness = mat.roughness !== undefined ? mat.roughness : 1
            }
        }

        // painter's algorithm: sort faces by average z depth (furthest first)
        var faceOrder = []
        for (var fi = 0; fi < faces.length; ++fi) {
            var faceIdx = faces[fi].idx
            var avgZ = 0
            for (var fj = 0; fj < faceIdx.length; ++fj) {
                avgZ += transformed[faceIdx[fj]].z
            }
            avgZ /= faceIdx.length
            faceOrder.push({ index: fi, depth: avgZ })
        }
        faceOrder.sort(function(a, b) { return a.depth - b.depth })

        // draw each face
        for (var di = 0; di < faceOrder.length; ++di) {
            var faceInfo = faces[faceOrder[di].index]
            var worldNormal = transNormals[faceOrder[di].index]

            // basic diffuse lighting
            var brightness = 0.15 // ambient
            for (var lj = 0; lj < lights.length; ++lj) {
                var light = lights[lj]
                var lightDir = __normalize(light.position ? light.position : Qt.vector3d(200, 400, 300))
                var diff = __dot(worldNormal, lightDir)
                if (diff < 0) diff = 0
                var lb = light.brightness !== undefined ? light.brightness : 1.0
                brightness += diff * lb
            }
            if (brightness > 1.0) brightness = 1.0

            // shade base color
            var shadedColor = __shadeColor(baseColor, brightness, metalness, roughness)

            // project face vertices to screen
            ctx.beginPath()
            var allVisible = true
            for (var pi = 0; pi < faceInfo.idx.length; ++pi) {
                var pt = transformed[faceInfo.idx[pi]]
                if (pt.z >= 0) { allVisible = false; break }
                var sx = (pt.x * focalLen / (-pt.z)) + w / 2
                var sy = -(pt.y * focalLen / (-pt.z)) + h / 2
                if (pi === 0) ctx.moveTo(sx, sy)
                else ctx.lineTo(sx, sy)
            }
            if (!allVisible) continue
            ctx.closePath()
            ctx.fillStyle = shadedColor
            ctx.fill()

            // subtle edge
            ctx.strokeStyle = __shadeColor(baseColor, brightness * 0.7, metalness, roughness)
            ctx.lineWidth = 1
            ctx.stroke()
        }
    }

    function __rotateEuler(v, euler) {
        // rotation order: Y -> X -> Z (matching QtQuick3D convention)
        var ax = euler.x * Math.PI / 180
        var ay = euler.y * Math.PI / 180
        var az = euler.z * Math.PI / 180

        // Y
        var cosY = Math.cos(ay); var sinY = Math.sin(ay)
        var x1 = v.x * cosY + v.z * sinY
        var z1 = -v.x * sinY + v.z * cosY
        v = Qt.vector3d(x1, v.y, z1)

        // X
        var cosX = Math.cos(ax); var sinX = Math.sin(ax)
        var y2 = v.y * cosX - v.z * sinX
        var z2 = v.y * sinX + v.z * cosX
        v = Qt.vector3d(v.x, y2, z2)

        // Z
        var cosZ = Math.cos(az); var sinZ = Math.sin(az)
        var x3 = v.x * cosZ - v.y * sinZ
        var y3 = v.x * sinZ + v.y * cosZ
        v = Qt.vector3d(x3, y3, v.z)

        return v
    }

    function __normalize(v) {
        var len = Math.sqrt(v.x * v.x + v.y * v.y + v.z * v.z)
        if (len < 0.0001) return Qt.vector3d(0, 0, 0)
        return Qt.vector3d(v.x / len, v.y / len, v.z / len)
    }

    function __dot(a, b) {
        return a.x * b.x + a.y * b.y + a.z * b.z
    }

    function __shadeColor(baseColorStr, brightness, metalness, roughness) {
        var c = __parseColor(baseColorStr)
        // simple PBR approximation: metalness darkens diffuse, roughness reduces specular highlight
        var specular = (1.0 - roughness) * 0.3
        var f = brightness + specular
        if (f > 1.0) f = 1.0
        var r = Math.round(Math.min(255, c.r * f * (1.0 - metalness * 0.5)))
        var g = Math.round(Math.min(255, c.g * f * (1.0 - metalness * 0.5)))
        var b = Math.round(Math.min(255, c.b * f * (1.0 - metalness * 0.5)))
        return "rgb(" + r + "," + g + "," + b + ")"
    }

    function __parseColor(str) {
        // handle #rrggbb, #rrggbbaa, and named colors
        str = "" + str
        if (str.charAt(0) === '#') {
            var hex = str.substring(1)
            if (hex.length >= 6) {
                return {
                    r: parseInt(hex.substring(0, 2), 16),
                    g: parseInt(hex.substring(2, 4), 16),
                    b: parseInt(hex.substring(4, 6), 16)
                }
            }
        }
        // fallback
        return { r: 200, g: 200, b: 200 }
    }

    function __getFirstMaterial(mats) {
        if (!mats) return null
        // materials can be a list or an array
        if (typeof mats.length !== "undefined" && mats.length > 0) return mats[0]
        if (typeof mats === "object" && mats.hasOwnProperty("baseColor")) return mats
        return null
    }
}
