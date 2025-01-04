#include <X11/XF86keysym.h>

/* appearance */
static const unsigned int borderpx               = 1;        /* border pixel of windows */
static const unsigned int snap                   = 32;       /* snap pixel */
static const unsigned int gappih                 = 20;       /* horiz inner gap between windows */
static const unsigned int gappiv                 = 10;       /* vert inner gap between windows */
static const unsigned int gappoh                 = 10;       /* horiz outer gap between windows and screen edge */
static const unsigned int gappov                 = 30;       /* vert outer gap between windows and screen edge */
static       int smartgaps                       = 1;        /* 1 means no outer gap when there is only one window */
static const int showbar                         = 1;        /* 0 means no bar */
static const int topbar                          = 1;        /* 0 means bottom bar */
static const char *barlayout                     = "tn|s";
static const char *fonts[]                       = { 
    "Hack Nerd Font:size=18", 
    "JetBrains Mono:size=18",
    "Noto Color Emoji:size=18",
};
static const char col_bg[]                       = "#27251f";
static const char col_active[]                   = "#9bf6ff";
static const char col_inactive[]                 = "#c4ced4";
static const char *colors[][3]                   = {
	/*               fg         bg         border   */
	[SchemeNorm] = { col_inactive, col_bg, col_bg },
	[SchemeSel]  = { col_active, col_bg, col_bg },
};

/* appicons */
/* NOTE: set to 0 to set to default (whitespace) */
static char outer_separator_beg      = '|';
static char outer_separator_end      = '|';
static char inner_separator          = ' ';
static unsigned truncate_icons_after = 2; /* will default to 1, that is the min */
static char truncate_symbol[]         = "...";

static const Rule rules[] = {
	/* xprop(1):
	 *	WM_CLASS(STRING) = instance, class
	 *	WM_NAME(STRING) = title
	 */
	/* class  instance  title  tags mask  isfloating  monitor  scratch key  appicon*/
	{ NULL, NULL, "scratchpad", NULL, 1, -1 , 's', ""},
	{ NULL, NULL, "st", NULL, NULL, -1 , 0, ""},
	{ "Thunar", NULL, NULL, NULL, 1, -1 , 'f', "📁"},
	{ NULL, NULL, "xmessage", NULL, 1, -1 , '?', "❔"},
	{ NULL, NULL, "Zen Browser", NULL, NULL, -1 , 0, "󰈹"},
	{ "Emacs", NULL, NULL, NULL, NULL, -1 , 0, ""},
};

/* tagging */
static const char *tags[] = { "1", "2", "3", "4", "5", "6", "7", "8", "9" };

/* layout(s) */
#define MASTER_DEFAULT 0.55
static const float mfact     = MASTER_DEFAULT; /* factor of master area size [0.05..0.95] */
static const int nmaster     = 1;    /* number of clients in master area */
static const int resizehints = 0;    /* 1 means respect size hints in tiled resizals */
static const int lockfullscreen = 1; /* 1 will force focus on the fullscreen window */

#define FORCE_VSPLIT 1  /* nrowgrid layout: force two clients to always split vertically */
#include "vanitygaps.c"


static const Layout layouts[] = {
	/* symbol     arrange function */
	{ "[]=",      tile },    /* first entry is default */
	{ "><>",      NULL },    /* no layout function means floating behavior */
	{ "[M]",      monocle },
};

#define MODKEY Mod4Mask
#define TAGKEYS(KEY,TAG)                                                                                               \
       &((Keychord){1, {{MODKEY, KEY}},                                        view,           {.ui = 1 << TAG} }), \
       &((Keychord){1, {{MODKEY|ControlMask, KEY}},                            toggleview,     {.ui = 1 << TAG} }), \
       &((Keychord){1, {{MODKEY|ShiftMask, KEY}},                              tag,            {.ui = 1 << TAG} }), \
       &((Keychord){1, {{MODKEY|ControlMask|ShiftMask, KEY}},                  toggletag,      {.ui = 1 << TAG} }),

