#include "SDL/SDL.h"
#include "main.hpp"
#include "lcd.hpp"
#include "cpu.hpp"
#include "rom.hpp"
#include "mmio.hpp"

using namespace std;

void log(string text) {
    cout << "[" << SDL_GetTicks() << "] " << text << endl;
}

int main(int argc, char * argv[]) {
    SDL_Surface * screen;
    SDL_Surface * lcd;

    cout << "fx-emu " << VERSION << endl;

    SDL_Init(SDL_INIT_VIDEO);
    screen = SDL_SetVideoMode(384, 128, 16, SDL_SWSURFACE);
    lcd = SDL_CreateRGBSurface(SDL_SWSURFACE, 96, 32, 16,
        0xF800, 0x07E0, 0x001F, 0);
    SDL_WM_SetCaption("fx-emu", NULL);

    rom_init();
    cpu_loop(100);
    
    printf("STATUS = %02x, A = %02x\n", mmio_read_byte(REG_STATUS), mmio_read_byte(REG_ACC));

    lcd_flush(lcd);

    SDL_SoftStretch(lcd, NULL, screen, NULL);

    SDL_Flip(screen);

    SDL_Delay(5000);

    SDL_FreeSurface(screen);
    SDL_Quit();

    return 0;
}
