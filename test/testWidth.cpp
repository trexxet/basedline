#include "Basedtest/Suite.hpp"

#include <string>
#include <string_view>

namespace Basedline {
int grapheme_width (std::u8string_view u8bytes);
ssize_t u8string_width (std::u8string_view u8str);
}

using namespace Basedtest;

int grapheme_width (const std::u8string_view& str) {
	std::u8string_view s (str);
	return Basedline::grapheme_width (s);
}

int u8string_width (const std::u8string_view& str) {
	std::u8string_view s (str);
	return Basedline::u8string_width (s);
}

constexpr auto make_case (std::string_view name, std::u8string_view str, int expect_width) {
	return ValueCase { name, str, expect_width };
}

BT_SCENARIO_TEST (test_graphwidth_ascii) {
	BT_ASSERT_RC (Suite ("testGraphemeWidthASCII", cases <grapheme_width> (
		make_case ("empty", u8"",      0),
		make_case ("A",     u8"A",     1),
		make_case ("SP",    u8" ",     1),
		make_case ("~",     u8"~",     1),
		make_case ("LF",    u8"\n",   -1),
		make_case ("ESC",   u8"\x1B", -1)
	)).run_rc());
	BT_SUCCESS;
}

BT_SCENARIO_TEST (test_graphwidth_combining) {
	BT_ASSERT_RC (Suite ("testGraphemeWidthCombining", cases <grapheme_width> (
		make_case ("e1",   u8"é",       1),
		make_case ("e2",   u8"e\u0301", 1),
		make_case ("lone", u8"\u0301",  0)
	)).run_rc());
	BT_SUCCESS;
}

