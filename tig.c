#if __GNUC__ >= 3
#define __NORETURN __attribute__((__noreturn__))
#else
#define __NORETURN
#endif

static void __NORETURN die(const char *err, ...);
#define SIZEOF_REVGRAPH	19	/* Size of revision ancestry graphics. */
static int
string_enum_compare(const char *str1, const char *str2, int len)
{
	size_t i;

#define string_enum_sep(x) ((x) == '-' || (x) == '_' || (x) == '.')

	/* Diff-Header == DIFF_HEADER */
	for (i = 0; i < len; i++) {
		if (toupper(str1[i]) == toupper(str2[i]))
			continue;

		if (string_enum_sep(str1[i]) &&
		    string_enum_sep(str2[i]))
			continue;

		return str1[i] - str2[i];
	}

	return 0;
}

	REQ_(TOGGLE_LINENO,	"Toggle line numbers"), \
	REQ_(TOGGLE_REV_GRAPH,	"Toggle revision graph visualization")
	REQ_INFO,
	REQ_UNKNOWN,
	char *name;
	int namelen;
#define REQ_GROUP(help)	{ 0, NULL, 0, (help) },
#define REQ_(req, help)	{ REQ_##req, (#req), STRING_SIZE(#req), (help) }
static enum request
get_request(const char *name)
{
	int namelen = strlen(name);
	int i;

	for (i = 0; i < ARRAY_SIZE(req_info); i++)
		if (req_info[i].namelen == namelen &&
		    !string_enum_compare(req_info[i].name, name, namelen))
			return req_info[i].request;

	return REQ_UNKNOWN;
}


static bool opt_rev_graph	= TRUE;
		die("unknown option '%s'\n\n%s", opt, usage);
/*
 * Line-oriented content detection.
 */
		    !string_enum_compare(line_info[type].name, name, namelen))
/*
 * Keys
 */

struct keybinding {
	int alias;
	enum request request;
	struct keybinding *next;
};

static struct keybinding default_keybindings[] = {
	/* View switching */
	{ 'm',		REQ_VIEW_MAIN },
	{ 'd',		REQ_VIEW_DIFF },
	{ 'l',		REQ_VIEW_LOG },
	{ 'p',		REQ_VIEW_PAGER },
	{ 'h',		REQ_VIEW_HELP },
	{ '?',		REQ_VIEW_HELP },

	/* View manipulation */
	{ 'q',		REQ_VIEW_CLOSE },
	{ KEY_TAB,	REQ_VIEW_NEXT },
	{ KEY_RETURN,	REQ_ENTER },
	{ KEY_UP,	REQ_PREVIOUS },
	{ KEY_DOWN,	REQ_NEXT },

	/* Cursor navigation */
	{ 'k',		REQ_MOVE_UP },
	{ 'j',		REQ_MOVE_DOWN },
	{ KEY_HOME,	REQ_MOVE_FIRST_LINE },
	{ KEY_END,	REQ_MOVE_LAST_LINE },
	{ KEY_NPAGE,	REQ_MOVE_PAGE_DOWN },
	{ ' ',		REQ_MOVE_PAGE_DOWN },
	{ KEY_PPAGE,	REQ_MOVE_PAGE_UP },
	{ 'b',		REQ_MOVE_PAGE_UP },
	{ '-',		REQ_MOVE_PAGE_UP },

	/* Scrolling */
	{ KEY_IC,	REQ_SCROLL_LINE_UP },
	{ KEY_DC,	REQ_SCROLL_LINE_DOWN },
	{ 'w',		REQ_SCROLL_PAGE_UP },
	{ 's',		REQ_SCROLL_PAGE_DOWN },

	/* Misc */
	{ 'Q',		REQ_QUIT },
	{ 'z',		REQ_STOP_LOADING },
	{ 'v',		REQ_SHOW_VERSION },
	{ 'r',		REQ_SCREEN_REDRAW },
	{ 'n',		REQ_TOGGLE_LINENO },
	{ 'g',		REQ_TOGGLE_REV_GRAPH},
	{ ':',		REQ_PROMPT },

	/* wgetch() with nodelay() enabled returns ERR when there's no input. */
	{ ERR,		REQ_SCREEN_UPDATE },

	/* Use the ncurses SIGWINCH handler. */
	{ KEY_RESIZE,	REQ_SCREEN_RESIZE },
};

#define KEYMAP_INFO \
	KEYMAP_(GENERIC), \
	KEYMAP_(MAIN), \
	KEYMAP_(DIFF), \
	KEYMAP_(LOG), \
	KEYMAP_(PAGER), \
	KEYMAP_(HELP) \

enum keymap {
#define KEYMAP_(name) KEYMAP_##name
	KEYMAP_INFO
#undef	KEYMAP_
};

