#include "main.hpp"
#include "lcd.hpp"
#include "cpu.hpp"
#include "rom.hpp"
#include "mmio.hpp"
#include "timer.hpp"

using namespace std;

void log(string text) {
    cout << "[" << SDL_GetTicks() << "] " << text << endl;
}

int main(int argc, char * args[]) {
	SDL_Window * window;
    SDL_Surface * screen;
    SDL_Surface * lcd;
	SDL_Event event;
	uint8_t quit = 0;

    cout << "fx-emu " << VERSION << endl;

    SDL_Init(SDL_INIT_VIDEO);
	window = SDL_CreateWindow("fx-emu", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 384, 128, SDL_WINDOW_SHOWN);
	screen = SDL_GetWindowSurface(window);
	lcd = SDL_CreateRGBSurface(0, 96, 32, 32, 0, 0, 0, 0);

    rom_init();
	mmio_init();
	mmio_reset();
	cpu_reset();
	lcd_reset();
	timer_reset();
    cpu_loop(150);
    
    printf("STATUS = %02x, A = %02x\n", mmio_read_byte(REG_STATUS), mmio_read_byte(REG_ACC));

    lcd_flush(lcd);

	SDL_BlitScaled(lcd, NULL, screen, NULL);

	SDL_UpdateWindowSurface(window);

	while (!quit) {
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
			case SDL_QUIT:
				quit = 1;
				break;
			default:
				break;
			}
		}
	}

	SDL_DestroyWindow(window);
    SDL_FreeSurface(screen);
	SDL_FreeSurface(lcd);
    SDL_Quit();

    return 0;
}
