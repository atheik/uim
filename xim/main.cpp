/*

  Copyright (c) 2003,2004 uim Project http://uim.freedesktop.org/

  All rights reserved.

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions
  are met:

  1. Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
  2. Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in the
     documentation and/or other materials provided with the distribution.
  3. Neither the name of authors nor the names of its contributors
     may be used to endorse or promote products derived from this software
     without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
  ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
  FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
  OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
  HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
  OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
  SUCH DAMAGE.
*/

// XIM Server supporting CJK languages
// initialize many modules
//
#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <stdio.h>
#include <string.h>
#include <locale.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/select.h>

#include "xim.h"
#include "xdispatch.h"
#include "ximserver.h"
#include "canddisp.h"
#include "connection.h"
#include "util.h"
#include "helper.h"

#include "uim/uim-util.h"
#include "uim/uim-im-switcher.h"

Display *XimServer::gDpy;
std::map<Window, XimServer *> XimServer::gServerMap;

// Configuration
int g_option_mask;
int scr_width, scr_height;
int host_byte_order;

#define VERSION_NAME "uim-xim under the way! Version "PACKAGE_VERSION"\n"
const char *version_name=VERSION_NAME;
const char *usage=
"--help , --version :Show usage or version\n"
"--list             :Show available backend conversion engines\n"
"--engine=ENGINE    :Use ENGINE as a backend conversion engine at startup\n"
"--async            :Use on-demand-synchronous method of XIM event flow\n"
"                    (using this option is not safe for Tcl/Tk GUI toolkit)\n"
"--trace            :trace-connection\n"
"--trace-xim        :trace-xim-message\n";
const char *default_engine;


static Atom atom_locales;
static Atom atom_transport;
Atom xim_servers;

struct fd_watch_struct {
    int mask;
    void (*fn)(int, int);
};
static std::map<int, fd_watch_struct> fd_watch_stat;
static std::map<unsigned int, WindowIf *> window_watch_stat;

static char *supported_locales;
std::list<UIMInfo> uim_info;

bool
pretrans_register()
{
    xim_servers = XInternAtom(XimServer::gDpy, "XIM_SERVERS", 0);
    atom_locales = XInternAtom(XimServer::gDpy, "LOCALES", 0);
    atom_transport = XInternAtom(XimServer::gDpy, "TRANSPORT", 0);
    XFlush(XimServer::gDpy);
    scr_width = DisplayWidth(XimServer::gDpy, 0);
    scr_height = DisplayHeight(XimServer::gDpy, 0);
    return true;
}

WindowIf::~WindowIf()
{
}

void
WindowIf::resize(Window, int, int)
{
    // do nothing
}

void remove_current_fd_watch(int fd)
{
    std::map<int, fd_watch_struct>::iterator i;
    i = fd_watch_stat.find(fd);
    if (i == fd_watch_stat.end())
	return;

    fd_watch_stat.erase(i);
}

void add_fd_watch(int fd, int mask, void (*fn)(int, int))
{
    remove_current_fd_watch(fd);

    fd_watch_struct s;
    s.mask = mask;
    s.fn = fn;
    std::pair<int, fd_watch_struct> p(fd, s);
    fd_watch_stat.insert(p);
}

static void main_loop()
{
    fd_set rfds, wfds;
    while (1) {
	FD_ZERO(&rfds);
	FD_ZERO(&wfds);
	std::map<int, fd_watch_struct>::iterator it;
	int  fd_max = 0;
	for (it = fd_watch_stat.begin(); it != fd_watch_stat.end(); it++) {
	    int fd = it->first;
	    if (it->second.mask & READ_OK)
		FD_SET(fd, &rfds);
	    if (it->second.mask & WRITE_OK)
		FD_SET(fd, &wfds);
	    if (fd_max < fd)
		fd_max = fd;
	}
	select(fd_max + 1, &rfds, &wfds, NULL, NULL);
	for (it = fd_watch_stat.begin(); it != fd_watch_stat.end(); it++) {
	    int fd = it->first;
	    if (FD_ISSET(fd, &rfds))
		it->second.fn(fd, READ_OK);
	    if (FD_ISSET(fd, &wfds))
		it->second.fn(fd, WRITE_OK);
	}
    }
}

void
add_window_watch(Window id, WindowIf *w, int mask)
{
    std::pair<unsigned int, WindowIf *> p(id, w);
    window_watch_stat.insert(p);

    // Event mash has same value defined in X,
    // but do not depend on.
    int emask = 0;
    if (mask & EXPOSE_MASK)
	emask |= ExposureMask;
    if (mask & STRUCTURE_NOTIFY_MASK)
	emask |= StructureNotifyMask;

    XSelectInput(XimServer::gDpy, id, emask);
}

void
remove_window_watch(Window id)
{
    std::map<unsigned int, WindowIf *>::iterator i;
    i = window_watch_stat.find(id);
    if (i != window_watch_stat.end())
	window_watch_stat.erase(i);
}

