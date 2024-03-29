#ifndef OPTIONS_H
#define OPTIONS_H
/* See LICENSE file for copyright and license details. */

/* spmenu options */
static char *class                     = "spmenu"; /* Class for spmenu */
static int protocol                    = 1; /* Protocol to try first (0: X11, 1: Wayland) */
static int fast                        = 0; /* Grab keyboard first */
static int xresources                  = 1; /* Enable .Xresources support */
static int loadconfig                  = 1; /* Load configuration (~/.config/spmenu/spmenu.conf) on runtime */
static int loadtheme                   = 1; /* Load theme (~/.config/spmenu/theme.conf) on runtime */
static int loadbinds                   = 1; /* Load keybind file (~/.config/spmenu/binds.conf) on runtime */
static int mon                         = -1; /* Monitor to run spmenu on */
static int managed                     = 0; /* Let your window manager manage spmenu? */
static int grabkeyboard                = 1; /* Grab keyboard/general input */

/* Wayland options */
static int scrolldistance              = 512; /* Distance to scroll for a scroll action to count */

/* Config file options */
#if CONFIG
static char *configfile                = NULL; /* Config file path. Default is ~/.config/spmenu/spmenu.conf */
static char *themefile                 = NULL; /* Theme file path. Default is ~/.config/spmenu/theme.conf */
static char *bindsfile                 = NULL; /* Keybind file path. Default is ~/.config/spmenu/binds.conf */
#endif

/* Window options */
static int alpha                       = 1; /* Enable alpha */
static int menuposition                = 2; /* Position of the menu (0: Bottom, 1: Top, 2: Center */
static int menupaddingv                = 0; /* Vertical padding inside the menu (px) */
static int menupaddingh                = 0; /* Horizontal padding inside the menu (px) */
static int menuwidth                   = 0; /* spmenu window width */
static int menumarginv                 = 0; /* Vertical padding around the menu */
static int menumarginh                 = 0; /* Horizontal padding around the menu */
static int centerwidth                 = 1000; /* Width when centered */
static double inputwidth               = 0.3; /* Width reserved for input text, (Menu width * input width) */
static int xpos                        = 0; /* X position to offset spmenu */
static int ypos                        = 0; /* Y position to offset spmenu */

/* Powerline options */
static int powerlineprompt             = 1; /* Enable powerline for the prompt */
static int powerlinecount              = 1; /* Enable powerline for the match count */
static int powerlinemode               = 1; /* Enable powerline for the mode indicator */
static int powerlinecaps               = 1; /* Enable powerline for the caps lock indicator */
static int powerlineitems              = 1; /* Enable powerline for the items */
static int promptpwlstyle              = 2; /* Prompt powerline style (0: Arrow, 1: Slash, 2: Rounded) */
static int matchcountpwlstyle          = 2; /* Match count powerline style (0: Arrow, 1: Slash, 2: Rounded) */
static int modepwlstyle                = 2; /* Mode indicator powerline style (0: Arrow, 1: Slash, 2: Rounded) */
static int capspwlstyle                = 2; /* Caps lock indicator powerline style (0: Arrow, 1: Slash, 2: Rounded) */
static int itempwlstyle                = 2; /* Item powerline style (0: Arrow, 1: Slash, 2: Rounded)) */

/* Window properties */
static int dockproperty                = 1; /* Set _NET_WM_WINDOW_TYPE_DOCK */

/* Image options */
static int imagewidth                  = 200; /* Default image width (px) */
static int imageheight                 = 200; /* Default image height (px) */
static int imageresize                 = 1; /* Allow the spmenu window to resize itself to fit the image (0/1) */
static int imagegaps                   = 0; /* Image gaps */
static int imageposition               = 0; /* Image position (0: Top, 1: Bottom, 2: Center, 3: Top center) */
static int imagetype                   = 1; /* Image type (0: Icon, 1: Image) */
static int generatecache               = 1; /* Generate image cache by default */
static int maxcache                    = 512; /* Max image size to cache */
static char *cachedir                  = "default"; /* Cache directory. Default means spmenu will determine automatically */

/* FIFO options */
#if FIFO
static char *fifofile                  = "/tmp/spmenu.fifo"; /* spmenu FIFO path */
#endif

/* Screenshot options */
static char *screenshotfile            = NULL; /* Screenshot file path. If set to NULL, the default path will be used. */
static char *screenshotname            = NULL; /* Screenshot file name. If set to NULL, the default name will be used. */
static char *screenshotdir             = NULL; /* Screenshot file directory. If set to NULL, the default directory will be used. */

