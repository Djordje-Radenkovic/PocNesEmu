#pragma once

#include <cstdint>
#include <memory>
#include <random>

#include "sdl/SDL.h"

#include "INesPpu.h"


class PPU_2C02 final : public INesPpu {
public:
	PPU_2C02();
	~PPU_2C02();

	void reset() override;
	void tick()  override;

	bool inline isRunning() override;
	bool inline getNmi()	override;
	void inline clearNmi()	override;

	int inline	getCycle()	  override;
	int inline	getScanline() override;



	// From IBusMaster
	void connectBus(std::shared_ptr<IBus<uint16_t, uint8_t>> bus) override;
	//	--------------


	// From IBusSlave
	inline const uint16_t size() override { return m_size; }
	uint8_t read(uint16_t address, bool readOnly = false) override;
	void write(uint16_t address, uint8_t data) override;
	// --------------

private:
	// From IBusMaster
	inline uint8_t readFrom(uint16_t address, bool readOnly = false) override {
		return m_bus->read(address, readOnly);
	}

	inline void writeTo(uint16_t address, uint8_t data) override {
		m_bus->write(address, data);
	}
	//	--------------

private:
	bool m_isRunning	  = false;
	bool m_busConnected	  = false;
	bool m_frameComplete  = false;
	bool m_nmi			  = false;

	uint16_t m_size		  = 0x08;
	uint8_t  m_tempData	  = 0x00;
	uint16_t m_cycle	  = 0x00;
	uint16_t m_scanline   = 0x00;
						  
	uint16_t ppuAddress	  = 0x00;
	uint8_t  addressLatch = 0x00;
	uint8_t  dataBuffer	  = 0x00;

	uint8_t  m_tile_lsb	  = 0x00;
	uint8_t  m_tile_msb   = 0x00;
	uint8_t	 m_pos		  = 0x00;
	uint8_t	 m_shift	  = 0x00;

	// SDL Rendering Stuff
	SDL_Color*    m_screenBuffer;
	SDL_Window*	  m_window;
	SDL_Renderer* m_renderer;

	SDL_Color*	  m_patternBuffer;
	SDL_Window*	  m_patternWindow;
	SDL_Renderer* m_patternRenderer;
	SDL_Texture*  m_patternScreen;

	SDL_Texture*  m_screen;
	SDL_Event	  m_event;
	uint8_t		  m_selectedPalette = 0x00;
	// -------------------


	// TODO: Make this modular eventually so that we could
	// swap out for a different pallette
	SDL_Color m_palette[0x40] {
		{ 84,  84,  84, 255}, {  0,  30, 116, 255}, {  8,  16, 144, 255}, { 48,   0, 136, 255},
		{ 68,   0, 100, 255}, { 92,   0,  48, 255}, { 84,   4,   0, 255}, { 60,  24,   0, 255},
		{ 32,  42,   0, 255}, {  8,  58,   0, 255}, {  0,  64,   0, 255}, {  0,  60,   0, 255},
		{  0,  50,  60, 255}, {  0,   0,   0, 255}, {  0,   0,   0, 255}, {  0,   0,   0, 255},

		{152, 150, 152, 255}, {  8,  76, 196, 255}, { 48,  50, 236, 255}, { 92,  30, 228, 255},
		{136,  20, 176, 255}, {160,  20, 100, 255}, {152,  34,  32, 255}, {120,  60,   0, 255},
		{ 84,  90,   0, 255}, { 40, 114,   0, 255}, {  8, 124,   0, 255}, {  0, 118,  40, 255},
		{  0, 102, 120, 255}, {  0,   0,   0, 255}, {  0,   0,   0, 255}, {  0,   0,   0, 255},

		{236, 238, 236, 255}, { 76, 154, 236, 255}, {120, 124, 236, 255}, {176,  98, 236, 255},
		{228,  84, 236, 255}, {236,  88, 180, 255}, {236, 106, 100, 255}, {212, 136,  32, 255},
		{160, 170,   0, 255}, {116, 196,   0, 255}, { 76, 208,  32, 255}, { 56, 204, 108, 255},
		{ 56, 180, 204, 255}, { 60,  60,  60, 255}, {  0,   0,   0, 255}, {  0,   0,   0, 255},

		{236, 238, 236, 255}, {168, 204, 236, 255}, {188, 188, 236, 255}, {212, 178, 236, 255},
		{236, 174, 236, 255}, {236, 174, 212, 255}, {236, 180, 176, 255}, {228, 196, 144, 255},
		{204, 210, 120, 255}, {180, 222, 120, 255}, {168, 226, 144, 255}, {152, 226, 180, 255},
		{160, 214, 228, 255}, {160, 162, 160, 255}, {  0,   0,   0, 255}, {  0,   0,   0, 255}
	};



	//						+---------------+
	//						|	Registers	|
	//						+---------------+

	// PPU Control Register		($2000)
	union PPUCTRL {
		struct {
			uint8_t nameTableX		: 1;	//	Base Nametable Address X
			uint8_t nameTableY		: 1;	//	Base Nametable Address Y
			uint8_t incrementMode	: 1;	//	VRAM Address Increment
			uint8_t sprPattern		: 1;	//	Sprite Pattern Table Address
			uint8_t bgPattern		: 1;	//	Background Pattern Table Address
			uint8_t sprSize			: 1;	//	Sprite Size
			uint8_t slaveMode		: 1;	//	PPU Master/Slave Select
			uint8_t enableNmi		: 1;	//	Generate NMI at VBlank Start
		};
		uint8_t data;
	}							PPU_CTRL;


	// PPU Mask Register		($2001)
	union PPUMASK {
		struct {
			uint8_t greyScale		: 1;	//	Greyscale
			uint8_t bgShowLeft		: 1;	//	Show BG in Leftmost 8px
			uint8_t sprShowLeft		: 1;	//	Show Sprites in Leftmost 8px
			uint8_t bgShow			: 1;	//	Show Background
			uint8_t sprShow			: 1;	//	Show Sprites
			uint8_t emphasizeRed	: 1;	//	Emphasize Red
			uint8_t emphasizeGreen	: 1;	//	Emphasize Green
			uint8_t emphasizeBlue	: 1;	//	Emphasize Blue
		};
		uint8_t data;
	}							PPU_MASK;


	//	PPU Status Register		($2002)
	union PPUSTATUS {
		struct {
			uint8_t unused			: 5;	//	Unused
			uint8_t sprOverflow		: 1;	//	Sprite Overflow
			uint8_t spr0Hit			: 1;	//	Sprite Zero Hit
			uint8_t verticalBlank	: 1;	//	Vertical Blank
		};
		uint8_t data;
	}							PPU_STATUS;

	// TODO: Add all other registers

};
