# GraphQL request context lost across a thread hop in the server request pipeline

Status: **actual root cause found 2026-07-14, fix applied, not yet rebuilt/reverified**
- see "Actual root cause found" section below; everything above it is the original
2026-07-13 investigation, which chased a thread-hop/propagation theory that turned out
to be a red herring (kept for the reasoning trail and because the affected components
listed in "Why this matters beyond Puma" are still accurate). Written 2026-07-13 after
a long debugging session that started as "get Puma's `pumatest.exe` AuthClientSdk/PAT
tests green" and ended up here. This file exists so a future session doesn't have to
re-derive all of this from scratch.

## TL;DR

Any mutation that goes through `imtservergql::CPermissibleGqlRequestHandlerComp::
CheckPermissions()` (i.e. anything with a `CommandPermissions` component wired - which
is most user/role/group mutations, but *not* PAT, see below) is denied for **every**
caller, including the superuser, whenever the request has been relayed through a
`imtclientgql::TClientRequestManagerCompWrap`-based forwarder (the `Remote*Controller`
classes in `imtauthgql`, used by `PumaGQLClientEngine`/`AuthorizableServerFramework` to
proxy a product-embedded server's incoming requests to the central Puma backend).

Root cause: the GraphQL request context (which carries the caller's auth token/user)
is created on one thread but the actual request handler chain (`CreateResponse()` and
everything downstream, including the outbound forwarding call) runs on a **different**
thread. `imtgql::CGqlRequestContextManager` - the mechanism several handlers (PAT, see
below) rely on to fetch "the context of the request currently being processed" - is
backed by `thread_local` storage, so it is empty on the worker thread. Separately, and
more fundamentally: the `imtgql::CGqlRequest` object that actually reaches the handler
on the worker thread has **no context attached to it either** (`GetRequestContext() ==
nullptr`), even though a context object with a valid token was demonstrably created
(via `imtservergql::CAuthenticationManagerComp::CreateGqlContext()`) moments earlier on
the other thread. So this isn't just "read `CGqlRequestContextManager` on the wrong
thread" - the context is not making it onto the request object at all by the time it
reaches the worker thread. Where exactly that attachment is supposed to happen and why
it doesn't survive the thread hop was not located - see "Where to look next".

## How this was found (reproduction recipe)

This was diagnosed against **Puma** (`d:\ImagingTools\Git\Puma`), using its `pumatest.exe`
suite (`Include/pumatest`) run against `PumaServerSlTest.exe` (SQLite-backed, isolated
test server, ports 17789 HTTP / 18789 WS - see `Impl/PumaServerSlTest/PumaServerSlTest.acc`).
The bug is not Puma-specific (the broken code lives in ImtCore's `imtclientgql`/
`imtservergql`/`imtgql`), but this is the easiest reproduction available.

1. Build `PumaServerSlTest.exe` and `pumatest.exe` (Debug_Qt6_VC17_x64 or similar).
   **Known packaging gap**: `AuthClientSdk.dll`/`AuthServerSdk.dll` land in
   `Bin\<config>\Plugins\` but `pumatest.exe`'s import table needs them next to itself
   (Windows default DLL search order doesn't include `Plugins\`). Also
   `Qt6QuickWidgetsd.dll` is a real dependency of `AuthClientSdk.dll` that
   `windeployqt` doesn't copy into `Bin\<config>\`. Until the build/deploy config is
   fixed, copy those three DLLs manually before running:
   ```powershell
   Copy-Item "Bin\<config>\Plugins\AuthClientSdk.dll","Bin\<config>\Plugins\AuthServerSdk.dll" "Bin\<config>\"
   Copy-Item "<QtInstallDir>\<version>\msvc2022_64\bin\Qt6QuickWidgetsd.dll" "Bin\<config>\"
   ```
2. Start `PumaServerSlTest.exe` from `Bin\<config>\`. It listens on 17789/18789 and
   auto-creates `%PUBLIC%\ImagingTools\Puma\Puma Server\puma_test.sqlite` (delete this
   file + `-wal`/`-shm` siblings first for a clean run).
3. Run `pumatest.exe -v2`. `CAuthClientSdkTest::UserCrudTest` (and `RoleCrudTest`,
   `GroupCrudTest`, and every `CPersonalAccessTokenTest` case that needs a user to
   exist first) fails at the first `CreateUser()` call *after a successful login*:
   `'!userId.isEmpty()' returned FALSE. (User creation failed)`.
   `Tests/PumaTestKit/Run-AuthSdkTests.ps1` automates steps 2-3 (it targets
   `PumaServerSlTest.exe` specifically so a developer's persistent `puma.sqlite` /
   `PumaServerSettings.xml` are never touched).

To see *why* it fails, temporarily instrument (already reverted, see "What was already
tried" below for the exact diff) and rebuild:

- `Include/imtservergql/CPermissibleGqlRequestHandlerComp.cpp`, top of
  `CreateResponse()`: log `gqlRequest.GetCommandId()`, whether
  `gqlRequest.GetRequestContext()` is null, its token length, `GetUserId()`, and
  `QThread::currentThreadId()`.
- `Include/imtclientgql/CGqlClientEngineComp.cpp`, in `CreateNetworkRequest()`: log the
  same plus the outbound URL, right after `const imtgql::IGqlContext* contextPtr =
  request.GetRequestContext();`.
- `Include/imtgql/CGqlRequestContextManager.h`: log `QThread::currentThreadId()` and
  whether a context is present, in both `SetContext()` and `GetContext()`.

Then run `pumatest.exe` under `cdb` (or any debugger/console that captures `qDebug()`
output - a `Start-Process -WindowStyle Hidden` child does **not** surface it, you need
either a debugger attached or `cmd /c ".\PumaServerSlTest.exe" > out.txt 2>&1` /
similar redirection) and grep for `UserAdd`.

## Evidence

With the instrumentation above, one `CreateUser()` call (which sends a `UserAdd`
mutation) produces, in order:

```
[TRACE GetContext]           thread=0x1ca4  found=true
[TRACE CreateNetworkRequest] thread=0x1ca4  command=UserAdd  url=http://localhost:7777/Test/graphql   contextPtr=true   token.len=256
                              (AuthClientSdk -> pumatest.exe's own embedded AuthServerSdk instance - context/token present, correct)
[TRACE CreateNetworkRequest] thread=0x1dd8  command=UserAdd  url=http://localhost:17789/Puma/graphql  contextPtr=false  token.len=-1
                              (AuthorizableServerFramework's PumaClientEngine forwarding to the real backend - context is GONE)
[TRACE CreateResponse]       thread=0x1dd8  command=UserAdd  contextPtr=false  token.len=-1  userId=""  userInfoPtr=false  isAdmin=false  commandPermissionsWired=true
                              (PumaServerSlTest.exe receiving the forwarded request - also sees no context, correctly denies as "Invalid permissions for the user ''")
```

Across the *entire* test run, `grep`-ing all `[TRACE SetContext]` and `[TRACE
CreateResponse]` lines for their thread IDs:

- `SetContext(contextPtr != nullptr)` (i.e. a real context, presumably created by
  `CAuthenticationManagerComp::CreateGqlContext()` after resolving the JWT/PAT from the
  `x-authentication-token` header) is **always** on thread `0x1ca4`.
- `CreateResponse()` for every command (`GetElementIds`, `Authorization`,
  `GetUserObjectId`, `UserAdd`, ...) is **always** on thread `0x1dd8`.

These never overlap in this run. `0x1ca4` looks like the thread that owns the
WebSocket/HTTP I/O and does the initial header parsing + context creation; `0x1dd8`
looks like a worker thread the actual request/response handling gets dispatched to.
Whatever marshals the request over to the worker thread does not carry the context
along - not via `CGqlRequestContextManager` (thread_local, so it wouldn't survive a
thread hop by construction) and not via the `IGqlContext` attached to the
`imtgql::CGqlRequest` object itself (`SetGqlContext()`/`GetRequestContext()`), which is
the part that's actually surprising and worth understanding before attempting a fix.

## Why PAT mutations don't hit this (and aren't a working reference for "how it should
work")

`imtauthgql::CClientRequestPersonalAccessTokenManagerComp` (Puma's PAT client wrapper)
does **not** rely on an already-attached context on the incoming request. Its `On*`
methods (`CreateToken`, `ValidateToken`, ...) are called as plain C++ methods (e.g.
`imtauth::IPersonalAccessTokenManager::CreateToken(userId, productId, ...)`), not as
GraphQL request handlers dispatched from an incoming wire request. They build a
**brand new** `imtgql::CGqlRequest` from scratch and explicitly call a local
`AttachCurrentContext(gqlRequest)` helper that pulls
`imtgql::CGqlRequestContextManager::GetContext()` and attaches a clone via
`gqlRequest.SetGqlContext(...)` - *on whatever thread is currently running that C++
method*. If PAT's `CreateToken()` is itself invoked from a `CPersonalAccessTokenControllerComp::
OnCreateToken()` resolver reached via the same worker-thread dispatch as `UserAdd`,
`imtgql::CGqlRequestContextManager::GetContext()` would presumably be just as empty
there - this was **not verified end-to-end** in this session, because `CreateUser()`
(needed to set up a user to own a PAT in the test fixtures) already fails first, so no
`CPersonalAccessTokenTest` case got far enough to exercise this. Do not assume PAT is
unaffected without checking - it may simply not have been exercised on the affected
code path yet.

Separately: `CPersonalAccessTokenControllerComp` (the actual PAT GraphQL handler) has
no `CommandPermissions` wired at all (a distinct, already-fixed issue - see
`Include/imtauthgql/CPersonalAccessTokenControllerComp.{h,cpp}`,
`IsCallerAuthorizedForUser()`), so `CheckPermissions()` short-circuits to `true`
regardless of context. That's *why* PAT mutations "worked" during this session's
testing even before that fix and even with this context bug present - not because PAT's
context propagation is actually correct.

## What was already tried / fixed this session (do not re-do these)

All in `d:\ImagingTools\Git\ImtCore` unless noted.

1. **`imtauthgql::CPersonalAccessTokenControllerComp`** (`Include/imtauthgql/
   CPersonalAccessTokenControllerComp.{h,cpp}`): added `IsCallerAuthorizedForUser()` -
   admin-or-owner check using `IGqlContext::GetUserId()` (not `GetUserInfo()->GetId()`,
   which depends on an extra user-collection lookup) plus `GetUserInfo()->IsAdmin()`
   for the admin fast-path. Gates `OnGetTokenList`/`OnGetToken`/`OnCreateToken`/
   `OnRevokeToken`/`OnDeleteToken`. This closed a real "anyone can manage anyone's PAT"
   hole and is unrelated to the bug documented here (it's a *missing* check, not
   context loss) - keep it.
2. **`imtauth::CUserInfo::IsAdmin()`** (`Include/imtauth/CUserInfo.cpp`) was found to
   unconditionally `return false` - a real bug, but a red herring for *this specific*
   failure: the actual user-loading path used by `SQLiteUserRepository`
   (`Partitura/ImtUserAdministrationVoce.arp/SQLiteUserRepository.acc`) goes through
   `ImtAuthPck::UserInfo` (`imtauth::CUserInfoComp`, which correctly compares `GetId()`
   against a configured `AdminId="su"`), not the plain `CUserInfo`/
   `imtbase::TIdentifiableWrap<CUserInfo>` (`CIdentifiableUserInfo`) class. Left
   unfixed - it may still be a legitimate bug for other callers of the plain
   `CUserInfo`/`CIdentifiableUserInfo` type, just confirmed not the cause here.
3. **SQL migration bug**: `Include/imtauthdb/Resources/SQL/{SQLite,Postgres}/
   CreatePersonalAccessTokenTable.sql` had a `--` comment containing a literal `;`
   (`-- Code stores scopes as comma-separated string; keep as text for compatibility`).
   `Include/imtdb/CSqlDatabaseObjectDelegateCompBase.cpp:755`
   (`createTableQuery.split(';')`) naively splits multi-statement scripts on `;`
   without understanding comments/string literals (this is *necessary* - Qt's SQLite
   driver can't execute multiple `;`-separated statements in one `exec()` call - just
   not comment-aware), which truncated the `CREATE TABLE` mid-comment and aborted the
   whole migration batch ("Collection table auto-creation failed; see previous
   errors. Component initialization was stopped"), which in turn broke unrelated
   tables that hadn't been created yet. Fixed by removing the `;` from the comment
   text. Real, confirmed, already fixed - keep it.
4. **`Impl/AuthServerSdk/AuthServerSdk.acc`** (Puma): its `DatabaseEngine` element used
   the generic `ImtCoreVoce::DatabaseEngine` nested assembly with no `DbName`
   override, which defaults to PostgreSQL's own `postgres` maintenance database
   (`localhost:5432`, user/password `postgres`/`root`) - i.e. any app embedding
   `AuthServerSdk` was, by default, writing its `UserSettingsRepository`/
   `UserActionSqlRepository` tables straight into Postgres's admin database if a local
   Postgres happened to be reachable with those default credentials. Fixed by giving
   it an explicit isolated `DbName="auth_server_sdk"` (kept on Postgres, not switched
   to SQLite - see next point for why). Real, confirmed, already fixed - keep it.
5. A SQLite attempt for the same `AuthServerSdk.acc` `DatabaseEngine` was tried first
   and **reverted**: `AuthorizableServerFramework`'s `DeskController` subsystem
   (`Conversations`/`Tickets`/`TicketEntityReferences` tables) uses Postgres-dialect
   SQL in its own migrations (`CREATE TYPE ... AS ENUM`, `gen_random_uuid()`, etc.)
   that doesn't run on SQLite, so switching the engine type broke that subsystem
   instead. Don't retry plain SQLite for `AuthServerSdk` without first checking
   whether `DeskController`'s migrations have (or get) SQLite variants.
6. **`Puma/Include/pumatest/CUserControllerTest.cpp`**: `CheckSuperuserTest()` was
   missing `arguments.input.emplace();` before touching `arguments.input->...`
   (every sibling test in the file has this call) - crashed the entire `pumatest.exe`
   process (`Q_ASSERT`/`qFatal` on an empty `istd::TNullableValue`, which Windows
   reports as `STATUS_STACK_BUFFER_OVERRUN`/`c0000409` even though it's not a real
   memory-safety bug) before any later-registered `I_ADD_TEST` class got to run. Fixed.
   `CUserControllerTest`/`CUserCollectionControllerTest` were subsequently deleted from
   Puma at the user's request (considered obsolete/redundant with the SDK-level
   tests) - if this file resurfaces from history, this bug goes with it.
7. **`Include/imtclientgql/TClientRequestManagerCompWrap.h`** (`SendModelRequest()`):
   added a fallback that fills in the outbound cloned request's context from
   `imtgql::CGqlRequestContextManager::GetContext()` **only if the clone doesn't
   already have one** (so PAT's explicit `AttachCurrentContext()` is untouched). This
   is safe and correct as far as it goes (and is the same mechanism PAT already
   relies on) but **does not fix the bug in this document** - it was proven by the
   thread-ID evidence above that `GetContext()` returns null on the worker thread
   regardless, because nothing ever calls `SetContext()` on that thread. Kept in place
   as a real, harmless improvement; do not expect it alone to fix `UserAdd`/
   `RoleAdd`/`GroupAdd`.
8. Temporary `qDebug()` instrumentation used to gather the evidence above
   (`CGqlRequestContextManager.h`, `CGqlClientEngineComp.cpp`,
   `CPermissibleGqlRequestHandlerComp.cpp`) has been **reverted**. Re-add it (see "How
   this was found" above) if you need to re-verify or extend this trace - e.g. also
   instrument `CAuthenticationManagerComp::CreateGqlContext()` itself, and whatever
   the WebSocketServerFramework's incoming-message dispatch path is, to see exactly
   where the thread hop happens and why the context doesn't come along.

## Where to look next

Not yet located in this session - the actual dispatch-to-worker-thread code was never
found. Starting points:

- `imtservergql::CAuthenticationManagerComp::CreateGqlContext()`
  (`Include/imtservergql/CAuthenticationManagerComp.cpp`) - confirmed to call
  `imtgql::CGqlRequestContextManager::SetContext(...)` and to run on thread `0x1ca4`
  in this reproduction. Find who calls `CreateGqlContext()` and on what thread that
  caller itself runs.
- Whatever implements `ImtHttpServerVoce`'s `WebSocketServerFramework` (the
  `PackageId="ImtHttpServerVoce" ComponentId="WebSocketServerFramework"` component
  referenced throughout `Partitura/ImtHttpServerVoce.arp/*.acc` and
  `Partitura/PumaVoce.arp/PumaServerBase.acc`) - this is the actual C++ WebSocket
  server; look for `QThreadPool`, `moveToThread`, `Qt::QueuedConnection`, or a
  worker-thread dispatch of incoming message handling. The natural fix, once found,
  is almost certainly to make the context travel as **data** with the queued
  call/signal (e.g. bundled into whatever gets posted to the worker thread) rather
  than trying to make `CGqlRequestContextManager` or the request's own
  `SetGqlContext()` work across the hop after the fact.
- Confirm whether `imtcom::CRequestSender::DoSyncPost()` (used by
  `imtclientgql::CApiClientComp::SendRequest()`) or the outbound
  `QNetworkAccessManager` itself has any thread affinity that could explain the
  `0x1ca4`/`0x1dd8` split on the *client* forwarding side specifically (as opposed to
  the incoming-request dispatch side) - both may be contributing, or it may turn out
  to be the same underlying cause once you find where the hop actually happens.

## Follow-up session (2026-07-14): narrowed via static read-through, still not confirmed

No debugger available this session - everything below comes from reading the full
ImtCore + Puma request pipeline end-to-end (not re-instrumenting/re-running). It
narrows the search meaningfully but does **not** replace actually confirming the
mechanism with a debugger/instrumentation - treat "strongest hypothesis" below as
exactly that, not a verified conclusion.

### Ruled out (traced and confirmed correct)

- `imtgql::CGqlRequest::CloneMe()` / `CopyFrom()` (`Include/imtgql/CGqlRequest.cpp:592-634`)
  - correctly deep-clones the attached `IGqlContext` via `m_gqlContextPtr->CloneMe()`.
  Not the leak point.
- `imtservergql::CGqlDemultiplexerComp::CreateResponse()` and
  `imtservergql::CPermissibleGqlRequestHandlerComp::CreateResponse()`/`CheckPermissions()`
  - both take `const imtgql::CGqlRequest&` and pass it through unchanged, no cloning,
  no reconstruction. Not the leak point.
- `imtclientgql::TClientRequestManagerCompWrap::SendModelRequest()`,
  `imtclientgql::CApiClientComp::SendRequest()`,
  `imtclientgql::CGqlClientEngineComp::CreateNetworkRequest()` - all correctly read
  `GetRequestContext()` off whatever request object they're handed and run
  synchronously on the calling thread; no marshaling/cloning bug visible in any of
  them.
- The **production WebSocket receive path** is NOT what `CWebSocketServletComp::
  ProcessGqlRequest()`'s bare `m_workerManagerCompPtr->ProcessRequest(request)` call
  looks like in isolation (forwarding the raw `CWebSocketRequest` with no headers/
  commandId set - looks broken on its own). In production,
  `imtrest::CWebSocketThread::OnWebSocketTextMessage()`
  (`Include/imtrest/CWebSocketThread.cpp:179-203`) intercepts `MT_QUERY` **before**
  it reaches the servlet and converts it into a real `CHttpRequest` with headers
  copied in from the WS envelope's `"headers"` JSON field, *then* hands that off to
  the worker pool. So for the WS path, the token travels as data on the request
  object across the thread hop, which is thread-safe by construction. This path is
  probably fine; do not waste time on `CWebSocketServletComp::ProcessGqlRequest`
  itself.

### New finding: `UserAdd`/`RoleAdd`/`GroupAdd` don't go through the file that was patched

Fix #7 (session above) patched `imtclientgql::TClientRequestManagerCompWrap::
SendModelRequest()`. But per `Partitura/ImtGraphQlVoce.arp/PumaGQLClientEngine.acc`
(`RemoteControllers` embedded registry, `RemoteRepresentationController` element,
`CommandIds` list at lines ~620-641), `UserAdd`/`UserUpdate`/`RoleAdd`/`RoleUpdate`/
`GroupAdd`/`GroupUpdate`/representation-getters are routed to
`PumaGqlRemoteRepresentationController`, whose `CreateInternalResponse()`
(`Include/imtclientgql/CGqlRemoteRepresentationControllerCompBase.cpp:20-44`) is a
**separate, independent forwarding implementation** - it clones the incoming
`gqlRequest` and calls `m_apiClientCompPtr->SendRequest()` directly, with **no
fallback at all** if `gqlRequest.GetRequestContext()` is already null at that point
(unlike the defensive fallback fix #7 added to `SendModelRequest`). This is a
concrete, verifiable gap regardless of whether it's the root cause: the one file
that got patched for context-loss is not the one `UserAdd` actually uses.

### Strongest remaining hypothesis: shared mutable `m_lastRequest` + worker-thread reuse

`imtservergql::CHttpGraphQLServletComp::m_lastRequest` (`CHttpGraphQLServletComp.h:60`)
is a `mutable` **member variable**, not a per-call local - `OnPost()` does
`m_lastRequest.ResetData()` / `ParseQuery()` / `SetGqlContext()` /
`requestHandlerPtr->CreateResponse(m_lastRequest, ...)` all against this one shared
instance. It has to be a member because `GetGqlRequest()` (the
`imtgql::IGqlRequestProvider` interface) exposes `&m_lastRequest` to *sibling*
handler components processing the *same* request
(`Include/imtservergql/CObjectCollectionControllerCompBase.cpp`,
`CRoleCollectionControllerComp.cpp`, `CSessionModelObserverComp.cpp`,
`COperationContextControllerComp.cpp` all call `GetGqlRequest()` mid-chain instead of
receiving the request as a parameter) - so this is a deliberate "current request for
this servlet instance" pattern, safe only as long as a given servlet instance never
processes two requests concurrently/reentrantly.

Separately, `imtrest::CWorkerManagerComp::ProcessRequest()`
(`Include/imtrest/CWorkerManagerComp.cpp:45-84`) **reuses an existing idle
(`ST_CLOSE`) worker thread/servlet instance** for subsequent requests rather than
spinning up a fresh one each time - this is consistent with the original evidence
that `CreateResponse()` was *always* on the same thread ID across the whole test run
(a single worker gets reused throughout a sequential test run).

The hypothesis: for a product like the embedded "Test" `AuthServerSdk`, token
validation inside `CreateGqlContext()` (`CAuthenticationManagerComp::CreateGqlContext()`)
itself requires a *network round-trip* (`SlaveJwtSessionController`/
`PersonalAccessTokenManager` both wired to `PumaClientEngine`, i.e. another outbound
GraphQL call to the central Puma backend - see `AuthorizableServerFramework.acc`
lines 19-24). If that blocking call (`imtcom::CRequestSender::DoSyncPost`, ultimately
a `QNetworkAccessManager` call) pumps a nested Qt event loop on the worker thread
while waiting, and that nested loop ends up processing a *second* queued
`CWorkerThread::StartProcess` for a different request on the same reused
instance, that second request's `OnPost()` would call `m_lastRequest.ResetData()`
and overwrite the shared context out from under the first, still-in-flight request.
This would also explain why `SetContext()` (thread A) and `CreateResponse()`
(thread B) were observed to *never* share a thread in the original trace: whichever
thread ends up driving the blocking network call for context validation differs from
the "logical" worker thread the outer `OnPost()` call is blocked on.

This has **not been confirmed** - it is the most code-consistent explanation found
so far, but requires actually observing reentrancy to prove.

### Concrete next step to confirm/refute the reentrancy hypothesis

Add one instrumentation line at the very top of `CHttpGraphQLServletComp::OnPost()`
logging `this` (the servlet instance pointer) alongside thread ID and command-ID, and
another right before the `return` statements. If the **same `this` pointer** shows a
second `OnPost` entry log before a first one's exit log for that same pointer, that
confirms reentrant corruption of `m_lastRequest` and points straight at the fix
(guard `OnPost` with an instance-level mutex, or stop sharing `m_lastRequest`across
requests e.g. by giving each in-flight request its own `CGqlRequest` and threading it
through `IGqlRequestProvider` differently).

### If reentrancy is confirmed, likely safe fix (not yet applied)

Serialize `OnPost()` per servlet instance with a `QMutex` so a reused worker/servlet
instance can never process two requests' state concurrently, even if a nested event
loop pumps in the middle of one. Not done this session - needs the reentrancy
confirmation step above first, and needs checking whether `OnPost` can legitimately
be called reentrantly by design anywhere (subscriptions?) before locking it down.

### Applied this session

Gave `CGqlRemoteRepresentationControllerCompBase::CreateInternalResponse()`
(`Include/imtclientgql/CGqlRemoteRepresentationControllerCompBase.cpp`) the same
defensive "attach from `CGqlRequestContextManager::GetContext()` if the clone's
context is missing" fallback fix #7 added to `TClientRequestManagerCompWrap::
SendModelRequest()`. This is a cheap, low-risk improvement that closes a real gap
(this is the actual code path `UserAdd`/`RoleAdd`/`GroupAdd` go through, and it had
no fallback at all) - **but per the same caveat as fix #7, this alone will not fix
the bug if the thread-local `CGqlRequestContextManager` is also empty on whatever
thread ends up calling this method** (which the original thread-ID evidence suggests
is likely). Re-run the repro after this change; if `UserAdd` still fails, the
reentrancy hypothesis above (or something still unlocated) is the real cause and
this fallback was necessary-but-not-sufficient.

## Verification: fix #7b did NOT resolve the bug (2026-07-14)

Ran the full suite (`Tests/PumaTestKit/Run-AuthSdkTests.ps1 -ResetDatabase -TestArgs
"-v2"`, fresh Postgres-backed `PumaServerSlTest.exe`, freshly rebuilt
`AuthClientSdk.dll`/`AuthServerSdk.dll` from the same build as the
`CGqlRemoteRepresentationControllerCompBase` fix) after the "Applied this session"
fix above landed. Result: **unchanged**.

```
CAuthClientSdkTest:        3 passed, 5 failed
CAuthServerLifecycleTest:  4 passed, 0 failed
CPersonalAccessTokenTest:  2 passed, 7 failed
```

Every failure is still the exact same symptom as before the fix: `UserAdd`/
`RoleAdd`/`GroupAdd`/PAT `CreateToken` return an empty id even though the caller is
logged in as `su` (superuser) moments earlier in the same test. `PumaServerSlTestLog.txt`
(the real backend on 17789) confirms the request arrives anonymous - every forwarded
mutation is logged as:

```
[...] Invalid permissions for the user ''
```

which is `CPermissibleGqlRequestHandlerComp::CreateResponse()`'s exact message for
`contextPtr == nullptr || userInfoPtr == nullptr`. So the fallback added to
`CreateInternalResponse()` is not firing effectively - either
`CGqlRequestContextManager::GetContext()` is also empty on the thread that ends up
calling it (thread-local doesn't survive the hop, as the original thread-ID tracing
suggested), or `UserAdd`/`RoleAdd`/`GroupAdd` are routed through some other path
that still bypasses both patched call sites entirely. The reentrancy hypothesis
(`CHttpGraphQLServletComp::m_lastRequest` + worker-thread reuse) above remains
unconfirmed and is the next thing to check - concretely, instrument `OnPost()` with
`this` + thread ID as described above, since that's still the only concrete
unexplored lead.

Unrelated observation from this run: after `CPersonalAccessTokenTest` finishes,
`pumatest.exe` hard-crashes (`STATUS_STACK_BUFFER_OVERRUN` / `Q_ASSERT
"m_threadId == GetCurrentThreadId()"` in `qfunctions_win.cpp`, triggered from
`QEventDispatcherWin32::wakeUp` called off its owning thread). This looks like a
pre-existing test-teardown/threading issue independent of the context-loss bug, and
prevents any test classes registered after `CPersonalAccessTokenTest` from running.
Not investigated further this session - flagging so it isn't mistaken for a symptom
of the same root cause.

## Actual root cause found (2026-07-14, after fix #7b's failure) - NOT a threading bug

The failure of fix #7b was the crucial data point: if a *thread-local* fallback
still can't find a context, the context was never created in the first place on
that process/thread - it isn't a propagation/hop problem at all. That reframing
led to the real bug, a plain **ACF wiring gap**, found by re-reading `AuthServerSdk.acc`
(Puma) against `CHttpGraphQLServletComp::OnPost()`:

```cpp
if (m_gqlContextCreatorCompPtr.IsValid()){
    ...
    m_lastRequest.SetGqlContext(std::move(gqlContextPtr));
}
else{
    // Q_ASSERT(false);
}
```

If `GqlContextCreator` (the `AuthenticationManager` reference) isn't wired for a
given product's `GraphQlRoot` instance, this whole block is **silently skipped** -
no error, no 401, processing just continues with `m_lastRequest` carrying no
context at all. That commented-out `Q_ASSERT(false)` is exactly why this was silent
instead of crashing loudly in a debug build.

Tracing the attribute chain for `GqlContextCreator` (`ExportId="AuthenticationManager"`
at every level) from `GraphQlRoot` up to the product root:

- `Partitura/ImtGraphQlVoce.arp/StandardGraphQlHandlers.acc` - `GraphQlRoot`'s
  `GqlContextCreator` attribute: unset, exported as `AuthenticationManager`.
- `Partitura/ImtGraphQlVoce.arp/ApplicationHandlers.acc` - its `ServerFramework`
  element (= `StandardGraphQlHandlers`) attribute `AuthenticationManager`: unset,
  re-exported as `AuthenticationManager`.
- `Impl/AuthServerSdk/AuthServerSdk.acc` (Puma) - its `ApplicationHandlers` element:
  **had no `AuthenticationManager` attribute entry at all.** Every sibling
  attribute that needed to reach into the nested `AuthorizableServerFramework`
  element was wired (`UserSettingsCollection` -> `AuthorizableServerFramework/
  UserSettingsRepository`, etc.) - `AuthenticationManager` was simply missing from
  that list, even though `AuthorizableServerFramework.acc` does export it
  (`<Component ExportedId="AuthenticationManager" ComponentId="AuthenticationManager"/>`
  in its `ExportedComponents`).

End to end: `AuthServerSdk`'s own `CAuthenticationManagerComp` element (nested
inside its `AuthorizableServerFramework`) exists and is fully configured, it's just
never *connected* to the servlet that needs it, because the reference chain has a
missing link at the outermost `AuthServerSdk.acc` level. `m_gqlContextCreatorCompPtr`
resolves to invalid for every request AuthServerSdk's embedded server receives, so
`CreateGqlContext()`/`CGqlRequestContextManager::SetContext()` are **never called on
the server side at all** for these requests - the request is processed anonymously
from the moment it's parsed, and every downstream permission check correctly (from
its own point of view) sees no caller.

This also fully explains the original thread-ID evidence without needing any
threading/reentrancy explanation: `[TRACE SetContext]` entries were always on
`0x1ca4` because that instrumentation was in the *shared* `CGqlRequestContextManager::
SetContext()`, and the only call sites that ever actually fire it in this repro are
client-side (`imtauthgql::CSimpleLoginWrapComp`, after login, on pumatest.exe's own
thread) - `CAuthenticationManagerComp::CreateGqlContext()`'s call to `SetContext()`
never fires on the server side at all, so it could never show up on the worker
thread `0x1dd8` no matter which thread that worker happened to be. The "never
overlap" observation was a red herring caused by comparing a client-side call site
against a server-side one that was structurally unreachable, not evidence of a
same-process thread hop. The reentrancy hypothesis above is very likely not needed
to explain this bug (it may still be a latent risk in `CWorkerManagerComp` reuse +
`m_lastRequest`, but is not what's causing this specific failure).

### Fix applied

`Impl/AuthServerSdk/AuthServerSdk.acc` - added the missing attribute to the
`ApplicationHandlers` element:

```xml
<AttributeInfo Id="AuthenticationManager" Type="Reference" ExportId="">
    <Data IsEnabled="true" Value="AuthorizableServerFramework/AuthenticationManager"/>
</AttributeInfo>
```

Same pattern as the `UserSettingsCollection` attribute two lines below it in the
same element. Verified only one `ComponentId="ApplicationHandlers"` usage exists in
the Puma repo (this one), so no sibling `.acc` needs the same fix there - but any
other product assembling `ImtGraphQlVoce::ApplicationHandlers` (or embedding
`StandardGraphQlHandlers` directly without going through `ApplicationHandlers`)
should have its `AuthenticationManager` wiring checked the same way; this class of
bug (an attribute chain that type-checks and builds fine but silently resolves to
an invalid reference at the top) won't show up as a build error.

**Not yet rebuilt/reverified in this session** - needs a rebuild of
`AuthServerSdk.dll` and a re-run of `Tests/PumaTestKit/Run-AuthSdkTests.ps1
-ResetDatabase -TestArgs "-v2"` to confirm. If this is indeed the root cause, `su`
should now be able to create users/roles/groups and PAT `CreateToken` should stop
returning empty ids.

## Verification of the ACF-wiring fix (2026-07-14, later same day): rebuilt, results got WORSE

Confirmed via timestamps that `AuthServerSdk.dll` was rebuilt after the `.acc` fix
landed (`Plugins\AuthServerSdk.dll` newer than `AuthServerSdk.acc`'s edit time).
Refreshed the manually-copied root-level DLL copies (see packaging-gap note above)
and re-ran the full suite (`Run-AuthSdkTests.ps1 -ResetDatabase`, then re-ran
`pumatest.exe -v2` directly under `cdb` to capture full output since `Out-Host`
piping swallows it).

Result: **not fixed, and measurably worse than before this fix**:

```
CAuthClientSdkTest:        2 passed, 6 failed,  54243ms  (was 3 passed, 5 failed,  1372ms)
CAuthServerLifecycleTest:  4 passed, 0 failed,     38ms  (unchanged)
CPersonalAccessTokenTest:  0 passed, 2 failed,  12668ms  (was 2 passed, 7 failed,  1382ms)
```

Critically, this is a **different failure mode**, not just "still failing the same
way": `LoginLogoutTest`'s legitimate second login (`ok2`, correct password) now
fails (`iauth::ILogin::Login() returned false`), `SuperuserExistsTest` and
`CPersonalAccessTokenTest::initTestCase()` now fail with `err.isEmpty()` ==
false / "Response is invalid" on `CheckSuperuserExists` - the very first real
request of the run. Every subsequent `Authorization` call in every test method logs
`QWARN [Login] Failed: iauth::ILogin::Login() returned false`. Before this fix,
login itself always worked fine and only the *forwarded* mutations
(`UserAdd`/`RoleAdd`/`GroupAdd`/PAT `CreateToken`) failed anonymously.

The ~40x latency increase (1.3s -> 12-54s per test class) is the more telling
signal: this smells like requests now blocking/timing out rather than failing
fast. `PumaServerSlTestLog.txt` (the real backend, port 17789) stayed essentially
empty this run (just the three startup lines - no `LogonUser failed`/`Invalid
permissions` spam at all, unlike the previous run where every forwarded request
logged there). That's consistent with requests now hanging somewhere *before*
reaching the real backend, rather than reaching it and being correctly/incorrectly
authorized.

`CAuthenticationManagerComp::CreateGqlContext()`
(`Include/imtservergql/CAuthenticationManagerComp.cpp:29-56`) itself looks safe for
the token-less case (skips `ResolveUserId()` entirely - no network call - when the
`x-authentication-token` header is empty, which is exactly `CheckSuperuserExists`'s
situation), so the hang is not obviously in that function taken in isolation - it
was not traced further this session. Given the latency signature, the shared
`m_lastRequest` + `CWorkerManagerComp` worker-thread-reuse reentrancy hypothesis
from the "Follow-up session" section above (never confirmed, dismissed as
"probably not needed" once the ACF wiring gap was found) may actually be relevant
again now that `AuthenticationManager` is wired and its dependent
`SlaveJwtSessionController`/`PersonalAccessTokenManager` outbound calls
(`AuthorizableServerFramework.acc` lines ~19-24) are firing for the first time on
this code path - worth revisiting rather than assuming it's fully ruled out.

**Net effect: do not consider this fixed, and do not assume the `.acc` change is a
pure improvement** - it exposed a new, more severe problem (hangs / broken login)
that wasn't visible before because the code path it wired up was previously dead.
The `AuthenticationManager` attribute addition itself may still be *directionally*
correct (the missing wiring is real), but something downstream of it - most likely
in the outbound token-validation call chain now actually being exercised - needs
to be found before this is usable. Next step: instrument
`CAuthenticationManagerComp::ResolveUserId()` and whatever `SlaveJwtSessionController`/
`PersonalAccessTokenManager` do over `PumaClientEngine`, and/or retry the
`OnPost()` reentrancy instrumentation now that this code path is live.

## Why this matters beyond Puma

`imtclientgql::TClientRequestManagerCompWrap` and `imtservergql::
CPermissibleGqlRequestHandlerComp` are shared ImtCore infrastructure, not Puma- or
PAT-specific. Any product embedding `AuthorizableServerFramework` (the generic
"product server" framework any app links against to get a Puma-backed identity/auth
layer - see `Partitura/ImtHttpServerVoce.arp/AuthorizableServerFramework.acc`) and
proxying authenticated mutations through it to a central Puma backend is likely
affected the same way. This was only reproduced/observed via Puma's own test suite
because that's what this session was working on.
