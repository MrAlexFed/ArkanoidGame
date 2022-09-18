#include "Game.h"
#include "Engine.h"
#include <stdlib.h>
#include <memory.h>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <windows.h>

#include <algorithm>


//  is_key_pressed(int button_vk_code) - check if a key is pressed,
//                                       use keycodes (VK_SPACE, VK_RIGHT, VK_LEFT, VK_UP, VK_DOWN, 'A', 'B')
//
//  get_cursor_x(), get_cursor_y() - get mouse cursor position
//  is_mouse_button_pressed(int button) - check if mouse button is pressed (0 - left button, 1 - right button)
//  clear_buffer() - set all pixels in buffer to 'black'
//  is_window_active() - returns true if window is active
//  schedule_quit_game() - quit game after act()

const int ÒBallColor = 0xC8F703;
const int cBallSize = 10;
const int cBallSpeed = 200;
const int ÒBorderColor = 0xB5EEDA;
const int cBorderThick = 6;
const int cBorderOffset = 4;
const int cStickColor = 0x8FF569;
const int cStickLength = 200;
const int cStickHeight = 16;
const int cStickSpeed = 600;
const int cBricksColor = 0x34EB86;
const int cBrickHeight = 40;

CBall ball;
CBorders borders;
CStick stick;
CBricks bricks;

bool bGameisRunning = true;

// change from usuall coordinates to coordinates convenient for drawing
static I2 convertCoord(I2 inCoord)
{
	return I2(SCREEN_HEIGHT - inCoord.y, inCoord.x);
}

static void setBollInitRandPos(CBall& boll)
{
	std::srand(int(std::time(nullptr)));
	I2 pos;
	pos.x = std::rand() % int(SCREEN_WIDTH*0.33) + int(SCREEN_WIDTH*0.33);
	pos.y = std::rand() % int(SCREEN_HEIGHT*0.33) + int(SCREEN_HEIGHT*0.33);
	boll.setPos(pos);
}

// initialize game data in this function
void initialize()
{
	ball.setColor(ÒBallColor);
	ball.setSize(cBallSize);
	ball.setVelocity(I2(cBallSpeed, -2*cBallSpeed));
	setBollInitRandPos(ball);
	borders.setColor(ÒBorderColor);
	stick.setColor(cStickColor);
	bricks.setColor(cBricksColor);
}

static void restart()
{
	ball.setVelocity(I2(cBallSpeed, -2*cBallSpeed));
	setBollInitRandPos(ball);
	stick.reset();
	bricks.reset();
}

// this function is called to update game data,
// dt - time elapsed since the previous update (in seconds)
void act(float dt)
{
	if(dt < 0.005)
	{
		const float dtOld = dt;
		dt = 0.005;
		Sleep((dt - dtOld)*1000);
	}

	if(bGameisRunning)
	{
		ball.setVelocityCoef(dt);
		stick.checkCrashWithBall(ball);
		stick.setVelocityCoef(dt);
		borders.checkCrashWithBall(ball);
		bricks.checkCrashWithBall(ball);
		ball.move();
		stick.move();
	}

	int button = 0;
	if(!bGameisRunning && (is_key_pressed(VK_SPACE) || is_mouse_button_pressed(button)))
	{
		restart();
		bGameisRunning = true;
	}

	if(is_key_pressed(VK_ESCAPE))
		schedule_quit_game();
}

// fill buffer in this function
// uint32_t buffer[SCREEN_HEIGHT][SCREEN_WIDTH] - is an array of 32-bit colors (8 bits per R, G, B)
void draw()
{
	memset(buffer, 0, SCREEN_HEIGHT * SCREEN_WIDTH * sizeof(uint32_t));

	ball.draw(buffer);
	borders.draw(buffer);
	stick.draw(buffer);
	bricks.draw(buffer);
}

// free game data in this function
void finalize()
{
	
}

void CRectangle::draw(uint32_t buffer[SCREEN_HEIGHT][SCREEN_WIDTH]) const
{
	I2 ptMinTemp = convertCoord(m_ptMin);
	ptMinTemp.x = ptMinTemp.x - m_height;
	for (size_t i = ptMinTemp.x; i < ptMinTemp.x + m_height; ++i)
	{
		for (size_t j = ptMinTemp.y; j < ptMinTemp.y + m_width; ++j)
		{
			buffer[i][j] = m_color;
		}
	}
}

void CRectangle::setWH(const int width, const int height)
{
	m_width = width;
	m_height = height;
}

static bool valueInRange(const int val, const int min, const int max)
{
	return (val >= min) && (val <= max);
}

static bool rectOverlap(const CRectangle& rect1, const CRectangle& rect2)
{
	bool xOverlap = valueInRange(rect1.pos().x, rect2.pos().x, rect2.pos().x + rect2.WH().x) || valueInRange(rect2.pos().x, rect1.pos().x, rect1.pos().x + rect1.WH().x);
	bool yOverlap = valueInRange(rect1.pos().y, rect2.pos().y, rect2.pos().y + rect2.WH().y) || valueInRange(rect2.pos().y, rect1.pos().y, rect1.pos().y + rect1.WH().y);
	return xOverlap && yOverlap;
}

