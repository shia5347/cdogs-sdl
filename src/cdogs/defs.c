/*
    C-Dogs SDL
    A port of the legendary (and fun) action/arcade cdogs.
    Copyright (C) 1995 Ronny Wester
    Copyright (C) 2003 Jeremy Chin 
    Copyright (C) 2003-2007 Lucas Martin-King 

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

    This file incorporates work covered by the following copyright and
    permission notice:

    Copyright (c) 2014, Cong Xu
    All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:

    Redistributions of source code must retain the above copyright notice, this
    list of conditions and the following disclaimer.
    Redistributions in binary form must reproduce the above copyright notice,
    this list of conditions and the following disclaimer in the documentation
    and/or other materials provided with the distribution.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
    AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
    ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
    LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
    CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
    SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
    INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
    CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
    ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
    POSSIBILITY OF SUCH DAMAGE.
*/
#include "defs.h"

#include <math.h>

#include "tile.h"


int CmdGetReverse(int cmd)
{
	int newCmd = cmd & ~(CMD_LEFT | CMD_RIGHT | CMD_UP | CMD_DOWN);
	if (cmd & CMD_LEFT) newCmd |= CMD_RIGHT;
	if (cmd & CMD_RIGHT) newCmd |= CMD_LEFT;
	if (cmd & CMD_UP) newCmd |= CMD_DOWN;
	if (cmd & CMD_DOWN) newCmd |= CMD_UP;
	return newCmd;
}

special_damage_e StrSpecialDamage(const char *s)
{
	S2T(SPECIAL_FLAME, "Flame");
	S2T(SPECIAL_POISON, "Poison");
	S2T(SPECIAL_PETRIFY, "Petrify");
	S2T(SPECIAL_CONFUSE, "Confuse");
	CASSERT(false, "unknown special damage type");
	return SPECIAL_NONE;
}

int cmd2dir[16] = {
	0,			// Nothing
	DIRECTION_LEFT,		// CMD_LEFT
	DIRECTION_RIGHT,	// CMD_RIGHT
	0,			// CMD_LEFT + CMD_RIGHT
	DIRECTION_UP,		// CMD_UP
	DIRECTION_UPLEFT,	// CMD_UP + CMD_LEFT
	DIRECTION_UPRIGHT,	// CMD_UP + CMD_RIGHT
	0,			// CMD_UP + CMD_LEFT + CMD_RIGHT
	DIRECTION_DOWN,		// CMD_DOWN
	DIRECTION_DOWNLEFT,	// CMD_DOWN + CMD_LEFT
	DIRECTION_DOWNRIGHT,	// CMD_DOWN + CMD_RIGHT
	0,			// CMD_DOWN + CMD_LEFT + CMD_RIGHT
	0,			// CMD_UP + CMD_DOWN
	0,			// CMD_UP + CMD_DOWN + CMD_LEFT
	0,			// CMD_UP + CMD_DOWN + CMD_RIGHT
	0			// CMD_UP + CMD_DOWN + CMD_LEFT + CMD_RIGHT
};

int dir2cmd[8] = {
	CMD_UP,
	CMD_UP + CMD_RIGHT,
	CMD_RIGHT,
	CMD_DOWN + CMD_RIGHT,
	CMD_DOWN,
	CMD_DOWN + CMD_LEFT,
	CMD_LEFT,
	CMD_UP + CMD_LEFT
};

float dir2radians[8] =
{
	0,
	M_PIF * 0.25f,
	M_PIF * 0.5f,
	M_PIF * 0.75f,
	M_PIF,
	M_PIF * 1.25f,
	M_PIF * 1.5f,
	M_PIF * 1.75f,
};


struct vec Vec2FromRadiansScaled(const float radians)
{
	return to_vector2(
		sinf(radians), -cosf(radians) * TILE_HEIGHT / TILE_WIDTH);
}
struct vec Vec2FromRadians(const float radians)
{
	struct vec v = vector2_rotate(to_vector2(0, -1), radians);
	// Scale Y so that they match the tile ratios
	v.y *= (float)TILE_HEIGHT / TILE_WIDTH;
	return v;
}
direction_e RadiansToDirection(const double r)
{
	// constrain to range [0, 2PI)
	double radians = r;
	while (radians < 0)
	{
		radians += 2 * M_PI;
	}
	while (radians >= 2 * M_PI)
	{
		radians -= 2 * M_PI;
	}
	int d = (int)floor((radians + M_PI / 8.0) / (M_PI / 4.0));
	if (d < DIRECTION_UP)
	{
		d += DIRECTION_COUNT;
	}
	if (d >= DIRECTION_COUNT)
	{
		d -= DIRECTION_COUNT;
	}
	return (direction_e)d;
}
direction_e DirectionOpposite(const direction_e d)
{
	return (direction_e)(((int)d + 4) % DIRECTION_COUNT);
}
