# 🔋 static
Dead-simple, single-file, header-only, dependency-free libraries for testing, logging, tracing and terminal interaction in C.

|Library|LOC|Tests|Coverage|Result|
|-|-|-|-|-|
|[test.h](#testh)|||||
|[log.h](#logh)|||||
|trace.h|||||
|terminal.h|||||

## Features
- [x] blazing fast compile times
- [x] static memory footprint, no dynamic allocations
- [x] compile time configuration & customization
- [x] concise code for easy review/audit and modification
- [x] standard C99 and C11 using no extensions
- [x] no warnings on clang, gcc and msvc

### [test.h](static/test.h)
* implements [xUnit](https://en.wikipedia.org/wiki/XUnit) framework structure
* [JUnit XML](https://github.com/junit-team/junit5/blob/main/platform-tests/src/test/resources/jenkins-junit.xsd) report generation
* colored command line output
* xUnit structure testing (run-suite-case)
* gherkin/behavior testing (given-when-then)
* <500 LOC

### [log.h](static/log.h)

### *trace.h*

### *terminal.h*