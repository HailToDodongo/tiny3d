/**
* @copyright 2023 - Max Bebök
* @license MIT
*/
#pragma once

#include <string>
#include <unordered_map>

class EnvArgs
{
  private:
    EnvArgs(const EnvArgs&) = delete;
    EnvArgs(EnvArgs&&) = delete;
    std::unordered_map<std::string, std::string> argMap;
    std::vector<std::string> fileArgs;

  public:
    EnvArgs(int argc, char** argv)
    {
      for(auto i=1; i<argc; ++i)
      {
        auto arg = std::string(argv[i]);

        if(arg.length() < 2 || arg[0] != '-'){
          fileArgs.push_back(arg);
          continue;
        }
        auto equalPos = arg.find('=');
        auto argName = arg.substr(0, equalPos);
        auto argVal = equalPos == std::string::npos ? "" : arg.substr(equalPos+1);
        argMap[argName] = argVal;
      }
    }

    bool checkArg(const std::string &argName) const
    {
      return argMap.contains(argName);
    }

    std::string getStringArg(const std::string &argName)
    {
      return argMap[argName];
    }

    uint32_t getU32Arg(const std::string &argName, uint32_t fallback = 0) {
      if(argMap.contains(argName)) {
        return std::stoul(argMap[argName]);
      }
      return fallback;
    }

    std::string getFilenameArg(uint32_t index) {
      if(index < fileArgs.size()) {
        return fileArgs[index];
      }
      return "";
    }
};
