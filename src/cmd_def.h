#ifndef __CMD_DEF_H__
#define __CMD_DEF_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

#include <iostream>
#include <vector>
#include <string>
#include <map>

#define NAMESPACE_BEG(spaceName) namespace spaceName {
#define NAMESPACE_END }

#ifndef max
# define max(a, b) ((b) > (a) ? (b) : (a))
#endif
#ifndef min
# define min(a, b) ((b) < (a) ? (b) : (a))
#endif

#define HINT(fmt, ...)                                      \
    do { fprintf(stdout, fmt, ##__VA_ARGS__); } while(0)

#define TRACE(fmt, ...)    do { trace(fmt "\n", ##__VA_ARGS__); } while(0)

#define DEBUG(fmt, ...)    do { logPrint(fmt "\n", ##__VA_ARGS__); } while(0)
#define INFO(fmt, ...)     do { logPrint(fmt "\n", ##__VA_ARGS__); } while(0)
#define WARNING(fmt, ...)  do { logPrint(fmt "\n", ##__VA_ARGS__); } while(0)
#define ERROR(fmt, ...)    do { logPrint(fmt "\n", ##__VA_ARGS__); } while(0)

#define regWrFlock(fd, offset, whence, len)             \
	regflock(fd, F_SETLK, F_WRLCK, offset, whence, len)

#define USERNAME_LEN         99
#define PWD_LEN              99
#define BUF_SIZE             64
#define MAX_BUFSIZE          1024
#define ADDR_LEN             512
#define CMD_SIZE             1024

const char *programName();

// 日志打印
void logPrint(int loglv, const char *fmt, ...);

// 打印到终端
void trace(const char *fmt, ...);
void enableTrace(bool b);

int regflock(int fd, int cmd, int type, off_t offset, int whence, off_t len);

template<typename T>
inline void split(const std::basic_string< T >& s, T c, std::vector< std::basic_string< T > > &v)
{
    if(s.size() == 0)
        return;

    typename std::basic_string< T >::size_type i = 0;
    typename std::basic_string< T >::size_type j = s.find(c);

    while(j != std::basic_string<T>::npos)
    {
        std::basic_string<T> buf = s.substr(i, j - i);

        if(buf.size() > 0)
            v.push_back(buf);

        i = ++j; j = s.find(c, j);
    }

    if(j == std::basic_string<T>::npos)
    {
        std::basic_string<T> buf = s.substr(i, s.length() - i);
        if(buf.size() > 0)
            v.push_back(buf);
    }
}

inline static std::string &trim(std::string &s)
{
    if(!s.empty())
    {
        s.erase(0, s.find_first_not_of(" \n\r\t"));
        s.erase(s.find_last_not_of(" \n\r\t") + 1);
    }
    return s;
}

#endif // __CMD_DEF_H__
