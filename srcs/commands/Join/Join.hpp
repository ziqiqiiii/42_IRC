#include "Command.hpp"

namespace IRC
{
	class Join : public Command
	{
		public:
			Join();

		private:
			std::vector<string>	_channel_names;

			Join(const Join &copy);
			Join &operator=(const Join &copy);
			~Join();

			bool	_validateChannel(Channel *channel);
			void	_validate();
			void	_operate();
			void	_parse(std::vector<string>& args);
	};
}