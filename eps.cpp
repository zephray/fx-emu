//ePS6800 MCU Emulator

#include "eps.hpp"
#include "lcd.hpp"
#include "cpu.hpp"
#include "rom.hpp"
#include "mmio.hpp"
#include "timer.hpp"
#include "kbd.hpp"
#include "debug.hpp"

#define TARGET_FPS  50
#define TARGET_FREQ 500000

SDL_Surface * lcd;
SDL_Window * window;
SDL_Surface * screen;
enum run_mode runmode;

enum run_mode eps_get_mode() {
	return runmode;
}

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
	uint8_t key = 51;//scan code, 50 for on, 51 for none
	switch (event.key.keysym.sym) {
	case SDLK_a: key = 50;break;
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
	case SDLK_F1: runmode = MODE_DEBUG; break; //F1 to enter debug mode
	default:
		break;
	}
	if (event.type == SDL_KEYUP) {
		if (key == 50) kbd_onup(); else
			if (key != 51) kbd_keyup(key);
	}
	else {
		if (key == 50) kbd_ondown(); else
			if (key != 51) kbd_keydown(key);
	}
}

void eps_debug() {
	int quit = 0;
	int i;
	char line[100];
	char *cmd;
	char *next_token = NULL;

	while (!quit) {
		printf("debug> ");
		fflush(stdout);
		fflush(stderr);
		fgets(line, 100, stdin);
		cmd = strtok_s(line, " \n", &next_token);
		if (!cmd) {
			continue;
		}
		else if (!_stricmp(cmd, "?") || !_stricmp(cmd, "h")) {
			printf(
					"Debugger commands:\n"
					"c - continue\n"
					"k <RAM address> <+r|+w|-r|-w> - add/remove RAM breakpoint\n"
					"k - show RAM breakpoints\n"
					"b <ROM address> <+x|-x> - add/remove ROM breakpoint\n"
					"b - show ROM breakpoints\n"
					"m - show low registers\n"
					"m <address> - show memory address(beaware of POSTID!)\n"
					"m <address> <value> - change RAM/register value\n"
					"s - step\n");
		} 
		else if (!_stricmp(cmd, "c")) {
			quit = 1;
		} 
		else if (!_stricmp(cmd, "m")) {
			char *addr = strtok_s(NULL, " \n", &next_token);
			char *valu = strtok_s(NULL, " \n", &next_token);
			uint32_t address, value;
			uint8_t pid_backup;
			if (!addr) { //reg dump
				printf("Registers: \n");
				//We need to be aware that reading INDF and LCDDATA is a potentially dangerous action
				pid_backup = mmio_read_byte(REG_POSTID);
				mmio_write_byte(REG_POSTID, 0x00);//Disable Post ID
				for (i = 0; i < 0x13; i++) {
					printf("%s: %02x\n", reg_names[i], mmio_read_byte(i));
				}
				printf("%s: %02x\n", reg_names[0x20], mmio_read_byte(0x20));
				mmio_write_byte(REG_POSTID, pid_backup);
			}
			else if (!valu) { //show mem
				address = atoi(addr);
				if (address > 0xFF)
					printf("Invaild address\n");
				else
					printf("Memory Location %02x: %02x\n", address, mmio_read_byte(address));

			}
			else { //mem mod
				address = atoi(addr);
				value = atoi(valu);
				if ((address > 0xFF) || (value > 0xFF))
					printf("Invaild parameters\n");
				else {
					printf("Writing %02x to %02x\n", value, address);
					mmio_write_byte(address, value);
				}
					
			}
		}
		else if (!_stricmp(cmd, "s")) {
			cpu_loop(1);
			printf("PC: %02x%02x, ALU: %02x, STATUS: %02x\n", mmio_read_byte_internal(REG_PCM), mmio_read_byte_internal(REG_PCL), mmio_read_byte_internal(REG_ACC), mmio_read_byte_internal(REG_STATUS));
		} else {
			printf("Unknown command %s\n", cmd);
		}
	}
	runmode = MODE_RUN;
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
			case SDL_KEYUP:
				if (event.key.repeat == 0)
					eps_process_key(event);
				break;
			case SDL_QUIT:
				quit = 1;
				break;
			default:
				break;
			}
		}
		if (runmode == MODE_RUN) {
			last_time = SDL_GetTicks();
			for (i = 0; i < 20; i++) {
				cpu_loop(100);
				timer_tick();
			}
			for (i = 0; i < 20; i++) {
				cpu_loop(100);
				//timer_tick();
			}
			current_time = SDL_GetTicks();
			current_time -= last_time;
			if (current_time != 0)
				current_time = 20000 / current_time;
			printf("PC: %02x%02x, ALU: %02x, STATUS: %02x\n", mmio_read_byte_internal(REG_PCM), mmio_read_byte_internal(REG_PCL), mmio_read_byte_internal(REG_ACC), mmio_read_byte_internal(REG_STATUS));
			lcd_flush(lcd);
			SDL_BlitScaled(lcd, NULL, screen, NULL);
			SDL_UpdateWindowSurface(window);
			while ((SDL_GetTicks() - last_time) < 40);//125kHz
		}
		else {
			eps_debug();
		}
	}
}



void eps_deinit() {
	SDL_FreeSurface(lcd);
	SDL_DestroyWindow(window);
	SDL_FreeSurface(screen);
	SDL_Quit();
}