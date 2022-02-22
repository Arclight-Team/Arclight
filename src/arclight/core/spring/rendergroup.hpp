/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 rendergroup.hpp
 */

#pragma once

#include "spritebatch.hpp"
#include "ctgrouptablebuffer.hpp"

#include <vector>
#include <unordered_map>



using CTAllocationTable = std::vector<u32>;
using CTBindings = std::vector<std::pair<u32, u32>>;

class RenderGroup {

public:

	RenderGroup() : ctUpdate(true) {}

	SpriteBatch& getBatch();
	const SpriteBatch& getBatch() const;

	const CTAllocationTable& getCTAllocationTable() const;
	const CTBindings& getCTBindings() const;

	void addCTReference(u32 ctID);
	void removeCTReference(u32 ctID);

	void prepareCTTable(const CTAllocationTable& prev, bool forceRecalculate = false);

	void syncData();
	void render();

private:

	SpriteBatch batch;
	bool ctUpdate;

	std::unordered_map<u32, u32> ctReferences;

	CTBindings ctBindings;
	CTAllocationTable ctAllocationTable;
	CTGroupTableBuffer ctGroupTableBuffer;

};