// PathView of QtQuick (qquickpathview.cpp, Qt 6.12): the delegates of a model laid out along a Path.
//  - offset: where the items are on the path (an index i is at positionOfIndex: (i + offset) mod count, from the start
//    of the highlight range); pathItemCount items are on the path (all without it), the others at 100% off it;
//  - StrictlyEnforceRange (the default with a highlight range): the current index follows the offset
//    (calcCurrentIndex), a new current index moves the offset to it (snapToIndex: highlightMoveDuration, the shorter
//    way round);
//  - a press on an item (or within dragMargin of the path) and a move past startDragDistance along the path drag the
//    offset by the percent of the path moved; the release flicks with the speed of the last moves (deceleration
//    flickDeceleration / 10 items per second²), then snaps (fixOffset);
//  - the attached PathView: view, isCurrentItem, onPath, and the PathAttribute values at the item's place.
const engine = require('../Core/engine')
const { defineType } = require('../Core/type')
const { GroupObject } = require('../Core/QtObject')
const { Item } = require('./Item')
const { Path } = require('./Path')
const { ModelContext, modelRows } = require('../QtQml/Models/ModelContext')
const { clock, DRAG_THRESHOLD } = require('./Flickable')

const NoHighlightRange = 0, ApplyRange = 1, StrictlyEnforceRange = 2
const NoSnap = 0, SnapToItem = 1, SnapOneItem = 2
const Shortest = 0, Negative = 1, Positive = 2
const Beginning = 0, Center = 1, End = 2, Contain = 4, SnapPosition = 5
const OTHER = 0, SET_INDEX = 1, MOUSE = 2
const MIN_FLICK_VELOCITY = 75           // _q_MinimumFlickVelocity
const FLICK_SAMPLEBUFFER = 3, FLICK_DISCARDSAMPLES = 0, FLICK_VELOCITY_DECAY_TIME = 50
const fmod = (a, n) => a % n
const qRound = (d) => (d >= 0 ? Math.floor(d + 0.5) : Math.ceil(d - 0.5))
const fuzzyCompare = (a, b) => Math.abs(a - b) * 1000000000000 <= Math.min(Math.abs(a), Math.abs(b))
const fuzzyIsNull = (a) => Math.abs(a) <= 0.000000000001

const EASING = {
    linear: (t) => t,
    inQuad: (t) => t * t,
    outQuad: (t) => 1 - (1 - t) * (1 - t),
    inOutQuad: (t) => (t < 0.5 ? 2 * t * t : 1 - Math.pow(-2 * t + 2, 2) / 2),
}

// ---- attached PathView ----

class PathViewAttached extends GroupObject {
    constructor(owner){
        super(owner)
        this.$percent = -1
        this.$values = new Map()
        // a delegate being made (QQuickPathView::initItem, before its bindings): the view, the attributes at its place
        let view = owner && owner.$parent
        if(view instanceof PathView && view.$requested) view.$initAttached(this, view.$requested.index, view.$requested.z)
    }

    // a value of a PathAttribute: a property of the attached object (QQuickPathViewAttached::setValue)
    $setValue(name, value){
        if(!Object.prototype.hasOwnProperty.call(this, name)){
            Object.defineProperty(this, name, {
                get(){
                    engine.track(this, PathViewAttached.SIGNAL_valuesChanged)
                    return this.$values.get(name)
                },
                configurable: true,
            })
        }
        if(this.$values.has(name) && this.$values.get(name) === value) return
        this.$values.set(name, value)
        engine.emit(this, PathViewAttached.SIGNAL_valuesChanged)
    }
}
defineType(PathViewAttached, {
    props: {
        view: { kind: 'var', readonly: true },
        isCurrentItem: { kind: 'bool', readonly: true },
        onPath: { kind: 'bool', readonly: true },
    },
    signals: ['valuesChanged'],
})

// ---- PathView ----

class PathView extends Item {
    constructor(parent = null){
        super(parent)
        this.$isFocusScope = true
        this.$model = null
        this.$modelCount = 0
        this.$items = []                // the delegates on (or near) the path, in path order
        this.$refs = new Map()          // index -> { item, ctx, refs }
        this.$currentItem = null
        this.$currentItemOffset = 0
        this.$offset = 0
        this.$offsetAdj = 0
        this.$mappedRange = 1
        this.$mappedCache = 0
        this.$cacheSize = 0
        this.$moveReason = OTHER
        this.$moveDirection = Shortest
        this.$highlightPosition = 0
        this.$highlightUp = false
        this.$tl = null                 // the running timeline of the offset or the highlight
        this.$inRefill = false
        this.$stealMouse = false
        this.$timerValid = false
        this.$velocity = []
        this.$flickDuration = 0
        engine.scheduleComponentComplete(this)
    }

    // ---- state ----

    $count(){ return this.$model !== null && this.$modelCount > 0 ? this.$modelCount : 0 }
    $path(){ let p = this.$v[PathView.PATH]; return p instanceof Path ? p : null }
    $valid(){ return this.$modelCount > 0 && !!this.$path() && this.$v[PathView.DELEGATE] !== null }
    $pathItems(){ let n = this.$v[PathView.PATHITEMCOUNT]; return n === undefined || n < 0 ? -1 : n }
    $haveRange(){ return this.$v[PathView.PREFERREDHIGHLIGHTBEGIN] <= this.$v[PathView.PREFERREDHIGHLIGHTEND] }
    $rangeMode(){ return this.$v[PathView.HIGHLIGHTRANGEMODE] }
    $strict(){ return this.$haveRange() && this.$rangeMode() === StrictlyEnforceRange }

    $updateMappedRange(){
        let pathItems = this.$pathItems()
        this.$cacheSize = Math.max(0, Math.min(this.$v[PathView.CACHEITEMCOUNT], this.$modelCount - pathItems))
        if(pathItems !== -1 && pathItems < this.$modelCount){
            this.$mappedRange = this.$modelCount / pathItems
            this.$mappedCache = this.$cacheSize / pathItems / 2
        } else {
            this.$mappedRange = 1
            this.$mappedCache = 0
        }
    }

    // QQuickPathViewPrivate::positionOfIndex
    $positionOfIndex(index){
        let count = this.$modelCount
        if(!(index >= 0 && index < count)) return -1
        let start = 0
        if(this.$haveRange() && (this.$rangeMode() !== NoHighlightRange || this.$v[PathView.SNAPMODE] !== NoSnap)) start = this.$v[PathView.PREFERREDHIGHLIGHTBEGIN]
        let globalPos = fmod(index + this.$offset, count) / count
        let pathItems = this.$pathItems()
        if(pathItems !== -1 && pathItems < count){
            globalPos += start / this.$mappedRange
            globalPos = fmod(globalPos, 1)
            return globalPos * this.$mappedRange
        }
        return fmod(globalPos + start, 1)
    }

