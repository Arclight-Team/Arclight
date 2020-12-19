#include "util/random.h"
#include "util/assert.h"

#include <chrono>


static void kekw() {}


Random::Random() {

	int* x = new int;
	delete x;

	std::seed_seq seed{ std::chrono::high_resolution_clock::now().time_since_epoch().count(), reinterpret_cast<long long>(&kekw), reinterpret_cast<long long>(&x) };
	rng.seed(seed);

}

Random::Random(u64 seed) : rng(seed) {};



i64 Random::getInt() {
	return std::uniform_int_distribution<i64>{std::numeric_limits<i64>::min(), std::numeric_limits<i64>::max()}(rng);
}



i64 Random::getInt(i64 min, i64 max) {
	arc_assert(min < max, "Requested random minimum value greater than maximum");
	return std::uniform_int_distribution<i64>{min, max}(rng);
}



u64 Random::getUint() {
	return std::uniform_int_distribution<u64>{0, std::numeric_limits<u64>::max()}(rng);
}



u64 Random::getUint(u64 min, u64 max) {
	arc_assert(min < max, "Requested random minimum value greater than maximum");
	return std::uniform_int_distribution<u64>{min, max}(rng);
}



Random& Random::getRandom() {
	static Random random;
	return random;
}