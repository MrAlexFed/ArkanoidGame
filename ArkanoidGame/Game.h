#pragma once
#include "Engine.h"
#include <optional>
#include <array>
#include <vector>
#include <memory>

#define NUM_BRICKS_IN_LINE 10
#define NUM_BRICKS_ROW 4

struct D2
{
	D2(const double _x, const double _y) : x(_x), y(_y) {}
	D2 operator*(unsigned mult) { return D2(x * mult, y * mult); }

	double x;
	double y;
};

struct I2
{
	I2() = default;
	I2(const int _x, const int _y) : x(_x), y(_y) {}
	I2 operator+(const I2 sum) const { return I2(x+sum.x, y+sum.y); }
	I2 operator*(const float mult) const { return I2(x*mult, y*mult); }

	int x;
	int y;
};

class CBall;

class CRectangle
{
public:
	CRectangle() = default;
	CRectangle(const CRectangle&) = default;
	void draw(uint32_t buffer[SCREEN_HEIGHT][SCREEN_WIDTH]) const;
	void setColor(const int color) { m_color = color; }
	void setPos(const I2& pos) { m_ptMin = pos; }
	I2 pos() const { return m_ptMin; }
	I2 WH() const { return I2(m_width, m_height); }
	void setWH(const int width, const int height);
	virtual bool checkCrashWithBall(CBall& ball) const;
protected:
	I2 m_ptMin;
	int m_width;
	int m_height;
	int m_color;
};

class CBall : public CRectangle
{
public:
	void move();
	void setSize(const int size) { setWH(size, size); }
	void setVelocity(const I2& vel) { m_velocity = vel; }
	void setVelocityCoef(float coef) { m_coef = coef; }
	I2 velocityByCoef() const { return m_velocity * m_coef; }
	I2 velocity() const { return m_velocity; }

private:
	I2 m_velocity;
	float m_coef;
};

class CBorders
{
public:
	CBorders();
	void setColor(const int color);
	void draw(uint32_t buffer[SCREEN_HEIGHT][SCREEN_WIDTH]) const;
	void checkCrashWithBall(CBall& ball);
private:
	std::array<CRectangle, 4> m_aBorder;
};

class CStick : public CRectangle
{
public:
	CStick();
	void reset();
	void move();
	void setVelocityCoef(float coef) { m_coefVel = coef; }

private:
	float m_coefVel;
};

struct SBrick
{
	CRectangle m_rec;
	bool m_bIsActive = true;
};

class CBricks
{
public:
	CBricks();
	void reset();
	void draw(uint32_t buffer[SCREEN_HEIGHT][SCREEN_WIDTH]) const;
	void setColor(const int color);
	void checkCrashWithBall(CBall& ball);

private:
	std::array<std::array<SBrick, NUM_BRICKS_IN_LINE>, NUM_BRICKS_ROW> m_aBrick;
};