    $isInBound(position, lower, upper, emptyRangeCheck = true){
        if(emptyRangeCheck && fuzzyCompare(lower, upper)) return true
        if(lower > upper){
            if(position > upper && position > lower) position -= this.$mappedRange
            lower -= this.$mappedRange
        }
        return position >= lower && position < upper
    }

    // ---- items ----

    $getItem(index, z){
        let entry = this.$refs.get(index)
        if(entry){
            entry.refs++
            return entry.item
        }
        let delegate = this.$v[PathView.DELEGATE]
        if(!delegate) return null
        let rows = modelRows(this.$model)
        let ctx = new ModelContext(rows.row(index), index, rows.data(index))
        ctx.$view = this
        let previous = engine.beginCreate()
        let item = null
        this.$requested = { index, z }
        try {
            item = delegate.createObject(this, null, ctx)
            if(item){
                // QQuickPathView::initItem: the attached object before the delegate is complete
                let att = item.PathView
                if(!att.$inited) this.$initAttached(att, index, z)
                engine.creatingRoot(item)
            }
        } finally {
            this.$requested = null
            engine.endCreate(previous)
        }
        if(!item) return null
        this.$refs.set(index, { item, ctx, refs: 1 })
        // QQuickPathViewPrivate::itemGeometryChanged: a delegate of a new size is put again
        let relayout = () => {
            let att = this.$attached(item)
            if(att) att.$percent = -1
            this.$scheduleLayout()
        }
        engine.connect(item, Item.SIGNAL_widthChanged, relayout, this)
        engine.connect(item, Item.SIGNAL_heightChanged, relayout, this)
        return item
    }

    $initAttached(att, index, z){
        att.$inited = true
        att.$write(PathViewAttached.VIEW, this)
        let percent = this.$positionOfIndex(index)
        let path = this.$path()
        // the attached type has a property for each attribute of the path (QQuickPathViewPrivate::attachedType)
        if(path) for(let name of path.$attributes()) att.$setValue(name, undefined)
        if(percent < 1 && path){
            for(let name of path.$attributes()) att.$setValue(name, path.$attributeAt(name, percent))
            if(att.$owner) att.$owner.$write(Item.Z, z)
        }
        att.$write(PathViewAttached.ONPATH, percent < 1)
    }

    $indexOf(item){
        for(let [index, entry] of this.$refs) if(entry.item === item) return index
        return -1
    }

    $releaseItem(item){
        if(!item) return
        let index = this.$indexOf(item)
        if(index < 0) return
        let entry = this.$refs.get(index)
        if(--entry.refs > 0) return
        this.$refs.delete(index)
        let att = item.$v[Item.$type.propByName.get('PathView').id]
        if(att) att.$write(PathViewAttached.ONPATH, false)
        item.$releaseLater()
    }

    $releaseCurrentItem(){
        let item = this.$currentItem
        this.$currentItem = null
        this.$releaseItem(item)
    }

    $clear(){
        this.$releaseCurrentItem()
        for(let item of this.$items) this.$releaseItem(item)
        this.$items = []
        this.$stopTimeline()
    }

    $attached(item){
        if(!item) return null
        let id = Item.$type.propByName.get('PathView').id
        return item.$v[id] || null
    }

    // QQuickPathViewPrivate::updateItem: the attributes at percent, onPath, the item centered on the point
    $updateItem(item, percent){
        let path = this.$path()
        if(!path) return
        let att = this.$attached(item)
        if(att){
            if(att.$percent === percent) return
            att.$percent = percent
            for(let name of path.$attributes()) att.$setValue(name, path.$attributeAt(name, percent))
            att.$write(PathViewAttached.ONPATH, percent < 1)
        }
        // setCulled: an item off the path is not drawn
        item.$culled = percent >= 1
        if(item.$dom) item.$dom.style.visibility = item.$culled ? 'hidden' : ''
        let p = path.$pointAt(Math.min(percent, 1))
        item.$write(Item.X, p.x - item.$v[Item.WIDTH] / 2)
        item.$write(Item.Y, p.y - item.$v[Item.HEIGHT] / 2)
    }

    $scheduleLayout(){
        if(this.$layoutScheduled) return
        this.$layoutScheduled = true
        engine.queuePolish(this)
    }

    $polish(){ this.$refill() }

    // QQuickPathView::refill
    $refill(){
        if(this.$inRefill){
            this.$scheduleLayout()
            return
        }
        this.$layoutScheduled = false
        if(!this.$valid() || !(this.$f & engine.COMPLETED)) return
        this.$inRefill = true
        try {
            this.$refillItems()
        } finally {
            this.$inRefill = false
        }
    }

