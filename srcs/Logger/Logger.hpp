#pragma once

# include "ircserv.hpp"

//--------------------Colors-----------------------------//
// ANSI escape codes for colored output
# define RESET			"\x1B[0m" /**< Resets text formatting to default. */
# define RED				"\x1B[31m" /**< Red text color. */
# define LIGHT_RED		"\x1B[91m" /**< Light red text color. */
# define WHITE			"\x1B[37m" /**< White text color. */
# define BLINK			"\x1b[5m" /**< Blinking text. */
# define YELLOW			"\x1B[33m" /**< Yellow text color. */
# define LIGHT_BLUE		"\x1B[94m" /**< Light blue text color. */
# define CYAN			"\x1B[36m" /**< Cyan text color. */
# define DARK_GREY		"\x1B[90m" /**< Dark grey text color. */
# define LIGHTMAGENTA	"\x1B[95m" /**< Light magenta text color. */
//--------------------Time Offset-----------------------------//
# define GST				8; /**< Malaysia time offset for GMT+8. */

namespace IRC
{
	class Logger
	{
		private:
			static Logger *instancePtr; /**< Pointer to the Singleton instance of Logger. */
			static pthread_mutex_t mtx; /**< Mutex for thread-safe Singleton access. */

			Logger();
			~Logger();
			Logger(const Logger &other);
			Logger &operator=(const Logger &other);
		public:
			static Logger* getInstance();

			void	logMsg(const char *color, const char *msg, ...);
			string	getCurrTime();
	};
}
