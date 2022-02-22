/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 notification.hpp
 */

#pragma once

#include "image/image.hpp"
#include "util/bitmaskenum.hpp"

#include <memory>



class Notification {

public:

	enum class Options {
		None = 0x0,
		Hidden = 0x1,
		NoSound = 0x2
	};

	ARC_CREATE_MEMBER_BITMASK_ENUM(Options)

	using enum Options;


	Notification();
	~Notification();

	Notification(Notification&&) = default;
	Notification& operator=(Notification&&) = default;

	Options getOptions() const;

	void setTitle(const std::string& title);
	void setText(const std::string& text);
	void setTooltip(const std::string& tooltip);

	void setStandardIcon(const std::string& name);
	void setIcon(const Image<Pixel::RGBA8>& icon);
	void setImage(const Image<Pixel::RGBA8>& image);

	void removeIcon();
	void removeImage();

	void setOptions(Options options);

	void show();
	void remove();

	static Notification post(const std::string& title, const std::string& text, const std::string& tooltip, const std::string& icon, Options options = Options::None);
	static Notification post(const std::string& title, const std::string& text, const std::string& tooltip, const Image<Pixel::RGBA8>& icon, Options options = Options::None);
	static Notification post(const std::string& title, const std::string& text, const std::string& tooltip, const Image<Pixel::RGBA8>& icon, const Image<Pixel::RGBA8>& image, Options options = Options::None);

	static void purgeAll();

private:

	void createHandle();

	std::unique_ptr<class NotificationHandle> handle;
	Options options;

};