WindowIf *
findWindowIf(Window w)
{
    std::map<unsigned int, WindowIf *>::iterator i;
    i = window_watch_stat.find(w);
    if (i == window_watch_stat.end())
	return NULL;

    return (*i).second;
}

static int
X_ErrorHandler(Display *d, XErrorEvent *e)
{
    if (g_option_mask & OPT_TRACE)
	printf("X error occured.\n");

    return 0;
}

static int
X_IOErrorHandler(Display *d)
{
    return 0;
}


static void
sendSelectionNotify(XEvent *ev, char *buf, int len)
{
    XEvent e;
    e.type = SelectionNotify;
    e.xselection.requestor = ev->xselectionrequest.requestor;
    e.xselection.selection = ev->xselectionrequest.selection;
    e.xselection.target = ev->xselectionrequest.target;
    e.xselection.time = ev->xselectionrequest.time;
    e.xselection.property = ev->xselectionrequest.property;
    XChangeProperty(XimServer::gDpy, e.xselection.requestor,
		    e.xselection.property,
		    e.xselection.target,
		    8, PropModeReplace,
		    (unsigned char *)buf, len);
    XSendEvent(XimServer::gDpy, e.xselection.requestor, 0, 0, &e);
    XFlush(XimServer::gDpy);
}

void
notifyLocale(XEvent *ev)
{
    sendSelectionNotify(ev, supported_locales, strlen(supported_locales) + 1);
    if (g_option_mask & OPT_TRACE)
	printf("selection notify request for locale.\n");
}

void
notifyTransport(XEvent *ev)
{
    sendSelectionNotify(ev, "@transport=X/", 13 + 1);
    if (g_option_mask & OPT_TRACE)
	printf("selection notify request for transport.\n");
}

void
ProcXEvent(XEvent *e)
{
    Atom p;
    WindowIf *i;
    switch (e->type) {
    case SelectionRequest:
	p = e->xselectionrequest.property;
	if (p == atom_locales)
	    notifyLocale(e);
	else if (p == atom_transport)
	    notifyTransport(e);
	else
	    printf("property %s?\n",
		   XGetAtomName(XimServer::gDpy, e->xselection.property));
	break;
    case Expose:
	if (e->xexpose.count == 0) {
	    i = findWindowIf(e->xexpose.window);
	    if (i)
		i->expose(e->xexpose.window);
	}
	break;
    case ConfigureNotify:
	i = findWindowIf(e->xconfigure.window);
	if (i)
	    i->resize(e->xconfigure.window, e->xconfigure.x, e->xconfigure.y);
	break;
    case DestroyNotify:
	i = findWindowIf(e->xdestroywindow.window);
	if (i)
	    i->destroy(e->xdestroywindow.window);
	remove_window_watch(e->xdestroywindow.window);
	break;
    case ClientMessage:
	procXClientMessage(&e->xclient);
	break;
    default:;
	//printf("unknown type of X event. %d\n", e->type);
    }
}

static void
xEventRead(int fd, int ev)
{
    XFlush(XimServer::gDpy);

    XEvent e;
    while (XPending(XimServer::gDpy)) {
	XNextEvent(XimServer::gDpy, &e);
	ProcXEvent(&e);
    }
}

static int
pretrans_setup()
{
    XSetErrorHandler(X_ErrorHandler);
    XSetIOErrorHandler(X_IOErrorHandler);
    int fd = XConnectionNumber(XimServer::gDpy);

    add_fd_watch(fd, READ_OK, xEventRead);
    return fd;
}

static void
print_version()
{
    printf(version_name);
}

static void
print_usage()
{
    print_version();
    printf(usage);
    exit(0);
}

static void
get_uim_info()
{
    int res;

    res = uim_init();
    if (res) {
	printf("Failed to init uim\n");
	exit(1);
    }
    uim_context uc = uim_create_context(NULL, "UTF-8", NULL,
					NULL, uim_iconv, NULL);

    struct UIMInfo ui;

    int nr = uim_get_nr_im(uc);
    for (int i = 0; i < nr; i++) {
	ui.name = strdup(uim_get_im_name(uc, i));
	ui.lang = strdup(uim_get_im_language(uc, i));
	ui.locale = createLocale(ui.lang);
	ui.desc = strdup(uim_get_im_short_desc(uc, i));
	if (!ui.locale)
	    continue;
	uim_info.push_back(ui);
    }
    uim_release_context(uc);
}

static void
print_uim_info()
{
    std::list<UIMInfo>::iterator it;

    printf("Supported conversion engines:\n");
    if (uim_info.empty())
	printf("  None.\n");
    else
	for (it = uim_info.begin(); it != uim_info.end(); it++)
	    printf("  %s (%s)\n", it->name, it->lang);
    
}

