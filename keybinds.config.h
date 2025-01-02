#include <X11/XF86keysym.h>

/* key definitions */
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
static const char *termcmd[]  = { "st", NULL };
static const char *dmenucmd[] = { "dmenu_run", NULL };
static const char *emacscmd[] = { "emacsclient", "-c", NULL };
static const char *browsercmd[] = { "zen", NULL };
static const char *filemanagercmd[] = { "thunar", NULL };
static const char *musiccmd[] = { "spotube", NULL };
static const char *brightnessupcmd[] = { "brightnessctl", "set", "+5", NULL };
static const char *brightnessdowncmd[] = { "brightnessctl", "set", "-5", NULL };
static const char *volumeupcmd[] = { "pamixer", "-i", "10", NULL };
static const char *volumedowncmd[] = { "pamixer", "-d", "10", NULL };
static const char *mutecmd[] = { "pamixer", "-t", NULL };
static const char *screenshotcmd[] = { "flameshot", "gui", NULL };
static const char *xkillcmd[] = { "xkill", NULL };
static const char *powermenucmd[] = { "~/.local/share/scripts/power-menu.sh", NULL };
static const char *emojicmd[] = { "~/.local/share/scripts/emojis.sh", NULL };
static const char *mathcmd[] = { "~/.local/share/scripts/math-symbols.sh", NULL };
static const char *clipboardcmd[] = { "~/.local/share/scripts/clipboard.sh", NULL };
static const char *wallpapercmd[] = { "~/.local/share/scripts/wallpaper.sh", NULL };

static Keychord *keychords[] = {
       /* Keys        function        argument */
       &((Keychord){1, {{MODKEY, XK_Return}},                                  spawn,          {.v = termcmd } }),

       /* Applications */
       &((Keychord){2, {{MODKEY, XK_x}, {NULL, XK_Return}},                    spawn,          {.v = emacscmd } }),
       &((Keychord){2, {{MODKEY, XK_x}, {NULL, XK_b}},                         spawn,          {.v = browsercmd } }),
       &((Keychord){2, {{MODKEY, XK_x}, {NULL, XK_f}},                         spawn,          {.v = filemanagercmd } }),
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
       &((Keychord){1, {{MODKEY|ShiftMask, XK_q}},                             spawn,          {.v = powermenucmd} }),
       &((Keychord){1, {{MODKEY, XK_e}},                                       spawn,          {.v = emojicmd } }),
       &((Keychord){1, {{MODKEY|ShiftMask, XK_e}},                             spawn,          {.v = mathcmd } }),
       &((Keychord){1, {{MODKEY|ShiftMask, XK_v}},                             spawn,          {.v = clipboardcmd} }),
       &((Keychord){1, {{MODKEY, XK_w}},                                       spawn,          {.v = wallpapercmd} }),

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

