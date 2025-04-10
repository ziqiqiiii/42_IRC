#include "Command.hpp"

namespace IRC
{
	class User : public Command
	{
		public:
			User();

		private:
			string	_username;

			User(const User &copy);
			User &operator=(const User &copy);
			~User();

			void	_validate();
			void	_operate();
			void	_parse(std::vector<string>& args);
	};
}