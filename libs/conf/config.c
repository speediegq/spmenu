/* See LICENSE file for copyright and license details. */
#include <libconfig.h>
#include "../theme/theme.c"

void conf_init(void) {
    char *xdg_conf;
    char *cfgfile = NULL;
    char *bindfile = NULL;
    char *home = NULL;
    const char *dest;

    // get path for configuration file
    if (!configfile) {
        if (!(xdg_conf = getenv("XDG_CONFIG_HOME"))) {
            // ~/.config/spmenu/spmenu.conf
            home = getenv("HOME");

            // malloc
            if (!(cfgfile = malloc(snprintf(NULL, 0, "%s/%s", home, ".config/spmenu/spmenu.conf") + 1))) {
                die("spmenu: failed to malloc cfgfile");
            }

            sprintf(cfgfile, "%s/%s", home, ".config/spmenu/spmenu.conf");
        } else {
            // malloc
            if (!(cfgfile = malloc(snprintf(NULL, 0, "%s/%s", xdg_conf, "spmenu/spmenu.conf") + 1))) {
                die("spmenu: failed to malloc cfgfile");
            }

            // XDG_CONFIG_HOME is set, so let's use that instead
            sprintf(cfgfile, "%s/%s", xdg_conf, "spmenu/spmenu.conf");
        }
    } else { // custom config path
        if (!(cfgfile = malloc(snprintf(NULL, 0, "%s", configfile) + 1))) {
            die("spmenu: failed to malloc cfgfile");
        }

        sprintf(cfgfile, "%s", configfile);
    }

    // don't bother trying to load if it doesn't exist.
    if (access(cfgfile, F_OK) != 0) {
        loadconfig = 0;
    }

    // init config
    config_t cfg;
    config_init(&cfg);

    // attempt to read config file to cfg
    if (loadconfig) {
        if (!config_read_file(&cfg, cfgfile)) {
            // invalid configuration, but let's try to read it anyway
            fprintf(stderr, "spmenu: Invalid configuration.\n");
        }
    }

    // load options spmenu.window
    config_setting_t *window_setting = config_lookup(&cfg, "spmenu.window");
    if (window_setting != NULL && loadconfig) {
        // look up window entries
        for (unsigned int i = 0; i < config_setting_length(window_setting); ++i) {
            config_setting_t *conf = config_setting_get_elem(window_setting, i);

            config_setting_lookup_int(conf, "position", &menuposition); // spmenu.window.menuposition
            config_setting_lookup_int(conf, "paddingv", &menupaddingv); // spmenu.window.paddingv
            config_setting_lookup_int(conf, "paddingh", &menupaddingh); // spmenu.window.paddingh
            config_setting_lookup_int(conf, "padding-vertical", &menupaddingv); // spmenu.window.padding-vertical
            config_setting_lookup_int(conf, "padding-horizontal", &menupaddingh); // spmenu.window.padding-horizontal
            config_setting_lookup_int(conf, "margin-vertical", &menumarginv); // spmenu.window.margin-vertical
            config_setting_lookup_int(conf, "margin-horizontal", &menumarginh); // spmenu.window.margin-horizontal
            config_setting_lookup_int(conf, "x", &xpos); // spmenu.window.x
            config_setting_lookup_int(conf, "y", &xpos); // spmenu.window.y
            config_setting_lookup_int(conf, "width", &menuwidth); // spmenu.window.width
            config_setting_lookup_int(conf, "border", &borderwidth); // spmenu.window.border
            config_setting_lookup_int(conf, "managed", &managed); // spmenu.window.managed
            config_setting_lookup_int(conf, "monitor", &mon); // spmenu.window.monitor
            config_setting_lookup_int(conf, "alpha", &alpha); // spmenu.window.alpha

            if (!protocol_override) {
                config_setting_lookup_int(conf, "protocol", &protocol); // spmenu.window.protocol
            }
        }
    }

    // load options spmenu.properties
    config_setting_t *prop_setting = config_lookup(&cfg, "spmenu.properties");
    if (prop_setting != NULL && loadconfig) {
        for (unsigned int i = 0; i < config_setting_length(prop_setting); ++i) {
            config_setting_t *conf = config_setting_get_elem(prop_setting, i);

            // look up
            config_setting_lookup_string(conf, "class", &dest); // spmenu.properties.class
            class = strdup(dest);

            config_setting_lookup_int(conf, "dock", &dockproperty); // spmenu.properties.dock
        }
    }

    // load options spmenu.powerline
    config_setting_t *pwl_setting = config_lookup(&cfg, "spmenu.powerline");
    if (pwl_setting != NULL && loadconfig) {
        for (unsigned int i = 0; i < config_setting_length(pwl_setting); ++i) {
            config_setting_t *conf = config_setting_get_elem(pwl_setting, i);

            // look up
            config_setting_lookup_int(conf, "promptstyle", &promptpwlstyle); // spmenu.powerline.promptstyle
            config_setting_lookup_int(conf, "matchcountstyle", &matchcountpwlstyle); // spmenu.powerline.matchcountstyle
            config_setting_lookup_int(conf, "modestyle", &modepwlstyle); // spmenu.powerline.modestyle
            config_setting_lookup_int(conf, "capsstyle", &capspwlstyle); // spmenu.powerline.capsstyle
            config_setting_lookup_int(conf, "prompt", &powerlineprompt); // spmenu.powerline.prompt
            config_setting_lookup_int(conf, "matchcount", &powerlinecount); // spmenu.powerline.matchcount
            config_setting_lookup_int(conf, "mode", &powerlinemode); // spmenu.powerline.mode
            config_setting_lookup_int(conf, "caps", &powerlinecaps); // spmenu.powerline.caps
        }
    }

    // load options spmenu.center
    config_setting_t *center_setting = config_lookup(&cfg, "spmenu.center");
    if (center_setting != NULL && loadconfig) {
        for (unsigned int i = 0; i < config_setting_length(center_setting); ++i) {
            config_setting_t *conf = config_setting_get_elem(center_setting, i);

            config_setting_lookup_int(conf, "width", &minwidth); // spmenu.center.width
        }
    }

    // load options spmenu.text
    config_setting_t *text_setting = config_lookup(&cfg, "spmenu.text");
    if (text_setting != NULL && loadconfig) {
        for (unsigned int i = 0; i < config_setting_length(text_setting); ++i) {
            config_setting_t *conf = config_setting_get_elem(text_setting, i);

            // look up
            if (config_setting_lookup_string(conf, "font", &dest)) // spmenu.text.font
                sp_strncpy(font, strdup(dest), sizeof(font));

            config_setting_lookup_int(conf, "padding", &textpadding); // spmenu.text.padding
            config_setting_lookup_int(conf, "normitempadding", &normitempadding); // spmenu.text.normitempadding
            config_setting_lookup_int(conf, "selitempadding", &selitempadding); // spmenu.text.selitempadding
            config_setting_lookup_int(conf, "priitempadding", &priitempadding); // spmenu.text.priitempadding

            if (config_setting_lookup_string(conf, "leftarrow", &dest)) // spmenu.text.leftarrow
                leftarrow = strdup(dest);

            if (config_setting_lookup_string(conf, "rightarrow", &dest)) // spmenu.text.rightarrow
                rightarrow = strdup(dest);

            if (config_setting_lookup_string(conf, "password", &dest))
                password = strdup(dest);

            if (config_setting_lookup_string(conf, "prompt", &dest))
                prompt = strdup(dest);

            if (config_setting_lookup_string(conf, "capslockon", &dest))
                capslockontext = strdup(dest);

            if (config_setting_lookup_string(conf, "capslockoff", &dest))
                capslockofftext = strdup(dest);

            if (config_setting_lookup_string(conf, "normal", &dest))
                normtext = strdup(dest);

            if (config_setting_lookup_string(conf, "insert", &dest))
                instext = strdup(dest);

            if (config_setting_lookup_string(conf, "input", &dest))
                input = strdup(dest);
        }
    }

    // load options spmenu.alpha
    config_setting_t *alpha_setting = config_lookup(&cfg, "spmenu.alpha");
    if (alpha_setting != NULL && loadconfig) {
        for (unsigned int i = 0; i < config_setting_length(alpha_setting); ++i) {
            config_setting_t *conf = config_setting_get_elem(alpha_setting, i);

            // look up
            config_setting_lookup_int(conf, "itemnormfg", &alpha_itemnormfg); // spmenu.alpha.itemnormfg
            config_setting_lookup_int(conf, "itemnormbg", &alpha_itemnormbg); // spmenu.alpha.itemnormbg

            if (!config_setting_lookup_int(conf, "itemnormfg2", &alpha_itemnormfg2)) // spmenu.alpha.itemnormfg2
                config_setting_lookup_int(conf, "itemnormfg", &alpha_itemnormfg2);

            if (!config_setting_lookup_int(conf, "itemnormbg2", &alpha_itemnormbg2)) // spmenu.alpha.itemnormbg2
                config_setting_lookup_int(conf, "itemnormbg", &alpha_itemnormbg2);

            config_setting_lookup_int(conf, "itemselfg", &alpha_itemselfg); // spmenu.alpha.itemselfg
            config_setting_lookup_int(conf, "itemselbg", &alpha_itemselbg); // spmenu.alpha.itemselbg

            if (!config_setting_lookup_int(conf, "itemmarkedfg", &alpha_itemmarkedfg))
                config_setting_lookup_int(conf, "itemselfg", &alpha_itemmarkedfg);

            if (!config_setting_lookup_int(conf, "itemmarkedbg", &alpha_itemmarkedbg))
                config_setting_lookup_int(conf, "itemselbg", &alpha_itemmarkedbg);

            config_setting_lookup_int(conf, "itemnormprifg", &alpha_itemnormprifg); // spmenu.alpha.itemnormprifg
            config_setting_lookup_int(conf, "itemnormpribg", &alpha_itemnormpribg); // spmenu.alpha.itemnormpribg
            config_setting_lookup_int(conf, "itemselprifg", &alpha_itemselprifg); // spmenu.alpha.itemselprifg
            config_setting_lookup_int(conf, "itemselpribg", &alpha_itemselpribg); // spmenu.alpha.itemselpribg

            config_setting_lookup_int(conf, "inputfg", &alpha_inputfg); // spmenu.alpha.inputfg
            config_setting_lookup_int(conf, "inputbg", &alpha_inputbg); // spmenu.alpha.inputbg

            config_setting_lookup_int(conf, "menu", &alpha_menu); // spmenu.alpha.menu

            config_setting_lookup_int(conf, "promptfg", &alpha_promptfg); // spmenu.alpha.promptfg
            config_setting_lookup_int(conf, "promptbg", &alpha_promptbg); // spmenu.alpha.promptbg

            config_setting_lookup_int(conf, "larrowfg", &alpha_larrowfg); // spmenu.alpha.larrowfg
            config_setting_lookup_int(conf, "larrowbg", &alpha_larrowbg); // spmenu.alpha.larrowbg
            config_setting_lookup_int(conf, "rarrowfg", &alpha_rarrowfg); // spmenu.alpha.rarrowfg
            config_setting_lookup_int(conf, "rarrowbg", &alpha_rarrowbg); // spmenu.alpha.rarrowbg

            config_setting_lookup_int(conf, "hlnormfg", &alpha_hlnormfg); // spmenu.alpha.hlnormfg
            config_setting_lookup_int(conf, "hlnormbg", &alpha_hlnormbg); // spmenu.alpha.hlnormbg
            config_setting_lookup_int(conf, "hlselfg", &alpha_hlselfg); // spmenu.alpha.hlselfg
            config_setting_lookup_int(conf, "hlselbg", &alpha_hlselbg); // spmenu.alpha.hlselbg

            config_setting_lookup_int(conf, "numfg", &alpha_numfg); // spmenu.alpha.numfg
            config_setting_lookup_int(conf, "numbg", &alpha_numbg); // spmenu.alpha.numbg

            config_setting_lookup_int(conf, "border", &alpha_border); // spmenu.alpha.border

            config_setting_lookup_int(conf, "caretfg", &alpha_caretfg); // spmenu.alpha.caretfg
            config_setting_lookup_int(conf, "caretbg", &alpha_caretbg); // spmenu.alpha.caretbg

            config_setting_lookup_int(conf, "modefg", &alpha_modefg); // spmenu.alpha.modefg
            config_setting_lookup_int(conf, "modebg", &alpha_modebg); // spmenu.alpha.modebg

            config_setting_lookup_int(conf, "capsfg", &alpha_capsfg); // spmenu.alpha.capsfg
            config_setting_lookup_int(conf, "capsbg", &alpha_capsbg); // spmenu.alpha.capsbg
        }
    }

    // load options spmenu.color
    config_setting_t *color_setting = config_lookup(&cfg, "spmenu.color");
    if (color_setting != NULL && loadconfig) {
        for (unsigned int i = 0; i < config_setting_length(color_setting); ++i) {
            config_setting_t *conf = config_setting_get_elem(color_setting, i);

            // items
            if (config_setting_lookup_string(conf, "itemnormfg", &dest))
                sp_strncpy(col_itemnormfg, strdup(dest), sizeof(col_itemnormfg));

            if (config_setting_lookup_string(conf, "itemnormbg", &dest))
                sp_strncpy(col_itemnormbg, strdup(dest), sizeof(col_itemnormbg));

            if (config_setting_lookup_string(conf, "itemnormfg2", &dest))
                sp_strncpy(col_itemnormfg2, strdup(dest), sizeof(col_itemnormfg2));
            else if (config_setting_lookup_string(conf, "itemnormfg", &dest))
                sp_strncpy(col_itemnormfg2, strdup(dest), sizeof(col_itemnormfg2));

            if (config_setting_lookup_string(conf, "itemnormbg2", &dest))
                sp_strncpy(col_itemnormbg2, strdup(dest), sizeof(col_itemnormbg2));
            else if (config_setting_lookup_string(conf, "itemnormbg", &dest))
                sp_strncpy(col_itemnormbg2, strdup(dest), sizeof(col_itemnormbg2));

            if (config_setting_lookup_string(conf, "itemselfg", &dest))
                sp_strncpy(col_itemselfg, strdup(dest), sizeof(col_itemselfg));

            if (config_setting_lookup_string(conf, "itemselbg", &dest))
                sp_strncpy(col_itemselbg, strdup(dest), sizeof(col_itemselbg));

            if (config_setting_lookup_string(conf, "itemmarkedfg", &dest))
                sp_strncpy(col_itemmarkedfg, strdup(dest), sizeof(col_itemmarkedfg));
            else if (config_setting_lookup_string(conf, "itemselfg", &dest))
                sp_strncpy(col_itemmarkedfg, strdup(dest), sizeof(col_itemmarkedfg));

            if (config_setting_lookup_string(conf, "itemmarkedbg", &dest))
                sp_strncpy(col_itemmarkedbg, strdup(dest), sizeof(col_itemmarkedbg));
            else if (config_setting_lookup_string(conf, "itemselbg", &dest))
                sp_strncpy(col_itemmarkedbg, strdup(dest), sizeof(col_itemmarkedbg));

            // items with priority
            if (config_setting_lookup_string(conf, "itemnormprifg", &dest))
                sp_strncpy(col_itemnormprifg, strdup(dest), sizeof(col_itemnormprifg));

            if (config_setting_lookup_string(conf, "itemnormpribg", &dest))
                sp_strncpy(col_itemnormpribg, strdup(dest), sizeof(col_itemnormpribg));

            if (config_setting_lookup_string(conf, "itemselprifg", &dest))
                sp_strncpy(col_itemselprifg, strdup(dest), sizeof(col_itemselprifg));

            if (config_setting_lookup_string(conf, "itemselpribg", &dest))
                sp_strncpy(col_itemselpribg, strdup(dest), sizeof(col_itemselpribg));

            // input
            if (config_setting_lookup_string(conf, "inputfg", &dest))
                sp_strncpy(col_inputfg, strdup(dest), sizeof(col_inputfg));

            if (config_setting_lookup_string(conf, "inputbg", &dest))
                sp_strncpy(col_inputbg, strdup(dest), sizeof(col_inputbg));

            // menu
            if (config_setting_lookup_string(conf, "menu", &dest))
                sp_strncpy(col_menu, strdup(dest), sizeof(col_menu));

            // prompt
            if (config_setting_lookup_string(conf, "promptfg", &dest))
                sp_strncpy(col_promptfg, strdup(dest), sizeof(col_promptfg));

            if (config_setting_lookup_string(conf, "promptbg", &dest))
                sp_strncpy(col_promptbg, strdup(dest), sizeof(col_promptbg));

            // arrows
            if (config_setting_lookup_string(conf, "larrowfg", &dest))
                sp_strncpy(col_larrowfg, strdup(dest), sizeof(col_larrowfg));

            if (config_setting_lookup_string(conf, "larrowbg", &dest))
                sp_strncpy(col_larrowbg, strdup(dest), sizeof(col_larrowbg));

            if (config_setting_lookup_string(conf, "rarrowfg", &dest))
                sp_strncpy(col_rarrowfg, strdup(dest), sizeof(col_rarrowfg));

            if (config_setting_lookup_string(conf, "rarrowbg", &dest))
                sp_strncpy(col_rarrowbg, strdup(dest), sizeof(col_rarrowbg));

            // highlight
            if (config_setting_lookup_string(conf, "hlnormfg", &dest))
                sp_strncpy(col_hlnormfg, strdup(dest), sizeof(col_hlnormfg));

            if (config_setting_lookup_string(conf, "hlnormbg", &dest))
                sp_strncpy(col_hlnormbg, strdup(dest), sizeof(col_hlnormbg));

            if (config_setting_lookup_string(conf, "hlselfg", &dest))
                sp_strncpy(col_hlselfg, strdup(dest), sizeof(col_hlselfg));

            if (config_setting_lookup_string(conf, "hlselbg", &dest))
                sp_strncpy(col_hlselbg, strdup(dest), sizeof(col_hlselbg));

            // number
            if (config_setting_lookup_string(conf, "numfg", &dest))
                sp_strncpy(col_numfg, strdup(dest), sizeof(col_numfg));

            if (config_setting_lookup_string(conf, "numbg", &dest))
                sp_strncpy(col_numbg, strdup(dest), sizeof(col_numbg));

            // mode
            if (config_setting_lookup_string(conf, "modefg", &dest))
                sp_strncpy(col_modefg, strdup(dest), sizeof(col_modefg));

            if (config_setting_lookup_string(conf, "modebg", &dest))
                sp_strncpy(col_modebg, strdup(dest), sizeof(col_modebg));

            // caps
            if (config_setting_lookup_string(conf, "capsfg", &dest))
                sp_strncpy(col_capsfg, strdup(dest), sizeof(col_capsfg));

            if (config_setting_lookup_string(conf, "capsbg", &dest))
                sp_strncpy(col_capsbg, strdup(dest), sizeof(col_capsbg));

            // border
            if (config_setting_lookup_string(conf, "border", &dest))
                sp_strncpy(col_border, strdup(dest), sizeof(col_border));

            // caret
            if (config_setting_lookup_string(conf, "caretfg", &dest))
                sp_strncpy(col_caretfg, strdup(dest), sizeof(col_caretfg));

            if (config_setting_lookup_string(conf, "caretbg", &dest))
                sp_strncpy(col_caretbg, strdup(dest), sizeof(col_caretbg));

            // sgr colors
            if (config_setting_lookup_string(conf, "sgr0", &dest))
                sp_strncpy(textcolors[0], strdup(dest), sizeof(textcolors[0]));
            if (config_setting_lookup_string(conf, "sgr1", &dest))
                sp_strncpy(textcolors[1], strdup(dest), sizeof(textcolors[1]));
            if (config_setting_lookup_string(conf, "sgr2", &dest))
                sp_strncpy(textcolors[2], strdup(dest), sizeof(textcolors[2]));
            if (config_setting_lookup_string(conf, "sgr3", &dest))
                sp_strncpy(textcolors[3], strdup(dest), sizeof(textcolors[3]));
            if (config_setting_lookup_string(conf, "sgr4", &dest))
                sp_strncpy(textcolors[4], strdup(dest), sizeof(textcolors[4]));
            if (config_setting_lookup_string(conf, "sgr5", &dest))
                sp_strncpy(textcolors[5], strdup(dest), sizeof(textcolors[5]));
            if (config_setting_lookup_string(conf, "sgr6", &dest))
                sp_strncpy(textcolors[6], strdup(dest), sizeof(textcolors[6]));
            if (config_setting_lookup_string(conf, "sgr7", &dest))
                sp_strncpy(textcolors[7], strdup(dest), sizeof(textcolors[7]));
            if (config_setting_lookup_string(conf, "sgr8", &dest))
                sp_strncpy(textcolors[8], strdup(dest), sizeof(textcolors[8]));
            if (config_setting_lookup_string(conf, "sgr9", &dest))
                sp_strncpy(textcolors[9], strdup(dest), sizeof(textcolors[9]));
            if (config_setting_lookup_string(conf, "sgr10", &dest))
                sp_strncpy(textcolors[10], strdup(dest), sizeof(textcolors[10]));
            if (config_setting_lookup_string(conf, "sgr11", &dest))
                sp_strncpy(textcolors[11], strdup(dest), sizeof(textcolors[11]));
            if (config_setting_lookup_string(conf, "sgr12", &dest))
                sp_strncpy(textcolors[12], strdup(dest), sizeof(textcolors[12]));
            if (config_setting_lookup_string(conf, "sgr13", &dest))
                sp_strncpy(textcolors[13], strdup(dest), sizeof(textcolors[13]));
            if (config_setting_lookup_string(conf, "sgr14", &dest))
                sp_strncpy(textcolors[14], strdup(dest), sizeof(textcolors[14]));
            if (config_setting_lookup_string(conf, "sgr15", &dest))
                sp_strncpy(textcolors[15], strdup(dest), sizeof(textcolors[15]));

            // coloritems int
            config_setting_lookup_int(conf, "coloritems", &coloritems);
            config_setting_lookup_int(conf, "sgr", &sgr);
        }
    }

    // load options spmenu.image
    config_setting_t *img_setting = config_lookup(&cfg, "spmenu.image");
    if (img_setting != NULL && loadconfig) {
        for (unsigned int i = 0; i < config_setting_length(img_setting); ++i) {
            config_setting_t *conf = config_setting_get_elem(img_setting, i);

            // look up
            config_setting_lookup_int(conf, "width", &imagewidth); // spmenu.image.width
            config_setting_lookup_int(conf, "height", &imageheight); // spmenu.image.height
            config_setting_lookup_int(conf, "resize", &imageresize); // spmenu.image.resize
            config_setting_lookup_int(conf, "gaps", &imagegaps); // spmenu.image.gaps
            config_setting_lookup_int(conf, "position", &imageposition); // spmenu.image.position
            config_setting_lookup_int(conf, "cache", &generatecache); // spmenu.image.cache
            config_setting_lookup_int(conf, "maxcache", &maxcache); // spmenu.image.maxcache
            if (config_setting_lookup_string(conf, "cachedir", &dest)) // spmenu.image.cachedir
                cachedir = strdup(dest);
        }
    }

    // load options spmenu.file
    config_setting_t *file_setting = config_lookup(&cfg, "spmenu.file");
    if (file_setting != NULL && loadconfig) {
        for (unsigned int i = 0; i < config_setting_length(file_setting); ++i) {
            config_setting_t *conf = config_setting_get_elem(file_setting, i);

            // look up
            config_setting_lookup_int(conf, "theme", &loadtheme); // spmenu.file.theme
            config_setting_lookup_int(conf, "binds", &loadbinds); // spmenu.file.binds
            config_setting_lookup_int(conf, "global", &globalcolors); // spmenu.file.global
            config_setting_lookup_int(conf, "xresources", &xresources); // spmenu.file.xresources

            if (config_setting_lookup_string(conf, "themefile", &dest)) {
                themefile = strdup(dest);
            }

            if (config_setting_lookup_string(conf, "bindsfile", &dest)) {
                bindsfile = strdup(dest);
            }
        }
    }

    // load options spmenu.input
    config_setting_t *input_setting = config_lookup(&cfg, "spmenu.input");
    if (input_setting != NULL && loadconfig) {
        for (unsigned int i = 0; i < config_setting_length(input_setting); ++i) {
            config_setting_t *conf = config_setting_get_elem(input_setting, i);

            // look up
            config_setting_lookup_int(conf, "fast", &fast); // spmenu.input.fast
            config_setting_lookup_int(conf, "type", &type); // spmenu.input.type
            config_setting_lookup_int(conf, "password", &passwd); // spmenu.input.password
        }
    }

    // load options spmenu.caret
    config_setting_t *caret_setting = config_lookup(&cfg, "spmenu.caret");
    if (caret_setting != NULL && loadconfig) {
        for (unsigned int i = 0; i < config_setting_length(caret_setting); ++i) {
            config_setting_t *conf = config_setting_get_elem(caret_setting, i);

            // look up
            config_setting_lookup_int(conf, "width", &caretwidth); // spmenu.caret.width
            config_setting_lookup_int(conf, "height", &caretheight); // spmenu.caret.height
            config_setting_lookup_int(conf, "padding", &caretpadding); // spmenu.caret.padding
        }
    }

    // load options spmenu.output
    config_setting_t *output_setting = config_lookup(&cfg, "spmenu.output");
    if (output_setting != NULL && loadconfig) {
        for (unsigned int i = 0; i < config_setting_length(output_setting); ++i) {
            config_setting_t *conf = config_setting_get_elem(output_setting, i);

            // look up
            config_setting_lookup_int(conf, "printindex", &printindex); // spmenu.output.printindex
            config_setting_lookup_int(conf, "incremental", &incremental); // spmenu.output.incremental
        }
    }

    // load options spmenu.mode
    config_setting_t *mode_setting = config_lookup(&cfg, "spmenu.mode");
    if (mode_setting != NULL && loadconfig) {
        for (unsigned int i = 0; i < config_setting_length(mode_setting); ++i) {
            config_setting_t *conf = config_setting_get_elem(mode_setting, i);

            // look up
            config_setting_lookup_int(conf, "default", &mode); // spmenu.mode.default
        }
    }

    // load options spmenu.match
    config_setting_t *match_setting = config_lookup(&cfg, "spmenu.match");
    if (match_setting != NULL && loadconfig) {
        for (unsigned int i = 0; i < config_setting_length(match_setting); ++i) {
            config_setting_t *conf = config_setting_get_elem(match_setting, i);

            // look up
            config_setting_lookup_int(conf, "sort", &sortmatches); // spmenu.match.sort
            config_setting_lookup_int(conf, "casesensitive", &casesensitive); // spmenu.match.casesensitive
            config_setting_lookup_int(conf, "fuzzy", &fuzzy); // spmenu.match.fuzzy
            config_setting_lookup_int(conf, "preselected", &preselected); // spmenu.match.preselected
            config_setting_lookup_int(conf, "mark", &mark); // spmenu.match.mark
            config_setting_lookup_string(conf, "delimiters", &dest); // spmenu.match.delimiters
            worddelimiters = strdup(dest);
            if (config_setting_lookup_string(conf, "listfile", &dest)) // spmenu.match.listfile
                if (dest && strcmp(strdup(dest), "NULL"))
                    listfile = strdup(dest);
        }
    }

    // load options spmenu.line
    config_setting_t *line_setting = config_lookup(&cfg, "spmenu.line");
    if (line_setting != NULL && loadconfig) {
        for (unsigned int i = 0; i < config_setting_length(line_setting); ++i) {
            config_setting_t *conf = config_setting_get_elem(line_setting, i);

            // look up
            config_setting_lookup_int(conf, "height", &lineheight); // spmenu.line.height
            config_setting_lookup_int(conf, "lines", &lines); // spmenu.line.lines
            config_setting_lookup_int(conf, "columns", &columns); // spmenu.line.columns
            config_setting_lookup_int(conf, "indentitems", &indentitems); // spmenu.line.indentitems
        }
    }

    // load options spmenu.history
    config_setting_t *hist_setting = config_lookup(&cfg, "spmenu.history");
    if (hist_setting != NULL && loadconfig) {
        for (unsigned int i = 0; i < config_setting_length(hist_setting); ++i) {
            config_setting_t *conf = config_setting_get_elem(hist_setting, i);

            // look up
            config_setting_lookup_int(conf, "max", &maxhist); // spmenu.history.max
            config_setting_lookup_int(conf, "duplicate", &histdup); // spmenu.history.duplicate
        }
    }

    // load options spmenu.hide
    config_setting_t *hide_setting = config_lookup(&cfg, "spmenu.hide");
    if (hide_setting != NULL && loadconfig) {
        for (unsigned int i = 0; i < config_setting_length(hide_setting); ++i) {
            config_setting_t *conf = config_setting_get_elem(hide_setting, i);

            // look up
            config_setting_lookup_int(conf, "input", &hideinput); // spmenu.hide.input
            config_setting_lookup_int(conf, "larrow", &hidelarrow); // spmenu.hide.larrow
            config_setting_lookup_int(conf, "rarrow", &hiderarrow); // spmenu.hide.rarrow
            config_setting_lookup_int(conf, "prompt", &hideprompt); // spmenu.hide.prompt
            config_setting_lookup_int(conf, "items",  &hideitem); // spmenu.hide.items
            config_setting_lookup_int(conf, "powerline", &hidepowerline); // spmenu.hide.powerline
            config_setting_lookup_int(conf, "caret", &hidecaret); // spmenu.hide.caret
            config_setting_lookup_int(conf, "highlight", &hidehighlight); // spmenu.hide.highlight
            config_setting_lookup_int(conf, "matchcount", &hidematchcount); // spmenu.hide.matchcount
            config_setting_lookup_int(conf, "mode", &hidemode); // spmenu.hide.mode
            config_setting_lookup_int(conf, "caps", &hidecaps); // spmenu.hide.caps
            config_setting_lookup_int(conf, "hideimage", &hideimage); // spmenu.hide.image
        }
    }

    // load options spmenu.pango
    config_setting_t *pango_setting = config_lookup(&cfg, "spmenu.pango");
    if (pango_setting != NULL && loadconfig) {
        for (unsigned int i = 0; i < config_setting_length(pango_setting); ++i) {
            config_setting_t *conf = config_setting_get_elem(pango_setting, i);

            // look up
            config_setting_lookup_int(conf, "item", &pango_item); // spmenu.pango.input
            config_setting_lookup_int(conf, "highlight", &pango_highlight); // spmenu.pango.highlight
            config_setting_lookup_int(conf, "prompt", &pango_prompt); // spmenu.pango.rarrow
            config_setting_lookup_int(conf, "input", &pango_input); // spmenu.pango.input
            config_setting_lookup_int(conf, "leftarrow", &pango_leftarrow); // spmenu.pango.leftarrow
            config_setting_lookup_int(conf, "rightarrow", &pango_rightarrow); // spmenu.pango.rightarrow
            config_setting_lookup_int(conf, "numbers", &pango_numbers); // spmenu.pango.numbers
            config_setting_lookup_int(conf, "mode", &pango_mode); // spmenu.pango.mode
            config_setting_lookup_int(conf, "caps", &pango_caps); // spmenu.pango.caps
            config_setting_lookup_int(conf, "password", &pango_password); // spmenu.pango.mode
        }
    }

    // load options spmenu.keys
    config_setting_t *key_setting = config_lookup(&cfg, "spmenu.keys");
    if (key_setting != NULL && loadconfig) {
        int nmode = 0;

        for (unsigned int i = 0; i < config_setting_length(key_setting); ++i) {
            config_setting_t *conf = config_setting_get_elem(key_setting, i);

            // look up
            config_setting_lookup_string(conf, "modifier", &dest);

#if USEX
            for (int j = 0; j < LENGTH(ml); j++) {
                if (!strcmp(ml[j].mod, strdup(dest))) {
                    ckeys[i].mod = ml[j].modifier;
                }
            }
#endif

#if USEWAYLAND
            for (int j = 0; j < LENGTH(wml); j++) {
                if (!strcmp(wml[j].mod, strdup(dest))) {
                    wl_ckeys[i].modifier = wml[j].modifier;
                }
            }
#endif

            if (config_setting_lookup_int(conf, "mode", &nmode)) {
#if USEX
                ckeys[i].mode = nmode;
#endif
#if USEWAYLAND
                wl_ckeys[i].mode = nmode;
#endif
            }

            config_setting_lookup_string(conf, "key", &dest);

#if USEX
            for (int j = 0; j < LENGTH(kl); j++) {
                if (!strcmp(kl[j].key, strdup(dest))) {
                    ckeys[i].keysym = kl[j].keysym;
                }
            }
#endif

#if USEWAYLAND
            for (int j = 0; j < LENGTH(wkl); j++) {
                if (!strcmp(wkl[j].key, strdup(dest))) {
                    wl_ckeys[i].keysym = wkl[j].keysym;
                }
            }
#endif

            config_setting_lookup_string(conf, "function", &dest);

            for (int j = 0; j < LENGTH(fl); j++) {
                if (!strcmp(fl[j].function, strdup(dest))) {
#if USEX
                    ckeys[i].func = fl[j].func;
#endif
#if USEWAYLAND
                    wl_ckeys[i].func = fl[j].func;
#endif
                }
            }

            config_setting_lookup_string(conf, "argument", &dest);

            for (int j = 0; j < LENGTH(al); j++) {
                if (!strcmp(al[j].argument, strdup(dest))) {
#if USEX
                    ckeys[i].arg = al[j].arg;
#endif
#if USEWAYLAND
                    wl_ckeys[i].arg = al[j].arg;
#endif
                }
            }

            config_setting_lookup_int(conf, "ignoreglobalkeys", &ignoreglobalkeys);
        }
    }

    // load options spmenu.mouse
    config_setting_t *mouse_setting = config_lookup(&cfg, "spmenu.mouse");
    if (mouse_setting != NULL && loadconfig) {
        for (unsigned int i = 0; i < config_setting_length(mouse_setting); ++i) {
            config_setting_t *conf = config_setting_get_elem(mouse_setting, i);

            config_setting_lookup_string(conf, "click", &dest);

            for (int j = 0; j < LENGTH(ctp); j++) {
                if (!strcmp(ctp[j].tclick, strdup(dest))) {
#if USEX
                    cbuttons[i].click = ctp[j].click;
#endif
#if USEWAYLAND
                    wl_cbuttons[i].click = ctp[j].click;
#endif
                }
            }

            config_setting_lookup_string(conf, "button", &dest);

#if USEX
            for (int j = 0; j < LENGTH(btp); j++) {
                if (!strcmp(btp[j].click, strdup(dest))) {
                    cbuttons[i].button = btp[j].button;
                }
            }
#endif

#if USEWAYLAND
            for (int j = 0; j < LENGTH(w_btp); j++) {
                if (!strcmp(w_btp[j].click, strdup(dest))) {
                    wl_cbuttons[i].button = w_btp[j].button;
                }
            }
#endif

            config_setting_lookup_string(conf, "function", &dest);

            for (int j = 0; j < LENGTH(fl); j++) {
                if (!strcmp(fl[j].function, strdup(dest))) {
#if USEX
                    cbuttons[i].func = fl[j].func;
#endif
#if USEWAYLAND
                    wl_cbuttons[i].func = fl[j].func;
#endif
                }
            }

            config_setting_lookup_string(conf, "argument", &dest);

            for (int j = 0; j < LENGTH(al); j++) {
                if (!strcmp(al[j].argument, strdup(dest))) {
#if USEX
                    cbuttons[i].arg = al[j].arg;
#endif
#if USEWAYLAND
                    wl_cbuttons[i].arg = al[j].arg;
#endif
                }
            }

            config_setting_lookup_int(conf, "ignoreglobalmouse", &ignoreglobalmouse);
        }
    }

    // we're done with this config
    config_destroy(&cfg);

    // load the theme now
    if (loadtheme) {
        theme_load();
    }

    if (!bindsfile || !strcmp(bindsfile, "NULL")) {
        if (!(xdg_conf = getenv("XDG_CONFIG_HOME"))) {
            home = getenv("HOME");

            if (!(bindfile = malloc(snprintf(NULL, 0, "%s/%s", home, ".config/spmenu/binds.conf") + 1))) {
                die("spmenu: failed to malloc bindfile");
            }

            sprintf(bindfile, "%s/%s", home, ".config/spmenu/binds.conf");
        } else {
            if (!(bindfile = malloc(snprintf(NULL, 0, "%s/%s", xdg_conf, "spmenu/binds.conf") + 1))) {
                die("spmenu: failed to malloc bindfile");
            }

            sprintf(bindfile, "%s/%s", xdg_conf, "spmenu/binds.conf");
        }
    } else { // custom keys path
        if (!(bindfile = malloc(snprintf(NULL, 0, "%s", bindsfile) + 1))) {
            die("spmenu: failed to malloc bindfile");
        }

        sprintf(bindfile, "%s", bindsfile);
    }

    // don't bother trying to load if it doesn't exist.
    if (access(bindfile, F_OK) != 0) {
        return;
    }

    // init config
    config_t bind;

    config_init(&bind);

    // attempt to read config file to cfg
    if (!config_read_file(&bind, bindfile)) {
        // invalid configuration, but let's try to read it anyway
        fprintf(stderr, "spmenu: Invalid keys file.\n");
    }

    // load options binds.keys
    config_setting_t *key_bind = config_lookup(&bind, "bind.keys");
    if (key_bind != NULL && loadbinds) {
        int nmode = 0;

#if USEX
        memset(ckeys, '\0', LENGTH(ckeys)-1);
#endif
#if USEWAYLAND
        memset(wl_ckeys, '\0', LENGTH(wl_ckeys)-1);
#endif
        for (unsigned int i = 0; i < config_setting_length(key_bind); ++i) {
            config_setting_t *conf = config_setting_get_elem(key_bind, i);

            // look up
            config_setting_lookup_string(conf, "modifier", &dest);

#if USEX
            for (int j = 0; j < LENGTH(ml); j++) {
                if (!strcmp(ml[j].mod, strdup(dest))) {
                    ckeys[i].mod = ml[j].modifier;
                }
            }
#endif

#if USEWAYLAND
            for (int j = 0; j < LENGTH(wml); j++) {
                if (!strcmp(wml[j].mod, strdup(dest))) {
                    wl_ckeys[i].modifier = wml[j].modifier;
                }
            }
#endif

            if (config_setting_lookup_int(conf, "mode", &nmode)) {
#if USEX
                ckeys[i].mode = nmode;
#endif
#if USEWAYLAND
                wl_ckeys[i].mode = nmode;
#endif
            }

            config_setting_lookup_string(conf, "key", &dest);

#if USEX
            for (int j = 0; j < LENGTH(kl); j++) {
                if (!strcmp(kl[j].key, strdup(dest))) {
                    ckeys[i].keysym = kl[j].keysym;
                }
            }
#endif

#if USEWAYLAND
            for (int j = 0; j < LENGTH(wkl); j++) {
                if (!strcmp(wkl[j].key, strdup(dest))) {
                    wl_ckeys[i].keysym = wkl[j].keysym;
                }
            }
#endif

            config_setting_lookup_string(conf, "function", &dest);

            for (int j = 0; j < LENGTH(fl); j++) {
                if (!strcmp(fl[j].function, strdup(dest))) {
#if USEX
                    ckeys[i].func = fl[j].func;
#endif
#if USEWAYLAND
                    wl_ckeys[i].func = fl[j].func;
#endif
                }
            }

            config_setting_lookup_string(conf, "argument", &dest);

            for (int j = 0; j < LENGTH(al); j++) {
                if (!strcmp(al[j].argument, strdup(dest))) {
#if USEX
                    ckeys[i].arg = al[j].arg;
#endif
#if USEWAYLAND
                    wl_ckeys[i].arg = al[j].arg;
#endif
                }
            }

            config_setting_lookup_int(conf, "ignoreglobalkeys", &ignoreglobalkeys);
        }
    }

    // load options binds.mouse
    config_setting_t *mouse_bind = config_lookup(&bind, "bind.mouse");
    if (mouse_bind != NULL && loadbinds) {
#if USEX
        memset(cbuttons, '\0', LENGTH(cbuttons)-1);
#endif
        for (unsigned int i = 0; i < config_setting_length(mouse_bind); ++i) {
            config_setting_t *conf = config_setting_get_elem(mouse_bind, i);

            config_setting_lookup_string(conf, "click", &dest);

            for (int j = 0; j < LENGTH(ctp); j++) {
                if (!strcmp(ctp[j].tclick, strdup(dest))) {
#if USEX
                    cbuttons[i].click = ctp[j].click;
#endif
#if USEWAYLAND
                    wl_cbuttons[i].click = ctp[j].click;
#endif
                }
            }

            config_setting_lookup_string(conf, "button", &dest);

#if USEX
            for (int j = 0; j < LENGTH(btp); j++) {
                if (!strcmp(btp[j].click, strdup(dest))) {
                    cbuttons[i].button = btp[j].button;
                }
            }
#endif

#if USEWAYLAND
            for (int j = 0; j < LENGTH(w_btp); j++) {
                if (!strcmp(w_btp[j].click, strdup(dest))) {
                    wl_cbuttons[i].button = w_btp[j].button;
                }
            }
#endif

            config_setting_lookup_string(conf, "function", &dest);

            for (int j = 0; j < LENGTH(fl); j++) {
                if (!strcmp(fl[j].function, strdup(dest))) {
#if USEX
                    cbuttons[i].func = fl[j].func;
#endif
#if USEWAYLAND
                    wl_cbuttons[i].func = fl[j].func;
#endif
                }
            }

            config_setting_lookup_string(conf, "argument", &dest);

            for (int j = 0; j < LENGTH(al); j++) {
                if (!strcmp(al[j].argument, strdup(dest))) {
#if USEX
                    cbuttons[i].arg = al[j].arg;
#endif
#if USEWAYLAND
                    wl_cbuttons[i].arg = al[j].arg;
#endif
                }
            }

            config_setting_lookup_int(conf, "ignoreglobalmouse", &ignoreglobalmouse);
        }
    }

    // finally done
    config_destroy(&bind);

    return;
}
