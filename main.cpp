#include <iostream>
#include <cstdio>
#include "Utility/Input_parser.h"
using namespace std;

int main(int argc, char **argv) {
    input_parser prs(argc, argv);

    ifstream ifs;

    vector<string> config_strings;
    vector<command> config_commands;
//    string config_strings[100];
//    command_ config_commands[100];
    int cnt_comm = -1;

    int len = prs.config_path.size();

    if (prs.config_path.empty()){

        cerr << "Usage: " << argv[0] << " -c <config.txt> <output.wav> <input1.wav> [<input2.wav> …]" << std::endl;

    }else if (prs.config_path[len-1] == 't' and prs.config_path[len-2] == 'x' and prs.config_path[len-3] == 't'){

        ifs.open(prs.config_path);
        read_config_file(ifs, &config_strings);
        cnt_comm = input_parser::parsed_config_lines(config_strings, &config_commands);

    }
    ifs.close();

    if (cnt_comm != -1){

        ifstream  o_wav(prs.input_files[0], ios::binary | ios::in);
        char c;
        o_wav.get(c);
        for (int i = 0; i < cnt_comm; i++){

            size_t pos_mu = config_commands[i].command_.find("mute");
            size_t pos_mi = config_commands[i].command_.find("mix");

            if (pos_mu != string::npos){

                command::parse_command(config_commands[i]);

            }else if (pos_mi != string::npos){

                command::parse_command(config_commands[i]);

            }

        }
        o_wav.close();

    }
    cout << config_commands[0].command_ << endl;

    return 0;
}
