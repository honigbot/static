# 🔋 static

Dead-simple, single-file, header-only, dependency-free libraries for **testing**, **logging**, **tracing** and **terminal** interaction in **C**. 

- [x] blazing fast compile times
- [x] standard C99 and C11 using no extensions
- [x] static memory footprint, no dynamic allocations
- [x] concise, commented code for easy review/audit
- [x] built around established tooling & formats like JUnit, TAP, syslog and so on.
- [x] no warnings on clang, gcc and msvc
- [x] tested on Windows, macOS, Linux

|[test.h](static/test.h)|[log.h](static/log.h)|trace.h|terminal.h|
|-|-|-|-|
|✅ &nbsp; [xUnit](https://en.wikipedia.org/wiki/XUnit) inspired structure|✅ &nbsp; command line logging|||
|✅ &nbsp; [JUnit](https://github.com/junit-team/junit5/blob/main/platform-tests/src/test/resources/jenkins-junit.xsd) report generation|✅ &nbsp; [Syslog](https://en.wikipedia.org/wiki/Syslog) (Linux/Unix)|||
|✅ &nbsp; colored command line output|✅ &nbsp; [OSLog](https://developer.apple.com/documentation/os/logging/os_log_t?language=objc) (macOS/iOS)|||
|✅ &nbsp; zero setup|✅ &nbsp; [EventLog]() (Windows)|||
|✅ &nbsp; rich assertions|✅ &nbsp; callback for custom log|||
|✅ &nbsp; custom formatted messages||||6
|✅ &nbsp; <500 LOC||||