/* Mode options */
static int mode                        = 0; /* Mode to start speedwm in (0: Normal mode, 1: Insert mode) */
static int forceinsertmode             = 1; /* Force insert mode, meaning normal mode will be disabled (0/1) */
static char *normtext                  = "Normal"; /* Text to display for normal mode */
static char *instext                   = "Insert"; /* Text to display for insert mode */
static char *regextext                 = "Regex"; /* Text to display for insert mode when regex is enabled */
static char *capslockontext            = "Caps Lock"; /* Text to display for the caps lock indicator when caps lock is on */
static char *capslockofftext           = ""; /* Text to display for the caps lock indicator when caps lock is off */

/* Window border options */
static int borderwidth                 = 0; /* Width of the border */

/* Font options */
static char font[]                     = "Noto Sans Mono 10"; /* Font to draw text and Pango markup with. */
static int textpadding                 = 0; /* Global text padding */
static int normitempadding             = 0; /* Text padding for normal items */
static int selitempadding              = 0; /* Text padding for the selected item */
static int priitempadding              = 0; /* Text padding for the high priority items */

/* Text options */
static char *leftarrow                 = "<"; /* Left arrow, used to indicate you can move to the left */
static char *rightarrow                = ">"; /* Right arrow, used to indicate you can move to the right */
static char *password                  = "*"; /* Password character, when the -P argument is active this will replace all characters typed */
static char *prompt                    = NULL; /* Default prompt */
static char *input                     = NULL; /* Default input text */
static char *pretext                   = NULL; /* Default pretext */

/* Match options */
static int type                        = 1; /* Allow typing into spmenu or only allow keybinds. */
static int passwd                      = 0; /* Replace input with another character and don't read stdin */
static int sortmatches                 = 1; /* Sort matches (0/1) */
static int casesensitive               = 0; /* Case-sensitive by default? (0/1) */
static int mark                        = 1; /* Enable marking items (multi selection) (0/1) */
static int preselected                 = 0; /* Which line should spmenu preselect? */
static int fuzzy                       = 1; /* Whether or not to enable fuzzy matching by default */
static int regex                       = 0; /* Whether or not to enable regex matching by default */
static char *listfile                  = NULL; /* File to read entries from instead of stdin. NULL means read from stdin instead. */

/* Line options */
static int itemposition                = 0; /* Item position (0: Bottom, 1: Top) */
static int lineheight                  = 1; /* Line height (0: Calculate automatically) */
static int lines                       = 0; /* Default number of lines */
static int columns                     = 10; /* Default number of columns */
static int overridelines               = 1; /* Allow overriding lines using keybinds */
static int overridecolumns             = 1; /* Allow overriding columns using keybinds */
static int minlines                    = 0; /* Minimum number of lines allowed */

/* History options */
static char *histfile                  = NULL; /* History file, NULL means no history file */
static int maxhist                     = 64; /* Max number of history entries */
static int histdup                     = 0;	/* If 1, record repeated histories */

/* Prompt options */
static int indentitems                 = 0; /* Indent items to prompt width? (0/1) */

/* Caret options */
static int caretwidth                  = 0; /* Caret width (0: Calculate automatically) */
static int caretheight                 = 0; /* Caret height (0: Calculate automatically) */
static int caretpadding                = 0; /* Caret padding (px) */

/* Hide options */
static int hideinput                   = 0; /* Hide input (0/1) */
static int hidepretext                 = 0; /* Hide pretext (0/1) */
static int hidelarrow                  = 0; /* Hide left arrow (0/1) */
static int hiderarrow                  = 0; /* Hide right arrow (0/1) */
static int hideitem                    = 0; /* Hide item (0/1) */
static int hideprompt                  = 0; /* Hide prompt (0/1) */
static int hidecaps                    = 0; /* Hide caps lock indicator (0/1) */
static int hidepowerline               = 0; /* Hide powerline (0/1) */
static int hidecaret                   = 0; /* Hide caret (0/1) */
static int hidehighlight               = 0; /* Hide highlight (0/1) */
static int hidematchcount              = 0; /* Hide match count (0/1) */
static int hidemode                    = 0; /* Hide mode (0/1) */
static int hideimage                   = 0; /* Hide image (0/1) */

/* Color options
 *
 * Item colors */