static struct int_map keymap_table[] = {
#define KEYMAP_(name) { #name, STRING_SIZE(#name), KEYMAP_##name }
	KEYMAP_INFO
#undef	KEYMAP_
};

#define set_keymap(map, name) \
	set_from_int_map(keymap_table, ARRAY_SIZE(keymap_table), map, name, strlen(name))

static struct keybinding *keybindings[ARRAY_SIZE(keymap_table)];

static void
add_keybinding(enum keymap keymap, enum request request, int key)
{
	struct keybinding *keybinding;

	keybinding = calloc(1, sizeof(*keybinding));
	if (!keybinding)
		die("Failed to allocate keybinding");

	keybinding->alias = key;
	keybinding->request = request;
	keybinding->next = keybindings[keymap];
	keybindings[keymap] = keybinding;
}

/* Looks for a key binding first in the given map, then in the generic map, and
 * lastly in the default keybindings. */
static enum request
get_keybinding(enum keymap keymap, int key)
{
	struct keybinding *kbd;
	int i;

	for (kbd = keybindings[keymap]; kbd; kbd = kbd->next)
		if (kbd->alias == key)
			return kbd->request;

	for (kbd = keybindings[KEYMAP_GENERIC]; kbd; kbd = kbd->next)
		if (kbd->alias == key)
			return kbd->request;

	for (i = 0; i < ARRAY_SIZE(default_keybindings); i++)
		if (default_keybindings[i].alias == key)
			return default_keybindings[i].request;

	return (enum request) key;
}


struct key {
	char *name;
	int value;
};

static struct key key_table[] = {
	{ "Enter",	KEY_RETURN },
	{ "Space",	' ' },
	{ "Backspace",	KEY_BACKSPACE },
	{ "Tab",	KEY_TAB },
	{ "Escape",	KEY_ESC },
	{ "Left",	KEY_LEFT },
	{ "Right",	KEY_RIGHT },
	{ "Up",		KEY_UP },
	{ "Down",	KEY_DOWN },
	{ "Insert",	KEY_IC },
	{ "Delete",	KEY_DC },
	{ "Hash",	'#' },
	{ "Home",	KEY_HOME },
	{ "End",	KEY_END },
	{ "PageUp",	KEY_PPAGE },
	{ "PageDown",	KEY_NPAGE },
	{ "F1",		KEY_F(1) },
	{ "F2",		KEY_F(2) },
	{ "F3",		KEY_F(3) },
	{ "F4",		KEY_F(4) },
	{ "F5",		KEY_F(5) },
	{ "F6",		KEY_F(6) },
	{ "F7",		KEY_F(7) },
	{ "F8",		KEY_F(8) },
	{ "F9",		KEY_F(9) },
	{ "F10",	KEY_F(10) },
	{ "F11",	KEY_F(11) },
	{ "F12",	KEY_F(12) },
};

static int
get_key_value(const char *name)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(key_table); i++)
		if (!strcasecmp(key_table[i].name, name))
			return key_table[i].value;

	if (strlen(name) == 1 && isprint(*name))
		return (int) *name;

	return ERR;
}

static char *
get_key(enum request request)
{
	static char buf[BUFSIZ];
	static char key_char[] = "'X'";
	int pos = 0;
	char *sep = "    ";
	int i;

	buf[pos] = 0;

	for (i = 0; i < ARRAY_SIZE(default_keybindings); i++) {
		struct keybinding *keybinding = &default_keybindings[i];
		char *seq = NULL;
		int key;

		if (keybinding->request != request)
			continue;

		for (key = 0; key < ARRAY_SIZE(key_table); key++)
			if (key_table[key].value == keybinding->alias)
				seq = key_table[key].name;

		if (seq == NULL &&
		    keybinding->alias < 127 &&
		    isprint(keybinding->alias)) {
			key_char[1] = (char) keybinding->alias;
			seq = key_char;
		}

		if (!seq)
			seq = "'?'";

		if (!string_format_from(buf, &pos, "%s%s", sep, seq))
			return "Too many keybindings!";
		sep = ", ";
	}

	return buf;
}


static struct int_map color_map[] = {
#define COLOR_MAP(name) { #name, STRING_SIZE(#name), COLOR_##name }
	COLOR_MAP(DEFAULT),
	COLOR_MAP(BLACK),
	COLOR_MAP(BLUE),
	COLOR_MAP(CYAN),
	COLOR_MAP(GREEN),
	COLOR_MAP(MAGENTA),
	COLOR_MAP(RED),
	COLOR_MAP(WHITE),
	COLOR_MAP(YELLOW),
};

#define set_color(color, name) \
	set_from_int_map(color_map, ARRAY_SIZE(color_map), color, name, strlen(name))
