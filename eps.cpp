//ePS6800 MCU Emulator

#include "eps.hpp"
#include "lcd.hpp"
#include "cpu.hpp"
#include "rom.hpp"
#include "mmio.hpp"
#include "timer.hpp"
#include "kbd.hpp"

#define TARGET_FPS  50
#define TARGET_FREQ 500000

SDL_Surface * lcd;
SDL_Window * window;
SDL_Surface * screen;

void eps_init() {
	SDL_Init(SDL_INIT_VIDEO);
	window = SDL_CreateWindow("fx-emu", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 384, 128, SDL_WINDOW_SHOWN);
	screen = SDL_GetWindowSurface(window);
	lcd = SDL_CreateRGBSurface(0, 96, 32, 32, 0, 0, 0, 0);

	rom_init();
	mmio_init();
	rom_open("rom.bin");
}

void eps_reset() {
	mmio_reset();
	cpu_reset();
	lcd_reset();
	timer_reset();
	kbd_reset();
}

void eps_process_key(SDL_Event &event) {
	uint8_t key;//scan code, 50 for on
	switch (event.key.keysym.sym) {
	case SDLK_a:
		printf("ON!\n");
		key = 50;
		break;
	case SDLK_b: key = 0; break;
	case SDLK_c: key = 1; break;
	case SDLK_d: key = 2; break;
	case SDLK_e: key = 3; break;
	case SDLK_f: key = 4; break;
	case SDLK_g: key = 5; break;
	case SDLK_h: key = 6; break;
	case SDLK_i: key = 7; break;
	case SDLK_j: key = 8; break;
	case SDLK_k: key = 9; break;
	case SDLK_l: key = 10; break;
	case SDLK_m: key = 11; break;
	case SDLK_n: key = 12; break;
	case SDLK_o: key = 13; break;
	case SDLK_p: key = 14; break;
	case SDLK_q: key = 15; break;
	case SDLK_r: key = 16; break;
	case SDLK_s: key = 17; break;
	case SDLK_t: key = 18; break;
	case SDLK_u: key = 19; break;
	case SDLK_v: key = 20; break;
	case SDLK_w: key = 21; break;
	case SDLK_x: key = 22; break;
	case SDLK_y: key = 23; break;
	case SDLK_z: key = 24; break;
	default:
		key = 0;
		break;
	}
	if (event.key.type == SDL_KEYUP) {
		printf("Released ");
		if (key == 50) kbd_onup(); else
			kbd_keyup(key);
	}
	else {
		printf("Pressed ");
		if (key == 50) kbd_ondown(); else
			kbd_keydown(key);
	}
	
	printf("%s\n", SDL_GetKeyName(event.key.keysym.sym));
}

void eps_run() {
	SDL_Event event;
	uint32_t last_time, current_time;
	int quit = 0;
	int i;

	while (!quit) {
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
			case SDL_KEYDOWN:
				printf("SDL Keydown\n");
				//eps_process_key(event);
				break;
			case SDL_KEYUP:
				printf("SDL Keyup\n");
				//eps_process_key(event);
				break;
			case SDL_QUIT:
				quit = 1;
				break;
			default:
				break;
			}
		}
		last_time = SDL_GetTicks();
		for (i = 0; i < 20; i++) {
			cpu_loop(100);
			timer_tick();
		}
		for (i = 0; i < 20; i++) {
			cpu_loop(100);
			//fuck you music
			//timer_tick();
		}
		current_time = SDL_GetTicks();
		current_time -= last_time;
		if (current_time != 0)
			current_time = 20000 / current_time;
		lcd_flush(lcd);
		SDL_BlitScaled(lcd, NULL, screen, NULL);
		SDL_UpdateWindowSurface(window);
		while ((SDL_GetTicks() - last_time) < 40);//125kHz
		printf("PC: %02x%02x, ALU: %02x, STATUS: %02x\n", mmio_read_byte_internal(REG_PCM), mmio_read_byte_internal(REG_PCL), mmio_read_byte_internal(REG_ACC), mmio_read_byte_internal(REG_STATUS));
	}
}

void eps_deinit() {
	SDL_FreeSurface(lcd);
	SDL_DestroyWindow(window);
	SDL_FreeSurface(screen);
	SDL_Quit();
}