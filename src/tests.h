#pragma once
#include "builder/builder.h"
#include "data/constant_pool.h"
#include "detail/mutf8.h"
#include "writer/writer.h"

namespace classcafe
{
	inline void run_tests() {
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

			/// Modified UTF-8

			static std::string hex(std::string_view s) {
				std::string out;
				for (unsigned char c : s)
					out += std::format("{:02x} ", c);
				if (!out.empty()) out.pop_back();
				return out;
			}

			void expect_mutf8(std::string_view in, std::string_view expected, std::string_view what) {
				std::string out;
				if (!detail::to_mutf8(in, out))
					fail("mutf8 [{}]: valid input [{}] was rejected", what, hex(in));
				else if (out != expected)
					fail("mutf8 [{}]: expected [{}], got [{}]", what, hex(expected), hex(out));
			}

			void expect_mutf8_rejected(std::string_view in, std::string_view what) {
				std::string out;
				if (detail::to_mutf8(in, out))
					fail("mutf8 [{}]: invalid input [{}] was accepted, produced [{}]", what, hex(in), hex(out));
			}

			void mutf8_encoding_test() {
				using namespace std::string_view_literals;

				// 1 byte: passes through, except NUL
				expect_mutf8(""sv, ""sv, "empty");
				expect_mutf8("Hello, World!"sv, "Hello, World!"sv, "ascii");
				expect_mutf8("\x7F"sv, "\x7F"sv, "U+007F, last 1-byte");
				expect_mutf8("\0"sv, "\xC0\x80"sv, "NUL becomes C0 80");
				expect_mutf8("a\0b"sv, "a\xC0\x80" "b"sv, "NUL embedded in a string");
				expect_mutf8("\0\0"sv, "\xC0\x80\xC0\x80"sv, "consecutive NULs");

				// 2 bytes: unchanged
				expect_mutf8("\xC2\x80"sv, "\xC2\x80"sv, "U+0080, first 2-byte");
				expect_mutf8("\xC3\xA9"sv, "\xC3\xA9"sv, "U+00E9");
				expect_mutf8("\xDF\xBF"sv, "\xDF\xBF"sv, "U+07FF, last 2-byte");

				// 3 bytes: unchanged
				expect_mutf8("\xE0\xA0\x80"sv, "\xE0\xA0\x80"sv, "U+0800, first 3-byte");
				expect_mutf8("\xE2\x82\xAC"sv, "\xE2\x82\xAC"sv, "U+20AC");
				expect_mutf8("\xED\x9F\xBF"sv, "\xED\x9F\xBF"sv, "U+D7FF, just below surrogates");
				expect_mutf8("\xEE\x80\x80"sv, "\xEE\x80\x80"sv, "U+E000, just above surrogates");
				expect_mutf8("\xEF\xBF\xBF"sv, "\xEF\xBF\xBF"sv, "U+FFFF, last 3-byte");

				// 4 bytes: re-encoded as a surrogate pair, 3 bytes per surrogate
				expect_mutf8("\xF0\x90\x80\x80"sv, "\xED\xA0\x80\xED\xB0\x80"sv, "U+10000, first supplementary");
				expect_mutf8("\xF0\x91\x80\x80"sv, "\xED\xA0\x84\xED\xB0\x80"sv, "U+11000, first code point above 0x10FFF");
				expect_mutf8("\xF0\x9F\x98\x80"sv, "\xED\xA0\xBD\xED\xB8\x80"sv, "U+1F600");
				expect_mutf8("\xF4\x8F\xBF\xBF"sv, "\xED\xAF\xBF\xED\xBF\xBF"sv, "U+10FFFF, last code point");

				// mixed widths in one string
				expect_mutf8(
					"a" "\xC3\xA9" "\xE2\x82\xAC" "\xF0\x9F\x98\x80" "z"sv,
					"a" "\xC3\xA9" "\xE2\x82\xAC" "\xED\xA0\xBD\xED\xB8\x80" "z"sv,
					"mixed widths");
			}

			void mutf8_rejection_test() {
				using namespace std::string_view_literals;

				// stray or invalid lead bytes
				expect_mutf8_rejected("\x80"sv, "lone continuation byte (low)");
				expect_mutf8_rejected("\xBF"sv, "lone continuation byte (high)");
				expect_mutf8_rejected("\xF8\x88\x80\x80\x80"sv, "5-byte lead F8");
				expect_mutf8_rejected("\xFE"sv, "invalid byte FE");
				expect_mutf8_rejected("\xFF"sv, "invalid byte FF");

				// truncated sequences
				expect_mutf8_rejected("\xC3"sv, "2-byte sequence cut after lead");
				expect_mutf8_rejected("\xE2\x82"sv, "3-byte sequence cut after 2");
				expect_mutf8_rejected("\xF0\x9F\x98"sv, "4-byte sequence cut after 3");
				expect_mutf8_rejected("ok\xE2"sv, "truncated sequence at the end of valid text");

				// bad continuation bytes
				expect_mutf8_rejected("\xC3\x28"sv, "2-byte, ASCII where continuation expected");
				expect_mutf8_rejected("\xE2\x28\xAC"sv, "3-byte, bad 2nd byte");
				expect_mutf8_rejected("\xE2\x82\x28"sv, "3-byte, bad 3rd byte");
				expect_mutf8_rejected("\xF0\x9F\x28\x80"sv, "4-byte, bad 3rd byte");

				// overlong encodings (incl. the standard-UTF-8 spelling of NUL, which must not be accepted as input)
				expect_mutf8_rejected("\xC0\x80"sv, "overlong NUL");
				expect_mutf8_rejected("\xC1\xBF"sv, "overlong U+007F");
				expect_mutf8_rejected("\xE0\x80\x80"sv, "overlong 3-byte NUL");
				expect_mutf8_rejected("\xE0\x9F\xBF"sv, "overlong 3-byte U+07FF");
				expect_mutf8_rejected("\xF0\x80\x80\x80"sv, "overlong 4-byte NUL");
				expect_mutf8_rejected("\xF0\x8F\xBF\xBF"sv, "overlong 4-byte U+FFFF");

				// surrogates are not valid in UTF-8 input (MUTF-8 only produces them)
				expect_mutf8_rejected("\xED\xA0\x80"sv, "encoded U+D800");
				expect_mutf8_rejected("\xED\xBF\xBF"sv, "encoded U+DFFF");
				expect_mutf8_rejected("\xED\xA0\xBD\xED\xB8\x80"sv, "pre-encoded surrogate pair");

				// out of range
				expect_mutf8_rejected("\xF4\x90\x80\x80"sv, "U+110000");
				expect_mutf8_rejected("\xF7\xBF\xBF\xBF"sv, "U+1FFFFF");
			}

			// The u2 length limit applies to the *encoded* size, which can be
			// bigger than the input (NUL: 1 -> 2 bytes, U+10000+: 4 -> 6 bytes).
			void mutf8_length_limit_test() {
				auto repeat = [](std::string_view unit, size_t n) {
					std::string s;
					s.reserve(unit.size() * n);
					for (size_t i = 0; i < n; ++i) s += unit;
					return s;
					};

				auto check = [&](std::string const& input, bool ok, std::string_view what) {
					ConstantPool pool;
					auto r = pool.intern_utf8(input);
					if (bool(r) != ok)
						fail("mutf8 length [{}]: expected {}, got {}", what,
							ok ? "success" : "failure", bool(r) ? "success" : "failure");
					else if (!ok && r.error() != Error::StringTooLong)
						fail("mutf8 length [{}]: wrong error", what);
					};

				using namespace std::string_view_literals;
				check(repeat("a"sv, 65535), true, "65535 ascii bytes");
				check(repeat("a"sv, 65536), false, "65536 ascii bytes");
				check(repeat("\0"sv, 32767), true, "32767 NULs = 65534 encoded");
				check(repeat("\0"sv, 32768), false, "32768 NULs = 65536 encoded, though input is only 32768");
				check(repeat("\xE0\xA0\x80"sv, 21845), true, "21845 x 3-byte = 65535 encoded");
				check(repeat("\xE0\xA0\x80"sv, 21846), false, "21846 x 3-byte = 65538 encoded");
				check(repeat("\xF0\x9F\x98\x80"sv, 10922), true, "10922 x 4-byte = 65532 encoded");
				check(repeat("\xF0\x9F\x98\x80"sv, 10923), false, "10923 x 4-byte = 65538 encoded, input is 43692");
			}

			void invalid_utf8_error_test() {
				ConstantPool pool;
				auto r = pool.intern_utf8("\xC3\x28");
				if (bool(r))
					fail("intern_utf8: invalid UTF-8 was accepted");
				else if (r.error() != Error::InvalidUTF8)
					fail("intern_utf8: wrong error for invalid UTF-8");
			}

			/// Bytecode Emission

			void hello_world_test() {
				auto c = Class::make("HelloWorld");
				ParameterSpec params[] = { { "args", JVMType("java/lang/String").array() } };
				auto m = c.add_method("main", params, BaseType::Void, MethodAccess::Public | MethodAccess::Static);
				
				/* a bit hands-on but probably

				CodeBuilder code = m.code();
				code.get_static(jdk::System_out);
				code.ldc("Hello, World!");
				code.invoke_virtual(jdk::PrintStream_println_String);
				code.ret();

				*/

				// ClassFile cf = c.lower();
				// ByteWriter writer;
				// writer.encode(cf);
				// std::ofstream file(/* path */);
				// writer.dump(file);

				// execute; fail if exit_code != 0
				// preferably also find out if it actually printed hello, world
			}

			// Hand-builds a ClassFile for Hello.class, encodes it and writes it to disk.
			// Prints "zero" with no arguments and "nonzero" otherwise, so running it
			// (java -cp classcafe/tests Hello [args]) also checks the branch and the StackMapTable.
			void class_file_test(u2 major = 52) {
				namespace C = Constant; namespace A = Attribute; namespace S = StackMap;

				Vec<C::Info> pool = { C::Unusable{} };
				auto add = [&](C::Info c) -> u2 {
					u2 idx = u2(pool.size());
					bool two = C::takes_two_slots(c);
					pool.push_back(std::move(c));
					if (two) pool.push_back(C::Unusable{});
					return idx;
					};
				auto cls = [&](std::string_view name) { return add(C::Class{ add(C::Utf8{ std::string(name) }) }); };
				auto nat = [&](std::string_view n, std::string_view d) {
					return add(C::NameAndType{ add(C::Utf8{ std::string(n) }), add(C::Utf8{ std::string(d) }) });
					};

				u2 c_hello = cls("Hello"), c_obj = cls("java/lang/Object");
				u2 c_sys = cls("java/lang/System"), c_ps = cls("java/io/PrintStream");
				u2 c_strarr = cls("[Ljava/lang/String;"), c_str = cls("java/lang/String");
				u2 f_out = add(C::Fieldref{ c_sys, nat("out", "Ljava/io/PrintStream;") });
				u2 m_println = add(C::Methodref{ c_ps, nat("println", "(Ljava/lang/String;)V") });
				u2 s_zero = add(C::String{ add(C::Utf8{ "zero" }) });
				u2 s_nonzero = add(C::String{ add(C::Utf8{ "nonzero" }) });
				u2 u_main = add(C::Utf8{ "main" }), u_desc = add(C::Utf8{ "([Ljava/lang/String;)V" });
				u2 u_code = add(C::Utf8{ "Code" }), u_smt = add(C::Utf8{ "StackMapTable" });

				// check every constant kind's layout
				add(C::Long{ 0x0102030405060708 }); add(C::Double{ 3.5 });
				add(C::Float{ 1.5f }); add(C::Integer{ -7 });
				add(C::Utf8{ "a\xC0\x80" "b\xED\xA0\xBD\xED\xB8\x80" });   // NUL and U+1F600, as modified UTF-8
				if (major >= 51) { add(C::MethodType{ u_desc }); add(C::MethodHandle{ C::RefKind::InvokeVirtual, m_println }); }

				auto hi = [](u2 v) { return u1(v >> 8); };
				auto lo = [](u2 v) { return u1(v); };
				Vec<u1> code = {
					0xB2, hi(f_out), lo(f_out),         //  0: getstatic System.out
					0x2A,                               //  3: aload_0
					0xBE,                               //  4: arraylength
					0x9A, 0x00, 0x08,                   //  5: ifne -> 13
					0x12, u1(s_zero),                   //  8: ldc "zero"
					0xA7, 0x00, 0x05,                   // 10: goto -> 15
					0x12, u1(s_nonzero),                // 13: ldc "nonzero"
					0xB6, hi(m_println), lo(m_println), // 15: invokevirtual println
					0xB1,                               // 18: return
				};

				Vec<A::Info> code_attrs;
				if (major >= 50) code_attrs.push_back({ u_smt, A::StackMapTable{ {
					S::SameLocals1{ 13, S::Object{ c_ps } },                                            // frame at 13
					S::Full{ 1,{ S::Object{ c_strarr } },{ S::Object{ c_ps }, S::Object{ c_str } } }, // frame at 15
				} } });

				ClassFile cf{};
				cf.minor_version = 0;
				cf.major_version = major;
				cf.constant_pool = pool;
				cf.access_flags = ClassAccess::Public | ClassAccess::Super;
				cf.this_class = c_hello;
				cf.super_class = c_obj;
				cf.methods.push_back(MethodInfo{ MethodAccess::Public | MethodAccess::Static, u_main, u_desc,
					{ { u_code, A::Code{ 2, 1, code,{}, code_attrs } } } });

				ByteWriter w;
				if (Error e = w.encode(cf); e != Error::None) {
					fail("class_file [v{}]: encode failed with error {}", major, int(e));
					return;
				}

				std::string_view path = "data/classcafe/tests/Hello.class";
				std::error_code ec;
				std::filesystem::create_directories(std::filesystem::path(path).parent_path(), ec);

				std::ofstream f(path.data(), std::ios::binary);
				if (!f) {
					fail("class_file [v{}]: could not open {} for writing", major, path);
					return;
				}
				w.dump(f);
				f.flush();
				if (!f)
					fail("class_file [v{}]: writing {} failed", major, path);
			}
		} test;
#undef fail

		test.mutf8_encoding_test();
		test.mutf8_rejection_test();
		test.mutf8_length_limit_test();
		test.invalid_utf8_error_test();
		// test.hello_world_test();  // enable once the writer exists
		test.class_file_test();

		if (test.is_ok())
			std::cout << "Tests " "\033[0;32m" "PASSED" "\033[0m" "\n";
		else
			std::cout << "Tests " "\033[0;31m" "FAILED" "\033[0m" "\n";
	}
}