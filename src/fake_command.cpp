#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "command_factory.h"

class FakeCommand : public Command
{
  public:
    COMMAND_DECLARATION(FakeCommand);

    FakeCommand(CommandFactory *factory)
            :Command(factory)
    {}

    virtual ~FakeCommand() {}

    virtual const char *desc() const
    {
        return "fake command";
    }

    virtual void run(int argc, char *argv[])
    {
        printf("this is a fake command!\n");
    }
};

COMMAND_IMPLEMENTION(FakeCommand, "fake");
COMMAND_FACTORY_REGISTRATION(FakeCommand);