    $refillItems(){
        let count = this.$modelCount
        let pathItems = this.$pathItems()
        let shown = pathItems === -1 ? count : Math.min(pathItems, count)
        let currentVisible = false
        let current = this.$v[PathView.CURRENTINDEX]
        // the items that stay move, those far off the path go
        for(let i = 0; i < this.$items.length;){
            let item = this.$items[i]
            let idx = this.$indexOf(item)
            let pos = this.$positionOfIndex(idx)
            this.$updateItem(item, pos)
            if(pos < 1){
                if(idx === current){
                    currentVisible = true
                    this.$currentItemOffset = pos
                }
                i++
            } else if(!this.$isInBound(pos, this.$mappedRange - this.$mappedCache, 1 + this.$mappedCache)){
                this.$releaseItem(item)
                this.$items.splice(i, 1)
            } else i++
        }
        let limit = shown + this.$cacheSize
        if(count && this.$items.length < limit){
            let endIdx = 0, endPos, startIdx = 0, startPos = 0
            let wasEmpty = !this.$items.length
            if(!wasEmpty){
                endPos = -1
                startPos = 2
                for(let item of this.$items){
                    let idx = this.$indexOf(item)
                    let pos = this.$positionOfIndex(idx)
                    if(pos > endPos){ endPos = pos; endIdx = idx }
                    if(pos < startPos){ startPos = pos; startIdx = idx }
                }
            } else {
                if(this.$haveRange() && (this.$rangeMode() !== NoHighlightRange || this.$v[PathView.SNAPMODE] !== NoSnap)) startPos = this.$v[PathView.PREFERREDHIGHLIGHTBEGIN]
                endIdx = Math.max(-1, (qRound(count - this.$offset) - 1) % count)
                endPos = this.$positionOfIndex(endIdx)
            }
            // append
            let idx = endIdx + 1
            if(idx >= count) idx = 0
            let nextPos = this.$positionOfIndex(idx)
            while((this.$isInBound(nextPos, endPos, 1 + this.$mappedCache, false) || !this.$items.length) && this.$items.length < limit){
                let item = this.$getItem(idx, idx + 1)
                if(!item) break
                if(this.$items.indexOf(item) >= 0){
                    this.$releaseItem(item)
                    break
                }
                if(current === idx){
                    currentVisible = true
                    this.$currentItemOffset = nextPos
                }
                this.$items.push(item)
                this.$updateItem(item, nextPos)
                endPos = nextPos
                if(++idx >= count) idx = 0
                nextPos = this.$positionOfIndex(idx)
            }
            // prepend
            idx = (wasEmpty ? this.$calcCurrentIndex() : startIdx) - 1
            if(idx < 0) idx = count - 1
            nextPos = this.$positionOfIndex(idx)
            while(this.$isInBound(nextPos, this.$mappedRange - this.$mappedCache, startPos) && this.$items.length < limit){
                let item = this.$getItem(idx, idx + 1)
                if(!item) break
                if(this.$items.indexOf(item) >= 0){
                    this.$releaseItem(item)
                    break
                }
                if(current === idx){
                    currentVisible = true
                    this.$currentItemOffset = nextPos
                }
                this.$items.unshift(item)
                this.$updateItem(item, nextPos)
                startIdx = idx
                startPos = nextPos
                if(--idx < 0) idx = count - 1
                nextPos = this.$positionOfIndex(idx)
            }
            // the holes left in the middle
            if(this.$items.length < limit){
                idx = startIdx
                let lastItem = this.$items[0]
                for(let i = 0; i < count && this.$items.length < limit; i++){
                    nextPos = this.$positionOfIndex(idx)
                    if(this.$isInBound(nextPos, this.$mappedRange - this.$mappedCache, 1 + this.$mappedCache)){
                        let item = this.$getItem(idx, idx + 1)
                        if(!item) break
                        if(this.$items.indexOf(item) < 0){
                            if(current === idx){
                                currentVisible = true
                                this.$currentItemOffset = nextPos
                            }
                            this.$items.splice(this.$items.indexOf(lastItem) + 1, 0, item)
                            this.$updateItem(item, nextPos)
                        } else this.$releaseItem(item)
                        lastItem = item
                    }
                    if(++idx >= count) idx = 0
                }
            }
        }
        let currentChanged = false
        if(!currentVisible){
            this.$currentItemOffset = 1
            if(this.$currentItem) this.$updateItem(this.$currentItem, 1)
            else if(current >= 0 && current < count && (this.$currentItem = this.$getItem(current, current))){
                currentChanged = true
                this.$updateItem(this.$currentItem, 1)
                this.$attached(this.$currentItem).$write(PathViewAttached.ISCURRENTITEM, true)
            }
        } else if(!this.$currentItem && (this.$currentItem = this.$getItem(current, current))){
            currentChanged = true
            this.$currentItem.$setFocus && this.$currentItem.$setFocus(true)
            this.$attached(this.$currentItem).$write(PathViewAttached.ISCURRENTITEM, true)
        }
        let highlight = this.$highlightItem
        if(highlight && this.$strict()){
            this.$updateItem(highlight, this.$v[PathView.PREFERREDHIGHLIGHTBEGIN])
            let att = this.$attached(highlight)
            if(att) att.$write(PathViewAttached.ONPATH, true)
        } else if(highlight && this.$moveReason !== SET_INDEX){
            this.$updateItem(highlight, this.$currentItemOffset)
            let att = this.$attached(highlight)
            if(att) att.$write(PathViewAttached.ONPATH, currentVisible)
        }
        if(currentChanged) engine.emit(this, PathView.SIGNAL_currentItemChanged)
    }

    $regenerate(){
        if(!(this.$f & engine.COMPLETED)) return
        this.$clear()
        if(!this.$valid()) return
        this.$updateMappedRange()
        this.$refill()
    }

    // ---- the current index ----

    $calcCurrentIndex(){
        let count = this.$modelCount
        if(!count || !this.$items.length) return 0
        this.$offset = fmod(this.$offset, count)
        if(this.$offset < 0) this.$offset += count
        return qRound(Math.abs(fmod(count - this.$offset, count))) % count
    }

    $createCurrentItem(){
        let current = this.$v[PathView.CURRENTINDEX]
        let inItems = this.$items.some(item => this.$indexOf(item) === current)
        if(inItems){
            if((this.$currentItem = this.$getItem(current, current))){
                if(this.$currentItem.$setFocus) this.$currentItem.$setFocus(true)
                this.$attached(this.$currentItem).$write(PathViewAttached.ISCURRENTITEM, true)
            }
        } else if(current >= 0 && current < this.$modelCount){
            if((this.$currentItem = this.$getItem(current, current))){
                this.$updateItem(this.$currentItem, 1)
                this.$attached(this.$currentItem).$write(PathViewAttached.ISCURRENTITEM, true)
            }
        }
    }

    // QQuickPathViewPrivate::updateCurrent: the current index follows the offset (StrictlyEnforceRange)
    $updateCurrent(){
        if(this.$moveReason === SET_INDEX) return
        if(!this.$modelCount || !this.$strict()) return
        let idx = this.$calcCurrentIndex()
        if(idx !== this.$v[PathView.CURRENTINDEX] || !this.$currentItem){
            if(this.$currentItem){
                let att = this.$attached(this.$currentItem)
                if(att) att.$write(PathViewAttached.ISCURRENTITEM, false)
                this.$releaseCurrentItem()
            }
            let old = this.$v[PathView.CURRENTINDEX]
            this.$v[PathView.CURRENTINDEX] = idx
            this.$currentItem = null
            this.$createCurrentItem()
            if(old !== idx) engine.emit(this, PathView.SIGNAL_currentIndexChanged)
            engine.emit(this, PathView.SIGNAL_currentItemChanged)
        }
    }

