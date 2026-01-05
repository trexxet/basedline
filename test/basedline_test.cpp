#include "Basedline.hpp"

#include <chrono>
#include <format>
#include <semaphore>
#include <stop_token>
#include <thread>

void test_printer (std::stop_token stop, Basedline::Basedline& bl, int timing, bool newline, int n) {
	std::binary_semaphore sem {0};
	std::stop_callback cb (stop, [&sem] { sem.release(); });
	static int i = 0;
	while (!stop.stop_requested()) {
		bl.print (std::move (std::format ("test {} {}{}", n, i++, newline ? "\n" : "")));
		sem.try_acquire_for (std::chrono::milliseconds (timing));
	}
}

int main() {
	Basedline::Basedline bl;
	Basedline::OptString input;

	std::jthread printingThread1 (test_printer, std::ref(bl), 300, false, 1);
	std::jthread printingThread2 (test_printer, std::ref(bl), 500, true, 2);
	std::jthread printingThread3 (test_printer, std::ref(bl), 700, true, 3);

	bl.print ("test main\n");
	bl.read ("> ");
	while (!(input = bl.loop()));
	return 0;
}
