#include "Basedtest/Suite.hpp"

#include <format>
#include <string>
#include <string_view>
#include <vector>

#include "Basedlib/Container/StaticVector.hpp"

namespace Basedline {
int grapheme_width (std::u8string_view u8bytes);
ssize_t u8string_width (std::u8string_view u8str);
std::vector<std::u8string_view> wrap (std::u8string_view str, size_t width);
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

struct Wrap {
	using Lines = Basedlib::StaticVector<std::u8string_view, 8>;
	std::u8string_view str;
	size_t width;
};
Wrap::Lines wrap (const Wrap& w) {
	return Wrap::Lines (Basedline::wrap (w.str, w.width));
}

constexpr auto make_width_case (std::string_view name, std::u8string_view str, int expect_width) {
	return ValueCase { name, str, expect_width };
}

constexpr auto make_wrap_case (std::string_view name, Wrap w, Wrap::Lines expect_lines) {
	return ValueCase { name, w, expect_lines };
}

namespace std {
std::string_view to_string (std::u8string_view u8str) {
	return {reinterpret_cast <const char*> (u8str.data()), u8str.size()};
}
std::string to_string (Wrap::Lines lines) {
	std::string s;
	for (const auto& line : lines)
		s += std::format ("{} ", to_string (line));
	return s;
}
}

BT_SCENARIO_TEST (test_graphwidth_ascii) {
	BT_ASSERT_RC (Suite ("testGraphemeWidthASCII", cases <grapheme_width> (
		make_width_case ("empty", u8"",      0),
		make_width_case ("A",     u8"A",     1),
		make_width_case ("SP",    u8" ",     1),
		make_width_case ("~",     u8"~",     1),
		make_width_case ("LF",    u8"\n",   -1),
		make_width_case ("ESC",   u8"\x1B", -1)
	)).run_rc());
	BT_SUCCESS;
}

BT_SCENARIO_TEST (test_graphwidth_combining) {
	BT_ASSERT_RC (Suite ("testGraphemeWidthCombining", cases <grapheme_width> (
		make_width_case ("e1",   u8"é",       1),
		make_width_case ("e2",   u8"e\u0301", 1),
		make_width_case ("lone", u8"\u0301",  0)
	)).run_rc());
	BT_SUCCESS;
}

BT_SCENARIO_TEST (test_graphwidth_cjk) {
	BT_ASSERT_RC (Suite ("testGraphemeWidthCJK", cases <grapheme_width> (
		make_width_case ("cjk1",   u8"界", 2),
		make_width_case ("cjk2",   u8"Ａ", 2),
		make_width_case ("cjk3",   u8"あ", 2),
		make_width_case ("cjk4",   u8"ア", 2),
		make_width_case ("cjk5",   u8"한", 2),
		make_width_case ("cjk6",   u8"中", 2),
		make_width_case ("fw1",    u8"！", 2),
		make_width_case ("fw2",    u8"？", 2),
		make_width_case ("hw1",    u8"ｶ", 1),
		make_width_case ("hw2",    u8"｡", 1)
	)).run_rc());
	BT_SUCCESS;
}

BT_SCENARIO_TEST (test_graphwidth_cuneiform) {
	BT_ASSERT_RC (Suite ("testGraphemeWidthCuneiform", cases <grapheme_width> (
		make_width_case ("cu01", u8"𒀀", 1),
		make_width_case ("cu02", u8"𒁹", 1),
		make_width_case ("cu03", u8"𒆠", 1) // font may draw them wider
	)).run_rc());
	BT_SUCCESS;
}

BT_SCENARIO_TEST (test_graphwidth_semitic) {
	BT_ASSERT_RC (Suite ("testGraphemeWidthSemitic", cases <grapheme_width> (
		make_width_case ("he01", u8"א", 1),
		make_width_case ("he02", u8"ש", 1),
		make_width_case ("ar01", u8"ا", 1),
		make_width_case ("ar02", u8"م", 1),
		make_width_case ("ar03", u8"اَ", 1), // \u0627 \u064E
		make_width_case ("ar04", u8"\u064E", 0)
	)).run_rc());
	BT_SUCCESS;
}

BT_SCENARIO_TEST (test_graphwidth_emoji) {
	BT_ASSERT_RC (Suite ("testGraphemeWidthEmoji", cases <grapheme_width> (
		make_width_case ("e01", u8"😀", 2),
		make_width_case ("e02", u8"🚀", 2),
		make_width_case ("e03", u8"👍", 2),
		make_width_case ("e04", u8"👍🏽", 2),
		make_width_case ("e05", u8"👩‍🔧", 2),
		make_width_case ("e06", u8"👨‍👩‍👧‍👦", 2),
		make_width_case ("zwj",  u8"\u200D", 0),
		make_width_case ("zwnj", u8"\u200C", 0)
	)).run_rc());
	BT_SUCCESS;
}

BT_SCENARIO_TEST (test_graphwidth_flag) {
	BT_ASSERT_RC (Suite ("testGraphemeWidthFlag", cases <grapheme_width> (
		make_width_case ("f01", u8"🇷🇺", 2),
		make_width_case ("f02", u8"🇹🇼", 2)
	)).run_rc());
	BT_SUCCESS;
}

BT_SCENARIO_TEST (test_graphwidth_vs15_vs16_keycap) {
	BT_ASSERT_RC (Suite ("testGraphemeWidth-VS15-VS16-Keycap", cases <grapheme_width> (
		make_width_case ("vs1500-0", u8"\uFE0E", 0),
		make_width_case ("vs1501-1", u8"\u2665\uFE0E", 1),
		make_width_case ("vs1600-0", u8"\uFE0F", 0),
		make_width_case ("vs1601-1", u8"♥", 1),
		make_width_case ("vs1601-2", u8"♥️", 2),
		make_width_case ("vs1602-1", u8"©", 1),
		make_width_case ("vs1602-2", u8"©️", 2),
		make_width_case ("vs1603-1", u8"™", 1),
		make_width_case ("vs1603-2", u8"™️", 2),
		make_width_case ("keycap0", u8"\u20E3", 0),
		make_width_case ("keycap1", u8"1️⃣", 2),
		make_width_case ("keycap2", u8"#️⃣", 2),
		make_width_case ("keycap3", u8"*️⃣", 2)
	)).run_rc());
	BT_SUCCESS;
}

BT_SCENARIO_TEST (test_graphwidth_ambiguous) {
	BT_ASSERT_RC (Suite ("testGraphemeWidthAmbiguous", cases <grapheme_width> (
		make_width_case ("a1", u8"Ω", 1),
		make_width_case ("a2", u8"·", 1),
		make_width_case ("a3", u8"─", 1),
		make_width_case ("a4", u8"…", 1)
	)).run_rc());
	BT_SUCCESS;
}

BT_SCENARIO_TEST (test_graphwidth_other) {
	BT_ASSERT_RC (Suite ("testGraphemeWidthOther", cases <grapheme_width> (
		make_width_case ("math01", u8"𝄞", 1),
		make_width_case ("math02", u8"𝛼", 1),
		make_width_case ("math03", u8"🜁", 1),
		make_width_case ("math04", u8"𝔘", 1),
		make_width_case ("goth01", u8"𐍈", 1)
	)).run_rc());
	BT_SUCCESS;
}

BT_SCENARIO_TEST (test_strwidth_ascii) {
	BT_ASSERT_RC (Suite ("testStringWidthASCII", cases <u8string_width> (
		make_width_case ("empty", u8"", 0),
		make_width_case ("ascii1", u8"A", 1),
		make_width_case ("ascii2", u8"abc", 3),
		make_width_case ("ascii3", u8"hello world", 11),
		make_width_case ("ascii4", u8"   ", 3)
	)).run_rc());
	BT_SUCCESS;
}

BT_SCENARIO_TEST (test_strwidth_combining) {
	BT_ASSERT_RC (Suite ("testStringWidthCombining", cases <u8string_width> (
		make_width_case ("comb1", u8"é", 1),
		make_width_case ("comb2", u8"éx", 2),
		make_width_case ("comb3", u8"xé", 2),
		make_width_case ("comb4", u8"éé", 2),
		make_width_case ("comb5", u8"a\u0301b\u0301c", 3),
		make_width_case ("comb6", u8"\u0301a", 1),
		make_width_case ("comb7", u8"a\u0301界", 3)
	)).run_rc());
	BT_SUCCESS;
}

BT_SCENARIO_TEST (test_strwidth_cjk) {
	BT_ASSERT_RC (Suite ("testStringWidthCJK", cases <u8string_width> (
		make_width_case ("cjk1", u8"a界b", 4),
		make_width_case ("cjk2", u8"AＢC", 4),
		make_width_case ("cjk3", u8"abc界", 5),
		make_width_case ("cjk4", u8"界abc", 5),
		make_width_case ("cjk5", u8"界界", 4),
		make_width_case ("cjk6", u8"あア한", 6)
	)).run_rc());
	BT_SUCCESS;
}

BT_SCENARIO_TEST (test_strwidth_emoji) {
	BT_ASSERT_RC (Suite ("testStringWidthEmoji", cases <u8string_width> (
		make_width_case ("e1", u8"😀", 2),
		make_width_case ("e2", u8"😀x", 3),
		make_width_case ("e3", u8"x😀", 3),
		make_width_case ("e4", u8"😀😀", 4),
		make_width_case ("e5", u8"👍🏽x", 3),
		make_width_case ("e6", u8"x👩‍🔧y", 4),
		make_width_case ("e7", u8"👨‍👩‍👧‍👦!", 3),
		make_width_case ("e8", u8"🇷🇺🇹🇼", 4),
		make_width_case ("e9", u8"A🇷🇺B", 4),
		make_width_case ("zwj1",  u8"\u200D", 0),
		make_width_case ("zwj2",  u8"a\u200Db", 2),
		make_width_case ("zwnj1", u8"\u200C", 0),
		make_width_case ("zwnj2", u8"a\u200Cb", 2)
	)).run_rc());
	BT_SUCCESS;
}

BT_SCENARIO_TEST (test_strwidth_vs15_vs16_keycap) {
	BT_ASSERT_RC (Suite ("testStringWidth-VS15-VS16-Keycap", cases <u8string_width> (
		make_width_case ("vs1", u8"♥", 1),
		make_width_case ("vs2", u8"♥️", 2),
		make_width_case ("vs3", u8"♥♥️", 3),
		make_width_case ("vs4", u8"♥️♥", 3),
		make_width_case ("vs5", u8"©️™️", 4),
		make_width_case ("vs6", u8"1️⃣2", 3),
		make_width_case ("vs7", u8"#️⃣*️⃣", 4),
		make_width_case ("vs8", u8"\uFE0Fx", 1),
		make_width_case ("vs9", u8"\u20E3x", 1)
	)).run_rc());
	BT_SUCCESS;
}

BT_SCENARIO_TEST (test_strwidth_nonprint) {
	BT_ASSERT_RC (Suite ("testStringWidthNonprint", cases <u8string_width> (
		make_width_case ("lf1",  u8"\n", -1),
		make_width_case ("lf2",  u8"abc\n", -1),
		make_width_case ("esc1", u8"\x1B", -1),
		make_width_case ("esc2", u8"abc\x1B" u8"def", -1)
	)).run_rc());
	BT_SUCCESS;
}

BT_SCENARIO_TEST (test_wrap_ascii) {
	BT_ASSERT_RC (Suite ("testWrapASCII", cases <wrap> (
		make_wrap_case ("empty", {u8"", 5}, {}),
		make_wrap_case ("abcd0", {u8"abcd", 0}, {}),
		make_wrap_case ("abcd1", {u8"abcd", 1}, {}),
		make_wrap_case ("abcd2", {u8"abcd", 2}, {u8"ab", u8"cd"}),
		make_wrap_case ("abcd3", {u8"abcd", 3}, {u8"abc", u8"d"}),
		make_wrap_case ("abcd4", {u8"abcd", 4}, {u8"abcd"}),
		make_wrap_case ("abcd5", {u8"abcd", 5}, {u8"abcd"}),
		make_wrap_case ("abcd6", {u8"abcd", 6}, {u8"abcd"})
	)).run_rc());
	BT_SUCCESS;
}

BT_SCENARIO_TEST (test_wrap_combining) {
	BT_ASSERT_RC (Suite ("testWrapCombining", cases <wrap> (
		make_wrap_case ("comb1-2", {u8"ééé", 2}, {u8"éé", u8"é"}),
		make_wrap_case ("comb1-3", {u8"ééé", 3}, {u8"ééé"}),
		make_wrap_case ("comb2-2", {u8"a\u0301b", 2}, {u8"a\u0301b"})
	)).run_rc());
	BT_SUCCESS;
}

BT_SCENARIO_TEST (test_wrap_cjk) {
	BT_ASSERT_RC (Suite ("testWrapCJK", cases <wrap> (
		make_wrap_case ("cjk1-2", {u8"a界b", 2}, {u8"a", u8"界", u8"b"}),
		make_wrap_case ("cjk1-3", {u8"a界b", 3}, {u8"a界", u8"b"}),
		make_wrap_case ("cjk1-4", {u8"a界b", 4}, {u8"a界b"}),
		make_wrap_case ("cjk1-5", {u8"a界b", 5}, {u8"a界b"}),
		make_wrap_case ("cjk2-2", {u8"界界", 2}, {u8"界", u8"界"}),
		make_wrap_case ("cjk2-3", {u8"界界", 3}, {u8"界", u8"界"}),
		make_wrap_case ("cjk2-4", {u8"界界", 4}, {u8"界界"}),
		make_wrap_case ("cjk2-5", {u8"界界", 5}, {u8"界界"})
	)).run_rc());
	BT_SUCCESS;
}

BT_SCENARIO_TEST (test_wrap_emoji) {
	BT_ASSERT_RC (Suite ("testWrapEmoji", cases <wrap> (
		make_wrap_case ("emo1", {u8"😀😀", 2}, {u8"😀", u8"😀"}),
		make_wrap_case ("emo2", {u8"😀x", 2}, {u8"😀", u8"x"}),
		make_wrap_case ("emo3", {u8"x😀", 3}, {u8"x😀"}),
		make_wrap_case ("emo4", {u8"x👩‍🔧y", 3}, {u8"x👩‍🔧", u8"y"}),
		make_wrap_case ("emo5", {u8"x👩‍🔧y", 2}, {u8"x", u8"👩‍🔧", u8"y"}),
		make_wrap_case ("emo6", {u8"🇷🇺🇹🇼", 2}, {u8"🇷🇺", u8"🇹🇼"})
	)).run_rc());
	BT_SUCCESS;
}

BT_SCENARIO_TEST (test_wrap_zw) {
	BT_ASSERT_RC (Suite ("testWrapZeroWidth", cases <wrap> (
		make_wrap_case ("zw1", {u8"\u200Dx", 2}, {u8"\u200Dx"}),
		make_wrap_case ("zw2", {u8"x\u200D", 2}, {u8"x\u200D"}),
		make_wrap_case ("zw3", {u8"\uFE0Fx", 2}, {u8"\uFE0Fx"}),
		make_wrap_case ("zw4", {u8"\u20E3x", 2}, {u8"\u20E3x"}),
		make_wrap_case ("zw5", {u8"a\u200Db", 2}, {u8"a\u200Db"})
	)).run_rc());
	BT_SUCCESS;
}

BT_SCENARIO_TEST (test_wrap_vs15_vs16_keycap) {
	BT_ASSERT_RC (Suite ("testWrap-VS15-VS16-Keycap", cases <wrap> (
		make_wrap_case ("vs1-1", {u8"♥♥️", 2}, {u8"♥", u8"♥️"}),
		make_wrap_case ("vs1-2", {u8"♥️♥", 2}, {u8"♥️", u8"♥"}),
		make_wrap_case ("vs1-3", {u8"♥♥️", 3}, {u8"♥♥️"}),
		make_wrap_case ("vs2-1", {u8"©️™️", 2}, {u8"©️", u8"™️"}),
		make_wrap_case ("vs2-2", {u8"©️™️", 3}, {u8"©️", u8"™️"}),
		make_wrap_case ("vs2-3", {u8"©️™️", 4}, {u8"©️™️"}),
		make_wrap_case ("vs3-1", {u8"1️⃣2", 2}, {u8"1️⃣", u8"2"}),
		make_wrap_case ("vs3-2", {u8"1️⃣2", 3}, {u8"1️⃣2"}),
		make_wrap_case ("vs3-3", {u8"21️⃣", 2}, {u8"2", u8"1️⃣"}),
		make_wrap_case ("vs4-1", {u8"#️⃣*️⃣", 2}, {u8"#️⃣", u8"*️⃣"}),
		make_wrap_case ("vs4-2", {u8"#️⃣*️⃣", 3}, {u8"#️⃣", u8"*️⃣"}),
		make_wrap_case ("vs4-3", {u8"#️⃣*️⃣", 4}, {u8"#️⃣*️⃣"})
	)).run_rc());
	BT_SUCCESS;
}

BT_SCENARIO_TEST (test_wrap_nonprint) {
	BT_ASSERT_RC (Suite ("testWrapNonprint", cases <wrap> (
		make_wrap_case ("lf1",  {u8"abc\n", 5}, {u8"abc"}),
		make_wrap_case ("lf2",  {u8"\nabc", 5}, {}),
		make_wrap_case ("lf3",  {u8"ab\ncd", 5}, {u8"ab"}),
		make_wrap_case ("esc1", {u8"\x1B" u8"abc", 5}, {}),
		make_wrap_case ("esc2", {u8"ab\x1B" u8"cd", 5}, {u8"ab"})
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

	BT_CHECK_RC (Suite ("testStringWidth", tests (
		BT_SUITE_SCENARIO (test_strwidth_ascii),
		BT_SUITE_SCENARIO (test_strwidth_combining),
		BT_SUITE_SCENARIO (test_strwidth_cjk),
		BT_SUITE_SCENARIO (test_strwidth_emoji),
		BT_SUITE_SCENARIO (test_strwidth_vs15_vs16_keycap),
		BT_SUITE_SCENARIO (test_strwidth_nonprint)
	)).run_rc());

	BT_CHECK_RC (Suite ("testWrap", tests (
		BT_SUITE_SCENARIO (test_wrap_ascii),
		BT_SUITE_SCENARIO (test_wrap_combining),
		BT_SUITE_SCENARIO (test_wrap_cjk),
		BT_SUITE_SCENARIO (test_wrap_emoji),
		BT_SUITE_SCENARIO (test_wrap_zw),
		BT_SUITE_SCENARIO (test_wrap_vs15_vs16_keycap),
		BT_SUITE_SCENARIO (test_wrap_nonprint)
	)).run_rc());
}
