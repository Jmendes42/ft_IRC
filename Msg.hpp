

#ifndef MSG_HPP
# define MSG_HPP

# include <iostream>
# include <queue>

struct s_info {
    std::string nick;
    std::string host;
    std::string real;
    std::string user;
    std::string pass;
} t_info;

class Msg
{
    private:

        std::queue<std::string &> msg_queue;

    public:

        void addMsg(std::string &msg) { msg_queue.push(msg); };
        

};


#endif