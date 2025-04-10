#include "Command.hpp"

namespace IRC
{
	class Nick : public Command
	{
		public:
			Nick();

		private:
			string	_nickname;

			Nick(const Nick &copy);
			Nick &operator=(const Nick &copy);
			~Nick();

			void	_validate();
			void	_operate();
			void	_parse(std::vector<string>& args);
	};
}