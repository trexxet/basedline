#include "Basedtest/Suite.hpp"

#include <string>
#include <string_view>

namespace Basedline {
int grapheme_width (std::u8string_view u8bytes);
}

using namespace Basedline;
using namespace Basedtest;

int width (const std::u8string_view& str) {
	std::u8string_view s (str);
	return grapheme_width (s);
}

constexpr auto make_case (std::string_view name, std::u8string_view str, int expect_width) {
	return ValueCase { name, str, expect_width };
}

BT_SCENARIO_TEST (test_width_ascii) {
	BT_ASSERT_RC (Suite ("testWidthASCII", cases <width> (
		make_case ("empty", u8"",      0),
		make_case ("A",     u8"A",     1),
		make_case ("SP",    u8" ",     1),
		make_case ("~",     u8"~",     1),
		make_case ("LF",    u8"\n",   -1),
		make_case ("ESC",   u8"\x1B", -1)
	)).run_rc());
	BT_SUCCESS;
}

BT_SCENARIO_TEST (test_width_combining) {
	BT_ASSERT_RC (Suite ("testWidthCombining", cases <width> (
		make_case ("e1",   u8"é",       1),
		make_case ("e2",   u8"e\u0301", 1),
		make_case ("lone", u8"\u0301",  0)
	)).run_rc());
	BT_SUCCESS;
}

BT_SCENARIO_TEST (test_width_cjk) {
	BT_ASSERT_RC (Suite ("testWidthCJK", cases <width> (
		make_case ("cjk1",   u8"界", 2),
		make_case ("cjk2",   u8"Ａ", 2),
		make_case ("cjk3",   u8"あ", 2),
		make_case ("cjk4",   u8"ア", 2),
		make_case ("cjk5",   u8"한", 2),
		make_case ("cjk6",   u8"中", 2),
		make_case ("fw1",    u8"！", 2),
		make_case ("fw2",    u8"？", 2),
		make_case ("hw1",    u8"ｶ", 1),
		make_case ("hw2",    u8"｡", 1)
	)).run_rc());
	BT_SUCCESS;
}

BT_SCENARIO_TEST (test_width_cuneiform) {
	BT_ASSERT_RC (Suite ("testWidthCuneiform", cases <width> (
		make_case ("cu01", u8"𒀀", 1),
		make_case ("cu02", u8"𒁹", 1),
		make_case ("cu03", u8"𒆠", 1) // font may draw them wider
	)).run_rc());
	BT_SUCCESS;
}

BT_SCENARIO_TEST (test_width_semitic) {
	BT_ASSERT_RC (Suite ("testWidthSemitic", cases <width> (
		make_case ("he01", u8"א", 1),
		make_case ("he02", u8"ש", 1),
		make_case ("ar01", u8"ا", 1),
		make_case ("ar02", u8"م", 1),
		make_case ("ar03", u8"اَ", 1), // \u0627 \u064E
		make_case ("ar04", u8"\u064E", 0)
	)).run_rc());
	BT_SUCCESS;
}

BT_SCENARIO_TEST (test_width_emoji) {
	BT_ASSERT_RC (Suite ("testWidthEmoji", cases <width> (
		make_case ("e01", u8"😀", 2),
		make_case ("e02", u8"🚀", 2),
		make_case ("e03", u8"👍", 2),
		make_case ("e04", u8"👍🏽", 2),
		make_case ("e05", u8"👩‍🔧", 2),
		make_case ("e06", u8"👨‍👩‍👧‍👦", 2)
	)).run_rc());
	BT_SUCCESS;
}

BT_SCENARIO_TEST (test_width_flag) {
	BT_ASSERT_RC (Suite ("testWidthFlag", cases <width> (
		make_case ("f01", u8"🇷🇺", 2),
		make_case ("f02", u8"🇹🇼", 2)
	)).run_rc());
	BT_SUCCESS;
}

BT_SCENARIO_TEST (test_width_vs15_vs16_keycap) {
	BT_ASSERT_RC (Suite ("testWidth-VS15-VS16-Keycap", cases <width> (
		make_case ("vs1500-0", u8"\uFE0E", 0),
		make_case ("vs1501-1", u8"\u2665\uFE0E", 1),
		make_case ("vs1600-0", u8"\uFE0F", 0),
		make_case ("vs1601-1", u8"♥", 1),
		make_case ("vs1601-2", u8"♥️", 2),
		make_case ("vs1602-1", u8"©", 1),
		make_case ("vs1602-2", u8"©️", 2),
		make_case ("vs1603-1", u8"™", 1),
		make_case ("vs1603-2", u8"™️", 2),
		make_case ("keycap0", u8"\u20E3", 0),
		make_case ("keycap1", u8"1️⃣", 2),
		make_case ("keycap2", u8"#️⃣", 2),
		make_case ("keycap3", u8"*️⃣", 2)
	)).run_rc());
	BT_SUCCESS;
}

BT_SCENARIO_TEST (test_width_ambiguous) {
	BT_ASSERT_RC (Suite ("testWidthAmbiguous", cases <width> (
		make_case ("a1", u8"Ω", 1),
		make_case ("a2", u8"·", 1),
		make_case ("a3", u8"─", 1),
		make_case ("a4", u8"…", 1)
	)).run_rc());
	BT_SUCCESS;
}

BT_SCENARIO_TEST (test_width_other) {
	BT_ASSERT_RC (Suite ("testWidthOther", cases <width> (
		make_case ("math01", u8"𝄞", 1),
		make_case ("math02", u8"𝛼", 1),
		make_case ("math03", u8"🜁", 1),
		make_case ("math04", u8"𝔘", 1),
		make_case ("goth01", u8"𐍈", 1)
	)).run_rc());
	BT_SUCCESS;
}

int main () {
	return Suite ("testWidth", tests (
		BT_SUITE_SCENARIO (test_width_ascii),
		BT_SUITE_SCENARIO (test_width_combining),
		BT_SUITE_SCENARIO (test_width_cjk),
		BT_SUITE_SCENARIO (test_width_cuneiform),
		BT_SUITE_SCENARIO (test_width_semitic),
		BT_SUITE_SCENARIO (test_width_emoji),
		BT_SUITE_SCENARIO (test_width_flag),
		BT_SUITE_SCENARIO (test_width_vs15_vs16_keycap),
		BT_SUITE_SCENARIO (test_width_ambiguous),
		BT_SUITE_SCENARIO (test_width_other)
	)).run_rc();
}
