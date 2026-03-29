/* drawing.h */
#ifndef DRAWING_H
#define DRAWING_H
#define STB_IMAGE_WRITE_IMPLEMENTATION


#include <glm/glm.hpp>
#include <stb_image_write.h>
#include <cstdio>
#include <iostream>

#include "types.h"
#include "font.h"
#include "utils.h"


namespace screen {

	//8x8 chars.
	#define WIDTH_GLYPH 8u
	#define HEIGHT_GLYPH 8u

	//64x48 canvas.
	#define WIDTH_CHARS 32u
	#define HEIGHT_CHARS 24u

	//Define as the above.
	#define WIDTH_PX (WIDTH_CHARS * WIDTH_GLYPH)
	#define HEIGHT_PX (HEIGHT_CHARS * HEIGHT_GLYPH)


	//Colour enums;
	//8 Colours = 3 bits, use for binary channels bitmask.
	enum Colour {
		C_BLACK   = 0b000,

		C_RED     = 0b100,
		C_GREEN   = 0b010,
		C_BLUE    = 0b001,

		C_YELLOW  = 0b110,
		C_CYAN    = 0b011,
		C_MAGENTA = 0b101,

		C_WHITE   = 0b111
	};

	inline uint8_t buffer[WIDTH_PX * HEIGHT_PX]; //Indexed colour.
}


namespace draw {

	inline bool pixel(screen::Colour c, glm::ivec2 position) {
		//Position in pixel coordinates.
		//Check bounds;
		if (
			((position.x < 0) || (position.x >= (int)(WIDTH_PX))) ||
			((position.y < 0) || (position.y >= (int)(HEIGHT_PX)))
		) {
			return false; //OOB.
		}

		screen::buffer[(position.y * WIDTH_PX) + position.x] = c;
		return true; //Success
	}


	inline bool horizontalLine(screen::Colour c, glm::ivec2 position, int length) {
		if ((position.y < 0) || (position.y >= (int)(HEIGHT_PX))) {return false;}
		if (length == 0) {return false;}

		glm::ivec2 start = position;
		if (length < 0) {
			length = -length; //Make positive.
			start.x -= length; //Move start backwards.
		}
		glm::ivec2 end = start += glm::ivec2(length, 0);

		//Limit to screen bounds.
		start.x = glm::max(0, start.x);
		end.x = glm::min((int)(WIDTH_PX-1), end.x);

		std::fill(
			screen::buffer + (start.y * WIDTH_PX) + start.x, //First
			screen::buffer + (end.y * WIDTH_PX) + end.x, //Last
			c //Fill value
		);

		return true;
	}


	inline void displayGlyph(const types::Glyph& glf) {
		for (unsigned int y=0u; y<HEIGHT_GLYPH; y++) {
			for (unsigned int x=0u; x<WIDTH_GLYPH; x++) {
				//Mask out a single bit.
				unsigned int bitShift = 63 - ((y*WIDTH_GLYPH)+x);
				assert(bitShift < 64); //Should never try shift too far.
				bool isForeground = (
					(glf >> bitShift) & 0x1u
				) != 0u;

				if (isForeground) {
					std::cout << "▓▓";
				} else {
					std::cout << "░░";
				}
			}
			std::cout << "\n";
		}
		std::cout << std::flush;
	}


	inline bool glyph(
		GlyphID ID, glm::ivec2 position,
		screen::Colour fg=screen::C_BLACK,
		screen::Colour bg=screen::C_WHITE
	) {
		//Position in text coordinates.
		//Check bounds;
		if (
			((position.x < 0) || (position.x >= (int)(WIDTH_CHARS))) ||
			((position.y < 0) || (position.y >= (int)(HEIGHT_CHARS)))
		) {
			return false; //OOB.
		}

		//Fetch and read px.
		const types::Glyph& glf = font::glyphs[ID]; //Single uint64_t (typenamed Glyph)
			
	#ifdef DEBUG_FONT
		displayGlyph(glf);
	#endif
		
		uint8_t* ptr = screen::buffer + (position.y * HEIGHT_GLYPH * WIDTH_PX) + (position.x * WIDTH_GLYPH); //Start ptr.
		//printf("\n\n%p", ptr);
		for (unsigned int y=0u; y<HEIGHT_GLYPH; y++) {
			for (unsigned int x=0u; x<WIDTH_GLYPH; x++) {
				//Mask out a single bit.
				unsigned int bitShift = 63 - ((y*WIDTH_GLYPH)+x);
				assert(bitShift < 64); //Should never try shift too far.
				bool isForeground = (
					(glf >> bitShift) & 0x1u
				) != 0u;
				//printf("\n%p : (%u, %u), << %u", ptr, x, y, bitShift);
				*ptr = (isForeground) ? fg : bg;

				//Increment ptr.
				//New column.
				ptr++;
			}
			//New row.
			ptr += WIDTH_PX - WIDTH_GLYPH;
		}
		return true; //Success
	}