    // QQuickPathView::setCurrentIndex
    $setCurrentIndex(idx){
        if(!(this.$f & engine.COMPLETED)){
            if(idx !== this.$v[PathView.CURRENTINDEX]){
                this.$v[PathView.CURRENTINDEX] = idx
                engine.emit(this, PathView.SIGNAL_currentIndexChanged)
            }
            return
        }
        let count = this.$modelCount
        idx = count ? ((idx % count) + count) % count : 0
        if(this.$model !== null && (idx !== this.$v[PathView.CURRENTINDEX] || !this.$currentItem)){
            let hadCurrent = !!this.$currentItem
            let old = this.$v[PathView.CURRENTINDEX]
            if(hadCurrent){
                let att = this.$attached(this.$currentItem)
                if(att) att.$write(PathViewAttached.ISCURRENTITEM, false)
                this.$releaseCurrentItem()
            }
            this.$moveReason = SET_INDEX
            this.$v[PathView.CURRENTINDEX] = idx
            if(count){
                this.$createCurrentItem()
                if(this.$strict()) this.$snapToIndex(idx, SET_INDEX)
                this.$currentItemOffset = this.$positionOfIndex(idx)
                this.$updateHighlight()
            }
            if(old !== idx) engine.emit(this, PathView.SIGNAL_currentIndexChanged)
            if(hadCurrent) engine.emit(this, PathView.SIGNAL_currentItemChanged)
        }
    }

    // ---- the offset ----

    $setOffset(o){
        if(fuzzyCompare(this.$offset, o)) return
        let count = this.$modelCount
        if(this.$valid() && this.$f & engine.COMPLETED){
            let offset = fmod(o, count)
            if(offset < 0) offset += count
            this.$offset = offset
            this.$refill()
        } else this.$offset = o
        engine.emit(this, PathView.SIGNAL_offsetChanged)
    }

    $setAdjustedOffset(o){ this.$setOffset(o + this.$offsetAdj) }

    // ---- the timeline (QQuickTimeLine of the offset or the highlight position) ----

    // segments: { to, easing, duration } moves, { set } values, { accel: { velocity, accel, distance } }
    $runTimeline(from, segments, write, done){
        // QQuickTimeLine::move of no time does nothing
        segments = segments.filter(seg => seg.set !== undefined || seg.accel || seg.duration > 0)
        this.$stopTimeline()
        let tl = { segments, index: 0, from, start: clock.now() }
        this.$tl = tl
        let value = from
        let step = () => {
            if(this.$tl !== tl || this.$f & engine.DESTROYED || engine.isStopped()) return
            engine.batch(() => {
                let now = clock.now()
                while(tl.index < segments.length){
                    let seg = segments[tl.index]
                    if(seg.set !== undefined){
                        value = seg.set
                        write(value)
                        tl.index++
                        tl.from = value
                        tl.start = now
                        continue
                    }
                    let duration = seg.duration
                    let elapsed = now - tl.start
                    if(seg.accel){
                        let a = seg.accel
                        let t = elapsed / 1000
                        let stop = a.accel > 0 ? Math.abs(a.velocity) / a.accel : 0
                        let sign = a.velocity < 0 ? -1 : 1
                        let tEnd = stop
                        let travelled = (tt) => sign * (Math.abs(a.velocity) * tt - a.accel * tt * tt / 2)
                        if(a.distance !== undefined && a.accel > 0){
                            // QQuickTimeLine::accel with maxDistance: it stops there
                            let full = a.velocity * a.velocity / (2 * a.accel)
                            if(full > a.distance) tEnd = (Math.abs(a.velocity) - Math.sqrt(Math.max(0, a.velocity * a.velocity - 2 * a.accel * a.distance))) / a.accel
                        }
                        if(t >= tEnd){
                            value = tl.from + travelled(tEnd)
                            write(value)
                            tl.index++
                            tl.from = value
                            tl.start = tl.start + tEnd * 1000
                            continue
                        }
                        value = tl.from + travelled(t)
                        write(value)
                        break
                    }
                    if(duration <= 0 || elapsed >= duration){
                        value = seg.to
                        write(value)
                        tl.index++
                        tl.from = value
                        tl.start = tl.start + Math.max(0, duration)
                        continue
                    }
                    value = tl.from + (seg.to - tl.from) * seg.easing(elapsed / duration)
                    write(value)
                    break
                }
                if(tl.index >= segments.length){
                    this.$tl = null
                    if(done) done()
                }
            })
            if(this.$tl === tl) clock.frame(step)
        }
        clock.frame(step)
    }

    $stopTimeline(){ this.$tl = null }

    $moveOffsetWrite(){
        return (value) => {
            this.$setAdjustedOffset(value)
            // QQuickPathView::ticked
            this.$updateCurrent()
        }
    }

    // QQuickPathViewPrivate::snapToIndex
    $snapToIndex(index, reason){
        let count = this.$modelCount
        if(this.$model === null || count <= 0) return
        let target = fmod(count - index, count)
        this.$moveReason = reason
        this.$offsetAdj = 0
        let offset = this.$offset
        let duration = this.$v[PathView.HIGHLIGHTMOVEDURATION]
        let pathItems = this.$pathItems()
        let shown = pathItems === -1 ? count : Math.min(pathItems, count)
        let length = this.$path() ? this.$path().$geometry().length : 0
        let threshold = 0.5 / (length / shown)
        let dir = this.$moveDirection
        let segs
        if(!duration || Math.abs(offset - target) < threshold || (fuzzyIsNull(target) && Math.abs(count - offset) < threshold)){
            segs = [{ set: target }]
        } else if(dir === Positive || (dir === Shortest && target - offset > count / 2)){
            let distance = count - target + offset
            if(target > offset){
                segs = [
                    { to: 0, easing: EASING.inQuad, duration: Math.trunc(duration * offset / distance) },
                    { set: count },
                    { to: target, easing: fuzzyIsNull(offset) ? EASING.inOutQuad : EASING.outQuad, duration: Math.trunc(duration * (count - target) / distance) },
                ]
            } else segs = [{ to: target, easing: EASING.inOutQuad, duration }]
        } else if(dir === Negative || target - offset <= -count / 2){
            let distance = count - offset + target
            if(target < offset){
                segs = [
                    { to: count, easing: fuzzyIsNull(target) ? EASING.inOutQuad : EASING.inQuad, duration: Math.trunc(duration * (count - offset) / distance) },
                    { set: 0 },
                    { to: target, easing: EASING.outQuad, duration: Math.trunc(duration * target / distance) },
                ]
            } else segs = [{ to: target, easing: EASING.inOutQuad, duration }]
        } else segs = [{ to: target, easing: EASING.inOutQuad, duration }]
        // tl.set of the target at once (no animation): the value is written now, as QQuickTimeLine::set of a value
        if(segs.length === 1 && segs[0].set !== undefined){
            this.$stopTimeline()
            this.$moveOffsetWrite()(target)
            this.$movementEnding()
            return
        }
        this.$runTimeline(offset, segs, this.$moveOffsetWrite(), () => this.$movementEnding())
    }

