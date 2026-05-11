#include "Basedline/Editor.hpp"
#include "Basedtest/Suite.hpp"

using namespace Basedline;
using namespace Basedtest;

namespace std {
std::string_view to_string (std::u8string_view u8str) {
	return {reinterpret_cast <const char*> (u8str.data()), u8str.size()};
}
}

BT_SCENARIO_TEST (test_ed_create) {
	Editor ed;
	BT_ASSERT (ed.empty());
	BT_ASSERT (ed.at_begin());
	BT_ASSERT (ed.at_end());
	BT_SUCCESS;
}

BT_SCENARIO_TEST (test_ed_accumulate_ascii) {
	Editor ed;

	ed.accumulate ('a');
	BT_ASSERT_EQ (ed.get_u8(), u8"a");
	BT_ASSERT (!ed.empty());
	BT_ASSERT (!ed.at_begin());
	BT_ASSERT (ed.at_end());

	ed.accumulate ('b');
	ed.accumulate (0);
	ed.accumulate ('c');
	BT_ASSERT_EQ (ed.get_u8(), u8"abc");

	BT_SUCCESS;
}

BT_SCENARIO_TEST (test_ed_accumulate_utf8_2byte) {
	Editor ed;

	ed.accumulate (0xC3);
	BT_ASSERT_EQ (ed.get_u8(), u8"");
	ed.accumulate (0xA9);
	BT_ASSERT_EQ (ed.get_u8(), u8"é");

	ed.clear();
	ed.accumulate(0xD0);
	BT_ASSERT_EQ (ed.get_u8(), u8"");
	ed.accumulate(0x96);
	BT_ASSERT_EQ (ed.get_u8(), u8"Ж");

	BT_SUCCESS;
}

BT_SCENARIO_TEST (test_ed_accumulate_utf8_3byte) {
	Editor ed;

	ed.accumulate(0xE2);
	BT_ASSERT_EQ (ed.get_u8(), u8"");
	ed.accumulate(0x82);
	BT_ASSERT_EQ (ed.get_u8(), u8"");
	ed.accumulate(0xAC);
	BT_ASSERT_EQ (ed.get_u8(), u8"€");

	BT_SUCCESS;
}

BT_SCENARIO_TEST (test_ed_accumulate_utf8_4byte) {
	Editor ed;

	ed.accumulate(0xF0);
	BT_ASSERT_EQ (ed.get_u8(), u8"");
	ed.accumulate(0x9F);
	BT_ASSERT_EQ (ed.get_u8(), u8"");
	ed.accumulate(0x98);
	BT_ASSERT_EQ (ed.get_u8(), u8"");
	ed.accumulate(0x80);
	BT_ASSERT_EQ (ed.get_u8(), u8"😀");

	BT_SUCCESS;
}

BT_SCENARIO_TEST (test_ed_accumulate_utf8_ascii_mixed) {
	Editor ed;

	ed.accumulate(u8'a');
	ed.accumulate(0xC3);
	BT_ASSERT_EQ (ed.get_u8(), u8"a");
	ed.accumulate(0xA9);
	ed.accumulate(u8'b');
	BT_ASSERT_EQ (ed.get_u8(), u8"aéb");

	BT_SUCCESS;
}

BT_SCENARIO_TEST (test_ed_accumulate_utf8_combine_mark) {
	Editor ed;

	ed.clear();
	ed.accumulate ('e');
	ed.accumulate (0xCC);
	BT_ASSERT_EQ (ed.get_u8(), u8"e");
	ed.accumulate (0x81);
	BT_ASSERT_EQ (ed.get_u8(), u8"e\u0301");

	BT_SUCCESS;
}

BT_SCENARIO_TEST (test_ed_accumulate_utf8_null_reset_acc) {
	Editor ed;

	ed.accumulate (0xC3);
	ed.accumulate (0);
	ed.accumulate (0xA9);
	BT_ASSERT_EQ (ed.get_u8(), u8"");
	ed.accumulate ('x');
	BT_ASSERT_EQ (ed.get_u8(), u8"x");

	BT_SUCCESS;
}

