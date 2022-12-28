
#include <stdexcept>
#include <algorithm>
#include <iostream>
#include "Input_parser.h"

input_parser::input_parser(int argc, char **argv) {
    if (argc < 5){
        std::cerr << "Usage: " << argv[0] << " -c <config.txt> <output.wav> <input1.wav> [<input2.wav> …]" << std::endl;
    }
    for (int i = 1; i < argc; i++){
        this->tokens.emplace_back(string(argv[i]));
    }
    this->config_path = this->tokens[2];
    this->output_wav = this->tokens[3];
    for (int i = 4; i < argc-1; i++){
        this->input_files.push_back(this->tokens[i]);
    }
}

//void input_parser::parse_str(string& str) {
//    input_parser tmp;
//    boost::split(tmp.tokens,str,boost::is_any_of(" "));
//    *this = parsed_args(tmp);
//}

const string &input_parser::get_option(const string &option) {
    vector<string>::const_iterator it;

    it = find(this->tokens.begin(), this->tokens.end(), option);

    if (it != this->tokens.end() && ++it != this->tokens.end()) {

        return *it;

    }

    static const string empty_string;

    return empty_string;
}

bool input_parser::option_exists(const string& option) {
    return find(this->tokens.begin(), this->tokens.end(), option) != this->tokens.end();
}

int input_parser::parsed_config_lines(vector<string> lines, vector<command> *commands) {
    int cnt = 0;

    for (auto & line : lines) {
        if (line[0] == '#'){
            if (cnt == 0){
                command tmp;
                tmp.comment = line;
                commands->push_back(tmp);
                cnt ++;
            } else if (!commands->at(cnt-1).command_.empty()){
                command tmp;
                tmp.comment = line;
                commands->push_back(tmp);
                cnt ++;
            }else{
                commands->at(cnt-1).comment += ' ' + line;
            }
        }else {
            commands->at(cnt-1).command_ = line;
        }
    }
    return cnt;
}

input_parser& input_parser::operator=(input_parser const&) = default;

input_parser::~input_parser() = default;

void read_config_file(ifstream& stream, vector<string> *lines){
    if (stream.is_open()){
        int i = 0;
        string tmp;
        while(getline(stream, tmp)){
            lines->push_back(tmp);
            i++;
        };
    }else{
        std::cerr << "Check input stream or config file";
    }

}

command::command() = default;

command::~command() = default;

void command::parse_command(command & c) {

    if (c.command_.find("mute") != string::npos){
        string tmp_s;
        string tmp_e;

        int i = 5;
        bool flag = true;
        while (i != c.command_.size()){
            if (c.command_[i] != ' '){
                if (flag) {
                    tmp_s += c.command_[i];
                } else{
                    tmp_e += c.command_[i];
                }
            }else{
                flag = false;
            }
            i++;
        }
        c.start = stoi(tmp_s);
        c.end = stoi(tmp_e);
    }else if(c.command_.find("mix") != string::npos){
        string tmp_n;
        string tmp_s;
        bool flag = true;
        int i = 5;
        while (c.command_[i]){
            if (c.command_[i] != ' '){
                if (flag) {
                    tmp_n += c.command_[i];
                } else{
                    tmp_s += c.command_[i];
                }
            }else{
                flag = false;
            }
            i++;
        }
        c.start = stoi(tmp_s);
        c.num_file = stoi(tmp_n);
    }

}
