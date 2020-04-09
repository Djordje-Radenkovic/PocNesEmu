#include "fmt/printf.h"

#include "PPU_2C02.h"


PPU_2C02::PPU_2C02() : m_size(8) {
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		fmt::print("Couldn't initialize SDL video!");
		return;
	}

	m_window = SDL_CreateWindow(
		"POCNESEMU", 
		SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
		1024, 960, NULL
	);

	m_renderer = SDL_CreateRenderer(m_window, -1, 0);
	m_screen = SDL_CreateTexture(m_renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STREAMING, 256, 240);
	m_screenBuffer = new SDL_Color[256*240];
}


PPU_2C02::~PPU_2C02() {
	if (m_renderer != nullptr)
		SDL_DestroyRenderer(m_renderer);
	if (m_window != nullptr)
		SDL_DestroyWindow(m_window);
	if (m_screen != nullptr)
		SDL_DestroyTexture(m_screen);
	if (m_screenBuffer != nullptr)
		delete[] m_screenBuffer;

	SDL_Quit();
}


bool PPU_2C02::isRunning() {
	return m_isRunning;
}

bool PPU_2C02::getNmi() {
	return m_nmi;
}

void PPU_2C02::clearNmi() {
	m_nmi = false;
}


void PPU_2C02::connectBus(std::shared_ptr<IBus<uint16_t, uint8_t>> bus) {
	m_bus = bus;
	m_busConnected = true;
}


void PPU_2C02::reset() {
	m_cycle = 0;
	m_isRunning = true;
	SDL_Color rm_pallette[4] = {
		{0, 0, 0, 255},
		{100, 50, 0, 255},
		{50, 100, 0, 255},
		{0, 50, 100, 255}
	};

	for (int tileY = 0; tileY < 16; tileY++) {
		for (int tileX = 0; tileX < 16; tileX++) {
			int offset = tileY * 256 + tileX * 16;

			for (int row = 0; row < 8; row++) {
				int tile_lsb = m_bus->read(offset + row + 0);
				int tile_msb = m_bus->read(offset + row + 8);

				for (int col = 0; col < 8; col++) {
					int pixel = (tile_lsb & 0x01)
						+ (tile_msb & 0x01);
					tile_lsb >>= 1; tile_msb >>= 1;

					m_screenBuffer[tileX * 8 + (7 - col) +
						(tileY * 8 + row) * 256] =
						rm_pallette[pixel];
				}
			}
		}
	}

	for (int tileY = 0; tileY < 16; tileY++) {
		for (int tileX = 0; tileX < 16; tileX++) {
			int offset = tileY * 256 + tileX * 16;

			for (int row = 0; row < 8; row++) {
				int tile_lsb = m_bus->read(0x1000 + offset + row + 0);
				int tile_msb = m_bus->read(0x1000 + offset + row + 8);

				for (int col = 0; col < 8; col++) {
					int pixel = (tile_lsb & 0x01)
						+ (tile_msb & 0x01);
					tile_lsb >>= 1; tile_msb >>= 1;

					m_screenBuffer[128 + tileX * 8 + (7 - col) +
						(tileY * 8 + row) * 256] =
						rm_pallette[pixel];
				}
			}
		}
	}
}


void PPU_2C02::tick() {
	// Set VBlank and NMI
	if (m_scanline == -1 && m_cycle == 1)
		PPU_STATUS.verticalBlank = 0;

	if (m_scanline == 241 && m_cycle == 1) {
		PPU_STATUS.verticalBlank = 1;

		if (PPU_CTRL.enableNmi)
			m_nmi = true;
	}
	// ------------------

	SDL_Color rm_pallette[4] = {
		{0, 0, 0, 255},
		{100, 50, 0, 255},
		{50, 100, 0, 255},
		{0, 50, 100, 255}
	};

	/*if (m_cycle <= 256 && m_scanline < 240)
	for (int y = 0; y < 30; y++) {
		for (int x = 0; x < 32; x++) {
			m_screenBuffer[m_cycle - 1 + m_scanline * 256] =
				m_pallette[m_bus->read(0x2000 + y * 32 + x) % 0x40];
		}
	}*/


	//// Draw Pixel
	//if (m_cycle <= 256 && m_scanline < 240) {
	//	m_screenBuffer[m_cycle - 1 + m_scanline * 256] =
	//		m_pallette[m_bus->read(0x2000 + m_cycle + m_scanline * 32)];
	//}
	//// ----------
	

	// Reset Cycles and Scanlines
	if (m_cycle >= 341) {
 		m_cycle = 0;
 		m_scanline++;
 		if (m_scanline >= 261) {
 			m_scanline = -1;
 			m_frameComplete = true;
 		}
	}


	if (m_frameComplete) {
		// Poll Events
		SDL_PollEvent(&m_event);
		if (m_event.type == SDL_QUIT)
			m_isRunning = false;

		// Render Frame
		SDL_UpdateTexture(m_screen, NULL, m_screenBuffer, sizeof(SDL_Color) * 256);
		SDL_RenderCopy(m_renderer, m_screen, NULL, NULL);
		SDL_RenderPresent(m_renderer);

		m_frameComplete = false;
	}

	m_cycle++;
}


uint8_t PPU_2C02::read(uint16_t address) {
	switch(address % m_size) {
	case 0x0000:	// Control
		break;
	case 0x0001:	// Mask
		break;
	case 0x0002:	// Status
		PPU_STATUS.verticalBlank = 0;
		addressLatch = 0;

		//	Return top 3 bits, the rest are residual data
		return (PPU_STATUS.data & 0xE0) | (dataBuffer & 0x1F);

	case 0x0003:	// OAM Address
		break;
	case 0x0004:	// OAM Data
		break;
	case 0x0005:	// Scroll
		break;
	case 0x0006:	// PPU Address
		break;
	case 0x0007:	// PPU Data
		m_tempData = dataBuffer;
		dataBuffer = readFrom(ppuAddress);

		// Pallette reads are not delayed
		if (ppuAddress >= 0x3F00) {
			ppuAddress++;
			return dataBuffer;
		}

		ppuAddress++;
		return m_tempData;
	}

	return -1;
}


void PPU_2C02::write(uint16_t address, uint8_t data) {
	switch (address % m_size) {
	case 0x0000:	// Control
		PPU_CTRL.data = data;

		break;
	case 0x0001:	// Mask
		PPU_MASK.data = data;

		break;
	case 0x0002:	// Status
		break;

	case 0x0003:	// OAM Address
		break;

	case 0x0004:	// OAM Data
		break;

	case 0x0005:	// Scroll
		break;

	case 0x0006:	// PPU Address
		if (addressLatch == 0) {
			ppuAddress = (ppuAddress & 0x00FF) | (data << 8);
			addressLatch = 1;
		}
		else {
			ppuAddress = (ppuAddress & 0xFF00) | data;
			addressLatch = 0;
		}

		break;
	case 0x0007:	// PPU Data
		writeTo(ppuAddress, data);
		ppuAddress += (PPU_CTRL.incrementMode ? 32 : 1);

		break;

	}
}