static char col_itemnormfg[]           = "#bbbbbb"; /* Normal foreground item color */
static char col_itemnormbg[]           = "#110f1f"; /* Normal background item color */
static char col_itemnormfg2[]          = "#bbbbbb"; /* Normal foreground item colors for the next item */
static char col_itemnormbg2[]          = "#110f1f"; /* Normal background item colors for the next item */
static char col_itemselfg[]            = "#110f1f"; /* Selected foreground item color */
static char col_itemselbg[]            = "#8e93c2"; /* Selected background item color */
static char col_itemmarkedfg[]         = "#110f1f"; /* Marked foreground item color */
static char col_itemmarkedbg[]         = "#8e93c2"; /* Marked background item color */
static char col_itemnormprifg[]        = "#bbbbbb"; /* Normal foreground item (high priority) color */
static char col_itemnormpribg[]        = "#110f1f"; /* Normal background item (high priority) color */
static char col_itemselprifg[]         = "#110f1f"; /* Selected foreground item (high priority) color */
static char col_itemselpribg[]         = "#8e93c2"; /* Selected background item (high priority) color */

/* Input colors */
static char col_inputfg[]              = "#eeeeee"; /* Input field foreground color */
static char col_inputbg[]              = "#110f1f"; /* Input field background color */

/* Pretext colors */
static char col_pretextfg[]            = "#999888"; /* Pretext foreground color */
static char col_pretextbg[]            = "#110f1f"; /* Pretext background color */

/* Menu colors */
static char col_menu[]                 = "#110f1f"; /* Menu background color */

/* Prompt colors */
static char col_promptfg[]             = "#110f1f"; /* Prompt foreground color */
static char col_promptbg[]             = "#c66e5b"; /* Prompt background color */

/* Arrow colors */
static char col_larrowfg[]             = "#bbbbbb"; /* Left arrow color */
static char col_rarrowfg[]             = "#bbbbbb"; /* Right arrow color */
static char col_larrowbg[]             = "#110f1f"; /* Left arrow color */
static char col_rarrowbg[]             = "#110f1f"; /* Right arrow color */

/* Normal highlight colors */
static char col_hlnormfg[]             = "#ffffff"; /* Normal text highlight color */
static char col_hlnormbg[]             = "#000000"; /* Normal background highlight color */

/* Selected highlight colors */
static char col_hlselfg[]              = "#ffffff"; /* Selected text highlight color */
static char col_hlselbg[]              = "#000000"; /* Selected background highlight color */

/* Match count colors */
static char col_numfg[]                = "#110f1f"; /* Match count text color */
static char col_numbg[]                = "#eba62a"; /* Match count background color */

/* Border color */
static char col_border[]               = "#8e93c2"; /* Border color */

/* Caret colors */
static char col_caretfg[]              = "#ffffff"; /* Foreground caret color */
static char col_caretbg[]              = "#110f1f"; /* Background caret color */

/* Mode colors */
static char col_modefg[]               = "#110f1f"; /* Mode text color */
static char col_modebg[]               = "#92c94b"; /* Mode background color */

/* Caps lock colors */
static char col_capsfg[]               = "#110f1f"; /* Caps lock text color */
static char col_capsbg[]               = "#f8d3e6"; /* Caps lock background color */

/* SGR colors */
static char col_sgr0[]                 = "#20201d"; /* SGR color #0 */
static char col_sgr1[]                 = "#d73737"; /* SGR color #1 */
static char col_sgr2[]                 = "#60ac39"; /* SGR color #2 */
static char col_sgr3[]                 = "#cfb017"; /* SGR color #3 */
static char col_sgr4[]                 = "#6684e1"; /* SGR color #4 */
static char col_sgr5[]                 = "#b854d4"; /* SGR color #5 */
static char col_sgr6[]                 = "#1fad83"; /* SGR color #6 */
static char col_sgr7[]                 = "#a6a28c"; /* SGR color #7 */
static char col_sgr8[]                 = "#7d7a68"; /* SGR color #8 */
static char col_sgr9[]                 = "#d73737"; /* SGR color #9 */
static char col_sgr10[]                = "#60ac39"; /* SGR color #10 */
static char col_sgr11[]                = "#cfb017"; /* SGR color #11 */
static char col_sgr12[]                = "#6684e1"; /* SGR color #12 */
static char col_sgr13[]                = "#b854d4"; /* SGR color #13 */
static char col_sgr14[]                = "#1fad83"; /* SGR color #14 */
static char col_sgr15[]                = "#fefbec"; /* SGR color #15 */