BT_SCENARIO_TEST (test_ed_accumulate_utf8_lone_continuation_byte) {
	Editor ed;

	ed.accumulate (0x80);
	BT_ASSERT_EQ (ed.get_u8(), u8"");
	ed.accumulate ('x');
	BT_ASSERT_EQ (ed.get_u8(), u8"x");

	BT_SUCCESS;
}

BT_SCENARIO_TEST (test_ed_accumulate_utf8_no_continuation_byte) {
	Editor ed;

	ed.accumulate (0xC3);
	BT_ASSERT_EQ (ed.get_u8(), u8"");
	ed.accumulate ('x');
	BT_ASSERT_EQ (ed.get_u8(), u8"");
	ed.accumulate ('y');
	BT_ASSERT_EQ (ed.get_u8(), u8"y");

	BT_SUCCESS;
}

BT_SCENARIO_TEST (test_ed_accumulate_utf8_overlong_encoding) {
	Editor ed;

	ed.accumulate (0xC0);
	ed.accumulate (0xAF);
	BT_ASSERT_EQ (ed.get_u8(), u8"");
	ed.accumulate ('x');
	BT_ASSERT_EQ (ed.get_u8(), u8"x");

	BT_SUCCESS;
}

BT_SCENARIO_TEST (test_ed_accumulate_utf8_utf16_surrogate) {
	Editor ed;

	ed.accumulate (0xED);
	ed.accumulate (0xA0);
	ed.accumulate (0x80);
	BT_ASSERT_EQ (ed.get_u8(), u8"");
	ed.accumulate ('x');
	BT_ASSERT_EQ (ed.get_u8(), u8"x");

	BT_SUCCESS;
}

BT_SCENARIO_TEST (test_ed_accumulate_utf8_cp_out_of_range) {
	Editor ed;

	ed.accumulate (0xF4);
	ed.accumulate (0x90);
	ed.accumulate (0x80);
	ed.accumulate (0x80);
	BT_ASSERT_EQ (ed.get_u8(), u8"");
	ed.accumulate ('x');
	BT_ASSERT_EQ (ed.get_u8(), u8"x");

	BT_SUCCESS;
}

BT_SCENARIO_TEST (test_ed_insert_sequential_ascii) {
	Editor ed;

	ed.insert (u8"");
	BT_ASSERT_EQ (ed.get_u8(), u8"");
	BT_ASSERT_EQ (ed.get_u8_pos(), u8"");
	BT_ASSERT (ed.empty());
	BT_ASSERT (ed.at_begin());
	BT_ASSERT (ed.at_end());

	ed.insert (u8"a");
	BT_ASSERT_EQ (ed.get_u8(), u8"a");
	BT_ASSERT_EQ (ed.get_u8_pos(), u8"");
	BT_ASSERT (!ed.empty());
	BT_ASSERT (!ed.at_begin());
	BT_ASSERT (ed.at_end());

	ed.insert (u8"bc");
	BT_ASSERT_EQ (ed.get_u8(), u8"abc");
	BT_ASSERT_EQ (ed.get_u8_pos(), u8"");
	BT_ASSERT (!ed.empty());
	BT_ASSERT (!ed.at_begin());
	BT_ASSERT (ed.at_end());

	ed.insert (u8"");
	BT_ASSERT_EQ (ed.get_u8(), u8"abc");
	BT_ASSERT_EQ (ed.get_u8_pos(), u8"");
	BT_ASSERT (!ed.empty());
	BT_ASSERT (!ed.at_begin());
	BT_ASSERT (ed.at_end());

	BT_SUCCESS;
}

BT_SCENARIO_TEST (test_ed_insert_sequential_utf8) {
	Editor ed;

	ed.insert (u8"Aы©");
	BT_ASSERT_EQ (ed.get_u8(), u8"Aы©");
	BT_ASSERT_EQ (ed.get_u8_pos(), u8"");

	ed.insert (u8"é€😀🇷🇺");
	BT_ASSERT_EQ (ed.get_u8(), u8"Aы©é€😀🇷🇺");
	BT_ASSERT_EQ (ed.get_u8_pos(), u8"");

	BT_SUCCESS;
}

