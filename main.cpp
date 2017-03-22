#include "main.hpp"
#include "lcd.hpp"
#include "cpu.hpp"
#include "rom.hpp"
#include "mmio.hpp"

using namespace std;

void log(string text) {
    cout << "[" << SDL_GetTicks() << "] " << text << endl;
}

int main(int argc, char * args[]) {
	SDL_Window * window;
    SDL_Surface * screen;
    SDL_Surface * lcd;

    cout << "fx-emu " << VERSION << endl;

    SDL_Init(SDL_INIT_VIDEO);
	window = SDL_CreateWindow("fx-emu", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 384, 128, SDL_WINDOW_SHOWN);
	screen = SDL_GetWindowSurface(window);
	lcd = SDL_CreateRGBSurface(0, 96, 32, 32, 0, 0, 0, 0);

    rom_init();
    cpu_loop(10);
    
    printf("STATUS = %02x, A = %02x\n", mmio_read_byte(REG_STATUS), mmio_read_byte(REG_ACC));

    lcd_flush(lcd);

	SDL_BlitScaled(lcd, NULL, screen, NULL);

	SDL_UpdateWindowSurface(window);

    SDL_Delay(5000);

    SDL_FreeSurface(screen);
    SDL_Quit();

    return 0;
}