/* Alpha options */
static int alpha_itemnormfg            = 255; /* Alpha for normal item foreground (0-255) */
static int alpha_itemnormbg            = 222; /* Alpha for normal item background (0-255) */
static int alpha_itemnormfg2           = 255; /* Alpha for next normal item foreground (0-255) */
static int alpha_itemnormbg2           = 222; /* Alpha for next normal item background (0-255) */
static int alpha_itemselfg             = 255; /* Alpha for selected item foreground (0-255) */
static int alpha_itemselbg             = 222; /* Alpha for selected item background (0-255) */
static int alpha_itemmarkedfg          = 255; /* Alpha for marked item foreground (0-255) */
static int alpha_itemmarkedbg          = 222; /* Alpha for marked item background (0-255) */
static int alpha_itemnormprifg         = 255; /* alpha for normal priority item foreground (0-255) */
static int alpha_itemnormpribg         = 222; /* Alpha for normal priority item background (0-255) */
static int alpha_itemselprifg          = 255; /* Alpha for selected priority item foreground (0-255) */
static int alpha_itemselpribg          = 222; /* Alpha for selected priority item background (0-255) */
static int alpha_inputfg               = 255; /* Alpha for input foreground (0-255) */
static int alpha_inputbg               = 222; /* Alpha for input background (0-255) */
static int alpha_pretextfg             = 255; /* Alpha for pretext foreground (0-255) */
static int alpha_pretextbg             = 222; /* Alpha for pretext background (0-255) */
static int alpha_menu                  = 222; /* Alpha for menu background (0-255) */
static int alpha_promptfg              = 255; /* Alpha for prompt foreground (0-255) */
static int alpha_promptbg              = 222; /* Alpha for prompt background (0-255) */
static int alpha_larrowfg              = 255; /* Alpha for left arrow foreground (0-255) */
static int alpha_larrowbg              = 222; /* Alpha for left arrow background (0-255) */
static int alpha_rarrowfg              = 255; /* Alpha for right arrow foreground (0-255) */
static int alpha_rarrowbg              = 222; /* Alpha for right arrow background (0-255) */
static int alpha_hlnormfg              = 255; /* Alpha for normal highlight foreground (0-255) */
static int alpha_hlnormbg              = 222; /* Alpha for normal highlight background (0-255) */
static int alpha_hlselfg               = 255; /* Alpha for selected highlight foreground (0-255) */
static int alpha_hlselbg               = 222; /* Alpha for selected highlight background (0-255) */
static int alpha_numfg                 = 255; /* Alpha for match count foreground (0-255) */
static int alpha_numbg                 = 222; /* Alpha for the match count background (0-255) */
static int alpha_border                = 255; /* Alpha for the border (0-255) */
static int alpha_caretfg               = 255; /* Alpha for the caret foreground (0-255) */
static int alpha_caretbg               = 222; /* Alpha for the caret background (0-255) */
static int alpha_modefg                = 255; /* Alpha for the mode indicator foreground (0-255) */
static int alpha_modebg                = 222; /* Alpha for the mode indicator background (0-255) */
static int alpha_capsfg                = 255; /* Alpha for the caps lock indicator foreground (0-255) */
static int alpha_capsbg                = 222; /* Alpha for the caps lock indicator background (0-255) */

/* Pango options */
static int pango_item                  = 1; /* Enable support for pango markup for the items */
static int pango_prompt                = 1; /* Enable support for pango markup for the prompt */
static int pango_caps                  = 1; /* Enable support for pango markup for the caps lock indicator */
static int pango_input                 = 1; /* Enable support for pango markup for the user input */
static int pango_pretext               = 1; /* Enable support for pango markup for the pretext */
static int pango_leftarrow             = 0; /* Enable support for pango markup for the left arrow */
static int pango_rightarrow            = 0; /* Enable support for pango markup for the right arrow */
static int pango_numbers               = 0; /* Enable support for pango markup for the match count */
static int pango_mode                  = 0; /* Enable support for pango markup for the mode indicator */
static int pango_password              = 0; /* Enable support for pango markup for the password text */

/* Misc */
static int printindex                  = 0; /* Print index instead of the text itself (0/1) */
static int requirematch                = 0; /* Require input text to match an item (0/1) */
static int incremental                 = 0; /* Print text every time a key is pressed (0/1) */
static int coloritems                  = 1; /* Color items (0/1) */
static int sgr                         = 1; /* Support SGR sequences (0/1) */
static char *worddelimiters            = " /?\"&[]"; /* Word delimiters used for keybinds that change words, Space is default. */
#endif