BT_SCENARIO_TEST (test_ed_clear) {
	Editor ed;

	ed.accumulate (0xC3);
	ed.insert (u8"Aы©");
	BT_ASSERT_EQ (ed.get_u8(), u8"Aы©");

	ed.clear();
	BT_ASSERT_EQ (ed.get_u8(), u8"");
	ed.accumulate (0xC3);
	BT_ASSERT_EQ (ed.get_u8(), u8"");

	ed.clear();
	ed.accumulate ('x');
	BT_ASSERT_EQ (ed.get_u8(), u8"x");

	BT_SUCCESS;
}


BT_SCENARIO_TEST (test_ed_move_begin) {
	Editor ed;

	ed.move_begin();
	BT_ASSERT_EQ (ed.get_u8_pos(), u8"");

	ed.insert (u8"abc");
	ed.move_begin();
	BT_ASSERT_EQ (ed.get_u8_pos(), u8"abc");
	ed.move_begin();
	BT_ASSERT_EQ (ed.get_u8_pos(), u8"abc");

	BT_SUCCESS;
}

BT_SCENARIO_TEST (test_ed_move_end) {
	Editor ed;

	ed.move_end();
	BT_ASSERT_EQ (ed.get_u8_pos(), u8"");

	ed.insert (u8"abc");
	ed.move_begin();
	ed.move_end();
	BT_ASSERT_EQ (ed.get_u8_pos(), u8"");
	ed.move_end();
	BT_ASSERT_EQ (ed.get_u8_pos(), u8"");

	BT_SUCCESS;
}

BT_SCENARIO_TEST (test_ed_move_next_grapheme_ascii) {
	Editor ed;

	ed.move_next_grapheme();
	BT_ASSERT_EQ (ed.get_u8_pos(), u8"");

	ed.insert (u8"abc");
	ed.move_begin();
	ed.move_next_grapheme();
	BT_ASSERT_EQ (ed.get_u8_pos(), u8"bc");
	ed.move_next_grapheme();
	BT_ASSERT_EQ (ed.get_u8_pos(), u8"c");
	ed.move_next_grapheme();
	BT_ASSERT_EQ (ed.get_u8_pos(), u8"");
	ed.move_next_grapheme();
	BT_ASSERT_EQ (ed.get_u8_pos(), u8"");

	BT_SUCCESS;
}

BT_SCENARIO_TEST (test_ed_move_prev_grapheme_ascii) {
	Editor ed;

	ed.move_prev_grapheme();
	BT_ASSERT_EQ (ed.get_u8_pos(), u8"");

	ed.insert (u8"abc");
	ed.move_prev_grapheme();
	BT_ASSERT_EQ (ed.get_u8_pos(), u8"c");
	ed.move_prev_grapheme();
	BT_ASSERT_EQ (ed.get_u8_pos(), u8"bc");
	ed.move_prev_grapheme();
	BT_ASSERT_EQ (ed.get_u8_pos(), u8"abc");
	ed.move_prev_grapheme();
	BT_ASSERT_EQ (ed.get_u8_pos(), u8"abc");

	BT_SUCCESS;
}

BT_SCENARIO_TEST (test_ed_move_begin_end_from_middle) {
	Editor ed;

	ed.insert (u8"abc");
	ed.move_prev_grapheme();
	ed.move_begin();
	BT_ASSERT_EQ (ed.get_u8_pos(), u8"abc");
	ed.move_next_grapheme();
	ed.move_end();
	BT_ASSERT_EQ (ed.get_u8_pos(), u8"");

	BT_SUCCESS;
}

