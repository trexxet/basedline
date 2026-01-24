#include "Basedline.hpp"

#include <chrono>
#include <format>
#include <semaphore>
#include <stop_token>
#include <thread>

using namespace std::chrono_literals;

/*
template <typename Duration>
void test_printer (std::stop_token stop, Basedline::Basedline& bl, Duration timing, bool newline, int n) {
	std::binary_semaphore sem {0};
	std::stop_callback cb (stop, [&sem] { sem.release(); });
	static int i = 0;
	while (!stop.stop_requested()) {
		bl.print (std::move (std::format ("test {} {}{}", n, i++, newline ? "\n" : "")));
		sem.try_acquire_for (timing);
	}
}

template <typename Duration>
auto make_printer (Basedline::Basedline& bl, Duration timing, bool newline, int n) {
	return [&bl, timing, newline, n] (std::stop_token st) { test_printer (st, bl, timing, newline, n); };
}
*/

int main() {
/*
	Basedline::Basedline bl;
	Basedline::OptString input;

	bl.set_print_interval (1s);

	std::jthread printingThread1 (make_printer (bl, 300ms, false, 1));
	std::jthread printingThread2 (make_printer (bl, 500ms, true, 2));
	std::jthread printingThread3 (make_printer (bl, 2s, false, 3));

	bl.print ("test main\n");
	bl.read ("> ");
	while (!(input = bl.loop()));
	bl.read ("> ");
	while (!(input = bl.loop()));
*/
	return 0;
}
