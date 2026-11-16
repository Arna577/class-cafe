// Modified UTF-8
#pragma once
#include <string>
#include <string_view>
#include "types.h"

namespace classcafe::detail
{
	inline void emit_unit3(std::string& out, u4 unit) {
		out.push_back(char(0xE0 | (unit >> 12)));
		out.push_back(char(0x80 | ((unit >> 6) & 0x3F)));
		out.push_back(char(0x80 | (unit & 0x3F)));
	}

	inline bool to_mutf8(std::string_view in, std::string& out) {
		static constexpr u4 min_codepoints[] = { 0, 0, 0x80, 0x800, 0x10000 };
		out.clear();
		size_t size = in.size();
		out.reserve(size);
		size_t i = 0;
		while (i < size) {
			u1 b = u1(in[i]);
			u4 cp;
			size_t len;
			if (b < 0x80) { cp = b; len = 1; }
			else if ((b & 0xE0) == 0xC0) { cp = b & 0x1F; len = 2; }
			else if ((b & 0xF0) == 0xE0) { cp = b & 0x0F; len = 3; }
			else if ((b & 0xF8) == 0xF0) { cp = b & 0x07; len = 4; }
			else return false;
			if (i + len > size) return false;

			for (size_t k = 1; k < len; ++k) {
				u1 c = u1(in[i + k]);
				if ((c & 0xC0) != 0x80) return false;
				cp = (cp << 6) | (c & 0x3F);
			}

			if (len > 1 && cp < min_codepoints[len]) return false; // overlong
			if (cp > 0x10FFF || (cp >= 0xD800 && cp <= 0xDFFF)) return false;
			i += len;

			if (cp == 0) { out.push_back(0xC0); out.push_back(0x80); }
			else if (cp < 0x80) { out.push_back(cp); }
			else if (cp < 0x800) {
				out.push_back(0xC0 | (cp >> 6));
				out.push_back(0x80 | (cp & 0x3F));
			}
			else if (cp < 0x10000) emit_unit3(out, cp);
			else {
				u4 v = cp - 0x10000;
				emit_unit3(out, 0xD800 + (v >> 10));
				emit_unit3(out, 0xDC00 + (v & 0x3FF));
			}
		}

		return true;
	}
}
