/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 RenderCursor.hpp
 */

#pragma once

#include "Math/Vector.hpp"
#include "Image/Pixel.hpp"

#include <unordered_map>



template<Pixel P>
class Image;

class Window;
class WindowHandle;
class GLFWcursor;

class RenderCursor {

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


	explicit RenderCursor(std::weak_ptr<WindowHandle> window) noexcept;
	~RenderCursor() noexcept;

	void restoreDefaultCursor();

	void setCurrent(CursorID id);
	CursorID getCurrent() const;
	bool isActive(CursorID id) const;

	bool set(CursorID id, const Image<Pixel::RGBA8>& image, const Vec2i& hotspot = {0, 0});
	bool set(CursorID id, StandardCursor cursor);

	void destroy(CursorID id) noexcept;
	void destroyAll() noexcept;

private:

	std::weak_ptr<WindowHandle> window;
	CursorID currentID;
	std::unordered_map<CursorID, GLFWcursor*> cursors;

};