#ifndef __COMMAND_FACTORY_H__
#define __COMMAND_FACTORY_H__

#include <iostream>
#include <string>
#include <vector>

#include "iterator_wrapper.h"

class CommandFactory;
class Command
{
  public:
    Command(CommandFactory *factory)
            :mFactory(factory)
            ,mLDis(-1)
    {}

    virtual ~Command() {}

    // 获取命令名称
    std::string getName();

    // 执行命令接口
    virtual void run(int argc, char *argv[]) = 0;

    // 命令的简单描述(单行)
    virtual const char *desc() const = 0;

    // 编辑距离
    virtual int editDistance(const std::string &cmdname);

    // 是否应提示给用户
    virtual bool shouldHint(const std::string &cmdname) const;

  protected:
    CommandFactory *mFactory;
    int mLDis;
};

// 命令的抽象工厂
class CommandFactory
{
  public:
    virtual std::string getName() = 0;
    virtual Command *getCommand() = 0;
};

// 工厂类
template <class CommandType>
class CommandFactoryImpl : public CommandFactory
{
  public:
    virtual std::string getName()
    {
        return CommandType::getName();
    }

    virtual Command *getCommand()
    {
        static CommandType s_cmd(this);
        return &s_cmd;
    }
};

// 工厂注册类
typedef std::vector<CommandFactory *> FactoryList;
class CommandFactoryRegistry
{
  public:
    CommandFactoryRegistry();
    ~CommandFactoryRegistry();

    static CommandFactoryRegistry *getRegistry();
    static bool isValid();

    void registerFactory(CommandFactory *factory);
    void unregisterFactory(CommandFactory *factory);

    CommandFactory *getCommandFactory(const std::string &cmdName);

    VectorIterator<FactoryList> getIter();

  protected:
    FactoryList mFactories;
    static bool mbValid;
};

// AutoRegisterCommand用于实现编译期注册，
// 编译期注册的好处是，新加入的命令不需要在现有代码里做任何改动
template<class CommandType>
class AutoRegisterCommand
{
  public:
    AutoRegisterCommand()
            :mRegistry(CommandFactoryRegistry::getRegistry())
    {
        mRegistry->registerFactory(&mFactory);
    }

    ~AutoRegisterCommand()
    {
        if (CommandFactoryRegistry::isValid())
        {
            mRegistry->unregisterFactory(&mFactory);
        }
    }

  private:
    CommandFactoryRegistry *mRegistry;
    CommandFactoryImpl<CommandType> mFactory;
};


#define COMMAND_DECLARATION(CommandType)        \
    static std::string getName();               \


#define COMMAND_IMPLEMENTION(CommandType, cmdName)  \
    std::string CommandType::getName()              \
    {                                               \
        return cmdName;                             \
    }


#define COMMAND_FACTORY_REGISTRATION(CommandType)                       \
    static AutoRegisterCommand<CommandType> _autoRegisterRegistry_##CommandType

#endif // __COMMAND_FACTORY_H__
