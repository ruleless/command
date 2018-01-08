#include "command_factory.h"

bool CommandFactoryRegistry::mbValid = false;

std::string Command::getName()
{
    return mFactory->getName();
}

int Command::editDistance(const std::string &cmdname)
{
    if (mLDis >= 0)
        return mLDis;

    // step 1
    std::string target(mFactory->getName());
    std::string source(cmdname);
    int n = source.length();
    int m = target.length();

    if (m == 0)
        return mLDis = n;
    if (n == 0)
        return mLDis = m;

    typedef std::vector<std::vector<int> > Matrix;
    Matrix matrix(n + 1);
    for (int i = 0; i <= n; i++)
        matrix[i].resize(m + 1);

    // step 2 Initialize
    for (int i = 1; i <= n; i++)
        matrix[i][0] = i;
    for(int i = 1; i <= m; i++)
        matrix[0][i] = i;

    // step 3
    for (int i = 1; i <= n; i++)
    {
        const char si = source[i - 1];

        // step 4
        for (int j = 1; j <= m; j++)
        {
            const char dj = target[j - 1];

            // step 5
            int cost;
            if (si == dj)
            {
                cost = 0;
            }
            else
            {
                cost = 1;
            }

            // step 6
            const int above = matrix[i-1][j] + 1;
            const int left = matrix[i][j-1] + 1;
            const int diag = matrix[i-1][j-1] + cost;
            matrix[i][j] = std::min(above, std::min(left, diag));
        }
    }

    return mLDis = matrix[n][m];
}

bool Command::shouldHint(const std::string &cmdname) const
{
    std::string target(mFactory->getName());
    std::string source(cmdname);
    int n = source.length();
    int m = target.length();

    if (mLDis >= 0 && mLDis < std::max(n, m))
        return true;

    return false;
}

CommandFactoryRegistry::CommandFactoryRegistry()
{
    mbValid = true;
}

CommandFactoryRegistry::~CommandFactoryRegistry()
{
    mbValid = false;
}

CommandFactoryRegistry *CommandFactoryRegistry::getRegistry()
{
    static CommandFactoryRegistry s_registry;
    return &s_registry;
}

bool CommandFactoryRegistry::isValid()
{
    return mbValid;
}

void CommandFactoryRegistry::registerFactory(CommandFactory *factory)
{
    mFactories.push_back(factory);
}

void CommandFactoryRegistry::unregisterFactory(CommandFactory *factory)
{
    FactoryList::iterator it = mFactories.begin();
    while (it != mFactories.end())
    {
        if ((*it) == factory)
        {
            it = mFactories.erase(it);
        }
        else
        {
            ++it;
        }
    }
}

CommandFactory *CommandFactoryRegistry::getCommandFactory(const std::string &cmdName)
{
    FactoryList::iterator it = mFactories.begin();
    for (; it != mFactories.end(); ++it)
    {
        if ((*it)->getName() == cmdName)
        {
            return (*it);
        }
    }
    return NULL;
}

VectorIterator<FactoryList> CommandFactoryRegistry::getIter()
{
    return VectorIterator<FactoryList>(mFactories.begin(), mFactories.end());
}
