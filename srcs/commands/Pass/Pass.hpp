#include "Command.hpp"

namespace IRC
{
	class Pass : public Command
	{
		public:
			Pass();

		private:
			string	_pass;

			Pass(const Pass &copy);
			Pass &operator=(const Pass &copy);
			~Pass();

			void	_validate();
			void	_operate();
			void	_parse(std::vector<string>& args);
	};
}