static struct int_map attr_map[] = {
#define ATTR_MAP(name) { #name, STRING_SIZE(#name), A_##name }
	ATTR_MAP(NORMAL),
	ATTR_MAP(BLINK),
	ATTR_MAP(BOLD),
	ATTR_MAP(DIM),
	ATTR_MAP(REVERSE),
	ATTR_MAP(STANDOUT),
	ATTR_MAP(UNDERLINE),
};

#define set_attribute(attr, name) \
	set_from_int_map(attr_map, ARRAY_SIZE(attr_map), attr, name, strlen(name))
/* Wants: object fgcolor bgcolor [attr] */
option_color_command(int argc, char *argv[])
	struct line_info *info;
	if (argc != 3 && argc != 4) {
		config_msg = "Wrong number of arguments given to color command";
		return ERR;
	}
	info = get_line_info(argv[0], strlen(argv[0]));
	if (!info) {
		config_msg = "Unknown color name";
		return ERR;
	}
	if (set_color(&info->fg, argv[1]) == ERR ||
	    set_color(&info->bg, argv[2]) == ERR) {
		config_msg = "Unknown color";
		return ERR;
	}

	if (argc == 4 && set_attribute(&info->attr, argv[3]) == ERR) {
		config_msg = "Unknown attribute";
		return ERR;
	}

	return OK;
}

