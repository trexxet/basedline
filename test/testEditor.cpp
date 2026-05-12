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

BT_SCENARIO_TEST (test_ed_move_next_prev_grapheme_ascii) {
	Editor ed;

	ed.move_next_grapheme();
	BT_ASSERT_EQ (ed.get_u8_pos(), u8"");
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

BT_SCENARIO_TEST (test_ed_move_next_prev_grapheme_utf8) {
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

BT_SCENARIO_TEST (test_ed_move_next_prev_grapheme_utf8_combine_mark) {
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

BT_SCENARIO_TEST (test_ed_move_next_prev_grapheme_utf8_zwj) {
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

BT_SCENARIO_TEST (test_ed_move_next_prev_grapheme_utf8_flag) {
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

BT_SCENARIO_TEST (test_ed_move_next_prev_word_ascii) {
	Editor ed;

	ed.move_next_word();
	BT_ASSERT_EQ (ed.get_u8_pos(), u8"");
	ed.move_prev_word();
	BT_ASSERT_EQ (ed.get_u8_pos(), u8"");

	ed.insert (u8"hello world test");

	ed.move_prev_word();
	BT_ASSERT_EQ (ed.get_u8_pos(), u8"test");
	ed.move_prev_word();
	BT_ASSERT_EQ (ed.get_u8_pos(), u8"world test");
	ed.move_prev_word();
	BT_ASSERT_EQ (ed.get_u8_pos(), u8"hello world test");
	ed.move_prev_word();
	BT_ASSERT_EQ (ed.get_u8_pos(), u8"hello world test");

	ed.move_next_word();
	BT_ASSERT_EQ (ed.get_u8_pos(), u8"world test");
	ed.move_next_word();
	BT_ASSERT_EQ (ed.get_u8_pos(), u8"test");
	ed.move_next_word();
	BT_ASSERT_EQ (ed.get_u8_pos(), u8"");
	ed.move_next_word();
	BT_ASSERT_EQ (ed.get_u8_pos(), u8"");

	BT_SUCCESS;
}

BT_SCENARIO_TEST (test_ed_move_next_prev_word_ascii_punctuation) {
	Editor ed;

	ed.insert (u8"hello,  wor_ld!!\ttest.");

	ed.move_prev_word();
	BT_ASSERT_EQ (ed.get_u8_pos(), u8"test.");
	ed.move_prev_word();
	BT_ASSERT_EQ (ed.get_u8_pos(), u8"wor_ld!!\ttest.");
	ed.move_prev_word();
	BT_ASSERT_EQ (ed.get_u8_pos(), u8"hello,  wor_ld!!\ttest.");
	ed.move_prev_word();
	BT_ASSERT_EQ (ed.get_u8_pos(), u8"hello,  wor_ld!!\ttest.");

	ed.move_next_word();
	BT_ASSERT_EQ (ed.get_u8_pos(), u8"wor_ld!!\ttest.");
	ed.move_next_word();
	BT_ASSERT_EQ (ed.get_u8_pos(), u8"test.");
	ed.move_next_word();
	BT_ASSERT_EQ (ed.get_u8_pos(), u8"");
	ed.move_next_word();
	BT_ASSERT_EQ (ed.get_u8_pos(), u8"");

	BT_SUCCESS;
}

BT_SCENARIO_TEST (test_ed_move_next_prev_word_ascii_from_inside_word) {
	Editor ed;

	ed.insert (u8"hello world");

	ed.move_prev_grapheme();
	ed.move_prev_grapheme();
	BT_ASSERT_EQ (ed.get_u8_pos(), u8"ld");
	ed.move_prev_word();
	BT_ASSERT_EQ (ed.get_u8_pos(), u8"world");
	ed.move_prev_grapheme();
	ed.move_prev_grapheme();
	BT_ASSERT_EQ (ed.get_u8_pos(), u8"o world");
	ed.move_prev_word();
	BT_ASSERT_EQ (ed.get_u8_pos(), u8"hello world");

	ed.move_next_grapheme();
	ed.move_next_grapheme();
	BT_ASSERT_EQ (ed.get_u8_pos(), u8"llo world");
	ed.move_next_word();
	BT_ASSERT_EQ (ed.get_u8_pos(), u8"world");
	ed.move_next_grapheme();
	ed.move_next_grapheme();
	BT_ASSERT_EQ (ed.get_u8_pos(), u8"rld");
	ed.move_next_word();
	BT_ASSERT_EQ (ed.get_u8_pos(), u8"");

	BT_SUCCESS;
}

BT_SCENARIO_TEST (test_ed_move_next_prev_word_ascii_from_separator) {
	Editor ed;

	ed.insert (u8"hello   world");

	ed.move_prev_word();
	ed.move_prev_grapheme();
	BT_ASSERT_EQ (ed.get_u8_pos(), u8" world");
	ed.move_prev_word();
	BT_ASSERT_EQ (ed.get_u8_pos(), u8"hello   world");

	ed.move_next_word();
	ed.move_prev_grapheme();
	ed.move_prev_grapheme();
	BT_ASSERT_EQ (ed.get_u8_pos(), u8"  world");
	ed.move_next_word();
	BT_ASSERT_EQ (ed.get_u8_pos(), u8"world");

	BT_SUCCESS;
}

BT_SCENARIO_TEST (test_ed_move_next_prev_word_utf8) {
	Editor ed;

	ed.insert (u8"Aé, e\u0301ы???   界!!");

	ed.move_prev_word();
	BT_ASSERT_EQ (ed.get_u8_pos(), u8"界!!");
	ed.move_prev_word();
	BT_ASSERT_EQ (ed.get_u8_pos(), u8"e\u0301ы???   界!!");
	ed.move_prev_word();
	BT_ASSERT_EQ (ed.get_u8_pos(), u8"Aé, e\u0301ы???   界!!");
	ed.move_prev_word();
	BT_ASSERT_EQ (ed.get_u8_pos(), u8"Aé, e\u0301ы???   界!!");

	ed.move_next_word();
	BT_ASSERT_EQ (ed.get_u8_pos(), u8"e\u0301ы???   界!!");
	ed.move_next_word();
	BT_ASSERT_EQ (ed.get_u8_pos(), u8"界!!");
	ed.move_next_word();
	BT_ASSERT_EQ (ed.get_u8_pos(), u8"");
	ed.move_next_word();
	BT_ASSERT_EQ (ed.get_u8_pos(), u8"");

	BT_SUCCESS;
}

BT_SCENARIO_TEST (test_ed_erase_empty) {
	Editor ed;

	ed.insert (u8"");
	ed.erase_prev_grapheme();
	ed.erase_pos_grapheme();
	ed.erase_prev_word();
	ed.erase_pos_word();
	ed.erase_begin_to_pos();
	ed.erase_pos_to_end();
	BT_ASSERT_EQ (ed.get_u8(), u8"");

	BT_SUCCESS;
}

BT_SCENARIO_TEST (test_ed_erase_pos_prev_grapheme) {
	Editor ed;

	ed.insert (u8"Aы©界𒁲ée\u0301€😀");
	for (size_t i = 0; i < 5; i++)
		ed.move_prev_grapheme();
	BT_ASSERT_EQ (ed.get_u8_pos(), u8"𒁲ée\u0301€😀");

	ed.erase_pos_grapheme();
	BT_ASSERT_EQ (ed.get_u8(), u8"Aы©界ée\u0301€😀");
	BT_ASSERT_EQ (ed.get_u8_pos(), u8"ée\u0301€😀");

	ed.erase_prev_grapheme();
	BT_ASSERT_EQ (ed.get_u8(), u8"Aы©ée\u0301€😀");
	BT_ASSERT_EQ (ed.get_u8_pos(), u8"ée\u0301€😀");

	ed.erase_pos_grapheme();
	BT_ASSERT_EQ (ed.get_u8(), u8"Aы©e\u0301€😀");
	BT_ASSERT_EQ (ed.get_u8_pos(), u8"e\u0301€😀");

	ed.erase_prev_grapheme();
	BT_ASSERT_EQ (ed.get_u8(), u8"Aыe\u0301€😀");
	BT_ASSERT_EQ (ed.get_u8_pos(), u8"e\u0301€😀");

	ed.erase_pos_grapheme();
	BT_ASSERT_EQ (ed.get_u8(), u8"Aы€😀");
	BT_ASSERT_EQ (ed.get_u8_pos(), u8"€😀");

	ed.erase_prev_grapheme();
	BT_ASSERT_EQ (ed.get_u8(), u8"A€😀");
	BT_ASSERT_EQ (ed.get_u8_pos(), u8"€😀");

	ed.erase_pos_grapheme();
	BT_ASSERT_EQ (ed.get_u8(), u8"A😀");
	BT_ASSERT_EQ (ed.get_u8_pos(), u8"😀");

	ed.erase_prev_grapheme();
	BT_ASSERT_EQ (ed.get_u8(), u8"😀");
	BT_ASSERT_EQ (ed.get_u8_pos(), u8"😀");

	ed.erase_prev_grapheme();
	BT_ASSERT_EQ (ed.get_u8(), u8"😀");
	BT_ASSERT_EQ (ed.get_u8_pos(), u8"😀");

	ed.erase_pos_grapheme();
	BT_ASSERT_EQ (ed.get_u8(), u8"");
	BT_ASSERT_EQ (ed.get_u8_pos(), u8"");

	BT_SUCCESS;
}

BT_SCENARIO_TEST (test_ed_erase_pos_prev_word) {
	Editor ed;

	ed.insert (u8"Aы, ée\u0301x   фваЫ\u0306Б,,, é??");
	ed.move_prev_word();
	ed.move_prev_word();
	ed.move_next_grapheme();
	ed.move_next_grapheme();
	BT_ASSERT_EQ (ed.get_u8_pos(), u8"аЫ\u0306Б,,, é??");

	ed.erase_pos_word();
	BT_ASSERT_EQ (ed.get_u8(), u8"Aы, ée\u0301x   фвé??");
	BT_ASSERT_EQ (ed.get_u8_pos(), u8"é??");

	ed.erase_prev_word();
	BT_ASSERT_EQ (ed.get_u8(), u8"Aы, ée\u0301x   é??");
	BT_ASSERT_EQ (ed.get_u8_pos(), u8"é??");

	ed.erase_prev_word();
	BT_ASSERT_EQ (ed.get_u8(), u8"Aы, é??");
	BT_ASSERT_EQ (ed.get_u8_pos(), u8"é??");

	ed.erase_pos_word();
	BT_ASSERT_EQ (ed.get_u8(), u8"Aы, ");
	BT_ASSERT_EQ (ed.get_u8_pos(), u8"");

	ed.erase_pos_word();
	BT_ASSERT_EQ (ed.get_u8(), u8"Aы, ");
	BT_ASSERT_EQ (ed.get_u8_pos(), u8"");

	ed.erase_prev_word();
	BT_ASSERT_EQ (ed.get_u8(), u8"");
	BT_ASSERT_EQ (ed.get_u8_pos(), u8"");

	BT_SUCCESS;
}

BT_SCENARIO_TEST (test_ed_erase_pos_begin_end) {
	Editor ed;

	ed.insert (u8"Aы, ée\u0301x   фваЫ\u0306Б,,, é??");
	ed.move_prev_word();
	ed.move_prev_word();
	ed.move_next_grapheme();
	ed.move_next_grapheme();
	BT_ASSERT_EQ (ed.get_u8_pos(), u8"аЫ\u0306Б,,, é??");

	ed.erase_begin_to_pos();
	BT_ASSERT_EQ (ed.get_u8(), u8"аЫ\u0306Б,,, é??");
	BT_ASSERT_EQ (ed.get_u8_pos(), u8"аЫ\u0306Б,,, é??");
	BT_ASSERT (ed.at_begin());

	ed.move_next_grapheme();
	ed.move_next_grapheme();
	BT_ASSERT_EQ (ed.get_u8_pos(), u8"Б,,, é??");

	ed.erase_pos_to_end();
	BT_ASSERT_EQ (ed.get_u8(), u8"аЫ\u0306");
	BT_ASSERT_EQ (ed.get_u8_pos(), u8"");
	BT_ASSERT (ed.at_end());

	ed.erase_begin_to_pos();
	BT_ASSERT_EQ (ed.get_u8(), u8"");
	BT_ASSERT_EQ (ed.get_u8_pos(), u8"");

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
		BT_SUITE_SCENARIO (test_ed_move_next_prev_grapheme_ascii),
		BT_SUITE_SCENARIO (test_ed_move_begin_end_from_middle),
		BT_SUITE_SCENARIO (test_ed_move_next_prev_grapheme_utf8),
		BT_SUITE_SCENARIO (test_ed_move_next_prev_grapheme_utf8_combine_mark),
		BT_SUITE_SCENARIO (test_ed_move_next_prev_grapheme_utf8_zwj),
		BT_SUITE_SCENARIO (test_ed_move_next_prev_grapheme_utf8_flag),
		BT_SUITE_SCENARIO (test_ed_insert_begin_middle),
		BT_SUITE_SCENARIO (test_ed_insert_reset_acc),
		BT_SUITE_SCENARIO (test_ed_move_next_prev_word_ascii),
		BT_SUITE_SCENARIO (test_ed_move_next_prev_word_ascii_punctuation),
		BT_SUITE_SCENARIO (test_ed_move_next_prev_word_ascii_from_inside_word),
		BT_SUITE_SCENARIO (test_ed_move_next_prev_word_ascii_from_separator),
		BT_SUITE_SCENARIO (test_ed_move_next_prev_word_utf8),
		BT_SUITE_SCENARIO (test_ed_erase_empty),
		BT_SUITE_SCENARIO (test_ed_erase_pos_prev_grapheme),
		BT_SUITE_SCENARIO (test_ed_erase_pos_prev_word),
		BT_SUITE_SCENARIO (test_ed_erase_pos_begin_end)
	)).run_rc();
}