bool CRectangle::checkCrashWithBall(CBall& ball) const
{
	CRectangle temp(ball);
	I2 newPosX = temp.pos() + I2(ball.velocityByCoef().x, 0);
	temp.setPos(newPosX);
	if(rectOverlap(*this, temp))
	{
		ball.setVelocity(I2(-ball.velocity().x, ball.velocity().y));
		return true;
	}

	I2 newPosY = temp.pos() + I2(0, ball.velocityByCoef().y);
	temp.setPos(newPosY);
	if(rectOverlap(*this, temp))
	{
		ball.setVelocity(I2(ball.velocity().x, -ball.velocity().y));
		return true;
	}

	return false;
}

void CBall::move()
{
	m_ptMin = m_ptMin + velocityByCoef();
}

CBorders::CBorders()
{
	//  __1__
	// 0|___|2
	//    3   
	m_aBorder[0].setPos(I2(cBorderOffset, cBorderOffset));
	m_aBorder[0].setWH(cBorderThick, SCREEN_HEIGHT - 2*cBorderOffset);
	m_aBorder[1].setPos(I2(cBorderOffset, SCREEN_HEIGHT - cBorderOffset - cBorderThick));
	m_aBorder[1].setWH(SCREEN_WIDTH - 2*cBorderOffset, cBorderThick);
	m_aBorder[2].setPos(I2(SCREEN_WIDTH - cBorderOffset - cBorderThick, cBorderOffset));
	m_aBorder[2].setWH(cBorderThick, SCREEN_HEIGHT - 2*cBorderOffset);
	m_aBorder[3].setPos(I2(cBorderOffset, cBorderOffset));
	m_aBorder[3].setWH(SCREEN_WIDTH - 2*cBorderOffset, cBorderThick + cBorderOffset);
}

void CBorders::setColor(const int color)
{
	for(int i = 0; i < 3; ++i)
		m_aBorder[i].setColor(color);
}

void CBorders::draw(uint32_t buffer[SCREEN_HEIGHT][SCREEN_WIDTH]) const
{
	// don't draw bottom border
	for(int i = 0; i < 3; ++i)
		m_aBorder[i].draw(buffer);
}

void CBorders::checkCrashWithBall(CBall& ball)
{
	for(int i = 0; i < 4; ++i)
	{
		if(m_aBorder[i].checkCrashWithBall(ball) && i == 3)
			bGameisRunning = false;
	}
}

CStick::CStick()
{
	reset();
}

void CStick::reset()
{
	setPos(I2( (SCREEN_WIDTH - cStickLength)/2, cBorderOffset ));
	setWH(cStickLength, cStickHeight);
}

void CStick::move()
{
	static const int stickOffset = 2*cBorderOffset + cBorderThick;
	const int stickSpeed = m_coefVel*cStickSpeed;
	if(is_window_active())
	{
		//const int cursX = get_cursor_x();
		const int delta = m_ptMin.x - get_cursor_x() + cStickLength/2;
		int shift = std::abs(delta) > stickSpeed ? stickSpeed : std::abs(delta);
		shift = delta > 0 ? -shift : shift;
		m_ptMin.x += shift;
		if(m_ptMin.x < stickOffset)
			m_ptMin.x = stickOffset;
		else if(m_ptMin.x > SCREEN_WIDTH - stickOffset - m_width)
			m_ptMin.x = SCREEN_WIDTH - stickOffset - m_width;
	}
}

CBricks::CBricks()
{
	const int lengthForBricks = SCREEN_WIDTH - (cBorderOffset + cBorderThick)*2;
	const int bricksOffset = lengthForBricks / NUM_BRICKS_IN_LINE * 0.1;
	const int brickWidth = (lengthForBricks - bricksOffset*(NUM_BRICKS_IN_LINE + 1)) / NUM_BRICKS_IN_LINE;

	int coordY = SCREEN_HEIGHT - cBorderOffset - cBorderThick - bricksOffset - cBrickHeight;
	for(size_t i = 0; i < NUM_BRICKS_ROW; ++i)
	{
		int coordX = cBorderOffset + cBorderThick + bricksOffset;
		for(size_t j = 0; j < NUM_BRICKS_IN_LINE; ++j)
		{
			m_aBrick[i][j].m_rec.setPos(I2(coordX, coordY));
			m_aBrick[i][j].m_rec.setWH(brickWidth, cBrickHeight);
			coordX += brickWidth + bricksOffset;
		}
		coordY -= bricksOffset + cBrickHeight;
	}
}

void CBricks::reset()
{
	for(auto& brRow : m_aBrick)
	{
		for(auto& br : brRow)
		{
			br.m_bIsActive = true;
		}
	}
}

void CBricks::draw(uint32_t buffer[SCREEN_HEIGHT][SCREEN_WIDTH]) const
{
	for(auto& brRow : m_aBrick)
	{
		for(auto& br : brRow)
		{
			if(br.m_bIsActive)
				br.m_rec.draw(buffer);
		}
	}
}

void CBricks::setColor(const int color)
{
	for(auto& brRow : m_aBrick)
	{
		for(auto& br : brRow)
		{
			br.m_rec.setColor(color);
		}
	}
}

void CBricks::checkCrashWithBall(CBall& ball)
{
	for(auto& brRow : m_aBrick)
	{
		for(auto& br : brRow)
		{
			if(br.m_bIsActive && br.m_rec.checkCrashWithBall(ball))
				br.m_bIsActive = false;
		}
	}
}