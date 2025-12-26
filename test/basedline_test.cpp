#include "Basedline.hpp"

#include <chrono>
#include <format>
#include <optional>
#include <semaphore>
#include <stop_token>
#include <string>
#include <thread>

void test_printer (std::stop_token stop, Basedline::Basedline& bl, int timing, int n) {
	std::binary_semaphore sem {0};
	std::stop_callback cb (stop, [&sem] { sem.release(); });
	while (!stop.stop_requested()) {
		bl.print (std::move (std::format("test {}\n", n)));
		sem.try_acquire_for (std::chrono::seconds (timing));
	}
}

int main() {
	Basedline::Basedline bl;
	std::optional<std::string> input;

	std::jthread printingThread1 (test_printer, std::ref(bl), 2, 1);
	std::jthread printingThread2 (test_printer, std::ref(bl), 3, 2);

	bl.read ("> ");
	bl.print ("test main\n");
	while (!(input = bl.loop()));
	return 0;
}