static void
init_supported_locales()
{
    std::list<char *> locale_list;
    char *locales;
    int len;

    asprintf(&supported_locales, "@locale=");
    len = strlen(supported_locales);

    // get all locales
    locales = strdup(compose_localenames_from_im_lang("*"));
    // replace ':' with ','
    char *sep;
    char *tmp = locales;
    while ((sep = strchr(tmp, ':')) != NULL) {
	*sep = ',';
	tmp = sep;
    }

    len += strlen(locales);
    supported_locales = (char *)realloc(supported_locales,
		    sizeof(char) * len + 1);
    if (!supported_locales) {
        fprintf(stderr, "Error: failed to register supported_locales. Aborting....");
        exit(1);
    }

    strcat(supported_locales, locales);
    free(locales);
}

static void
parse_args(int argc, char **argv)
{
    int i;
    for (i = 1; i < argc; i++) {
	if (!strncmp(argv[i], "--", 2)) {
	    char *opt;
	    opt = &argv[i][2];
	    if (!strcmp(opt, "version")) {
		print_version();
		exit(0);
	    } else if (!strcmp(opt, "help")) {
		print_usage();
	    } else if (!strcmp(opt, "list")) {
		get_uim_info();
		print_uim_info();
		exit(0);
	    } else if (!strcmp(opt, "trace")) {
		g_option_mask |= OPT_TRACE;
	    } else if (!strcmp(opt, "trace-xim")) {
		g_option_mask |= OPT_TRACE_XIM;
	    } else if (!strncmp(opt, "engine=", 7)) {
		default_engine = strdup(&argv[i][9]);
	    } else if (!strcmp(opt, "async")) {
		g_option_mask |= OPT_ON_DEMAND_SYNC;
	    }
	}
    }
}

static void check_default_engine(const char *locale)
{
    bool found = false;
    if (default_engine) {
	std::list<UIMInfo>::iterator it;
	for (it = uim_info.begin(); it != uim_info.end(); it++) {
	    if (!strcmp(it->name, default_engine)) {
		found = true;
		break;
	    }
	}
    }

    if (found == false)
	default_engine = uim_get_default_im_name(locale);
}

static void
get_runtime_env()
{
    int i = 1;
    char *v = (char *)&i;
    if (*v == 1)
	host_byte_order = LSB_FIRST;
    else
	host_byte_order = MSB_FIRST;
}

int
main(int argc, char **argv)
{
    const char *locale;

    printf("UIM-XIM bridge. Now supporting multiple locales.\n");

    get_runtime_env();

    parse_args(argc, argv);

    if (g_option_mask & OPT_ON_DEMAND_SYNC)
	printf("Using on-demand-synchronous XIM event flow (not safe for Tcl/TK)\n");
    else
	printf("Using full-synchronous XIM event flow\n");

    signal(SIGPIPE, SIG_IGN);

    check_helper_connection();

    XimServer::gDpy = XOpenDisplay(NULL);
    if (!XimServer::gDpy) {
	printf("failed to open display!\n");
	return 1;
    }
    if (!pretrans_register()) {
	printf("pretrans_register failed\n");
	return 1;
    }

    get_uim_info();
    print_uim_info();

    locale = setlocale(LC_CTYPE, "");
    if (!locale)
	locale = setlocale(LC_CTYPE, "C");

    check_default_engine(locale);
    init_supported_locales();
    init_modifier_keys();

    std::list<UIMInfo>::iterator it;
    bool res = false;

    // First, setup conversion engine selected by cmdline option or
    // "default-im-name" on ~/.uim.
    for (it = uim_info.begin(); it != uim_info.end(); it++) {
	if (strcmp(it->name, default_engine) == 0) {
	    XimServer *xs = new XimServer(it->locale, it->name, it->lang);
	    res = xs->setupConnection(true);
	    if (res)
		printf("XMODIFIERS=@im=uim registered, selecting %s (%s) as default conversion engine\n", it->name, it->lang);
	    else
		delete xs;
	    break;
	}
    }

#if 0
    // XXX Setup each backend as a separated XimServer, aimed for
    // backward compatibility, and should be removed in the future.
    for (it = uim_info.begin(); it != uim_info.end(); it++) {
	XimServer *xs = new XimServer(it->locale, it->name, it->lang);
	res = xs->setupConnection(false);
	if (res)
	    printf("XMODIFIERS=@im=uim-%s (%s) registered\n",
			    it->name, it->lang);
	else
	    delete xs;
    }
#endif

    if (!res) {
	printf("aborting...\n");
	return 1;
    }

    connection_setup();
    if (pretrans_setup() == -1)
	return 0;

    // Handle pending events to prevent hang just after startup
    XEvent e;
    while (XPending(XimServer::gDpy)) {
	XNextEvent(XimServer::gDpy, &e);
	ProcXEvent(&e);
    }

    main_loop();
    return 0;
}

/*
 * Local variables:
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 */