	inline void text(
		const std::string& text, glm::ivec2 position,
		screen::Colour fg=screen::C_BLACK,
		screen::Colour bg=screen::C_WHITE
	) {
		//Draws some string. First char at position.
		int x = position.x;
		for (const char& c : text) {
			if (x >= (int)(WIDTH_CHARS)) {break; /* If it starts to go offscreen, exit early. Can never re-enter screen. */}
			
		#ifdef DEBUG_FONT
			std::cout << std::format(
				"Char: \'{}\', glyphID: {}", c, std::to_string(font::asciiToGEnum(c))
			) << std::endl;
		#endif

			glyph(
				font::asciiToGEnum(c), //GlyphID
				glm::ivec2(x++, position.y), //Position, increment X.
				fg, bg //Colours
			);
		}
	}


	inline void clearBuffer(screen::Colour col=screen::C_WHITE) {
		//Fill pixels
		std::fill(
			screen::buffer, //First
			screen::buffer + (WIDTH_PX * HEIGHT_PX) - 1u, //Last
			col //Clear value
		);
	}


	inline void saveBuffer(const std::string filePath) {
	#ifdef BUFFER_2x

		//Double the size of the buffer. Each value is now a 2x2 block.
		std::vector<uint8_t> RGBbuf(WIDTH_PX * HEIGHT_PX * 12u); //3×2×2

		for (unsigned int y = 0; y < HEIGHT_PX; y++) {
			for (unsigned int x = 0; x < WIDTH_PX; x++) {
				uint8_t col = screen::buffer[y * WIDTH_PX + x];

				uint8_t r = (col & 0b100) ? 0xFFu : 0x00u;
				uint8_t g = (col & 0b010) ? 0xFFu : 0x00u;
				uint8_t b = (col & 0b001) ? 0xFFu : 0x00u;

				for (unsigned int dy=0u; dy<2u; dy++) {
					for (unsigned int dx=0u; dx<2u; dx++) {

						unsigned int outX = (x * 2u) + dx;
						unsigned int outY = (y * 2u) + dy;

						size_t idx = (outY * WIDTH_PX * 2u + outX) * 3u;

						RGBbuf[idx + 0u] = r;
						RGBbuf[idx + 1u] = g;
						RGBbuf[idx + 2u] = b;
					}
				}
			}
		}

		//Write data.
		stbi_write_png(
			filePath.c_str(), //Where
			WIDTH_PX * 2u, HEIGHT_PX * 2u, 3u, //Image data format
			RGBbuf.data(), //What
			WIDTH_PX * 6u //Stride
		);

	#else

		//Direct pixel-to-pixel buffer.
		std::vector<uint8_t> RGBbuf(WIDTH_PX * HEIGHT_PX * 3u);
		uint8_t* ptr = RGBbuf.data();
		for (uint8_t& col : screen::buffer) {
			//Add to buffer.
			ptr[0] = (col & 0b100) ? 0xFFu : 0x00u; //R
			ptr[1] = (col & 0b010) ? 0xFFu : 0x00u; //G
			ptr[2] = (col & 0b001) ? 0xFFu : 0x00u; //B
			ptr += 3u;
		}

		//Write data.
		stbi_write_png(
			filePath.c_str(), //Where
			WIDTH_PX, HEIGHT_PX, 3u, //Image data format
			RGBbuf.data(), //What
			WIDTH_PX * 3u //Stride
		);

	#endif
	}



	void debug() {
		//Testing fontset
		std::string alpha = "abcdefghijklmnopqrstuvwxyz";
		text(alpha, glm::ivec2(0, 0));
		utils::toUpper(alpha);
		text(alpha, glm::ivec2(0, 1));

		std::string digits = "0123456789";
		text(digits, glm::ivec2(0, 2));

		glyph(G_UNKNOWN, glm::ivec2(0, 3));
		std::string symbols = " ,.:;#()[]{}!?\"'+-*=/\\<>_|&%";
		text(symbols, glm::ivec2(1, 3));

		saveBuffer("font.debug.png");
		return;
	}
}


#endif