    // QQuickPathViewPrivate::fixOffset
    $fixOffset(){
        if(this.$model === null || !this.$items.length) return
        if(this.$haveRange() && (this.$rangeMode() === StrictlyEnforceRange || this.$v[PathView.SNAPMODE] !== NoSnap)){
            let curr = this.$calcCurrentIndex()
            if(curr !== this.$v[PathView.CURRENTINDEX] && this.$rangeMode() === StrictlyEnforceRange) this.$setCurrentIndex(curr)
            else this.$snapToIndex(curr, OTHER)
        }
    }

    // QQuickPathView::movementEnding
    $movementEnding(){
        if(this.$v[PathView.FLICKING]){
            this.$write(PathView.FLICKING, false)
            engine.emit(this, PathView.SIGNAL_flickEnded)
        }
        if(this.$v[PathView.MOVING] && !this.$stealMouse){
            this.$write(PathView.MOVING, false)
            engine.emit(this, PathView.SIGNAL_movementEnded)
        }
        this.$moveDirection = this.$v[PathView.MOVEMENTDIRECTION]
    }

    // ---- highlight ----

    $createHighlight(){
        if(!(this.$f & engine.COMPLETED)) return
        let changed = false
        if(this.$highlightItem){
            this.$highlightItem.$setParent(null)
            this.$highlightItem.destroy()
            this.$highlightItem = null
            changed = true
        }
        let component = this.$v[PathView.HIGHLIGHT]
        let item = null
        if(component) item = component.createObject(this)
        else item = new Item(this)
        if(item){
            this.$highlightItem = item
            changed = true
        }
        if(changed) engine.emit(this, PathView.SIGNAL_highlightItemChanged)
    }

    $updateHighlight(){
        if(!(this.$f & engine.COMPLETED) || !this.$valid()) return
        let highlight = this.$highlightItem
        if(!highlight) return
        if(this.$strict()){
            this.$updateItem(highlight, this.$v[PathView.PREFERREDHIGHLIGHTBEGIN])
            return
        }
        let target = this.$v[PathView.CURRENTINDEX]
        let count = this.$modelCount
        let pos = this.$highlightPosition
        let duration = this.$v[PathView.HIGHLIGHTMOVEDURATION]
        this.$offsetAdj = 0
        let segs
        if(target - pos > count / 2){
            this.$highlightUp = false
            let distance = count - target + pos
            segs = [
                { to: 0, easing: EASING.inQuad, duration: Math.trunc(duration * pos / distance) },
                { set: count - 0.01 },
                { to: target, easing: EASING.outQuad, duration: Math.trunc(duration * (count - target) / distance) },
            ]
        } else if(target - pos <= -count / 2){
            this.$highlightUp = true
            let distance = count - pos + target
            segs = [
                { to: count - 0.01, easing: EASING.inQuad, duration: Math.trunc(duration * (count - pos) / distance) },
                { set: 0 },
                { to: target, easing: EASING.outQuad, duration: Math.trunc(duration * target / distance) },
            ]
        } else {
            this.$highlightUp = pos - target < 0
            segs = [{ to: target, easing: EASING.inOutQuad, duration }]
        }
        this.$runTimeline(pos, segs, (value) => this.$setHighlightPosition(value), null)
    }

    // QQuickPathViewPrivate::setHighlightPosition
    $setHighlightPosition(pos){
        if(fuzzyCompare(pos, this.$highlightPosition)) return
        let start = 0, end = 1
        if(this.$haveRange() && this.$rangeMode() !== NoHighlightRange){
            start = this.$v[PathView.PREFERREDHIGHLIGHTBEGIN]
            end = this.$v[PathView.PREFERREDHIGHLIGHTEND]
        }
        let range = this.$modelCount
        let relative = fmod(pos + this.$offset, range) / range
        if(!this.$highlightUp && relative > end / this.$mappedRange){
            this.$setOffset(this.$offset + (1 - relative) * range)
        } else if(this.$highlightUp && relative >= (end - start) / this.$mappedRange){
            this.$setOffset(this.$offset - (relative - (end - start) / this.$mappedRange) * range - 0.00001)
        }
        this.$highlightPosition = pos
        let pathPos = this.$positionOfIndex(pos)
        this.$updateItem(this.$highlightItem, pathPos)
        let att = this.$attached(this.$highlightItem)
        if(att) att.$write(PathViewAttached.ONPATH, pathPos < 1)
    }

    // ---- model ----

    $setModel(model){
        if(this.$model && this.$model.$removeView) this.$model.$removeView(this)
        this.$clear()
        this.$model = model === undefined ? null : model
        if(this.$model && this.$model.$addView) this.$model.$addView(this)
        let old = this.$modelCount
        this.$modelCount = this.$model !== null ? modelRows(this.$model).count : 0
        if(this.$f & engine.COMPLETED){
            if(this.$v[PathView.CURRENTINDEX] !== 0){
                this.$v[PathView.CURRENTINDEX] = 0
                engine.emit(this, PathView.SIGNAL_currentIndexChanged)
            }
            if(!fuzzyIsNull(this.$offset)){
                this.$offset = 0
                engine.emit(this, PathView.SIGNAL_offsetChanged)
            }
        }
        this.$regenerate()
        if(this.$modelCount !== old) engine.emit(this, PathView.SIGNAL_countChanged)
    }

