/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 entropysource.cpp
 */

#include "random/entropysource.hpp"

#include <Windows.h>



struct EntropySourceContext {

	EntropySourceContext() : handle(nullptr) {
		BCryptOpenAlgorithmProvider(&handle, BCRYPT_RNG_ALGORITHM, nullptr, 0);
	}

	~EntropySourceContext() {

		if (handle) {
			BCryptCloseAlgorithmProvider(handle, 0);
		}

	}

	BCRYPT_ALG_HANDLE handle;

};



EntropySource::EntropySource() : context(std::make_shared<EntropySourceContext>()) {}



void EntropySource::fetch(std::span<u8> buffer) {

	if (context) {

		if (!BCRYPT_SUCCESS(BCryptGenRandom(context->handle, buffer.data(), buffer.size(), 0))) {
			context = nullptr;  //Subsequent calls should revert to the default entropy fetch function
		} else {
			return;
		}
	}

	defaultFetch(buffer);

}