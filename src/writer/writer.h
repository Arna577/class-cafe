#pragma once
#include "../class_file.h"

namespace classcafe
{
	struct ByteWriter {
		std::vector<u1> bytes;

		void U1(u1 val) { bytes.push_back(val); }
		void U2(u2 val) {
			U1(u1(val >> 8));
			U1(u1(val));
		}
		void U4(u4 val) {
			U2(u2(val >> 16));
			U2(u2(val));
		}
	};
}