/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 EntropySource.hpp
 */

#pragma once

#include "Common/Types.hpp"
#include "Common/Exception.hpp"

#include <span>
#include <memory>



class EntropySource {

public:

	EntropySource();

	void fetch(std::span<u8> buffer);

private:

	static void defaultFetch(std::span<u8> buffer) {
		throw UnsupportedOperationException("No default entropy sourcing method implemented");
	}

	std::shared_ptr<class EntropySourceContext> context;

};