BT_SCENARIO_TEST (test_graphwidth_cjk) {
	BT_ASSERT_RC (Suite ("testGraphemeWidthCJK", cases <grapheme_width> (
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

BT_SCENARIO_TEST (test_graphwidth_cuneiform) {
	BT_ASSERT_RC (Suite ("testGraphemeWidthCuneiform", cases <grapheme_width> (
		make_case ("cu01", u8"𒀀", 1),
		make_case ("cu02", u8"𒁹", 1),
		make_case ("cu03", u8"𒆠", 1) // font may draw them wider
	)).run_rc());
	BT_SUCCESS;
}

BT_SCENARIO_TEST (test_graphwidth_semitic) {
	BT_ASSERT_RC (Suite ("testGraphemeWidthSemitic", cases <grapheme_width> (
		make_case ("he01", u8"א", 1),
		make_case ("he02", u8"ש", 1),
		make_case ("ar01", u8"ا", 1),
		make_case ("ar02", u8"م", 1),
		make_case ("ar03", u8"اَ", 1), // \u0627 \u064E
		make_case ("ar04", u8"\u064E", 0)
	)).run_rc());
	BT_SUCCESS;
}

BT_SCENARIO_TEST (test_graphwidth_emoji) {
	BT_ASSERT_RC (Suite ("testGraphemeWidthEmoji", cases <grapheme_width> (
		make_case ("e01", u8"😀", 2),
		make_case ("e02", u8"🚀", 2),
		make_case ("e03", u8"👍", 2),
		make_case ("e04", u8"👍🏽", 2),
		make_case ("e05", u8"👩‍🔧", 2),
		make_case ("e06", u8"👨‍👩‍👧‍👦", 2),
		make_case ("zwj",  u8"\u200D", 0),
		make_case ("zwnj", u8"\u200C", 0)
	)).run_rc());
	BT_SUCCESS;
}

BT_SCENARIO_TEST (test_graphwidth_flag) {
	BT_ASSERT_RC (Suite ("testGraphemeWidthFlag", cases <grapheme_width> (
		make_case ("f01", u8"🇷🇺", 2),
		make_case ("f02", u8"🇹🇼", 2)
	)).run_rc());
	BT_SUCCESS;
}

BT_SCENARIO_TEST (test_graphwidth_vs15_vs16_keycap) {
	BT_ASSERT_RC (Suite ("testGraphemeWidth-VS15-VS16-Keycap", cases <grapheme_width> (
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

BT_SCENARIO_TEST (test_graphwidth_ambiguous) {
	BT_ASSERT_RC (Suite ("testGraphemeWidthAmbiguous", cases <grapheme_width> (
		make_case ("a1", u8"Ω", 1),
		make_case ("a2", u8"·", 1),
		make_case ("a3", u8"─", 1),
		make_case ("a4", u8"…", 1)
	)).run_rc());
	BT_SUCCESS;
}

BT_SCENARIO_TEST (test_graphwidth_other) {
	BT_ASSERT_RC (Suite ("testGraphemeWidthOther", cases <grapheme_width> (
		make_case ("math01", u8"𝄞", 1),
		make_case ("math02", u8"𝛼", 1),
		make_case ("math03", u8"🜁", 1),
		make_case ("math04", u8"𝔘", 1),
		make_case ("goth01", u8"𐍈", 1)
	)).run_rc());
	BT_SUCCESS;
}

BT_SCENARIO_TEST (test_strwidth_ascii) {
	BT_ASSERT_RC (Suite ("testStringWidthASCII", cases <u8string_width> (
		make_case ("empty", u8"", 0),
		make_case ("ascii1", u8"A", 1),
		make_case ("ascii2", u8"abc", 3),
		make_case ("ascii3", u8"hello world", 11),
		make_case ("ascii4", u8"   ", 3)
	)).run_rc());
	BT_SUCCESS;
}

BT_SCENARIO_TEST (test_strwidth_combining) {
	BT_ASSERT_RC (Suite ("testStringWidthCombining", cases <u8string_width> (
		make_case ("comb1", u8"é", 1),
		make_case ("comb2", u8"éx", 2),
		make_case ("comb3", u8"xé", 2),
		make_case ("comb4", u8"éé", 2),
		make_case ("comb5", u8"a\u0301b\u0301c", 3),
		make_case ("comb6", u8"\u0301a", 1),
		make_case ("comb7", u8"a\u0301界", 3)
	)).run_rc());
	BT_SUCCESS;
}

BT_SCENARIO_TEST (test_strwidth_cjk) {
	BT_ASSERT_RC (Suite ("testStringWidthCJK", cases <u8string_width> (
		make_case ("cjk1", u8"a界b", 4),
		make_case ("cjk2", u8"AＢC", 4),
		make_case ("cjk3", u8"abc界", 5),
		make_case ("cjk4", u8"界abc", 5),
		make_case ("cjk5", u8"界界", 4),
		make_case ("cjk6", u8"あア한", 6)
	)).run_rc());
	BT_SUCCESS;
}

BT_SCENARIO_TEST (test_strwidth_emoji) {
	BT_ASSERT_RC (Suite ("testStringWidthEmoji", cases <u8string_width> (
		make_case ("e1", u8"😀", 2),
		make_case ("e2", u8"😀x", 3),
		make_case ("e3", u8"x😀", 3),
		make_case ("e4", u8"😀😀", 4),
		make_case ("e5", u8"👍🏽x", 3),
		make_case ("e6", u8"x👩‍🔧y", 4),
		make_case ("e7", u8"👨‍👩‍👧‍👦!", 3),
		make_case ("e8", u8"🇷🇺🇹🇼", 4),
		make_case ("e9", u8"A🇷🇺B", 4),
		make_case ("zwj1",  u8"\u200D", 0),
		make_case ("zwj2",  u8"a\u200Db", 2),
		make_case ("zwnj1", u8"\u200C", 0),
		make_case ("zwnj2", u8"a\u200Cb", 2)
	)).run_rc());
	BT_SUCCESS;
}

BT_SCENARIO_TEST (test_strwidth_vs15_vs16_keycap) {
	BT_ASSERT_RC (Suite ("testStringWidth-VS15-VS16-Keycap", cases <u8string_width> (
		make_case ("vs1", u8"♥", 1),
		make_case ("vs2", u8"♥️", 2),
		make_case ("vs3", u8"♥♥️", 3),
		make_case ("vs4", u8"♥️♥", 3),
		make_case ("vs5", u8"©️™️", 4),
		make_case ("vs6", u8"1️⃣2", 3),
		make_case ("vs7", u8"#️⃣*️⃣", 4),
		make_case ("vs8", u8"\uFE0Fx", 1),
		make_case ("vs9", u8"\u20E3x", 1)
	)).run_rc());
	BT_SUCCESS;
}

BT_SCENARIO_TEST (test_strwidth_nonprint) {
	BT_ASSERT_RC (Suite ("testStringWidthNonprint", cases <u8string_width> (
		make_case ("lf1",  u8"\n", -1),
		make_case ("lf2",  u8"abc\n", -1),
		make_case ("esc1", u8"\x1B", -1),
		make_case ("esc2", u8"abc\x1B" u8"def", -1)
	)).run_rc());
	BT_SUCCESS;
}

int main () {
	BT_CHECK_RC (Suite ("testGraphemeWidth", tests (
		BT_SUITE_SCENARIO (test_graphwidth_ascii),
		BT_SUITE_SCENARIO (test_graphwidth_combining),
		BT_SUITE_SCENARIO (test_graphwidth_cjk),
		BT_SUITE_SCENARIO (test_graphwidth_cuneiform),
		BT_SUITE_SCENARIO (test_graphwidth_semitic),
		BT_SUITE_SCENARIO (test_graphwidth_emoji),
		BT_SUITE_SCENARIO (test_graphwidth_flag),
		BT_SUITE_SCENARIO (test_graphwidth_vs15_vs16_keycap),
		BT_SUITE_SCENARIO (test_graphwidth_ambiguous),
		BT_SUITE_SCENARIO (test_graphwidth_other)
	)).run_rc());

	return Suite ("testStringWidth", tests (
		BT_SUITE_SCENARIO (test_strwidth_ascii),
		BT_SUITE_SCENARIO (test_strwidth_combining),
		BT_SUITE_SCENARIO (test_strwidth_cjk),
		BT_SUITE_SCENARIO (test_strwidth_emoji),
		BT_SUITE_SCENARIO (test_strwidth_vs15_vs16_keycap),
		BT_SUITE_SCENARIO (test_strwidth_nonprint)
	)).run_rc();
}
