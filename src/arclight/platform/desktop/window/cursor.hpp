/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 cursor.hpp
 */

#pragma once

#include "math/vector.hpp"
#include "image/pixel.hpp"

#include <unordered_map>



template<Pixel P>
class Image;

class Window;
class GLFWcursor;

class Cursor {

public:

	enum class StandardCursor {
		Arrow,
		Text,
		Crosshair,
		Hand,
		HResize,
		VResize
	};

	using enum StandardCursor;

	using CursorID = u32;
	constexpr static CursorID DefaultCursorID = -1;


	explicit Cursor(const Window& window) noexcept;
	~Cursor() noexcept;

	void restoreDefaultCursor();
	void setCurrentCursor(CursorID id);
	bool isActiveCursor(CursorID id) const;

	bool setCursor(CursorID id, const Image<Pixel::RGBA8>& image, const Vec2i& hotspot = {0, 0});
	bool setCursor(CursorID id, StandardCursor cursor);

	void destroyCursor(CursorID id) noexcept;
	void destroyAllCursors() noexcept;

	double getX() const noexcept;
	double getY() const noexcept;
	Vec2d getPosition() const noexcept;

	void setX(double x) noexcept;
	void setY(double y) noexcept;
	void setPosition(const Vec2d& pos) noexcept;
	void setPosition(double x, double y) noexcept;

private:

	const Window& window;
	CursorID currentID;
	std::unordered_map<CursorID, GLFWcursor*> cursors;

};