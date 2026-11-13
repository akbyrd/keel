Tests
-----
Planning
	* Do we need to shuffle test order? Only for fuzzing/DST right?
	* GoogleTest format - I'm generally on board, but is this the most widely supported format?
	* I don't have any issue with a capacity limit. Simplicity and performance are high priority.

	It's not rocket science
	https://matklad.github.io/2025/12/06/mechanical-habits.html
	https://web.archive.org/web/20260727014259/https://graydon2.dreamwidth.org/1597.html
	https://matklad.github.io/2026/05/14/catch-flakes-on-main.html
	https://matklad.github.io/2023/12/24/ci-dream.html
	https://gregoryszorc.com/blog/2021/04/07/modern-ci-is-too-complex-and-misdirected/

	Self modifying tests?
	Use keel or not?

Implementation
	Separate file - foo.test.hpp
	Separate project? - keel.test.vcxproj
	Single TU or multiple or both?
	Command line execution
	IDE integration
	Continuous execution - run on file save
	Structured output
	Data driven tests (arrays of cases) with good output / debugging
	Sanitizers
	Linters
	Warnings as errors
	Parallel execution - multi-thread or multi-process?
	zig style progress
	Types:
		#include
		constexpr results
		constexpr callable
		template instantiation
		unit
		perf - { time, memory } x { compile time, run time }
		generated instruction, code size
		"doesn't compile" tests
		fuzz
		deterministic simulation
		cross platform agreement
		include dependencies



Pending
-------
String formatting
