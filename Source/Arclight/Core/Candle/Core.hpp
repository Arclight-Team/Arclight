/*
 *	 Copyright (c) 2024 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 Core.hpp
 */

#pragma once

#include "Meta/Concepts.hpp"
#include "Util/Preprocessor.hpp"
#include "Util/Identifier.hpp"

#include <source_location>
#include <unordered_map>
#include <utility>
#include <vector>
#include <format>
#include <stack>



namespace Candle::Detail {

	struct Failure {

		enum class Type {
			Equality,
			Comparison,
			Condition,
			Error
		};

		using enum Type;


		constexpr Failure(Type type, const std::source_location& location, std::string text, std::string values = "")
			: text(std::move(text)), values(std::move(values)), type(type), location(location) {}


		inline std::string toString(bool located) const {

			constexpr const char* Types[] = {"Equality", "Comparison", "Condition", "Error"};

			std::string string = Types[std::to_underlying(type)];

			if (!text.empty()) {
				string += " \"" + text + '"';
			}

			if (type != Error) {
				string += " failed";
			}

			if (!values.empty()) {
				string += " (" + values + ')';
			}

			if (located) {
				string += std::format(" at \"{}\" [{}:{}]", location.file_name(), location.line(), location.column());
			}

			return string;

		}


		std::source_location location;

		std::string text;
		std::string values;

		Type type;

	};

}


namespace Candle {

	enum class ExecutionFlags {
		None	= 0x0,
		Strict	= 0x1,
		Verbose	= 0x2,
	};

	ARC_CREATE_BITMASK_ENUM(ExecutionFlags);

	using enum ExecutionFlags;


	class TestContext {

	public:

		constexpr TestContext() = default;


		template<class A, CC::EqualityComparableWith<A> B>
		constexpr void equality(A&& a, B&& b, bool invert = false, const std::string& code = "", const std::source_location& location = std::source_location::current()) {

			if ((std::forward<A>(a) == std::forward<B>(b)) == invert) {
				failures.emplace_back(Detail::Failure::Equality, location, code, inequalityString(a, b));
			}

		}

		template<class A, CC::TotallyOrderedWith<A> B>
		constexpr void greater(A&& a, B&& b, const std::string& code = "", const std::source_location& location = std::source_location::current()) {

			if (!(std::forward<A>(a) > std::forward<B>(b))) {
				failures.emplace_back(Detail::Failure::Comparison, location, code, orderingString(a, b));
			}

		}

		template<class A, CC::TotallyOrderedWith<A> B>
		constexpr void greaterEqual(A&& a, B&& b, const std::string& code = "", const std::source_location& location = std::source_location::current()) {

			if (!(std::forward<A>(a) >= std::forward<B>(b))) {
				failures.emplace_back(Detail::Failure::Comparison, location, code, orderingString(a, b));
			}

		}

		template<class A, CC::TotallyOrderedWith<A> B>
		constexpr void less(A&& a, B&& b, const std::string& code = "", const std::source_location& location = std::source_location::current()) {

			if (!(std::forward<A>(a) < std::forward<B>(b))) {
				failures.emplace_back(Detail::Failure::Comparison, location, code, orderingString(a, b));
			}

		}

		template<class A, CC::TotallyOrderedWith<A> B>
		constexpr void lessEqual(A&& a, B&& b, const std::string& code = "", const std::source_location& location = std::source_location::current()) {

			if (!(std::forward<A>(a) <= std::forward<B>(b))) {
				failures.emplace_back(Detail::Failure::Comparison, location, code, orderingString(a, b));
			}

		}

		constexpr void condition(bool condition, const std::string& code = "", const std::source_location& location = std::source_location::current()) {

			if (!condition) {
				failures.emplace_back(Detail::Failure::Condition, location, code);
			}

		}

		constexpr void error(const std::string& text = "", const std::source_location& location = std::source_location::current()) {
			failures.emplace_back(Detail::Failure::Error, location, text);
		}


		constexpr void clear() {
			failures.clear();
		}


		constexpr bool success() const noexcept {
			return failures.empty();
		}

		constexpr std::span<const Detail::Failure> data() const noexcept {
			return {failures.data(), failures.size()};
		}

	private:

		template<class A, CC::EqualityComparableWith<A> B>
		static constexpr std::string inequalityString(A&& a, B&& b) {

			if constexpr (CC::Arithmetic<TT::Decay<A>> && CC::Arithmetic<TT::Decay<B>>) {
				return std::to_string(std::forward<A>(a)) + " != " + std::to_string(std::forward<B>(b));
			} else {
				return {};
			}

		}

