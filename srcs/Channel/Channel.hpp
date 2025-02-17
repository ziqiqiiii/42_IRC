#pragma once

# include "ircserv.hpp"

namespace IRC
{
    class Channel: public IObserver
    {
        private:

        public:
            Channel();
            ~Channel();
            Channel(const Channel &other);
            Channel &operator=(const Channel &other);
    };
}
