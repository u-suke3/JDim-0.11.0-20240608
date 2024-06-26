// ライセンス: GPL2

// 半角カナ -> 全角カナ変換テーブル

#ifndef _HKANA_H_
#define _HKANA_H_

constexpr const unsigned char hkana_table1[][2][8] =
{
    { "｡", "。" }, { "｢", "「" }, { "｣", "」" }, { "､", "、" }, { "･", "・" },
    { "ｦ", "ヲ" },
    { "ｧ", "ァ" }, { "ｨ", "ィ" }, { "ｩ", "ゥ" }, { "ｪ", "ェ" }, { "ｫ", "ォ" }, 
    { "ｬ", "ャ" }, { "ｭ", "ュ" }, { "ｮ", "ョ" }, { "ｯ", "ッ" },
    { "ｰ", "ー" },

    { "ｱ", "ア" }, { "ｲ", "イ" }, { "ｳ", "ウ" }, { "ｴ", "エ" }, { "ｵ", "オ" },
    { "ｶ", "カ" }, { "ｷ", "キ" }, { "ｸ", "ク" }, { "ｹ", "ケ" }, { "ｺ", "コ" },
    { "ｻ", "サ" }, { "ｼ", "シ" }, { "ｽ", "ス" }, { "ｾ", "セ" }, { "ｿ", "ソ" },
    { "ﾀ", "タ" }, { "ﾁ", "チ" }, { "ﾂ", "ツ" }, { "ﾃ", "テ" }, { "ﾄ", "ト" },
    { "ﾅ", "ナ" }, { "ﾆ", "ニ" }, { "ﾇ", "ヌ" }, { "ﾈ", "ネ" }, { "ﾉ", "ノ" },
    { "ﾊ", "ハ" }, { "ﾋ", "ヒ" }, { "ﾌ", "フ" }, { "ﾍ", "ヘ" }, { "ﾎ", "ホ" },
    { "ﾏ", "マ" }, { "ﾐ", "ミ" }, { "ﾑ", "ム" }, { "ﾒ", "メ" }, { "ﾓ", "モ" },
    { "ﾔ", "ヤ" }, { "ﾕ", "ユ" }, { "ﾖ", "ヨ" },
    { "ﾗ", "ラ" }, { "ﾘ", "リ" }, { "ﾙ", "ル" }, { "ﾚ", "レ" }, { "ﾛ", "ロ" },
    { "ﾜ", "ワ" }, { "ﾝ", "ン" },

    // 濁点
    { "ｳ", "ヴ" },
    { "ｶ", "ガ" }, { "ｷ", "ギ" }, { "ｸ", "グ" }, { "ｹ", "ゲ" }, { "ｺ", "ゴ" },
    { "ｻ", "ザ" }, { "ｼ", "ジ" }, { "ｽ", "ズ" }, { "ｾ", "ゼ" }, { "ｿ", "ゾ" },
    { "ﾀ", "ダ" }, { "ﾁ", "ヂ" }, { "ﾂ", "ヅ" }, { "ﾃ", "デ" }, { "ﾄ", "ド" },
    { "ﾊ", "バ" }, { "ﾋ", "ビ" }, { "ﾌ", "ブ" }, { "ﾍ", "ベ" }, { "ﾎ", "ボ" },

    // 半濁点
    { "ﾊ", "パ" }, { "ﾋ", "ピ" }, { "ﾌ", "プ" }, { "ﾍ", "ペ" }, { "ﾎ", "ポ" },

    { "", "" }
}; 


#endif
