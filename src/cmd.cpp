#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include <vector>
#include <algorithm>

#include "cmd_def.h"
#include "command_factory.h"

typedef std::vector<Command *> CmdList;

static const char *FLOCK_FILE = "/tmp/cmd.lock";

static char PROGRAM_NAME[64] = {0};
static char CMDNAME[64] = {0};
static FILE *LOGFP = NULL;

static bool s_enableTrace = false;

const char *programName()
{
    return PROGRAM_NAME;
}

int regflock(int fd, int cmd, int type, off_t offset, int whence, off_t len)
{
	struct flock lock;
	lock.l_type = type;
	lock.l_start = offset;
	lock.l_whence = whence;
	lock.l_len = len;
	lock.l_pid = 0;
	return fcntl(fd, cmd, &lock);
}

void logPrint(int loglv, const char *fmt, ...)
{
    va_list args;
    char buf[2048];

    va_start(args, fmt);
    vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);

    fprintf(LOGFP, buf);
}

void trace(const char *fmt, ...)
{
    if (!s_enableTrace)
        return;

    va_list args;
    char buf[2048];

    va_start(args, fmt);
    vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);

    fprintf(stdout, buf);
}

void enableTrace(bool b)
{
    s_enableTrace = b;
}

static bool cmpfunc(Command *a, Command *b)
{
    if (a->editDistance(CMDNAME) <= b->editDistance(CMDNAME))
        return true;

    return false;
}

static void openLog()
{
    LOGFP = fopen("/tmp/cmd.log", "w");
    if (!LOGFP)
        LOGFP = fopen("/dev/null", "w");
}

static void closeLog()
{
    if (LOGFP)
        fclose(LOGFP);
}

static void singleProcess()
{
    int fd = open(FLOCK_FILE, O_CREAT|O_WRONLY, 0666);
    if (fd < 0)
    {
        HINT("open lock file failed\n");
        exit(1);
    }
    if (regWrFlock(fd, 0, SEEK_SET, 0) < 0)
    {
        HINT("[%s] is being calling, please try late!\n", programName());
        exit(1);
    }
}

int main(int argc, char *argv[])
{
    CommandFactoryRegistry *registry = CommandFactoryRegistry::getRegistry();
    CommandFactory *factory;

    // get program name
    if (argc > 0)
    {
        const char *ptr = argv[0] + strlen(argv[0]);
        while (ptr > argv[0] && *ptr != '/')
            ptr--;
        if ('/' == *ptr)
            ptr++;
        snprintf(PROGRAM_NAME, sizeof(PROGRAM_NAME), "%s", ptr);
    }

    singleProcess();
    openLog();

    if (argc <= 1)
    {
        goto err;
    }

    snprintf(CMDNAME, sizeof(CMDNAME), "%s", argv[1]);
    factory = registry->getCommandFactory(CMDNAME);
    if (factory)
    {
        Command *cmd = factory->getCommand();
        cmd->run(argc - 1, argv + 1);
    }
    else
    {
        goto err;
    }

    closeLog();
    exit(0);

err:
    // 命令补全
    CmdList cmdlist;
    VectorIterator<FactoryList> itWrapper = registry->getIter();
    for (; itWrapper.hasMoreElements(); itWrapper.moveNext())
    {
        CommandFactory *factory = itWrapper.peekNext();
        Command *cmd = factory->getCommand();
        cmdlist.push_back(cmd);
    }

    std::sort(cmdlist.begin(), cmdlist.end(), cmpfunc);

    if (*CMDNAME)
    {
        HINT("\"%s\" is invalid, Possible commands are:\n", CMDNAME);
    }
    else
    {
        HINT("Supported commands\n");
    }
    for (CmdList::iterator it = cmdlist.begin(); it != cmdlist.end(); it++)
    {
        Command *c = *it;
        if (!*CMDNAME || c->shouldHint(CMDNAME))
        // if (c->shouldHint(CMDNAME))
        {
            HINT("  %-8s---- %s\n", c->getName().c_str(), c->desc());
        }
    }

    closeLog();
    exit(1);
}