/* helper for spawning shell commands in the pre dwm-5.0 fashion */
#define SHCMD(cmd) { .v = (const char*[]){ "/bin/sh", "-c", cmd, NULL } }

/* commands */
static char dmenumon[2] = "0"; /* component of dmenucmd, manipulated in spawn() */

static const char *helpcmd[]  = { "?", "xmessage", 
    "-center", "-buttons", "", 
    "-file", "/home/rumen/.config/dwm/docs/keybinds.txt",
    NULL 
};

static const char *termcmd[]  = { "st", NULL };
static const char *scratchpadcmd[] = {"s", "st", "-t", "scratchpad", NULL};
static const char *dmenucmd[] = { "dmenu_run", NULL };
static const char *emacscmd[] = { "emacsclient", "-c", NULL };
static const char *browsercmd[] = { "zen", NULL };
static const char *filemanagercmd[] = { "f", "thunar", NULL };
static const char *musiccmd[] = { "spotube", NULL };
static const char *brightnessupcmd[] = { "brightnessctl", "set", "+5", NULL };
static const char *brightnessdowncmd[] = { "brightnessctl", "set", "5-", NULL };
static const char *volumeupcmd[] = { "pamixer", "-i", "10", NULL };
static const char *volumedowncmd[] = { "pamixer", "-d", "10", NULL };
static const char *mutecmd[] = { "pamixer", "-t", NULL };
static const char *screenshotcmd[] = { "flameshot", "gui", NULL };
static const char *xkillcmd[] = { "xkill", NULL };

/* scripts */
static const char powermenu[] = "~/.local/share/scripts/power-menu.sh";
static const char emoji[] = "~/.local/share/scripts/emojis.sh";
static const char math[] = "~/.local/share/scripts/math-symbols.sh";
static const char clipboard[] = "~/.local/share/scripts/clipboard.sh";
static const char wallpaper[] = "~/.local/share/scripts/wallpaper.sh";

