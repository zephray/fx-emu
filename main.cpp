#include "main.hpp"
#include "eps.hpp"

using namespace std;

void log(string text) {
    cout << "[" << SDL_GetTicks() << "] " << text << endl;
}

int main(int argc, char * args[]) {
	uint8_t quit = 0;

    cout << "fx-emu " << VERSION << endl;

	eps_init();
	eps_reset();
	eps_run();
	eps_deinit();
	
    return 0;
}