    // QQuickPathView::modelUpdated
    $modelChanged(kind, index, count, roles, to){
        if(!this.$path() || !(this.$f & engine.COMPLETED)) return
        let rows = modelRows(this.$model)
        if(kind === 'change'){
            for(let [i, entry] of this.$refs){
                if(i < index || i >= index + count) continue
                entry.ctx.$row = rows.row(i)
                entry.ctx.$write(ModelContext.MODELDATA, rows.data(i))
                entry.ctx.$rowChanged(roles)
            }
            return
        }
        if(kind !== 'insert' && kind !== 'remove'){
            this.$modelCount = rows.count
            this.$regenerate()
            engine.emit(this, PathView.SIGNAL_countChanged)
            return
        }
        let modelCount = this.$modelCount
        let current = this.$v[PathView.CURRENTINDEX]
        let currentChanged = false, changedOffset = false
        if(kind === 'remove'){
            if(current >= index + count){
                current -= count
                currentChanged = true
            } else if(current >= index && current < index + count){
                if(this.$currentItem){
                    let att = this.$attached(this.$currentItem)
                    if(att) att.$write(PathViewAttached.ISCURRENTITEM, true)
                    this.$releaseCurrentItem()
                }
                current = Math.min(index, this.$modelCount - count - 1)
                currentChanged = true
            }
            if(index > current){
                changedOffset = true
                this.$offset -= count
                this.$offsetAdj -= count
            }
            this.$modelCount -= count
        } else {
            if(this.$modelCount){
                if(index <= current){
                    current += count
                    currentChanged = true
                } else if(index > current){
                    this.$offset += count
                    this.$offsetAdj += count
                    changedOffset = true
                }
            }
            this.$modelCount += count
        }
        this.$offset = fmod(this.$offset, this.$modelCount || 1)
        if(this.$offset < 0) this.$offset += this.$modelCount
        if(current === -1) current = this.$calcCurrentIndex()
        this.$v[PathView.CURRENTINDEX] = current
        // the delegates are made again for the new rows (their indexes moved)
        for(let item of this.$items) this.$releaseItem(item)
        this.$items = []
        if(this.$currentItem) this.$releaseCurrentItem()
        if(!this.$modelCount){
            this.$offset = 0
            changedOffset = true
            this.$stopTimeline()
        } else {
            if(!this.$v[PathView.FLICKING] && !this.$v[PathView.MOVING] && this.$strict()){
                this.$offset = fmod(this.$modelCount - current, this.$modelCount)
                changedOffset = true
            }
            this.$updateMappedRange()
            this.$scheduleLayout()
        }
        if(changedOffset) engine.emit(this, PathView.SIGNAL_offsetChanged)
        if(currentChanged) engine.emit(this, PathView.SIGNAL_currentIndexChanged)
        if(this.$modelCount !== modelCount) engine.emit(this, PathView.SIGNAL_countChanged)
    }

    // ---- complete ----

    $componentComplete(){
        super.$componentComplete()
        let current = this.$v[PathView.CURRENTINDEX]
        if(this.$model !== null){
            this.$modelCount = modelRows(this.$model).count
            if(this.$modelCount && current !== 0){
                let rem = current < 0 ? this.$modelCount + current % this.$modelCount : current % this.$modelCount
                this.$offset = fmod(this.$modelCount - rem, this.$modelCount)
            }
        }
        this.$createHighlight()
        this.$regenerate()
        this.$updateHighlight()
        this.$updateCurrent()
        if(this.$modelCount) engine.emit(this, PathView.SIGNAL_countChanged)
    }

    // ---- the pointer (QQuickPathViewPrivate::handleMouse*Event, childMouseEventFilter) ----

    // QQuickPathViewPrivate::pointNear: the point of the path nearest to (x, y) and its percent
    $pointNear(x, y){
        let path = this.$path()
        let length = path.$geometry().length
        let samples = Math.min(length / 5, 500)
        let res = length / samples
        let best = 1e10, nearPc = 0, near = path.$pointAt(0)
        for(let i = 1; i < samples; i++){
            let p = path.$pointAt(i / samples)
            let d = (p.x - x) * (p.x - x) + (p.y - y) * (p.y - y)
            if(d < best){ best = d; near = p; nearPc = i }
        }
        let approx = nearPc
        for(let i = approx - 1; i < approx + 1; i += 1 / (2 * res)){
            let p = path.$pointAt(i / samples)
            let d = (p.x - x) * (p.x - x) + (p.y - y) * (p.y - y)
            if(d < best){ best = d; near = p; nearPc = i }
        }
        return { point: near, percent: nearPc / samples }
    }

    $pressAt(x, y){
        if(!this.$v[PathView.INTERACTIVE] || !this.$items.length || this.$model === null || !this.$modelCount) return false
        this.$velocity = []
        let hit = this.$items.some(item => {
            let ix = x - item.$v[Item.X], iy = y - item.$v[Item.Y]
            return !item.$culled && ix >= 0 && iy >= 0 && ix <= item.$v[Item.WIDTH] && iy <= item.$v[Item.HEIGHT]
        })
        if(!hit && fuzzyIsNull(this.$v[PathView.DRAGMARGIN])) return false
        let near = this.$pointNear(x, y)
        this.$startPoint = near.point
        this.$startPc = near.percent
        this.$startPos = { x, y }
        if(!hit){
            let distance = Math.abs(x - near.point.x) + Math.abs(y - near.point.y)
            if(distance > this.$v[PathView.DRAGMARGIN]) return false
        }
        let tl = this.$tl
        this.$stealMouse = !!(tl && this.$v[PathView.FLICKING] && this.$flickDuration && (clock.now() - tl.start) / this.$flickDuration < 0.8)
        this.$timerValid = true
        this.$lastPosTime = clock.now()
        this.$stopTimeline()
        return true
    }

    $moveAt(x, y){
        if(!this.$v[PathView.INTERACTIVE] || !this.$timerValid || this.$model === null || !this.$modelCount) return
        let now = clock.now()
        let near = this.$pointNear(x, y)
        if(!this.$stealMouse){
            let dx = x - this.$startPos.x, dy = y - this.$startPos.y
            if(Math.abs(dx) > DRAG_THRESHOLD || Math.abs(dy) > DRAG_THRESHOLD){
                let px = near.point.x - this.$startPoint.x, py = near.point.y - this.$startPoint.y
                if(Math.abs(px) > DRAG_THRESHOLD * 0.8 || Math.abs(py) > DRAG_THRESHOLD * 0.8) this.$stealMouse = true
            }
        } else {
            this.$moveReason = MOUSE
            let count = this.$modelCount
            let pathItems = this.$pathItems()
            let shown = pathItems === -1 ? count : Math.min(pathItems, count)
            let diff = (near.percent - this.$startPc) * shown
            if(!fuzzyIsNull(diff)){
                this.$moveReason = OTHER
                this.$setOffset(this.$offset + diff)
                this.$updateCurrent()
                if(diff > count / 2) diff -= count
                else if(diff < -count / 2) diff += count
                let elapsed = now - this.$lastPosTime
                if(elapsed > 0){
                    this.$velocity.push(diff / (elapsed / 1000))
                    if(this.$velocity.length > FLICK_SAMPLEBUFFER) this.$velocity.shift()
                }
            }
            if(!this.$v[PathView.MOVING]){
                this.$write(PathView.MOVING, true)
                engine.emit(this, PathView.SIGNAL_movementStarted)
            }
            this.$setDragging(true)
        }
        this.$startPc = near.percent
        this.$lastPosTime = now
    }