static Keychord *keychords[] = {
       /* Keys        function        argument */
       &((Keychord){1, {{MODKEY|ShiftMask, XK_slash}},                         togglescratch,  {.v = helpcmd } }),
       &((Keychord){1, {{MODKEY, XK_Return}},                                  spawn,          {.v = termcmd } }),
       &((Keychord){1, {{MODKEY|ShiftMask, XK_Return}},                        togglescratch,  {.v = scratchpadcmd } }),

       /* Applications */
       &((Keychord){2, {{MODKEY, XK_x}, {NULL, XK_Return}},                    spawn,          {.v = emacscmd } }),
       &((Keychord){2, {{MODKEY, XK_x}, {NULL, XK_b}},                         spawn,          {.v = browsercmd } }),
       &((Keychord){2, {{MODKEY, XK_x}, {NULL, XK_f}},                         togglescratch,  {.v = filemanagercmd } }),
       &((Keychord){2, {{MODKEY, XK_x}, {NULL, XK_m}},                         spawn,          {.v = musiccmd } }),
       &((Keychord){2, {{MODKEY, XK_x}, {NULL, XK_x}},                         spawn,          {.v = dmenucmd } }),

       /* Utils */
       &((Keychord){1, {{NULL, XF86XK_MonBrightnessUp}},                       spawn,          {.v = brightnessupcmd } }),
       &((Keychord){1, {{NULL, XF86XK_MonBrightnessDown}},                     spawn,          {.v = brightnessdowncmd } }),
       &((Keychord){1, {{NULL, XF86XK_AudioRaiseVolume}},                      spawn,          {.v = volumeupcmd } }),
       &((Keychord){1, {{NULL, XF86XK_AudioLowerVolume}},                      spawn,          {.v = volumedowncmd } }),
       &((Keychord){1, {{NULL, XF86XK_AudioMute}},                             spawn,          {.v = mutecmd } }),
       &((Keychord){1, {{MODKEY|ShiftMask, XK_s}},                             spawn,          {.v = screenshotcmd } }),
       &((Keychord){1, {{MODKEY|ShiftMask, XK_c}},                             spawn,          {.v = xkillcmd } }),

       /* Popup Menus */
       &((Keychord){1, {{MODKEY, XK_q}},                                       spawn,          SHCMD(powermenu) }),
       &((Keychord){1, {{MODKEY, XK_e}},                                       spawn,          SHCMD(emoji) }),
       &((Keychord){1, {{MODKEY|ShiftMask, XK_e}},                             spawn,          SHCMD(math) }),
       &((Keychord){1, {{MODKEY|ShiftMask, XK_v}},                             spawn,          SHCMD(clipboard) }),
       &((Keychord){1, {{MODKEY, XK_w}},                                       spawn,          SHCMD(wallpaper) }),

       /* System */
       &((Keychord){2, {{MODKEY, XK_c}, {NULL, XK_Return}},                    killclient,     {0} }),
       &((Keychord){2, {{MODKEY, XK_c}, {NULL, XK_b}},                         togglebar,      {0} }),
       &((Keychord){2, {{MODKEY, XK_c}, {NULL, XK_m}},                         zoom,           {0} }),
       &((Keychord){2, {{MODKEY, XK_c}, {NULL, XK_v}},                         view,           {.ui = ~0 } }),
       &((Keychord){2, {{MODKEY, XK_c}, {NULL, XK_f}},                         togglefloating, {0} }),
       &((Keychord){1, {{MODKEY, XK_j}},                                       focusstack,     {.i = +1 } }),
       &((Keychord){1, {{MODKEY, XK_k}},                                       focusstack,     {.i = -1 } }),
       &((Keychord){1, {{MODKEY, XK_h}},                                       setmfact,       {.f = -0.05} }),
       &((Keychord){1, {{MODKEY, XK_l}},                                       setmfact,       {.f = +0.05} }),
       &((Keychord){1, {{MODKEY, XK_equal}},                                   setmfact,       {.f = -1.0} }),
       &((Keychord){1, {{MODKEY, XK_comma}},                                   focusmon,       {.i = -1 } }),
       &((Keychord){1, {{MODKEY, XK_period}},                                  focusmon,       {.i = +1 } }),
       &((Keychord){1, {{MODKEY|ShiftMask, XK_comma}},                         tagmon,         {.i = -1 } }),
       &((Keychord){1, {{MODKEY|ShiftMask, XK_period}},                        tagmon,         {.i = +1 } }),
	   TAGKEYS(                        XK_1,                      0)
	   TAGKEYS(                        XK_2,                      1)
	   TAGKEYS(                        XK_3,                      2)
	   TAGKEYS(                        XK_4,                      3)
	   TAGKEYS(                        XK_5,                      4)
	   TAGKEYS(                        XK_6,                      5)
	   TAGKEYS(                        XK_7,                      6)
	   TAGKEYS(                        XK_8,                      7)
	   TAGKEYS(                        XK_9,                      8)
 };


/* button definitions */
/* click can be ClkTagBar, ClkLtSymbol, ClkStatusText, ClkWinTitle, ClkClientWin, or ClkRootWin */
static const Button buttons[] = {
	/* click                event mask      button          function        argument */
	{ ClkLtSymbol,          0,              Button1,        setlayout,      {0} },
	{ ClkLtSymbol,          0,              Button3,        setlayout,      {.v = &layouts[2]} },
	{ ClkWinTitle,          0,              Button2,        zoom,           {0} },
	{ ClkStatusText,        0,              Button2,        spawn,          {.v = termcmd } },
	{ ClkClientWin,         MODKEY,         Button1,        movemouse,      {0} },
	{ ClkClientWin,         MODKEY,         Button2,        togglefloating, {0} },
	{ ClkClientWin,         MODKEY,         Button3,        resizemouse,    {0} },
	{ ClkTagBar,            0,              Button1,        view,           {0} },
	{ ClkTagBar,            0,              Button3,        toggleview,     {0} },
	{ ClkTagBar,            MODKEY,         Button1,        tag,            {0} },
	{ ClkTagBar,            MODKEY,         Button3,        toggletag,      {0} },
};
