# How to Compile and Run Unit Tests

This project uses the *Erbsland Unit Test* framework.

## 🛠️ Compile the Unit Tests

To build the unit tests in debug mode using CMake and Ninja:

```shell
cmake -S . -B cmake-build-debug -G Ninja -DCMAKE_BUILD_TYPE=Debug
cmake --build cmake-build-debug --config Debug
```

## ▶️ Run All Unit Tests

Once compiled, run the tests with:

```shell
cd cmake-build-debug
ctest .
```

## 🎯 Run Individual Tests

To speed up testing, you can run a specific test suite directly using the test executable:

```shell
./cmake-build-debug/test/unittest/unittest name:DateTest
```

In this example, only the `DateTest` suite is executed.

To see all available test suites, targets, and tags:

```shell
./cmake-build-debug/test/unittest/unittest --list
```

### Filter Syntax

Use the form `[+|-]<type>:<name>` to include or exclude tests:

* No prefix (`name:TestX`) — runs only that test or suite
* `+tag:Slow` — includes tests tagged `Slow`, even if normally skipped
* `-name:DateTest` — runs all except `DateTest`

### Available Types

* `name`: Test class or test method
* `tag`: Custom tag applied to tests
* `target`: The software component being tested

## ⚙️ Options for Agents and Automation

These flags are useful for running in non-interactive or scripted environments:

* `-c`, `--no-color` — disables colored output
* `-e` — stops after the first error
* `-s`, `--no-summary` — skips the end-of-run summary

## 📚 More Information

**For humans:**
The full documentation is located at:
`test/unittest/erbsland-unittest/doc` (build with Sphinx)
or online: 👉 [Erbsland Unit Test Docs](https://erbsland-dev.github.io/erbsland-unittest/)

**For agents or systems:**
Run the test binary with `--help` to view all available options:

```shell
./cmake-build-debug/test/unittest/unittest --help
```
