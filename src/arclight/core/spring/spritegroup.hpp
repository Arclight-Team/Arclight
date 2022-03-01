/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 spritegroup.hpp
 */

#pragma once

#include "spritebatch.hpp"
#include "ctgrouptablebuffer.hpp"

#include <vector>
#include <unordered_map>



using CTAllocationTable = std::vector<u32>;
using CTBindings = std::vector<std::pair<u32, u32>>;

class SpriteGroup {

public:

	SpriteGroup() : visible(true), ctUpdate(true) {}

	SpriteBatch& getBatch();
	const SpriteBatch& getBatch() const;

	const CTAllocationTable& getCTAllocationTable() const;
	const CTBindings& getCTBindings() const;

	void addCTReference(u32 ctID);
	void removeCTReference(u32 ctID);

	void prepareCTTable(const CTAllocationTable& prev, bool forceRecalculate = false);

	void syncData();
	void render();

	constexpr bool isVisible() const { return visible; }

	constexpr void show() { visible = true; }
	constexpr void hide() { visible = false; }
	constexpr void setVisibility(bool visible) { this->visible = visible; }
	constexpr void toggleVisibility() { visible = !visible; }

private:

	bool visible;

	SpriteBatch batch;
	bool ctUpdate;

	std::unordered_map<u32, u32> ctReferences;

	CTBindings ctBindings;
	CTAllocationTable ctAllocationTable;
	CTGroupTableBuffer ctGroupTableBuffer;

};