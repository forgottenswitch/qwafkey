#ifndef LAYOUT_H_INCLUDED
#define LAYOUT_H_INCLUDED

// augment VK_ codes

#define LK_LBRACE 219
#define LK_RBRACE 221

#define LK_SEMICOLON 186
#define LK_QUOTE 222

#define LK_COMMA 188
#define LK_PERIOD 190
#define LK_SLASH 191

// utilites

#define l15(vk_phys, remap) l1(vk_phys, remap), l5(vk_phys, remap)
#define l0(vk_phys, remap) l15(vk_phys, remap), l3(vk_phys, remap), l7(vk_phys, remap)

#define l1(vk_phys, remap) { vk_phys, 1, remap }, { vk_phys, 2, remap }
#define l3(vk_phys, remap) { vk_phys, 3, remap }, { vk_phys, 4, remap }
#define l5(vk_phys, remap) { vk_phys, 5, remap }, { vk_phys, 6, remap }
#define l7(vk_phys, remap) { vk_phys, 7, remap }, { vk_phys, 8, remap }

#define l56(vk_phys, remap5, remap6) { vk_phys, 5, remap5 }, { vk_phys, 6, remap6 }

#define m15(vk_phys) m1(vk_phys), m5(vk_phys)
#define m0(vk_phys) m15(vk_phys), m3(vk_phys), m7(vk_phys)

#define m1(vk_phys) { vk_phys, 1 }, { vk_phys, 2 }
#define m3(vk_phys) { vk_phys, 3 }, { vk_phys, 4 }
#define m5(vk_phys) { vk_phys, 5 }, { vk_phys, 6 }
#define m7(vk_phys) { vk_phys, 7 }, { vk_phys, 8 }

// layout

Layout_BindMod_Key layout_def_mods[] = {
    m0(VK_LSHIFT), m0(VK_RSHIFT),
    m0(VK_LCONTROL), m0(VK_RCONTROL),
    m0(VK_LMENU), m0(VK_RMENU),
    m0(VK_LWIN), m0(VK_RWIN),
    //
    m0(VK_CAPITAL),
    //
    m0(LK_SEMICOLON),
    m5('S'), m3('S'),
    //
    { LK_COMMA, 1 },
    // end
    { 0, 0 }
};

Layout_BindAct_Key layout_def_acts[] = {
    l15(VK_LSHIFT, KbdAction_Level2_Shift),
    l15(VK_RSHIFT, KbdAction_Level2_Shift),
    l0(VK_RMENU, KbdAction_AltGr),
    //
    l15(LK_SEMICOLON, KbdAction_Level5_Shift),
    l5('S', KbdAction_Level2_Latch),
    //
    { LK_COMMA, 1, KbdAction_Level3_Latch },
    // end
    { 0, 0, nil }
};

Layout_BindKey_Key layout_def_keys[] = {
    l0(VK_CAPITAL, VK_LCONTROL),
    //
    l5('H', VK_LEFT), l5('J', VK_DOWN), l5('K', VK_UP), l5('L', VK_RIGHT),
    l5('U', VK_BACK), l5('O', VK_DELETE),
    l5('N', VK_NEXT), l5('Y', VK_PRIOR),
    l5(LK_COMMA, VK_HOME), l5(LK_PERIOD, VK_END),
    l5('I', VK_ESCAPE), l5(LK_QUOTE, VK_INSERT),
    l5('P', VK_PRINT), l5(LK_SLASH, VK_PAUSE),
    // end
    { 0, 0, 0 }
};

Layout_BindChr_Key layout_def_chrs[] = {
    l56('1', '/', '/'),
    l56('2', '+', '+'),
    l56('3', '<', '<'),
    l56('4', '>', '>'),
    l56('5', '-', '_'),
    l56('6', '^', '^'),
    l56('7', 0x2018, 0x201C),
    l56('8', 0x201A, 0x201E),
    l56('9', 0x201B, 0x201F),
    l56('0', 0x2019, 0x201D),
    //
    l56('Q', '!', '?'),
    l56('W', ':', '`'),
    l56('E', '(', '@'),
    l56('R', '=', '#'),
    l56('T', '\t', '\t'),
    //
    l56('A', '&', ','),
    l56('D', '{', '}'),
    l56('F', '[', ']'),
    l56('G', ')', '*'),
    //
    l56('Z', '%', '%'),
    l56('X', '$', '`'),
    l56('C', '\\', '|'),
    l56('V', '"', '\''),
    l56('B', ';', '/'),
    // end
    { 0, 0, 0 }
};

#endif // LAYOUT_H_INCLUDED
