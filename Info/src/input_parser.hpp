#ifndef INPUT_PARSER_HPP
#define INPUT_PARSER_HPP

// source InputParser: https://stackoverflow.com/questions/865668/how-to-parse-command-line-arguments-in-c
class InputParser
{
    public:
        InputParser(int argc, const char *argv[])
        {
			bool found_filename=false;
			const char* p = nullptr;
            for (int i=1; i < argc; ++i)
			{
				if (argv[i][0] != '-')
				{
					if (p != nullptr)
					{
						if (std::string("-i")==p)
							found_filename=true;
						tokens.insert(std::pair<std::string,std::string>(std::string(p),std::string(argv[i])));
					}
					else if (!found_filename)
					{
						tokens.insert(std::pair<std::string,std::string>(std::string("-i"),std::string(argv[i])));
						found_filename=true;
					}
					else
						std::cerr << "Unknown parameter " << argv[i] << std::endl;
					p=nullptr;
				}
				else
				{
					if (p!=nullptr)
						tokens.insert(std::pair<std::string,std::string>(std::string(p),std::string("")));
					std::string tmp(argv[i]);
					auto it = tmp.find('=');
					if (it != std::string::npos)
					{
						tokens.insert(std::pair<std::string,std::string>(tmp.substr(0,it),tmp.substr(it+1)));
						p=nullptr;
					}
					else
						p=argv[i];
				}
			}
			if (p!=nullptr)
				tokens.insert(std::pair<std::string,std::string>(std::string(p),std::string("")));
        }

        const std::string& getCmdOption(const std::string &option) const
        {
            auto itr = tokens.find(option);
            if (itr != tokens.end())
            {
                return itr->second;
            }
			else
			{
				static std::string emptystring=std::string("");
				return emptystring;
			}
        }

        bool cmdOptionExists(const std::string &option) const
        {
            return tokens.find(option) != tokens.end();
        }
    private:
        std::map<std::string,std::string> tokens;
};

#endif

