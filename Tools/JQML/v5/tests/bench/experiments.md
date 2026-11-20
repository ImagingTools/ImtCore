# JQML v5: journal of the experiments

Every hypothesis tried, kept or not. Numbers: `tests/bench/run.js`, dist/core.js, 10 runs, median; the noise threshold
of a metric is in `results/noise.json` (the greatest of 2% and 2 sd over three runs of the baseline). Rule: the target
metric better past its noise and by 3% at least, no other metric worse past its noise, all tests and stress tests green.
"Accepted state" is the branch with every accepted change before the experiment.

| # | Hypothesis | Source | Files | Target: before -> after | Other metrics | Verdict |
|---|---|---|---|---|---|---|
| E1 | A child leaving its parent from the end is taken off without indexOf (destroying N children was O(N^2)) | code reading of QtObject.$setParent; 10000-children stress test | Runtime/Core/QtObject.js | scale.children_10000_destroy 13.97 -> 2.74 ms (-80%) | all within noise | accepted, fa097a7a0e |
| E2 | A Repeater delegate made at the end right after the previous one is not moved (N delegates were O(N^2)) | code reading of Repeater.$placeAfter | Runtime/QtQuick/Repeater.js | scale.children_10000_create 35.34 -> 14.85 ms (-58%) | first full run: bind.conditional +25%; back to back with the accepted state +1.3% (noise: that benchmark builds its Repeater before measuring) | accepted, 13590fd87d |
| E3 | The minifier's mark_enclosed with Set lookups and without walking the functions of a scope again (uglify push_uniq was 8.2 of 14.6 s) | CPU profile of the compile | compiler/uglifyScope.js, compiler/index.js | compile.ProLife.total 18217 -> 9880 ms (-46%), minify 14561 -> 6071 ms; Lisa -39%, Puma -37% | the bundles are the same bytes; parse, model, codegen within noise | accepted, a5b605ccb2 |
| E4 | ListView positions cached by prefix sums from the first row (same additions, same order) | profile of scrolling far (26 ms a frame near row 80000) | Runtime/QtQuick/ListView.js | not measured to the end | - | superseded: the scenario looped in the original runtime (a bug); the Qt layout of visibleItems (0625181ca2) replaced the model: scroll_far 5249 -> 16 ms |
| E5 | The notify signal of a binding kept on the Binding instead of four lookups on each evaluation | code reading of engine.evaluate / isQPropertyBinding | Runtime/Core/engine.js | bind.chain -2.3%, bind.fanout -4.3% (both within noise) | docs.*.heapPerDocument +1.4% (one more field per binding, within noise) | neutral, not applied |
| E6 | familyCss memoized per family (TextLayout.measure built the font string on every call) | CPU profile of docs.ProLife (familyCss + measure 1.4%) | Runtime/QtQuick/TextLayout.js | docs.ProLife.create -1.8%, Lisa -3.1%, Puma -3.2% (all within noise: 6.2%, 5.6%, 3.6%) | all within noise | neutral, not applied |
| E7 | Minify without mangling | build time | compiler/index.js | minify 14025 -> 2630 ms | ProLife bundle +34% raw, +16% gzip | rejected: a size regression |

Bug fixes (separate commits, accepted even without a gain; their cost is noted):

| Fix | Commit | Effect on the metrics |
|---|---|---|
| Work queued while the update phase ends runs in the same update | 4766aebd7d | within noise |
| A Loader lets go of the item it releases | 8f6f8c25e6 | within noise |
| 120000 rows inserted or moved at once | adfbdc19ca | within noise |
| stats.links counted out on destruction | 17bcad9049 | within noise |
| Polish loop given up after 10000 (PolishLoopDetector) | acecce5ec0 | within noise |
| ListView keeps the positions of its visible items (QQuickListView) | 0625181ca2 | listview_scroll 95.6 -> 15.0 ms, scroll_far 5249 -> 15.9 ms; size.core +6.4 KB (+2.0%) |
| Image size updates give no dependency | e2cbbaa436 | within noise |
| A binding takes no dependency from the write of a binding it enables | 0bdd77113a | within noise |
| (before the audit) shared image cache, C locale | dc60986270, 165db2f3a7 | in the baseline |

All bug fixes together against the baseline: size.core.raw 329651 -> 336654 B (+2.1%, past the 2% threshold),
size.core.gzip 106130 -> 108446 B (+2.2%); the bundles of the applications +0.07% raw, +0.15% gzip (within noise).
The growth is the code of the Qt ListView layout; no variant without it was found.