    $releaseAt(){
        this.$stealMouse = false
        this.$setDragging(false)
        if(!this.$v[PathView.INTERACTIVE] || !this.$timerValid || this.$model === null || !this.$modelCount){
            this.$timerValid = false
            if(!this.$tl) this.$movementEnding()
            return
        }
        let samples = this.$velocity
        let velocity = 0
        if(samples.length > FLICK_DISCARDSAMPLES){
            let n = samples.length - FLICK_DISCARDSAMPLES
            for(let i = 0; i < n; i++) velocity += samples[i]
            velocity /= n
        }
        let elapsed = clock.now() - this.$lastPosTime
        velocity *= Math.max(0, FLICK_VELOCITY_DECAY_TIME - elapsed) / FLICK_VELOCITY_DECAY_TIME
        let count = this.$modelCount
        let pathItems = this.$pathItems()
        let shown = pathItems === -1 ? count : Math.min(pathItems, count)
        let itemLength = this.$path().$geometry().length / shown
        let pixelVelocity = itemLength * velocity
        if(Math.abs(pixelVelocity) > MIN_FLICK_VELOCITY){
            let maxVelocity = this.$v[PathView.MAXIMUMFLICKVELOCITY]
            let snapMode = this.$v[PathView.SNAPMODE]
            if(Math.abs(pixelVelocity) > maxVelocity || snapMode === SnapOneItem) velocity = (velocity < 0 ? -maxVelocity : maxVelocity) / itemLength
            let v2 = velocity * velocity
            let accel = this.$v[PathView.FLICKDECELERATION] / 10
            let dist = 0
            let offset = this.$offset
            if(this.$haveRange() && (this.$rangeMode() === StrictlyEnforceRange || snapMode !== NoSnap)){
                if(snapMode === SnapOneItem){
                    dist = velocity > 0 ? qRound(0.5 + offset) - offset : qRound(0.5 - offset) + offset
                } else {
                    dist = Math.min(count - 1, v2 / (accel * 2) + 0.25)
                    dist = velocity > 0 ? qRound(dist + offset) - offset : qRound(dist - offset) + offset
                }
                if(dist <= 0){
                    dist = 0
                    accel = 0
                } else accel = v2 / (2 * Math.abs(dist))
            } else dist = Math.min(count - 1, v2 / (accel * 2))
            this.$flickDuration = accel ? Math.trunc(1000 * Math.abs(velocity) / accel) : 0
            this.$offsetAdj = 0
            this.$runTimeline(offset, [{ accel: { velocity, accel, distance: dist } }], this.$moveOffsetWrite(), () => {
                this.$fixOffset()
                if(!this.$tl) this.$movementEnding()
            })
            if(!this.$v[PathView.FLICKING]){
                this.$write(PathView.FLICKING, true)
                engine.emit(this, PathView.SIGNAL_flickStarted)
            }
        } else this.$fixOffset()
        this.$timerValid = false
        if(!this.$tl) this.$movementEnding()
    }

    $setDragging(d){
        if(this.$v[PathView.DRAGGING] === d) return
        this.$write(PathView.DRAGGING, d)
        engine.emit(this, d ? PathView.SIGNAL_dragStarted : PathView.SIGNAL_dragEnded)
    }

    get $keepMouseGrab(){ return this.$stealMouse }

    $mousePress(x, y, button){
        if(button !== 1 || !this.$v[PathView.INTERACTIVE]) return false
        this.$pressAt(x, y)
        return true
    }
    $mouseMove(x, y){ engine.batch(() => this.$moveAt(x, y)) }
    $mouseRelease(){ engine.batch(() => this.$releaseAt()) }

    // QQuickPathView::mouseUngrabEvent
    $ungrabMouse(){
        if(this.$stealMouse || (!this.$v[PathView.FLICKING] && this.$v[PathView.SNAPMODE] !== NoSnap && qRound(this.$offset) !== this.$offset)){
            this.$stealMouse = false
            this.$timerValid = false
            this.$fixOffset()
            this.$setDragging(false)
            if(!this.$tl) this.$movementEnding()
        }
    }

    // QQuickPathView::childMouseEventFilter: the view sees the events of its delegates and takes a drag
    $filterPointer(kind, receiver, x, y){
        if(!this.$v[Item.VISIBLE] || !this.$v[PathView.INTERACTIVE]) return { grab: false, filter: false }
        let keeps = !!(receiver && receiver.$keepMouseGrab)
        let disabled = !!(receiver && !receiver.$v[Item.ENABLED])
        let steal = this.$stealMouse
        let inside = x >= 0 && y >= 0 && x <= this.$v[Item.WIDTH] && y <= this.$v[Item.HEIGHT]
        if((steal || inside) && (!keeps || disabled)){
            engine.batch(() => {
                if(kind === 'press'){
                    this.$pressAt(x, y)
                    steal = this.$stealMouse
                } else if(kind === 'move') this.$moveAt(x, y)
                else if(kind === 'release') this.$releaseAt()
            })
            // a move takes the grab with the steal decided before it (the next move after the threshold)
            return { grab: (steal && !keeps) || disabled, filter: steal || disabled }
        } else if(this.$timerValid){
            this.$timerValid = false
            this.$fixOffset()
        }
        if(kind === 'release' || (keeps && !disabled)) this.$stealMouse = false
        return { grab: false, filter: false }
    }

    // model, currentIndex and offset are set through their setters of Qt (QQuickPathView::setModel ...)
    $write(id, value){
        if(id === PathView.MODEL){
            if(value === this.$v[id] && this.$f & engine.COMPLETED) return
            this.$v[id] = value
            this.$setModel(value)
            engine.emit(this, PathView.SIGNAL_modelChanged)
            return
        }
        if(id === PathView.CURRENTINDEX){
            this.$setCurrentIndex(Math.trunc(Number(value)) || 0)
            return
        }
        if(id === PathView.OFFSET){
            this.$moveReason = OTHER
            this.$setOffset(Number(value) || 0)
            this.$updateCurrent()
            return
        }
        super.$write(id, value)
    }

    // ---- methods ----

    incrementCurrentIndex(){
        this.$moveDirection = Positive
        this.$setCurrentIndex(this.$v[PathView.CURRENTINDEX] + 1)
    }

