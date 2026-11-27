# Test infrastructure shared by the products

What ImtCore gives the tests of a product (IotPlatform, Puma, ProLife, ...), so that a product does not write it again.

## Registration with CTest

`Config/CMake/ImtCoreTestRegistration.cmake` registers the tests by name, so that `ctest -L <label>` runs the same set on every platform and never starts a server program by accident. A product includes it from its test CMake file:

```cmake
include("${IMTCOREDIR}/Config/CMake/ImtCoreTestRegistration.cmake")
```

| Function | Registers |
|---|---|
| `add_tests(LABEL <label> TARGETS <target>... [TIMEOUT <seconds>] [WORKING_DIRECTORY <dir>])` | Test programs, one test per program, under the name of its target. It is defined by ACF (`Config/CMake/AcfTestRegistration.cmake`), which this file includes, so ACF and ImtCore use the same function. |
| `add_qml_tests(<name> DIRECTORY <dir> [IMPORT_DIRS <dir>...] [LABEL <label>] [TIMEOUT <seconds>])` | The Qt Quick Tests (`tst_*.qml`) of a folder, run by `ImtCoreQmlTest`. Not registered when that program is not built. |
| `add_node_test_suite(<name> DIRECTORY <dir> LABEL <label> [SERVER <target>] [TIMEOUT <seconds>])` | A Node.js suite: `npm ci` as a fixture, then `node run-tests.mjs`. Not registered without Node.js and npm. |

Every test has a timeout (300 s for programs and QML tests, 1800 s for a Node.js suite unless given), so a test that hangs fails instead of blocking the run.

### Labels

| Label | Meaning |
|---|---|
| `unit` | In process, no services. |
| `database` | Needs PostgreSQL (`TEST_PG_*`, see below). |
| `api` | Test suites against a running server; they need PostgreSQL and Node.js. |
| `qml` | Qt Quick Tests of the QML screens; they need the built `ImtCoreQmlTest`. |

```bash
ctest --test-dir build -L unit
ctest --test-dir build -L "database|api"
ctest --test-dir build -L unit --output-junit unit.xml
```

## Doubles for the unit tests

`imttest` also holds components that stand in for a database or a collection, so a unit test needs neither a server nor a store. They are `CSim*`, wired as simulated components (`icomp::TSimComponentWrap`).

- **`imttest::CSimDatabaseEngineComp`** is an `imtdb::IDatabaseEngine` without a server. It records every query and answers the ones that contain the text of a rule (`AddRule`) with the rows of that rule from an in-memory SQLite connection.
- **`imttest::CSimTransactionManagerComp`** is an `imtbase::ITransactionManager` that counts the started, cancelled and ended transactions.
- **`imttest::CSimObjectCollectionComp`** is an in-memory object collection that applies the complex filter of a selection like the database delegates do (equality, AND and OR), counts the saves and can fail its iterator. A product derives from it and overrides `GetFieldValue` for the fields of its objects, as `iottest::CSimDocumentCollectionComp` does.

## Node.js test kit

`Tests/NodeTestKit` (package `imtcore-node-testkit`) starts a product's server for the Node.js tests and runs them against it. A product loads it from the ImtCore checkout named by the environment variable `IMTCOREDIR` (`import(<IMTCOREDIR>/Tests/NodeTestKit/index.mjs)` in its `run-tests.mjs`), so it holds no path to the kit and no copy of it. The kit has no packages of its own: it uses the `pg` and `newman` that the product installed. The CTest registration sets `IMTCOREDIR` for the suite.

- **`TestServer`** drops the test database (only a name ending with `_test`), removes the files a previous run left, starts the simulated services (`sims`) and the server, waits until it answers on the test database, and stops them again. `Log`, `IsPortOpen` and `FindProgram` serve the runners that start more programs.
- **`RunApiSuite(import.meta.url)`** runs a whole API suite: `TestServer`, then a newman collection and Node scripts, with JUnit reports in `results`. The `run-tests.mjs` of a suite is these two lines.

Everything product-specific is in the `suite.json` of the suite: the server program and its bin search folders, ports, the settings folder (company and product), the readiness request, the sims (a sim with more ports lists them under `otherPorts` and gets each as `--<name>-port`), the database (name, environment prefix, defaults) and the newman collection. `IotPlatform/Tests/ServerApiPostman` is the reference; a suite with more steps than the API run (a GUI suite, for example) uses `TestServer` directly and adds its own around it. A sim of the suite is a script listed under `file`; a sim of the kit is listed under `kit` and found in `Tests/NodeTestKit/sims`:

- **`sim-smtp.mjs`** is an SMTP server over implicit TLS (a self-signed test certificate made at every start, so the repository holds no key; any login) that keeps the messages it receives. The HTTP port lists them decoded (`GET /sim/messages?to=<address>`) and forgets them (`DELETE /sim/messages`); the SMTP port is given as `otherPorts.smtp`.

`add_node_test_suite` registers a suite with CTest.

## Permission consistency test

`imtpermtest::CPermissionConsistencyTestBase` (library `imtpermtest`, built with `BUILD_TESTING`) checks that the permissions of a product agree with everything that names them, following `Docs/Permissions-Rules.en.md`. The feature tree is filled by the product (`FillProduct`); the compositions, SQL migrations, QML and C++ sources and translation files are read from the source tree at run time, so the test follows every change without being edited. Nine checks run:

- composition permission values are full paths, name nodes of the tree, and header permissions name leaves;
- every permission a migration stores, after the later migrations converted it, is a leaf;
- every permission literal of the sources names a node of the tree;
- requirements name nodes, a view right never requires an edit right, and an Edit group requires the View of its area;
- every area with an Edit group has a `ViewPage`;
- the commands declared with an empty permission list are exactly the allowlisted ones;
- every request handler gets a permission provider and a permission checker;
- every feature name and description has a finished translation.

A product derives from the class, adds `Q_OBJECT` and `I_ADD_TEST`, and implements `CreateConfiguration`: the root of its source tree, its `FillProduct`, the folders to read, and the allowlists with a reason for each entry. An allowlist entry that no longer applies fails the test, so the lists cannot go stale. `IotPlatform/Tests/PermissionConsistencyTest` is the reference.

## Test databases

`imttest` (library) and `ImtTestPck` (package) hold the parts that every test with a PostgreSQL database needs. Both need ImtCore configured with `BUILD_TESTING=ON`.

- **`ImtTestPck/EnvironmentDatabaseSettings`** (`imttest::CEnvironmentDatabaseSettingsComp`) takes the values of another settings component (attribute `DefaultSettings`) and overrides them field by field with `<prefix>_HOST`, `<prefix>_PORT`, `<prefix>_USER` and `<prefix>_PASSWORD` (attribute `EnvironmentPrefix`, default `TEST_PG`). The settings are not registered as serializable, so a value of the environment is never persisted. A database whose name does not end with `_test` stops the program before a connection is opened.
- **`imttest::CTestDatabaseTools::DropDatabase`** drops a test database through the maintenance database `postgres` and closes the connections left to it. It refuses a name that does not end with `_test`.

A test that recreates its database on every run drops it first, lets the production migration logic create and migrate it, and drops it again at the end.

The database tests of ImtCore itself (`Tests/ObjectCollectionTest`, `Tests/StructureTest`) still hold their database and credentials in their compositions; they are not switched to these components yet.