BT_SCENARIO_TEST (test_ed_move_next_prev_utf8) {
	Editor ed;

	ed.insert (u8"A©é€😀ы界𒁲");

	ed.move_prev_grapheme();
	BT_ASSERT_EQ (ed.get_u8_pos(), u8"𒁲");
	ed.move_prev_grapheme();
	BT_ASSERT_EQ (ed.get_u8_pos(), u8"界𒁲");
	ed.move_prev_grapheme();
	BT_ASSERT_EQ (ed.get_u8_pos(), u8"ы界𒁲");
	ed.move_prev_grapheme();
	BT_ASSERT_EQ (ed.get_u8_pos(), u8"😀ы界𒁲");
	ed.move_prev_grapheme();
	BT_ASSERT_EQ (ed.get_u8_pos(), u8"€😀ы界𒁲");
	ed.move_prev_grapheme();
	BT_ASSERT_EQ (ed.get_u8_pos(), u8"é€😀ы界𒁲");
	ed.move_prev_grapheme();
	BT_ASSERT_EQ (ed.get_u8_pos(), u8"©é€😀ы界𒁲");
	ed.move_prev_grapheme();
	BT_ASSERT_EQ (ed.get_u8_pos(), u8"A©é€😀ы界𒁲");

	ed.move_next_grapheme();
	BT_ASSERT_EQ (ed.get_u8_pos(), u8"©é€😀ы界𒁲");
	ed.move_next_grapheme();
	BT_ASSERT_EQ (ed.get_u8_pos(), u8"é€😀ы界𒁲");
	ed.move_next_grapheme();
	BT_ASSERT_EQ (ed.get_u8_pos(), u8"€😀ы界𒁲");
	ed.move_next_grapheme();
	BT_ASSERT_EQ (ed.get_u8_pos(), u8"😀ы界𒁲");
	ed.move_next_grapheme();
	BT_ASSERT_EQ (ed.get_u8_pos(), u8"ы界𒁲");
	ed.move_next_grapheme();
	BT_ASSERT_EQ (ed.get_u8_pos(), u8"界𒁲");
	ed.move_next_grapheme();
	BT_ASSERT_EQ (ed.get_u8_pos(), u8"𒁲");
	ed.move_next_grapheme();
	BT_ASSERT_EQ (ed.get_u8_pos(), u8"");

	BT_SUCCESS;
}

BT_SCENARIO_TEST (test_ed_move_next_prev_utf8_combine_mark) {
	Editor ed;

	ed.insert (u8"e\u0301x");
	ed.move_prev_grapheme();
	BT_ASSERT_EQ (ed.get_u8_pos(), u8"x");
	ed.move_prev_grapheme();
	BT_ASSERT_EQ (ed.get_u8_pos(), u8"e\u0301x");
	ed.move_next_grapheme();
	BT_ASSERT_EQ (ed.get_u8_pos(), u8"x");

	ed.clear();
	ed.insert (u8"ы\u0306ъ");
	ed.move_prev_grapheme();
	BT_ASSERT_EQ (ed.get_u8_pos(), u8"ъ");
	ed.move_prev_grapheme();
	BT_ASSERT_EQ (ed.get_u8_pos(), u8"ы\u0306ъ");
	ed.move_next_grapheme();
	BT_ASSERT_EQ (ed.get_u8_pos(), u8"ъ");

	BT_SUCCESS;
}

BT_SCENARIO_TEST (test_ed_move_next_prev_utf8_zwj) {
	Editor ed;

	ed.insert (u8"a👨‍👩‍👧‍👦b");
	ed.move_prev_grapheme();
	BT_ASSERT_EQ (ed.get_u8_pos(), u8"b");
	ed.move_prev_grapheme();
	BT_ASSERT_EQ (ed.get_u8_pos(), u8"👨‍👩‍👧‍👦b");
	ed.move_prev_grapheme();
	BT_ASSERT_EQ (ed.get_u8_pos(), u8"a👨‍👩‍👧‍👦b");
	ed.move_next_grapheme();
	BT_ASSERT_EQ (ed.get_u8_pos(), u8"👨‍👩‍👧‍👦b");
	ed.move_next_grapheme();
	BT_ASSERT_EQ (ed.get_u8_pos(), u8"b");

	BT_SUCCESS;
}