    decrementCurrentIndex(){
        this.$moveDirection = Negative
        this.$setCurrentIndex(this.$v[PathView.CURRENTINDEX] - 1)
    }

    // QQuickPathView::positionViewAtIndex
    positionViewAtIndex(index, mode){
        if(!this.$valid()) return
        if(mode < Beginning || mode > SnapPosition || mode === 3) return
        let count = this.$modelCount
        let pathItems = this.$pathItems()
        if(mode === Contain && (pathItems < 0 || count <= pathItems)) return
        let shown = pathItems === -1 ? count : Math.min(pathItems, count)
        let idx = (index + count) % count
        let snap = this.$haveRange() && (this.$rangeMode() !== NoHighlightRange || this.$v[PathView.SNAPMODE] !== NoSnap)
        let begin, end
        if(snap){
            begin = count - idx - Math.floor(shown * this.$v[PathView.PREFERREDHIGHLIGHTBEGIN])
            end = begin + shown - 1
        } else {
            begin = count - idx
            end = fmod(begin + shown, count) - 0.000000000001
        }
        let offset = this.$offset
        if(mode === Beginning) offset = begin
        else if(mode === End) offset = end
        else if(mode === Center){
            offset = begin < end ? (begin + end) / 2 : (begin + (end + count)) / 2
            if(snap) offset = qRound(offset)
        } else if(mode === Contain){
            if((begin < end && (offset < begin || offset > end)) || (offset < begin && offset > end)){
                let d1 = fmod(begin - offset + count, count), d2 = fmod(offset - end + count, count)
                offset = d1 < d2 ? begin : end
            }
        } else if(mode === SnapPosition) offset = count - idx
        this.$stopTimeline()
        this.$moveReason = OTHER
        this.$setOffset(offset)
        this.$updateCurrent()
    }

    itemAt(x, y){
        for(let item of this.$items){
            let ix = x - item.$v[Item.X], iy = y - item.$v[Item.Y]
            if(ix >= 0 && iy >= 0 && ix <= item.$v[Item.WIDTH] && iy <= item.$v[Item.HEIGHT]) return item
        }
        return null
    }

    indexAt(x, y){
        let item = this.itemAt(x, y)
        return item ? this.$indexOf(item) : -1
    }

    itemAtIndex(index){
        for(let item of this.$items) if(this.$indexOf(item) === index) return item
        return null
    }

    $disposeNative(){
        if(this.$model && this.$model.$removeView) this.$model.$removeView(this)
        this.$model = null
        this.$tl = null
        super.$disposeNative()
    }
}

const regenerate = (view) => view.$regenerate()
const refill = (view) => view.$refill()

defineType(PathView, {
    props: {
        model: { kind: 'var', initial: null },
        delegate: { kind: 'component', effect(view){ view.$regenerate() } },
        path: {
            kind: 'var',
            initial: null,
            effect(view, value, old){
                if(old && old.$users) old.$users.delete(view)
                if(value && value.$users) value.$users.add(view)
                if(view.$f & engine.COMPLETED) view.$regenerate()
            },
        },
        count: { kind: 'int', readonly: true, get(){ engine.track(this, PathView.SIGNAL_countChanged); return this.$count() } },
        currentIndex: 'int',
        currentItem: { kind: 'var', readonly: true, get(){ engine.track(this, PathView.SIGNAL_currentItemChanged); return this.$currentItem } },
        offset: { kind: 'real', get(){ engine.track(this, PathView.SIGNAL_offsetChanged); return this.$offset } },
        pathItemCount: {
            kind: 'int',
            initial: -1,
            effect(view){
                if(view.$f & engine.COMPLETED){
                    view.$updateMappedRange()
                    view.$regenerate()
                }
            },
        },
        cacheItemCount: { kind: 'int', effect: (view) => { view.$updateMappedRange(); view.$refill() } },
        preferredHighlightBegin: { kind: 'real', effect: refill },
        preferredHighlightEnd: { kind: 'real', effect: refill },
        highlightRangeMode: {
            kind: 'int',
            initial: StrictlyEnforceRange,
            effect(view){
                if(!view.$haveRange()) return
                view.$regenerate()
                let index = view.$rangeMode() !== NoHighlightRange ? view.$v[PathView.CURRENTINDEX] : view.$calcCurrentIndex()
                if(index >= 0) view.$snapToIndex(index, OTHER)
            },
        },
        highlight: { kind: 'component', effect(view){ view.$createHighlight(); view.$updateHighlight() } },
        highlightItem: { kind: 'var', readonly: true, get(){ engine.track(this, PathView.SIGNAL_highlightItemChanged); return this.$highlightItem || null } },
        highlightMoveDuration: { kind: 'int', initial: 300 },
        dragMargin: 'real',
        flickDeceleration: { kind: 'real', initial: 100 },
        maximumFlickVelocity: { kind: 'real', initial: 2500 },
        interactive: { kind: 'bool', initial: true },
        moving: { kind: 'bool', readonly: true },
        flicking: { kind: 'bool', readonly: true },
        dragging: { kind: 'bool', readonly: true },
        snapMode: 'int',
        movementDirection: { kind: 'int', effect(view, value){ view.$moveDirection = value } },
    },
    signals: ['movementStarted', 'movementEnded', 'flickStarted', 'flickEnded', 'dragStarted', 'dragEnded', 'currentItemChanged', 'highlightItemChanged'],
})
const ENUMS = {
    NoHighlightRange, ApplyRange, StrictlyEnforceRange, NoSnap, SnapToItem, SnapOneItem, Shortest, Negative, Positive,
    Beginning, Center, End, Contain, SnapPosition,
}
Object.assign(PathView, ENUMS)
// PathView.SnapToItem in a delegate reads the attached object (as ListView)
Object.assign(PathViewAttached.prototype, ENUMS)
// the path changed: the items are put again (QQuickPathView::pathUpdated)
PathView.prototype.$pathChanged = function(){
    for(let item of this.$items){
        let att = this.$attached(item)
        if(att) att.$percent = -1
    }
    this.$refill()
}

// the attached PathView of every item
{
    const prop = Item.$type.propByName.get('PathView')
    prop.group = PathViewAttached
    Object.defineProperty(Item.prototype, 'PathView', {
        get(){ return this.$v[prop.id] || (this.$v[prop.id] = new PathViewAttached(this)) },
        configurable: true,
    })
}

module.exports = { PathView, PathViewAttached }