/* Wants: name = value */
static int
option_set_command(int argc, char *argv[])
{
	if (argc != 3) {
		config_msg = "Wrong number of arguments given to set command";
		return ERR;
	}

	if (strcmp(argv[1], "=")) {
		config_msg = "No value assigned";
		return ERR;
	}

	if (!strcmp(argv[0], "show-rev-graph")) {
		opt_rev_graph = (!strcmp(argv[2], "1") ||
				 !strcmp(argv[2], "true") ||
				 !strcmp(argv[2], "yes"));
		return OK;
	}

	if (!strcmp(argv[0], "line-number-interval")) {
		opt_num_interval = atoi(argv[2]);
		return OK;
	}
	if (!strcmp(argv[0], "tab-size")) {
		opt_tab_size = atoi(argv[2]);
	if (!strcmp(argv[0], "commit-encoding")) {
		char *arg = argv[2];
		int delimiter = *arg;
		int i;

		switch (delimiter) {
		case '"':
		case '\'':
			for (arg++, i = 0; arg[i]; i++)
				if (arg[i] == delimiter) {
					arg[i] = 0;
					break;
				}
		default:
			string_copy(opt_encoding, arg);
			return OK;
		}
	}

	config_msg = "Unknown variable name";
	return ERR;
}

/* Wants: mode request key */
static int
option_bind_command(int argc, char *argv[])
{
	enum request request;
	int keymap;
	int key;

	if (argc != 3) {
		config_msg = "Wrong number of arguments given to bind command";
		return ERR;
	}

	if (set_keymap(&keymap, argv[0]) == ERR) {
		config_msg = "Unknown key map";
		return ERR;
	}

	key = get_key_value(argv[1]);
	if (key == ERR) {
		config_msg = "Unknown key";
		return ERR;
	}

	request = get_request(argv[2]);
	if (request == REQ_UNKNOWN) {
		config_msg = "Unknown request name";
		return ERR;
	}

	add_keybinding(keymap, request, key);

	return OK;
}

static int
set_option(char *opt, char *value)
{
	char *argv[16];
	int valuelen;
	int argc = 0;

	/* Tokenize */
	while (argc < ARRAY_SIZE(argv) && (valuelen = strcspn(value, " \t"))) {
		argv[argc++] = value;

		value += valuelen;
		if (!*value)
			break;

		*value++ = 0;
		while (isspace(*value))
			value++;
	}

	if (!strcmp(opt, "color"))
		return option_color_command(argc, argv);

	if (!strcmp(opt, "set"))
		return option_set_command(argc, argv);

	if (!strcmp(opt, "bind"))
		return option_bind_command(argc, argv);

	config_msg = "Unknown option command";
	int status = OK;

	/* Check for comment markers, since read_properties() will
	 * only ensure opt and value are split at first " \t". */
	optlen = strcspn(opt, "#");
	if (optlen == 0)
	if (opt[optlen] != 0) {
		config_msg = "No option value";
		status = ERR;

	}  else {
		/* Look for comment endings in the value. */
		int len = strcspn(value, "#");

		if (len < valuelen) {
			valuelen = len;
			value[valuelen] = 0;
		}

		status = set_option(opt, value);
	if (status == ERR) {
		fprintf(stderr, "Error on line %d, near '%.*s': %s\n",
	enum keymap keymap;	/* What keymap does this view have */

#define VIEW_STR(name, cmd, env, ref, ops, map) \
	{ name, cmd, #env, ref, ops, map}
	VIEW_STR(name, TIG_##id##_CMD,  TIG_##id##_CMD, ref, ops, KEYMAP_##id)
		unsigned int view_lines = view->offset + view->height;
				   ? MIN(view_lines, view->lines) * 100 / view->lines
		/* Incrementally draw avoids flickering. */
		redraw_view_from(view, redraw_from);
	}

	/* Update the title _after_ the redraw so that if the redraw picks up a
	 * commit reference in view->ref it'll be available here. */
	update_view_title(view);

	if (ferror(view->pipe)) {
		report("Failed to read: %s", strerror(errno));
		goto end;

	} else if (feof(view->pipe)) {
		report("");
		goto end;
	}

	return TRUE;

alloc_error:
	report("Allocation failure");

end:
	end_update(view);
	return FALSE;
}


/*
 * View opening
 */

static void open_help_view(struct view *view)
{
	char buf[BUFSIZ];
	int lines = ARRAY_SIZE(req_info) + 2;
	int i;

	if (view->lines > 0)
		return;

	for (i = 0; i < ARRAY_SIZE(req_info); i++)
		if (!req_info[i].request)
			lines++;

	view->line = calloc(lines, sizeof(*view->line));
	if (!view->line) {
		report("Allocation failure");
		return;
	view->ops->read(view, "Quick reference for tig keybindings:");
	for (i = 0; i < ARRAY_SIZE(req_info); i++) {
		char *key;
		if (!req_info[i].request) {
			view->ops->read(view, "");
			view->ops->read(view, req_info[i].help);
			continue;
		}
		key = get_key(req_info[i].request);
		if (!string_format(buf, "%-25s %s", key, req_info[i].help))
			continue;
		view->ops->read(view, buf);
	}
	if (view == VIEW(REQ_VIEW_HELP)) {
		open_help_view(view);

	} else if ((reload || strcmp(view->vid, view->id)) &&
		   !begin_update(view)) {
	case REQ_TOGGLE_REV_GRAPH:
		opt_rev_graph = !opt_rev_graph;
		redraw_display();
		break;

	char id[41];			/* SHA1 ID. */
	char title[75];			/* First line of the commit message. */
	char author[75];		/* Author of the commit. */
	struct tm time;			/* Date from the author ident. */
	struct ref **refs;		/* Repository references. */
	chtype graph[SIZEOF_REVGRAPH];	/* Ancestry chain graphics. */
	size_t graph_size;		/* The width of the graph array. */
	if (opt_rev_graph && commit->graph_size) {
		size_t i;

		wmove(view->win, lineno, col);
		/* Using waddch() instead of waddnstr() ensures that
		 * they'll be rendered correctly for the cursor line. */
		for (i = 0; i < commit->graph_size; i++)
			waddch(view->win, commit->graph[i]);

		col += commit->graph_size + 1;
	}

	wmove(view->win, lineno, col);
		commit->graph[commit->graph_size++] = ACS_LTEE;
static int
read_prompt(void)
{
	enum { READING, STOP, CANCEL } status = READING;
	char buf[sizeof(opt_cmd) - STRING_SIZE("git \0")];
	int pos = 0;

	while (status == READING) {
		struct view *view;
		int i, key;

		foreach_view (view, i)
			update_view(view);

		report(":%.*s", pos, buf);
		/* Refresh, accept single keystroke of input */
		key = wgetch(status_win);
		switch (key) {
		case KEY_RETURN:
		case KEY_ENTER:
		case '\n':
			status = pos ? STOP : CANCEL;
			break;

		case KEY_BACKSPACE:
			if (pos > 0)
				pos--;
			else
				status = CANCEL;
			break;

		case KEY_ESC:
			status = CANCEL;
			break;

		case ERR:
			break;

		default:
			if (pos >= sizeof(buf)) {
				report("Input string too long");
				return ERR;
			}

			if (isprint(key))
				buf[pos++] = (char) key;
		}
	}

	if (status == CANCEL) {
		/* Clear the status window */
		report("");
		return ERR;
	}

	buf[pos++] = 0;
	if (!string_format(opt_cmd, "git %s", buf))
		return ERR;
	opt_request = REQ_VIEW_PAGER;

	return OK;
}
	if (!strncmp(name, "refs/tags/", STRING_SIZE("refs/tags/"))) {
		/* Commits referenced by tags has "^{}" appended. */
		if (name[namelen - 1] != '}')
			return OK;

		namelen -= STRING_SIZE("refs/tags/");
		name	+= STRING_SIZE("refs/tags/");
		namelen -= STRING_SIZE("refs/heads/");
		name	+= STRING_SIZE("refs/heads/");
	ref->name = malloc(namelen + 1);
	strncpy(ref->name, name, namelen);
	ref->name[namelen] = 0;

		request = get_keybinding(display[current_view]->keymap, key);
			if (read_prompt() == ERR)