BT_SCENARIO_TEST (test_ed_move_next_prev_utf8_flag) {
	Editor ed;

	ed.insert (u8"a🇷🇺b");
	ed.move_prev_grapheme();
	BT_ASSERT_EQ (ed.get_u8_pos(), u8"b");
	ed.move_prev_grapheme();
	BT_ASSERT_EQ (ed.get_u8_pos(), u8"🇷🇺b");
	ed.move_prev_grapheme();
	BT_ASSERT_EQ (ed.get_u8_pos(), u8"a🇷🇺b");
	ed.move_next_grapheme();
	BT_ASSERT_EQ (ed.get_u8_pos(), u8"🇷🇺b");
	ed.move_next_grapheme();
	BT_ASSERT_EQ (ed.get_u8_pos(), u8"b");

	BT_SUCCESS;
}

BT_SCENARIO_TEST (test_ed_insert_begin_middle) {
	Editor ed;

	ed.insert (u8"é€😀🇷🇺");
	ed.move_begin();
	ed.insert (u8"Aы©");
	BT_ASSERT_EQ (ed.get_u8(), u8"Aы©é€😀🇷🇺");
	BT_ASSERT_EQ (ed.get_u8_pos(), u8"é€😀🇷🇺");
	ed.insert (u8"界𒁲");
	BT_ASSERT_EQ (ed.get_u8(), u8"Aы©界𒁲é€😀🇷🇺");
	BT_ASSERT_EQ (ed.get_u8_pos(), u8"é€😀🇷🇺");

	BT_SUCCESS;
}

BT_SCENARIO_TEST (test_ed_insert_reset_acc) {
	Editor ed;

	ed.accumulate (0xC3);
	ed.insert (u8"x");
	ed.accumulate (0xA9);
	BT_ASSERT_EQ (ed.get_u8(), u8"x");

	BT_SUCCESS;
}

int main () {
	return Suite ("testEditor", tests (
		BT_SUITE_SCENARIO (test_ed_create),
		BT_SUITE_SCENARIO (test_ed_accumulate_ascii),
		BT_SUITE_SCENARIO (test_ed_accumulate_utf8_2byte),
		BT_SUITE_SCENARIO (test_ed_accumulate_utf8_3byte),
		BT_SUITE_SCENARIO (test_ed_accumulate_utf8_4byte),
		BT_SUITE_SCENARIO (test_ed_accumulate_utf8_ascii_mixed),
		BT_SUITE_SCENARIO (test_ed_accumulate_utf8_combine_mark),
		BT_SUITE_SCENARIO (test_ed_accumulate_utf8_null_reset_acc),
		BT_SUITE_SCENARIO (test_ed_accumulate_utf8_lone_continuation_byte),
		BT_SUITE_SCENARIO (test_ed_accumulate_utf8_no_continuation_byte),
		BT_SUITE_SCENARIO (test_ed_accumulate_utf8_overlong_encoding),
		BT_SUITE_SCENARIO (test_ed_accumulate_utf8_utf16_surrogate),
		BT_SUITE_SCENARIO (test_ed_accumulate_utf8_cp_out_of_range),
		BT_SUITE_SCENARIO (test_ed_insert_sequential_ascii),
		BT_SUITE_SCENARIO (test_ed_insert_sequential_utf8),
		BT_SUITE_SCENARIO (test_ed_clear),
		BT_SUITE_SCENARIO (test_ed_move_begin),
		BT_SUITE_SCENARIO (test_ed_move_end),
		BT_SUITE_SCENARIO (test_ed_move_next_grapheme_ascii),
		BT_SUITE_SCENARIO (test_ed_move_prev_grapheme_ascii),
		BT_SUITE_SCENARIO (test_ed_move_begin_end_from_middle),
		BT_SUITE_SCENARIO (test_ed_move_next_prev_utf8),
		BT_SUITE_SCENARIO (test_ed_move_next_prev_utf8_combine_mark),
		BT_SUITE_SCENARIO (test_ed_move_next_prev_utf8_zwj),
		BT_SUITE_SCENARIO (test_ed_move_next_prev_utf8_flag),
		BT_SUITE_SCENARIO (test_ed_insert_begin_middle),
		BT_SUITE_SCENARIO (test_ed_insert_reset_acc)
	)).run_rc();
}
