#pragma once
#include <string>

namespace utils
{
	class toast
	{
	public:
		static toast show(const std::string& title, const std::string& text);
		static toast show(const std::string& title, const std::string& text, const std::string& image);

		operator bool() const;
		void hide() const;

	private:
		toast(int64_t id);
		int64_t id_;
	};
}
