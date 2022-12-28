#include <iostream>
#include <cstdio>
#include "Utility/Input_parser.h"
#include "Proc/Proccessor.h"

using namespace std;

int getFileSize(FILE* inFile)
{
    int fileSize = 0;
    fseek(inFile, 0, SEEK_END);

    fileSize = ftell(inFile);

    fseek(inFile, 0, SEEK_SET);
    return fileSize;
}

void foo(int start, int end, FILE *orig, FILE *out, long long& offset) {
    int diff = end - start;
    static const uint32_t FILL_SIZE = 88200 * diff;
    int8_t fill[FILL_SIZE];

    for (int8_t & cell : fill){
        cell = 0;
    }
    fseek(out, offset + start, SEEK_SET);

    fwrite(&fill, 1, FILL_SIZE, out);

}

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
        wav_hdr orig_wav_head;
        FILE* orig_wav = fopen(prs.input_files[0].c_str(), "rb");

        int header_size = sizeof(orig_wav_head), filelength = 0;
        size_t bytes_read = fread(&orig_wav_head, 1, header_size, orig_wav);

        int track_info_size = orig_wav_head.Subchunk2Size+8;
        char track_info[track_info_size];
        fread(&track_info, 1, track_info_size, orig_wav);
        char data_s[4];
        fread(&data_s, 1, 4, orig_wav);
        int sound_data_size;
        fread(&sound_data_size, 1, sizeof (sound_data_size), orig_wav);
        char sound_data[sound_data_size];
        fread(&sound_data_size, 1, sound_data_size, orig_wav);

        FILE* output = fopen(prs.output_wav.c_str(), "wb");
        fwrite(&orig_wav_head, 1, header_size, output);
        fwrite(track_info, 1, track_info_size, output);
        fwrite(data_s, 1, sizeof (data_s), output);
        fwrite(&sound_data_size, 1, sizeof(sound_data_size), output);
        fwrite(sound_data, 1, sound_data_size, output);

        long long offset = header_size + track_info_size;

        for (int i = 0; i < cnt_comm; i++){

            size_t pos_mu = config_commands[i].command_.find("mute");
            size_t pos_mi = config_commands[i].command_.find("mix");

            if (pos_mu != string::npos){
                command::parse_command(config_commands[i]);

                foo(config_commands[i].start, config_commands[i].end, orig_wav, output, offset);

            }
//            else if (pos_mi != string::npos){
//
//                command::parse_command(config_commands[i]);
//
//            }

        }
        fclose(orig_wav);

    }
    cout << config_commands[0].command_ << endl;

    return 0;
}
