/*
 * main.cpp
 *
 *  Created on: Aug 1, 2017
 *      Author: tomas1
 */

#include <stdio.h>
#include <unistd.h>
#include <algorithm>
#include <iostream>
#include <iterator>
#include <vector>

#include "myviterbi.h"

static auto rtObj = myviterbiModelClass();

int main(int argc, char **argv) {
	rtObj.initialize();

	auto buf = std::vector<char>(4536);
	auto buf1 = std::vector<char>(3024);
	std::cerr << "Viterbi..." << std::endl;

	while (!feof(stdin)) {
		auto count = 0;
		do {
			count += read(STDIN_FILENO, buf.data() + count, buf.size() - count);
		} while (count != buf.size());
		auto it { 0 };
		for (auto c : buf) {
			for (auto i { 0 }; i < 8; i++) {
				rtObj.rtU.In1[it++] = (c >> i) & 1;
			}
		}
		rtObj.step();
		std::copy(rtObj.rtY.Out1, rtObj.rtY.Out1 + sizeof(rtObj.rtY.Out1),
				begin(buf1));
		write(STDOUT_FILENO, buf1.data(), buf1.size());
	}

	std::cerr << "Exiting.." << std::endl;
	return 0;
}

