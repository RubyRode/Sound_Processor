#ifndef GAME_OF_LIFE_INPUT_PARSER_H
#define GAME_OF_LIFE_INPUT_PARSER_H

#include <cstdlib>
#include <vector>
#include <string>
#include <boost/algorithm/string.hpp>
#include <fstream>
using namespace std;

class command{
public:
    string comment;
    string command_;
    int start{};
    int end{};
    int num_file{};

    static void parse_command(command&);

    command();
    ~command();

};

using namespace std;
class input_parser{
public:
    string config_path;
    string output_wav;
    vector<string> input_files;
    vector<command> commands;

    // input_parser();
    //void parse_str(string&);
    input_parser(int argc, char** argv);
    ~input_parser();
    input_parser& operator=(input_parser const&);

    const string& get_option(const string& option);
    bool option_exists(const string& option);
    static int parsed_config_lines(vector<string>, vector<command>*);
private:
    vector<string> tokens;
};

void read_config_file(ifstream&, vector<string>*);


#endif //GAME_OF_LIFE_INPUT_PARSER_H
