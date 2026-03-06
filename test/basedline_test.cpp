#include "Basedline.hpp"

#include <chrono>
#include <format>
#include <semaphore>
#include <stop_token>
#include <thread>

#define BL_LINEBUF_SIZE 1024

using namespace std::chrono_literals;

template <typename Duration>
void test_printer (std::stop_token stop, Basedline::Basedline& bl, Duration timing, int n) {
	std::binary_semaphore sem {0};
	std::stop_callback cb (stop, [&sem] { sem.release(); });
	static int i = 0;
	while (!stop.stop_requested()) {
		bl.print (std::move (std::format ("test {} {}", n, i++)));
		sem.try_acquire_for (timing);
	}
}

template <typename Duration>
auto make_printer (Basedline::Basedline& bl, Duration timing, int n) {
	return [&bl, timing, n] (std::stop_token st) { test_printer (st, bl, timing, n); };
}

int main() {
	Basedline::Basedline bl (BL_LINEBUF_SIZE);
	Basedline::OptString input;

	//std::string str = "⚠️ Hello, World! Привет, мир! 你好, 世界! 🚀";
	//bl.set_print_interval (1s);

	//std::jthread printingThread1 (make_printer (bl, 300ms, 1));
	std::jthread printingThread2 (make_printer (bl, 500ms, 2));
	std::jthread printingThread3 (make_printer (bl, 2s, 3));

	bl.print ("test main\n");
	bl.read ("> ");
	while (!(input = bl.loop()));
	bl.read ("> ");
	while (!(input = bl.loop()));

	return 0;
}
