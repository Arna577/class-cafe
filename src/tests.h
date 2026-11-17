#pragma once
#include "builder/builder.h"
#include <iostream>

namespace classcafe
{
	void run_tests() {
		class Test {
			bool _success = true;

			void _fail(std::string&& msg) {
				_success = false;
				std::cerr << "\033[97;41m" "Test Failed: " "\033[0m" << msg << "\n";
			}
		public:
			bool is_ok() const noexcept {
				return _success;
			}

#define fail(...) _fail(::std::format(__VA_ARGS__))

			// TODO: mutf8 test suite

			void hello_world_test() {
				auto c = Class::make("HelloWorld");
				ParameterSpec params[] = { { "args", JVMType("java/lang/String").array() } };
				auto m = c.add_method("Main", params, BaseType::Void, MethodAccess::Static);
				// not sure of the format or how much info has to be piped into the lhs to make it access static 'out' by name
				// c.emit_invoke_static("java/lang/System/out", "println", { "Hello, World!" }) ?

				// ClassFile cf = c.lower();
				// ByteWriter writer;
				// writer.encode(cf);
				// std::ofstream file(/* path */);
				// writer.dump(file);

				// execute; fail if exit_code != 0
				// preferably also find out if it actually printed hello, world
			}
		} test;

		if (test.is_ok())
			std::cout << "Tests " "\033[0;32m" "PASSED" "\033[0m" "\n";
		else
			std::cout << "Tests " "\033[0;31m" "FAILED" "\033[0m" "\n";
	}
}