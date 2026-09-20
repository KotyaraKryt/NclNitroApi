#include <string>
#include <string_view>
#include <vector>

#ifdef _WIN32
#include <Windows.h>
#endif

namespace nitro_utils
{
    size_t split(std::string_view str, std::string_view split_by, std::vector<std::string_view>& tokens)
    {
        tokens.clear();

        if (split_by.empty())
        {
            tokens.push_back(str);
            return 1;
        }

        size_t pos = 0;

        while (pos < str.size())
        {
            size_t found = str.find(split_by, pos);

            if (found == std::string_view::npos)
            {
                tokens.emplace_back(str.substr(pos));
                return tokens.size();
            }

            tokens.emplace_back(str.substr(pos, found - pos));
            pos = found + split_by.size();
        }

        if (pos == str.size())
        {
            tokens.emplace_back(std::string_view{});
        }

        return tokens.size();
    }

    size_t split_in_args(const std::string& str, std::vector<std::string>& args, size_t maxArgs)
    {
        size_t argsNum = 0;
        size_t len = str.length();
        bool qot = false, sqot = false;
        int argLen;

        for (int i = 0; i < len; i++)
        {
            int start = i;
            if (str[i] == '\"')
                qot = true;
            else if(str[i] == '\'')
                sqot = true;

            if (qot)
            {
                i++;
                start++;
                while (i < len && str[i] != '\"')
                    i++;
                if (i < len)
                    qot = false;
                argLen = i - start;
                i++;
            }
            else if(sqot)
            {
                i++;
                while (i < len && str[i] != '\'')
                    i++;
                if (i < len)
                    sqot = false;
                argLen = i - start;
                i++;
            }
            else
            {
                while(i < len && str[i] != ' ')
                    i++;
                argLen = i - start;
            }
            args.push_back(str.substr(start, argLen));
            if (++argsNum >= maxArgs)
                return argsNum;
        }
        return argsNum;
    }

#ifdef _WIN32
    std::string ConvertCurrentCodepageToUtf8(const std::string_view& str)
    {
        int size = MultiByteToWideChar(CP_ACP, MB_COMPOSITE, str.data(), str.length(), nullptr, 0);

        std::wstring utf16_str(size, '\0');
        MultiByteToWideChar(CP_ACP, MB_COMPOSITE, str.data(), str.length(), &utf16_str[0], size);

        int utf8_size = WideCharToMultiByte(CP_UTF8, 0, utf16_str.c_str(), str.length(), nullptr, 0, nullptr, nullptr);
        std::string utf8_str(utf8_size, '\0');
        WideCharToMultiByte(CP_UTF8, 0, utf16_str.c_str(), str.length(), &utf8_str[0], utf8_size, nullptr, nullptr);

        return utf8_str;
    }

    std::wstring utf8_to_wide(std::string_view str)
    {
        if (str.empty())
        {
            return {};
        }

        int size = MultiByteToWideChar(CP_UTF8, 0, str.data(), static_cast<int>(str.size()), nullptr, 0);

        if (size <= 0)
        {
            return {};
        }

        std::wstring wide(size, L'\0');
        MultiByteToWideChar(CP_UTF8, 0, str.data(), static_cast<int>(str.size()), wide.data(), size);

        return wide;
    }

    std::string wide_to_utf8(std::wstring_view str)
    {
        if (str.empty())
        {
            return {};
        }

        int size = WideCharToMultiByte(CP_UTF8, 0, str.data(), static_cast<int>(str.size()), nullptr, 0, nullptr, nullptr);

        if (size <= 0)
        {
            return {};
        }

        std::string utf8(size, '\0');
        WideCharToMultiByte(CP_UTF8, 0, str.data(), static_cast<int>(str.size()), utf8.data(), size, nullptr, nullptr);

        return utf8;
    }

    void to_lower(std::wstring& str)
    {
        CharLowerBuffW(str.data(), static_cast<DWORD>(str.size()));
    }

    std::wstring to_lower_copy(std::wstring_view str)
    {
        std::wstring result(str);
        to_lower(result);

        return result;
    }
#endif
}