		template<class A, CC::TotallyOrderedWith<A> B>
		static constexpr std::string orderingString(A&& a, B&& b) {

			if constexpr (CC::Arithmetic<TT::Decay<A>> && CC::Arithmetic<TT::Decay<B>> && CC::ThreeWayComparableWith<A, B, std::weak_ordering>) {

				const char* op;

				const std::weak_ordering ordering = std::weak_order(std::forward<A>(a), std::forward<B>(b));

				if (ordering == std::weak_ordering::less) {
					op = " < ";
				} else if (ordering == std::weak_ordering::greater) {
					op = " > ";
				} else { // std::weak_ordering::equivalent
					op = " == ";
				}

				return std::to_string(std::forward<A>(a)) + op + std::to_string(std::forward<B>(b));

			} else {

				return {};

			}

		}


		std::vector<Detail::Failure> failures;

	};

	struct TestHandle {

		using FunctionT = void(*)(TestContext& context);


		constexpr TestHandle(FunctionT function, std::string name = "") : name(std::move(name)), function(function) {}


		constexpr bool unnamed() const noexcept {
			return name.empty();
		}


		std::string name;
		FunctionT function;

	};


	extern bool execute(const std::string& module = "", ExecutionFlags flags = None) noexcept;

	extern bool registerModule(const std::string& module, const TestHandle& test) noexcept;


	inline bool execute(ExecutionFlags flags) {
		return execute("", flags);
	}

}


namespace Candle::Detail {

	struct StaticRegister {

		StaticRegister(const std::string& module, const TestHandle& test) noexcept;

	};


	struct TreeNode {

		explicit TreeNode() = default;


		inline bool isParent() const noexcept {
			return !children.empty();
		}

		inline bool hasTests() const noexcept {
			return !tests.empty();
		}


		std::unordered_map<std::string, TreeNode> children;
		std::vector<TestHandle> tests;

	};

	struct ExecuteNode {

		constexpr ExecuteNode(const Detail::TreeNode& node, std::string module, bool expanded = false)
			: node(node), module(std::move(module)), expanded(expanded) {}


		std::vector<std::string> process(const std::string& spacing, bool& terminate, ExecutionFlags flags) const;

		template<CC::Returns<bool, const ExecuteNode&, SizeT> F>
		void traverse(bool expand, F&& callback) const {

			std::stack<ExecuteNode> stack;

			if (expand) {

				for (const auto& [key, child] : node.children) {
					stack.emplace(child, key);
				}

			} else {

				stack.emplace(*this);

			}

			SizeT depth = 0;

			while (!stack.empty()) {

				auto& top = stack.top();


				if (top.expanded) {

					// Closing parent node, decrease current depth

					stack.pop();
					depth--;

					continue;

				} else if (top.node.isParent()) {

					// Entering parent node, increase current depth and expand children

					for (const auto& [key, child] : top.node.children) {
						stack.emplace(child, key);
					}

					top.expanded = true;

				}


				if (!callback(top, depth)) {
					break;
				}


				if (!top.node.isParent()) {
					stack.pop(); // Pop evaluated child
				} else {
					depth++;
				}

			}

		}


		const Detail::TreeNode& node;

		std::string module;
		bool expanded;

	};


	extern TreeNode* root;

}


#define __ARC_CANDLE_TEST_IMPL(id, module, name)							\
																			\
static void ARC_PP_CAT(candleTest, id)(::Candle::TestContext&);				\
																			\
static ::Candle::Detail::StaticRegister ARC_PP_CAT(candlerRegister, id) = {	\
	module, {&ARC_PP_CAT(candleTest, id), name}								\
};																			\
																			\
static void ARC_PP_CAT(candleTest, id)(::Candle::TestContext& candle) /* { ... } */


#define __ARC_CANDLE_TEST_UNNAMED(module)	__ARC_CANDLE_TEST_IMPL(__COUNTER__, module, "")
#define __ARC_CANDLE_TEST(module, name)		__ARC_CANDLE_TEST_IMPL(__COUNTER__, module, name)


#define candle_test(...) ARC_PP_GET2(__VA_ARGS__, __ARC_CANDLE_TEST, __ARC_CANDLE_TEST_UNNAMED)(__VA_ARGS__)

#define candle_execute			::Candle::execute


#define candle_equal(a, b)		candle.equality(a, b, false, #a " == " #b)

#define candle_not_equal(a, b)	candle.equality(a, b, true, #a " != " #b)

#define candle_greater(a, b)	candle.greater(a, b, #a " > " #b)

#define candle_less(a, b)		candle.less(a, b, #a " < " #b)

#define candle_greater_eq(a, b)	candle.greaterEqual(a, b, #a " >= " #b)

#define candle_less_eq(a, b)	candle.lessEqual(a, b, #a " <= " #b)

#define candle_condition(value)	candle.condition(value, #value)

#define candle_error(message)	candle.error(message);


#define candle_clear()			candle.clear()


#define candle_success()		candle.success()

#define candle_data()			candle.data()


#define candle_catch							\
												\
/* try {} */ catch (const std::exception& ex) {	\
	candle.error(Exception::getMessage(ex));	\
} catch (...) {									\
	candle.error("Unknown exception");			\
}												\
												\
do {} while (false) /* ; */
